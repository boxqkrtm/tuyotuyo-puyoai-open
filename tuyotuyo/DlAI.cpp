#include "DlAI.h"
#include "Duel.h"

#include <algorithm>

#define DO_DEPTH 4  //탐색으로 둬볼 깊이
#define SIMCOUNT 100//랜덤시뮬레이션 횟수

DlAI::DlAI()
{
	reset();
	randParams();
	_r = 0;
}

void DlAI::randParams()
{
	vector<int> model = { 18, 6,1 };
	_nn = new NeuralNet(model);
}

DlAI::DlAI(DlAI* fa)
{
	_input = fa->_input;
}

void DlAI::Learn(GameInfo g, vector<float> answer) {
	//while(_nn->getRecentAverageError() > 0.01){
	_nn->feedForward(g.getDlInput());
	_nn->backProp(answer);
	//}
}

void DlAI::LearnEnd() {
	_nn->updateWeights();
}

void DlAI::setRandom(float value) {
	_r = value;
}

void DlAI::reset()
{
	_bagIndex = 0;
	for (size_t i = 0; i < 128; i++)
	{
		_bag[i] = PuyoTsumo(0, 0);
	}
	_input = -1;
}

int DlAI::getBestMove(GameInfo g, int depth) {

	float best = -9999;
	float maxscore = 0;
	int besti = 2;
	for (int i = 0; i < ALL_MOVEMENT_MAX; i++) {
		Simulator s(-1);
		s.set(g._myField, g._my14remove);
		if (s.pushFastNoPop(ALL_MOVEMENT[i], g._myNext[depth])) {
			GameInfo gg(g);
			memcpy(gg._myField, s.getField(),sizeof(int)*13*6);
			s.forceFire();
			if (s.getGameover()) continue;
			gg._myOjama -= s.getScore()/70;
			if (gg._myOjama < 0) {
				gg._oppOjama += gg._myOjama;
				gg._myOjama = 0;
			}
			_nn->feedForward(gg.getDlInput());
			
			float score = _nn->getResultTop()[0].first;
			//cout << score << endl;
			if (score > maxscore) {
				maxscore = score;
				besti = i;
			}
		}
	}
	return ALL_MOVEMENT[besti];
}

float DlAI::getFieldScore(GameInfo g) {
	_nn->feedForward(g.getDlInput());
	vector<float> result;
	_nn->getResults(result);
	return result[0];
}

int DlAI::think(GameInfo g) {
	_bag[_bagIndex] = g._myNext[0];
	_bag[_bagIndex + 1] = g._myNext[1];
	_bagIndex++;
	int i, j;
	int loop;
	float score[ALL_MOVEMENT_MAX] = { 0 };
	float scoreCnt[ALL_MOVEMENT_MAX] = { 0 };
	int scoreMaxIndex = 2;
	string debug[ALL_MOVEMENT_MAX];

	if (_r < (float)rand() / RAND_MAX || true) {
		return getBestMove(g);
		//int i1 = 0;
		//int i2 = 0;

		////for (int i = 0; i < ALL_MOVEMENT_MAX; i++) {
		////	Simulator s(-1);
		////	s.set(g._myField, g._my14remove);
		////	GameInfo gg = GameInfo(g);
		////	if (s.pushFast(ALL_MOVEMENT[i], g._myNext[0])) {
		////		memcpy(gg._myField, (int(*))s.getField(), sizeof(int)*13*6);
		////		_nn->feedForward(gg.getDlInput());
		////		score[i] += _nn->getResultTop()[0].first;
		////		scoreCnt[i] += 1;
		////	}
		////}

		//FormAIv2 f = FormAIv2();
		//f._print = false;
		//f.AI_SEE = false;
		//f.AI_SURPRISE_ATTACK = false;

		//for (int i = 0; i < ALL_MOVEMENT_MAX; i++) {
		//	Simulator s(-1);
		//	s.set(g._myField, g._my14remove);
		//	if (s.pushFast(ALL_MOVEMENT[i], g._myNext[0]) == false) {
		//		score[i] = -9999;
		//		scoreCnt[i] = 1;
		//		continue;
		//	}
		//	if (s.getGameover() == true) {
		//		score[i] = -9998;
		//		scoreCnt[i] = 1;
		//		continue;
		//	}

		//	Duel d = Duel(g);
		//	d.run();


		//	//push 
		//	bool* win = NULL;
		//	d.input(ALL_MOVEMENT[i], -2);
		//	d.run();
		//	while (true) {
		//		if (d.needInput(0) == true|| d.needInput(1) == true || d.run() == false) break;
		//	}
		//	for (int sim = 0; sim < SIMCOUNT; sim++) {
		//		Duel dd(d);
		//		//나머지 게임 시뮬
		//		int turn = 0;
		//		while (true) {
		//			if (dd.needInput(0) == true) { i1 = -2;  turn++; }
		//			if (dd.needInput(1) == true) { i2 = -2; turn++; }
		//			dd.input(i1, i2);
		//			bool r = dd.run();
		//			//dd.print();

		//			//x의 수를 둬본 뒤 까지 탐색
		//			if (r == false || turn >= DO_DEPTH) {
		//				win = dd.getWin();
		//				break;
		//			}
		//		}
		//		scoreCnt[i] += 1;
		//		if ((win[0] == 1) && (win[1] == 0)) {
		//			score[i] += 1;
		//			//cout << "win" << endl;
		//		}
		//		else {
		//			score[i] -= 1;

		//		}
		//	}
		//}

		//Simulator s = Simulator(-1);
		//s.set(g._myField, g._my14remove);
		////if (s.getFieldSpaceMax1() > 0.8) return f.think(g);
		//float scoreMax = 0.9;

		//scoreMaxIndex = -1;
		//for (int loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {
		//	if (score[loop] / scoreCnt[loop] > scoreMax) {
		//		scoreMax = score[loop] / scoreCnt[loop];
		//		scoreMaxIndex = loop;
		//	}
		//	cout << score[loop]/ scoreCnt[loop] << " ";
		//}
		//cout << endl;

		//if (scoreMaxIndex == -1) return f.think(g);

		//_input = ALL_MOVEMENT[scoreMaxIndex];
		//return _input;
	}

	for (loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {//ALL_MOVEMENT_MAX
		Simulator s = Simulator(-1);
		s.set(g._myField, g._my14remove);
		score[loop] = testNoThink(s, g, loop, 0, debug[loop]);
	}
	//get max socre index
	_debug = "";

	float scoreMax = -10000.0;
	scoreMaxIndex = 0;
	for (int loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {
		if (score[loop] > scoreMax) {
			scoreMax = score[loop];
			scoreMaxIndex = loop;
		}
		//cout << score[loop] << " ";
	}

	_input = ALL_MOVEMENT[scoreMaxIndex];
	return _input;
}

PuyoTsumo* DlAI::createVirtualBag() {
	if (_bagIndex + 1 >= 128) {
		return _bag;
	}
	else {
		int table[6] = { 0, 64,64,64,64,64 };
		for (int i = 0; i < 128; i++) {
			table[_bag[i].down] -= 1;
			table[_bag[i].up] -= 1;
		}
		int tableKind = 0;
		for (int i = 1; i < 6; i++) {
			if (table[i] != 64) tableKind += 1;
		}
		PuyoTsumo resultBag[128];
		memcpy(resultBag, _bag, 128);

		for (int i = 1; i < 6; i++) {
			//없는색 큐 하나 0으로 비움
			if (table[i] == 64) {
				table[i] = 0;
				break;
			}
		}

		vector<int> puyoColorQueue;
		for (int i = 0; i < 6;) {
			if (table[i] > 0) {
				table[i] -= 1;
				puyoColorQueue.push_back(i);
			}
			else {
				i++;
				continue;
			}
		}

		//섞음
		for (int i = 0; i < puyoColorQueue.size(); i++) {
			int temp;
			int r = rand() % puyoColorQueue.size();
			temp = puyoColorQueue[i];
			puyoColorQueue[i] = puyoColorQueue[r];
			puyoColorQueue[r] = temp;
		}


		//섞은 색패로 가방 채움
		for (size_t i = 0; (_bagIndex + i) < 128; i++)
		{
			int u = puyoColorQueue.back();
			puyoColorQueue.pop_back();
			int d = puyoColorQueue.back();
			puyoColorQueue.pop_back();
			resultBag[_bagIndex + i] = PuyoTsumo(u, d);
		}

		return resultBag;
	}
}

int DlAI::input()
{
	return _input;
}

void DlAI::setDepth(int x)
{
	_maxDepth = x;
}

string DlAI::getDebugStr()
{
	return _debug;
}

float DlAI::testScoreOpp(Simulator s, GameInfo g, int loop, int depth, string& debug, PuyoTsumo next) {

	//if (depth >= 2 && (rand() % 10 || _nodeCnt > _maxNode)) return -1;
	if (depth == 0) debug = "";
	float score = 0;
	int i, j;
	int afterRensa = 1; //drop after rensa
	int afterScore = 0; //drop after score;
	bool isFire = false;
	if (next.down == 0 && next.up == 0)
		next = g._oppNext[depth];

	Simulator os = Simulator(-1);
	os.set(g._oppField, g._opp14remove);

	if (os.applyGravityOnce() > 0 || os.pushFast(ALL_MOVEMENT[loop], next, afterRensa, afterScore) == true) {
		os.applyGravity();
		if (os.getGameover() == true) {
			return 1;
		}
		int(*sField)[6] = (int(*)[6])os.getField();
		for (int i = 0; i < 13; i++)
			for (int j = 0; j < 6; j++)
				g._oppField[i][j] = sField[i][j];
		int sent = afterScore / 70;
		g._oppOjama -= sent;
		if (g._oppOjama < 0) {
			g._myOjama -= g._oppOjama;
			g._oppOjama = 0;
		}
		g._oppAllClear = os.getAllClear();

		_nn->feedForward(g.getDlInput());
		vector<float> result;
		_nn->getResults(result);
		return result[0];
	}
	else {
		//cant move
		return 1.1;
	}
}

float DlAI::testScore(Simulator s, GameInfo g, int loop, int depth, string& debug, PuyoTsumo next) {

	//if (depth >= 2 && (rand() % 10 || _nodeCnt > _maxNode)) return -1;
	if (depth == 0) debug = "";
	float score = 0;
	int i, j;
	int afterRensa = 1; //drop after rensa
	int afterScore = 0; //drop after score;
	bool isFire = false;
	if (next.down == 0 && next.up == 0)
		next = g._myNext[depth];

	if (s.pushFast(ALL_MOVEMENT[loop], next, afterRensa, afterScore) == true) {
		if (s.getGameover() == true) {
			return -998;
		}
		else {
			int(*sField)[6] = (int(*)[6])s.getField();
			for (int i = 0; i < 13; i++)
				for (int j = 0; j < 6; j++)
					g._myField[i][j] = sField[i][j];
			int sent = afterScore / 70;
			g._myOjama -= sent;
			if (g._myOjama < 0) {
				g._oppOjama -= g._myOjama;
				g._myOjama = 0;
			}
			g._myAllClear = s.getAllClear();

			_nn->feedForward(g.getDlInput());
			vector<float> result;
			_nn->getResults(result);
			return result[0];
		}
	}
	else {
		//cant move
		return -999;
	}
}

float DlAI::testNoThink(Simulator s, GameInfo g, int loop, int depth, string& debug, PuyoTsumo next) {
	if (depth == 0) debug = "";
	float score = 0;
	int i, j;
	int afterRensa = 1; //drop after rensa
	int afterScore = 0; //drop after score;
	bool isFire = false;
	if (next.down == 0 && next.up == 0)
		next = g._myNext[depth];
	if (s.pushFast(ALL_MOVEMENT[loop], next, afterRensa, afterScore) == true) {
		if (s.getGameover() == true) {
			return -998;
		}
		else {
			return (float)(rand() % 100);
		}
	}
	else {
		//cant move
		return -999;
	}
}

void DlAI::save()
{
	_nn->outParam();
}

void DlAI::load()
{
	_nn->inParam();
}
