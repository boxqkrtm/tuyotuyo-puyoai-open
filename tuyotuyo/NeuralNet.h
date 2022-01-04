#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::pair;

struct Connection
{
	float weight;
	float deltaWeight;
};


class Neuron;

typedef vector<Neuron> Layer;

class Neuron
{
public:
	void setOutputVal(float val) { m_outputVal = val; }
	float getOutputVal(void) const { return m_outputVal; }
	void feedForward(const Layer& prevLayer);
	Neuron(int numOutputs, int myIndex, int numInputs);
	void calcOutputGradients(float targetVal);
	void calcHiddenGradients(const Layer& nextLayer);
	void updateInputWeights(Layer& prevLayer);
	vector<Connection> getConnection() const;
	void setConnection(float value, int num);
	float m_gradient = 0;
	float m_gradientCnt = 0;
private:
	static float eta;
	static float alpha;
	static float transferFunction(float x);
	static float transferFunctionDerivative(float x);
	static float randomWeight(void) { return rand() / float(RAND_MAX); }
	float sumDOW(const Layer& nextLayer) const;
	float m_outputVal;
	vector<Connection> m_outputWeights;
	int m_myIndex;

};


class NeuralNet
{
public:
	NeuralNet(NeuralNet *nn);
	NeuralNet(const vector<int>& topology);
	void inParam();
	void outParam();
	int getMaxIndex();
	vector<pair<float, int>> getResultTop();
	void feedForward(const vector<float>& inputVals);
	void backProp(const vector<float>& targetVals);
	void updateWeights();
	void getResults(vector<float>& resultVals) const;
	float getRecentAverageError(void) const { return m_recentAverageError; }

private:
	vector<Layer> m_layers; // m_layers[layerNum][neuronNum]
	float m_error;
	float m_recentAverageError;
	static float m_recentAverageSmoothingFactor;
};
