#pragma once
#include <iostream>
#include <cstdlib>
#include <string>
#include "PuyoTsumo.h"
#include "Simulator.h"
#include "GameInfo.h"
#include "AI.h"
#include <random>
#include <ctime>
#include <vector>
#include <omp.h>

#include "PuyoConst.h"
#include "GameInfo.h"
#include "Simulator.h"
#include "Detected.h"
#include "Detector.h"
#include "Plan.h"
#include "CrossFunc.h"
#include "PuyoPattern.h"
#include "NeuralNet.h"

using std::cout;
using std::cout;
using std::endl;
using std::vector;
using std::swap;
using std::to_string;

class DlAI : public AI {
private:
	PuyoTsumo _bag[128];
	int _bagIndex = 0;
	int _input;
	int _maxDepth = 1;
	int _maxNode = 250000;
	int _nodeCnt;
	float _r;
	string _debug = "";

	int A = 0;
	int B = 0;
	int C = 0;
	int D = 0;
	bool _init = true;
	int _initCnt = 0;
	int _cantSeeColor = -1;//게임중 보이지 않는 컬러 5색중 안나오는 1색


public:
	NeuralNet* _nn = NULL;
	DlAI();
	void randParams();
	DlAI(DlAI* fa);
	void Learn(GameInfo g, vector<float> answer);
	void LearnEnd();
	void setRandom(float value);
	string getDebugStr();
	float testScoreOpp(Simulator s, GameInfo g, int loop, int depth, string& debug, PuyoTsumo next);
	void reset();
	int getBestMove(GameInfo g, int depth = 0);
	float getFieldScore(GameInfo g);
	void getPI(vector<float>& r);
	int think(GameInfo g);
	PuyoTsumo* createVirtualBag();
	int input();
	void setDepth(int x);
	float testScore(Simulator s, GameInfo g, int loop, int depth, string& debug, PuyoTsumo next = PuyoTsumo(0, 0));
	float testNoThink(Simulator s, GameInfo g, int loop, int depth, string& debug, PuyoTsumo next = PuyoTsumo(0, 0));
	void save();
	void load();

	//next[0] = now puyo
	//next[1] = right side next queue's first puyo
};
