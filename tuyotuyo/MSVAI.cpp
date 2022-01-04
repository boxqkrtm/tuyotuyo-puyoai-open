#include "MSVAI.h"
#include <iostream>
#include <random>
#include <ctime>
#include <vector>
#include <string>
#include <omp.h>
#include <queue>
#include <math.h>
#include <algorithm>
//
#include "PuyoConst.h"
#include "GameInfo.h"
#include "Simulator.h"
#include "Detected.h"
#include "Detector.h"
#include "Plan.h"
#include "CrossFunc.h"
#include "PuyoPattern.h"

using std::cout;
using std::endl;
using std::vector;
using std::swap;
using std::to_string;

MSVAI::MSVAI()
{
	reset();
}

MSVAI::MSVAI(MSVAI* fa)
{
	_input = fa->_input;
	_beforeMaxrensa = fa->_beforeMaxrensa;
	_opponentMaxScore = fa->_opponentMaxScore;
	_isIncomming = fa->_isIncomming;
	_opponentMaxRensa = fa->_opponentMaxRensa;
	_myFieldOjama = fa->_myFieldOjama;
	_enermyFullRensa = fa->_enermyFullRensa;
	_enermyScore = fa->_enermyScore;
	_beforePlanRensa = fa->_beforePlanRensa;
	_targetScore = fa->_targetScore;
	_targetScoreBefore = fa->_targetScoreBefore;
	_targetFailCount = fa->_targetFailCount;
	_mode = fa->_mode;
	_targetFieldCompleteFloor = fa->_targetFieldCompleteFloor;
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 6; j++) {
			_targetField[i][j] = fa->_targetField[i][j];
			_targetFieldComplete[i][j] = fa->_targetFieldComplete[i][j];
		}
	}
	for (int i = 0; i < 5; i++)_colorTable[i] = fa->_colorTable[i];
	_colorTableCount = fa->_colorTableCount;
	AI_FLAT_FIELD_SCORE = fa->AI_FLAT_FIELD_SCORE;
	AI_FLAT_FIELD_INCOMMING_SCORE = fa->AI_FLAT_FIELD_INCOMMING_SCORE;
	AI_SENT_ATTACK_SCORE = fa->AI_SENT_ATTACK_SCORE;
	AI_CONNECT_3_SCORE = fa->AI_CONNECT_3_SCORE;
	AI_CONNECT_2_SCORE = fa->AI_CONNECT_2_SCORE;
	AI_ALLCLEAR_SCORE = fa->AI_ALLCLEAR_SCORE;
	AI_STACK_RENSA_SCORE = fa->AI_STACK_RENSA_SCORE;
	AI_COUNTER_3COL_SCORE = fa->AI_COUNTER_3COL_SCORE;
	AI_CHIGIRI_SCORE = fa->AI_CHIGIRI_SCORE;
}

void MSVAI::reset()
{
	_init = true;
	_cantSeeColor = -1;
	_input = -1;
	_beforeMaxrensa = 0;
	_opponentMaxScore = 0;
	_isIncomming = false;
	_opponentMaxRensa = 0;
	_myFieldOjama = 0;
	_enermyFullRensa = 0;
	_enermyScore = 0;
	_beforePlanRensa = -1;
	_targetScore = 0;
	_targetScoreBefore = 0;
	_targetFailCount = 0;
	_mode = MODE_SAVE;
	_targetFieldCompleteFloor = 0;
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 6; j++) {
			_targetField[i][j] = 0;
			_targetFieldComplete[i][j] = false;
		}
	}
	for (int i = 0; i < 5; i++)_colorTable[i] = 0;
	_colorTableCount = 0;
}

int MSVAI::think(GameInfo g) {
	_beforeMaxrensa = 0;
	double score[ALL_MOVEMENT_MAX] = { 0 };
	double score1[ALL_MOVEMENT_MAX] = { 0 };
	string debug[ALL_MOVEMENT_MAX] = { "" };

	//depth0
	vector<SimulatorNode> q;
	_beforeMaxrensa = 0;
	_nodeCnt = 0;

	//#pragma omp parallel for
	for (int loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {//ALL_MOVEMENT_MAX bfs
		Simulator s(-1);
		s.set(g._myField, g._my14remove);
		int afterRensa = 1;
		double real = testScore(s, g, loop, 0, _enermyScore, debug[loop], _beforeScore, afterRensa);
		score1[loop] = real;
		score[loop] = Detector::ScoreToRensa(real);
		if (score1[loop] <= AI_GAMEOVER_SCORE) {
			//gameover or cantmove
		}
		else {
#pragma omp critical
			{
				q.push_back(SimulatorNode(s, loop));
			}
		}
#pragma omp critical
		{
			_nodeCnt++;
		}
	}

	
	vector<double> scorev;
	scorev.assign(score1, score1 + 22);
	std::sort(scorev.begin(), scorev.end(), std::greater<int>());
	//beam search
	for (int i = q.size()-1; i >= 0; i--) {
		if (score1[q[i]._startMove] < scorev[3]) {
			q.erase(q.begin()+i);
		}
	}
	

	//depth1
	if (_isIncomming == false ) {
		while (!q.empty()) {
			const auto& n = q.back();
			//#pragma omp parallel for
			for (int loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {
				Simulator s = n._s;
				int afterRensa = 1;
				double getScore = testScore(s, g, loop, 1, _enermyScore, debug[loop], _beforeScore, afterRensa);
				getScore = Detector::ScoreToRensa(getScore + score1[n._startMove]);
				#pragma omp critical
				if (score[n._startMove] < getScore) {
					score[n._startMove] = getScore;
				}
#pragma omp critical
				{
		_nodeCnt++;
		}
			}
			q.pop_back();
		}
	}

	double scoreMax = -999999;
	int scoreMaxIndex = 0;
	for (int loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {
		if (score[loop] > scoreMax) {
			scoreMax = score[loop];
			scoreMaxIndex = loop;
			_debug = "node" + to_string(_nodeCnt) +" 점수 " + to_string(scoreMax) + "\n" + debug[loop]+"\n\n\n\n\n";
		}
	}
	//for (int loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {
	//	if (loop == scoreMaxIndex) {
	//		cout << "[" << score[loop] << "]" << " ";
	//	}
	//	else {
	//		cout << score[loop] << " ";
	//	}
	//}

	return ALL_MOVEMENT[scoreMaxIndex];
}

int MSVAI::input()
{
	return _input;
}

double MSVAI::testScore(Simulator& s, GameInfo g, int loop, int depth, int enermyScore, string& debug, int befScore, int& afterRensa, PuyoTsumo next) {
	if (depth == 0) debug = "";
	int i, j;
	int afterScore = 0; //drop after score;
	bool isFire = false;
	if (next.down == 0 && next.up == 0)
		next = g._myNext[depth];
	int(*sf)[6] = (int(*)[6])s.getField();
	Simulator backS(s);
	Simulator oppS(-1);
	oppS.set(g._oppField, g._opp14remove);
	if (s.pushFast(ALL_MOVEMENT[loop], next, afterRensa, afterScore) == true) {
			double score = 0;
			if(s.getGameover()) return -999998;
			Detector d;
			Simulator ori = Simulator(-1);
			ori.set(g._myField);
			score = d.detectMoreMoreMSV(s, ori);
			return Detector::RensaToScore(afterRensa) + Detector::RensaToScore(score) + (s.getAllClear() == true? 2100. : 0.);
	}
	else {
		//cant move
		return -999999;
	}
}

string MSVAI::getDebugStr()
{
	return _debug;
}
