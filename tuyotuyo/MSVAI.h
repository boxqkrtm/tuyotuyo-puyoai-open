#pragma once
#include <iostream>
#include <cstdlib>
#include <string>
#include "PuyoTsumo.h"
#include "Simulator.h"
#include "SimulatorNode.h"
#include "GameInfo.h"
#include "AI.h"
#include <list>

using std::cout;
using std::vector;
using std::pair;
using std::list;

class MSVAI : public AI {
private:
	//params
	int _AI_FLAT_FIELD_SCORE;
	int _AI_FLAT_FIELD_INCOMMING_SCORE;
	int _AI_SENT_ATTACK_SCORE;
	int _AI_CONNECT_3_SCORE;
	int _AI_CONNECT_2_SCORE;
	int _AI_ALLCLEAR_SCORE;
	int _AI_STACK_RENSA_SCORE;
	int _AI_COUNTER_3COL_SCORE;
	int _AI_CHIGIRI_SCORE;

	int AI_SENT_ATTACK_SCORE = 1;
	int AI_FLAT_FIELD_INCOMMING_SCORE = 0;
	int AI_CONNECT_3_INCOMMING_SCORE = 10;
	int AI_ALLCLEAR_SCORE = 1000000;
	int AI_COUNTER_3COL_SCORE = -10;
	int AI_CHIGIRI_SCORE = 0;

	int AI_FLAT_FIELD_SCORE = 5;//5
	int AI_CONNECT_3_SCORE = 3;
	int AI_CONNECT_2_SCORE = 3;
	int AI_STACK_RENSA_SCORE = 99;

	int AI_MAX_TARGET_FAIL_COUNT = 1;//5
	bool AI_SURPRISE_ATTACK = false;//on 30:50 //off 30:50
	bool AI_SEE = false;

	int AI_LARGE_ENOUGH_CHAIN = 7;

	int ai_flat_field_score = AI_FLAT_FIELD_SCORE;
	const int AI_CANT_MOVE_SCORE = -200000000;
	const int AI_GAMEOVER_SCORE = -100000000;
	const int AI_SCORE_MAX = 100000;

	bool _isIncomming = false;
	int const MODE_SAVE = 0;
	int const MODE_INIT = 1;
	int const MODE_STACK = 2;
	int const MODE_COUNTER = 3;
	int _input;
	//int _maxDepth =5;
	//int _lastNextDepth = 2; //depth2부터 넥스트 없음
	//int _maxNode = 1000;
	int _mode = MODE_SAVE;
	int _nodeCnt;
	int _beforeMaxrensa = 0;
	int _opponentMaxScore = 0;
	int _opponentMaxRensa = 0;
	int _myFieldOjama = 0;
	int _myFieldSpace = 0;
	int _enermyFullRensa = 0;
	int _enermyScore = 0;
	int _beforeScore = 0;
	bool _init = true;
	int _cantSeeColor = -1;//게임중 보이지 않는 컬러 5색중 안나오는 1색

	//for plan
	bool _mcts = true;
	int _beforePlanRensa = 0;
	int _colorTable[5];
	int _colorTableCount;

	//puyo pattern to color
	string _debug;
public:
	int _targetField[13][6];
	bool _targetFieldComplete[13][6];
	int _targetFieldCompleteFloor;
	int _targetScoreBefore;
	int _targetFailCount;
	int _targetScore;
	MSVAI();
	MSVAI(MSVAI* fa);
	void reset();
	int think(GameInfo g);
	int input();
	double testScore(Simulator& s, GameInfo g, int loop, int depth, int enermyScore, string& debug, int befScore, int& afterRensa, PuyoTsumo next = PuyoTsumo(0, 0));
	string getDebugStr();

	//next[0] = now puyo
	//next[1] = right side next queue's first puyo
};
