#pragma once
#include <iostream>
#include <string>
#include <random>
#include <ctime>
#include <vector>
#include <string>

#include "PuyoTsumo.h"
#include "SimulatorNode.h"
#include "Simulator.h"
#include "GameInfo.h"
#include "Detector.h"
#include "AI.h"
#include "PuyoConst.h"
#include "GameInfo.h"
#include "Detected.h"
#include "Detector.h"
#include "Plan.h"
#include "CrossFunc.h"
#include "PuyoPattern.h"
#include <map>
#include <unordered_map>

using std::cout;
using std::endl;
using std::vector;
using std::swap;
using std::to_string;
//using std::sort;

/*.class OpenBook {
public:
	OpenBook(OpenBook** d, int** c) {
		next = d;
		input = c;
	};
	OpenBook() {
	};
	OpenBook** next = NULL; //다음 필드
	int** input = NULL;//다음필드로 가기위한 조작
};*/

class FormAIv2 : public AI {
private:
	//params
	int _AI_FLAT_FIELD_SCORE;
	int _AI_FLAT_FIELD_INCOMMING_SCORE;
	int _AI_CONNECT_3_SCORE;
	int _AI_CONNECT_2_SCORE;
	int _AI_ALLCLEAR_SCORE;
	int _AI_STACK_RENSA_SCORE;
	int _AI_COUNTER_3COL_SCORE;
	int _AI_CHIGIRI_SCORE;
	int AI_MAX_TARGET_FAIL_COUNT = 1;//5

	int AI_FLAT_FIELD_INCOMMING_SCORE = 0;
	int AI_CONNECT_3_INCOMMING_SCORE = 12;
	int AI_ALLCLEAR_SCORE = 1000;
	int AI_COUNTER_3COL_SCORE = -1000;


	/*max 14rensa
	int AI_FLAT_FIELD_SCORE = 5;//5
	int AI_CONNECT_3_SCORE = 3;
	int AI_CONNECT_2_SCORE = 3;
	int AI_STACK_RENSA_SCORE = 99;
	*/

	//
	float AI_LARGE_ENOUGH_CHAIN = 14;
	float AI_CANT_MOVE_SCORE = -100000000000.f;
	float AI_GAMEOVER_SCORE = -1000000000.f;
	float AI_SCORE_MAX = 1000000000;

	bool _isIncomming = false;
	int const MODE_SAVE = 0;
	int const MODE_INIT = 1;
	int const MODE_STACK = 2;
	int const MODE_COUNTER = 3;
	int _input;
	int _mode = MODE_SAVE;
	int _nodeCnt;
	float _beforeMaxrensa = 0;
	int _isEnermyAttack;
	Simulator _beforeMaxS = Simulator(-1);
	int _opponentMaxScore = 0;
	int _opponentMaxRensa = 0;
	int _myFieldOjama = 0;
	int _myFieldSpace = 0;
	int _enermyFullRensa = 0;
	int _enermyScore = 0;
	int _beforeScore = 0;
	bool _enermyShortAttack = false;
	bool isEnermyAttack = false;
	int A = 0;
	int B = 0;
	int C = 0;
	int D = 0;
	bool _init = true;
	int _initCnt = 0;
	int _cantSeeColor = -1;//게임중 보이지 않는 컬러 5색중 안나오는 1색

	//for plan
	int _beforePlanRensa = 0;
	int _befOppOjama = 0;
	int _colorTable[5];
	int _colorTableCount;
	int _noRensaNoiseCnt = 0;
	bool _isNoKey = false;
	long _stime;

	//puyo pattern to color
	string _debug;
	vector<SimulatorNode> q;
	vector<SimulatorNode> q2;

	float rensaCache[1000000] = { 0 };

public:
	float param[9] = { 3.22,86.51,0.21,56.99,35.57,84.76,78.92,43.89,50.75 };
	bool _multi = true;
	bool AI_SURPRISE_ATTACK = true;
	bool AI_SEE = true;
	bool _print = false;

	int _remainRensa;
	int  _maxDepth = 3;
	int _targetField[13][6];
	bool _targetFieldComplete[13][6];
	int _targetFieldCompleteFloor;
	int _targetScoreBefore;
	int _targetFailCount;
	int _targetScore;
	vector<PuyoTsumo> _vbag;
	int _bagindex = 0;
	FormAIv2();
	FormAIv2(FormAIv2* fa);
	void reset();
	int think(GameInfo g);
	static void thinkthread(float scores[], GameInfo g, int loop, int depth, int _enermyScore, string* debug, int _opponentMaxRensa, int _opponentMaxScore, int _remainRensa, vector<PuyoTsumo> _vbag, bool _enermyShortAttack);
	void see(GameInfo g);
	int input();
	void setDepth(int x);
	void setNextCount(int x);
	float connectScore(Simulator s, GameInfo g);
	SimulatorNode testScore(Simulator& s, GameInfo g, int loop, int depth, int enermyScore, string* debug, float befScore, int& afterRensa, int _opponentMaxRensa, int _opponentMaxScore, int _remainRensa, vector<PuyoTsumo> _vbag, bool _enermyShortAttack, PuyoTsumo next = PuyoTsumo(0, 0));
	float stackRensaScore(Simulator s, GameInfo g, int maxDepth, bool rush = false);
	string getDebugStr();
	int getMaxRensa();
	void getParams();
	void randParams();
	void saveParams();
	void revertParams();
	void permutation(int arr[4], int start, int end, vector<vector<int>>& result);
	//next[0] = now puyo
	//next[1] = right side next queue's first puyo
};
