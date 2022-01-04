#include "NeuralNet.h"
#include "CrossFunc.h"

float Neuron::eta = 0.0001;
float Neuron::alpha = 0;//momentum

void Neuron::updateInputWeights(Layer& prevLayer)
{

	for (int n = 0; n < prevLayer.size(); ++n) {
		Neuron& neuron = prevLayer[n];
		float oldDeltaWeight = neuron.m_outputWeights[m_myIndex].deltaWeight;

		float newDeltaWeight = eta * neuron.getOutputVal()
			* (m_gradient/m_gradientCnt) + alpha * oldDeltaWeight;

		neuron.m_outputWeights[m_myIndex].deltaWeight = newDeltaWeight;
		neuron.m_outputWeights[m_myIndex].weight += newDeltaWeight;
		//neuron.m_outputWeights[m_myIndex].weight = max(min(neuron.m_outputWeights[m_myIndex].weight, 1), -1);//grad clip
	}
}

inline vector<Connection> Neuron::getConnection() const
{
	return m_outputWeights;
}

void Neuron::setConnection(float value, int num)
{
	m_outputWeights[num].weight = value;
}

float Neuron::sumDOW(const Layer& nextLayer) const
{
	float sum = 0.0;


	for (int n = 0; n < nextLayer.size() - 1; ++n) {
		sum += m_outputWeights[n].weight * nextLayer[n].m_gradient;
	}

	return sum;
}

void Neuron::calcHiddenGradients(const Layer& nextLayer)
{
	float dow = sumDOW(nextLayer);
	m_gradient += dow * Neuron::transferFunctionDerivative(m_outputVal);
	m_gradientCnt += 1;
}

void Neuron::calcOutputGradients(float targetVal)
{
	float delta = targetVal - m_outputVal;
	m_gradient += delta * Neuron::transferFunctionDerivative(m_outputVal);
	m_gradientCnt += 1;
}

float Neuron::transferFunction(float x)
{
	//if (x > 1) return 1+(x-1)*0.0001;
	//return x > 0 ? x : x*0.0001;
	return tanh(x);
}

float Neuron::transferFunctionDerivative(float x)
{
	//if (x > 1)return 0.0001;
	//return x > 0 ? 1 : -0.0001;
	return 1.0 - x * x;
}

void Neuron::feedForward(const Layer& prevLayer)
{
	float sum = 0.0;

	for (int n = 0; n < prevLayer.size(); ++n) {
		sum += prevLayer[n].getOutputVal() *
			prevLayer[n].m_outputWeights[m_myIndex].weight;
	}

	m_outputVal = Neuron::transferFunction(sum);
}

Neuron::Neuron(int numOutputs, int myIndex, int numInputs)
{
	for (int c = 0; c < numOutputs; ++c) {
		m_outputWeights.push_back(Connection());
		m_outputWeights.back().weight = randomWeight();
			//randomWeight();
	}
	m_myIndex = myIndex;
}



float NeuralNet::m_recentAverageSmoothingFactor = 100.0; //100


void NeuralNet::getResults(vector<float>& resultVals) const
{
	resultVals.clear();

	for (int n = 0; n < m_layers.back().size() - 1; ++n) {
		resultVals.push_back(m_layers.back()[n].getOutputVal());
	}
}

void NeuralNet::backProp(const vector<float>& targetVals)
{

	Layer& outputLayer = m_layers.back();
	m_error = 0.0;

	for (int n = 0; n < outputLayer.size() - 1; ++n) {
		float delta = targetVals[n] - outputLayer[n].getOutputVal();
		m_error += delta * delta;
	}
	m_error /= 2; // get average error squared
	//m_error /= outputLayer.size() - 1; // get average error squared
	m_error = sqrt(m_error); // RMS

	m_recentAverageError = m_error;

	//m_recentAverageError =
	//	(m_recentAverageError * m_recentAverageSmoothingFactor + m_error)
	//	/ (m_recentAverageSmoothingFactor + 1.0);

	for (int n = 0; n < outputLayer.size() - 1; ++n) {
		outputLayer[n].calcOutputGradients(targetVals[n]);
	}

	for (int layerNum = m_layers.size() - 2; layerNum > 0; --layerNum) {
		Layer& hiddenLayer = m_layers[layerNum];
		Layer& nextLayer = m_layers[layerNum + 1];

		#pragma omp parallel for
		for (int n = 0; n < hiddenLayer.size(); ++n) {
			hiddenLayer[n].calcHiddenGradients(nextLayer);
		}
	}
}

void NeuralNet::updateWeights() {
	for (int layerNum = m_layers.size() - 1; layerNum > 0; --layerNum) {
		Layer& layer = m_layers[layerNum];
		Layer& prevLayer = m_layers[layerNum - 1];

		#pragma omp parallel for
		for (int n = 0; n < layer.size() - 1; ++n) {
			layer[n].updateInputWeights(prevLayer);
		}
	}

	//reset m_gradient
	for (int layerNum = m_layers.size() - 1; layerNum >= 0; --layerNum) {
		Layer& layer = m_layers[layerNum];
		for (auto &a : layer) {
			a.m_gradient = 0;

		}
	}
}

void NeuralNet::feedForward(const vector<float>& inputVals)
{
	assert(inputVals.size() == m_layers[0].size() - 1);

	for (int i = 0; i < inputVals.size(); ++i) {
		m_layers[0][i].setOutputVal(inputVals[i]);
		cout << inputVals[i] << " ";
	}
	cout << endl;

	for (int layerNum = 1; layerNum < m_layers.size(); ++layerNum) {
		Layer& prevLayer = m_layers[layerNum - 1];
		#pragma omp parallel for
		for (int n = 0; n < m_layers[layerNum].size() - 1; ++n) {
			m_layers[layerNum][n].feedForward(prevLayer);
		}
	}
}

NeuralNet::NeuralNet(NeuralNet *nn)
{
	m_layers = vector<Layer>(nn->m_layers);
	m_error = nn->m_error;
	m_recentAverageError = nn->m_recentAverageError;
	m_recentAverageSmoothingFactor = nn->m_recentAverageSmoothingFactor;
}

NeuralNet::NeuralNet(const vector<int>& topology)
{
	int numLayers = topology.size();
	for (int layerNum = 0; layerNum < numLayers; ++layerNum) {
		m_layers.push_back(Layer());
		int numOutputs = layerNum == topology.size() - 1 ? 0 : topology[layerNum + 1];
		int numInputs = layerNum == 0 ? 1 : topology[layerNum - 1];

		for (int neuronNum = 0; neuronNum <= topology[layerNum]; ++neuronNum) {
			m_layers.back().push_back(Neuron(numOutputs, neuronNum, numInputs ));
			//cout << "Made a Neuron!" << endl;
		}

		m_layers.back().back().setOutputVal(1.0);
	}
}

void NeuralNet::inParam() {
	vector <float> values;
	string in_line;
	std::ifstream in(string(DLLOC)+"param.txt");
	if (in.is_open()) {
		int lineNum = 0;
		while (getline(in, in_line)) {
			if (lineNum++ == 0)continue;
			values.push_back(atof(in_line.c_str()));
		}
	}
	else {
		return;//error not load
	}
	in.close();

	//set
	for (auto& i : m_layers)//layers
	{
		for (auto& j : i) {//neuron
			for (int k = 0; k < j.getConnection().size(); k++) {
				j.setConnection(values.back(), k);
				values.pop_back();
			}
		}
	}
}

void NeuralNet::outParam() {
	string out_line;
	std::ofstream out(string(DLLOC)+"param.txt");
	for (auto i : m_layers) {
		out << i.size() - 1 << " ";
	}
	out << endl;
	for (auto i : m_layers)
	{
		for (auto j : i) {
			for (auto k : j.getConnection()) {
				out << k.weight << endl;
			}
		}
	}
	out.close();
}

int NeuralNet::getMaxIndex() {
	const float Eps = 0.000000000000001;
	vector<float> result;
	getResults(result);
	float max = -9999999.0;
	int maxIndex = -1;
	for (int i = 0; i < result.size(); i++) {
		if (result[i] > max) {
			max = result[i];
			maxIndex = i;
		}
	}
	return maxIndex;
}

bool resultCompare(pair<float, int> a, pair<float, int> b) {
	return a.first > b.first ? true : false;
}

vector<pair<float, int>> NeuralNet::getResultTop() {
	vector<pair<float, int>> result;
	vector<float> temp;
	getResults(temp);
	for (int i = 0; i < temp.size(); i++) {
		result.push_back(pair<float, int>(temp[i], i));
	}
	sort(result.begin(), result.end(), resultCompare);
	return result;
}
