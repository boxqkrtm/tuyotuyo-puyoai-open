#include <iostream>
#include <random>
#include <ctime>
#include <vector>
#include <string>
#include <omp.h>
#include <queue>
//
#include "PPTAI.h"
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

PPTAI::PPTAI()
{
	reset();
}

PPTAI::PPTAI(PPTAI* fa)
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

void PPTAI::reset()
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

int PPTAI::think(GameInfo g) {
	int i, j;
	if (AI_SEE) see(g);//응시

	Simulator s = Simulator(-1);

	//read bag color and write to table
	if (s.getAllClear()) {
		_init = true;
		_initCnt = 0;
		A = 0;
		B = 0;
		C = 0;
		D = 0;
		int* ABCD[4] = { &A,&B,&C,&D };
		int nexts[4] = { g._myNext[0].up, g._myNext[0].down, g._myNext[1].up, g._myNext[1].down };
		//A 같은거 2개 이상 
		int table[6] = { 0 };
		for (int j = 0; j < 4; j++) {
			table[nexts[j]] += 1;
		}
		for (int j = 1; j < 6; j++) {
			if (table[j] >= 2) {
				A = j;
				break;
			}
		}
		for (int j = 0; j < 4; j++) {
			for (int i = 1; i < 4; i++) {
				//abcd가 비었으며  
				if (*ABCD[i] == 0) {
					//기존 abcd에 없으면 채용
					bool no = false; //기존에 없던색이면 true
					for (int k = 0; k < i; k++) {
						if (*ABCD[k] == nexts[j]) {
							no = true;
							break;
						}
					}
					if (no == true) {
						//기존에 있던 색이면 다음 색/으로 넘어감
						break;
					}
				}
			}
		}
	}
	//D도 채워주기 
	else if (D == 0) {
		int* ABCD[4] = { &A,&B,&C,&D };
		int nexts[4] = { g._myNext[0].up, g._myNext[0].down, g._myNext[1].up, g._myNext[1].down };
		for (int j = 1; j < 4; j++) {
			for (int i = 1; i < 4; i++) {
				//abcd가 비었으며  
				if (*ABCD[i] == 0) {
					//기존 abcd에 없으면 채용
					bool no = false; //기존에 없던색이면 true
					for (int k = 0; k < i; k++) {
						if (*ABCD[k] == nexts[j]) {
							no = true;
							break;
						}
					}
					if (no == true) {
						//기존에 있던 색이면 다음 색으로 넘어감
						break;
					}
				}
			}
		}
		//보이지 않는 색 메모
		int table[6] = { 0, };
		table[A] = 1;
		table[B] = 1;
		table[C] = 1;
		table[D] = 1;
		for (int i = 0; i < 6; i++) {
			if (table[i] == 0) {
				_cantSeeColor = i;
			}
		}
	}
	//check able to input
	s.reset(0);
	s.set(g._myField, g._my14remove);
	if (s.applyGravityOnce() > 0) return _input;
	//{
	//	//calculate now maxrensa;
	//	_beforeMaxrensa = 0;
	//	Detector det = Detector();
	//	vector<Detected> dtt = det.detect(g._myField);
	//	dtt.reserve(10);
	//	for (int i = 0; i < dtt.size(); i++) {
	//		_beforeMaxrensa = dtt[i]._rensa > _beforeMaxrensa ? dtt[i]._rensa : _beforeMaxrensa;
	//	}
	//}

	if (_mode == MODE_SAVE) {
		ai_flat_field_score = AI_FLAT_FIELD_SCORE;
	}
	else if (_mode == MODE_COUNTER) {
		ai_flat_field_score = AI_FLAT_FIELD_INCOMMING_SCORE;
	}

	int loop;
	int cnt[ALL_MOVEMENT_MAX];
	for (int i = 0; i < ALL_MOVEMENT_MAX; i++)
		cnt[i] = 0;
	float score1[ALL_MOVEMENT_MAX] = { 0 };
	float score[ALL_MOVEMENT_MAX] = { 0 };
	int scoreMaxIndex = 0;
	string debug[ALL_MOVEMENT_MAX];

	queue<SimulatorNode> result;
	_nodeCnt = 0;
	//depth0
	queue<SimulatorNode> q;
	_beforeMaxrensa = 0;
	_beforeMaxS.reset(-1);
	//#pragma omp parallel for
	for (loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {//ALL_MOVEMENT_MAX bfs
		Simulator s(-1);
		s.set(g._myField, g._my14remove);
		int afterRensa = 1;
		score1[loop] = testScore(s, g, loop, 0, _enermyScore, debug[loop], _beforeScore, afterRensa);
		//score[loop] = score1[loop];
		if (score1[loop] < AI_GAMEOVER_SCORE || s.getAllClear()) {
			//gameover or cantmove
		}
		else {
#pragma omp critical
			{
				if (afterRensa > 0 || g._myOjama != 0) {
					auto nd = SimulatorNode(s, loop);
					nd.evalScore = score1[loop];
					result.push(nd);
				}
				else
					q.push(SimulatorNode(s, loop));
			}
		}
#pragma omp critical
		{
			_nodeCnt++;
		}
	}

	if (g._myOjama == 0) {
		queue<SimulatorNode> q1;
		//#pragma omp parallel for
		while (q.empty() == false) {
			const auto& i = q.back();
			for (loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {//ALL_MOVEMENT_MAX bfs
				Simulator s(i._s);
				int afterRensa = 1;
				score1[loop] = testScore(s, g, loop, 1, _enermyScore, debug[loop], _beforeScore, afterRensa);
				//score[loop] = score1[loop];
				if (score1[loop] < AI_GAMEOVER_SCORE || s.getAllClear()) {
					//gameover or cantmove
				}
				else {
#pragma omp critical
					{
						if (afterRensa > 0) {
							auto nd = SimulatorNode(s, loop);
							nd.evalScore = score1[loop];
							result.push(nd);
						}
						else
							q1.push(SimulatorNode(s, i._startMove));
					}
				}
#pragma omp critical
				{
					_nodeCnt++;
				}
			}
			q.pop();
		}


		queue<SimulatorNode> q2;
		//#pragma omp parallel for
		while (q1.empty() == false) {
			const auto& i = q1.back();
			for (loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {//ALL_MOVEMENT_MAX bfs
				Simulator s(i._s);
				int afterRensa = 1;
				score1[loop] = testScore(s, g, loop, 2, _enermyScore, debug[loop], _beforeScore, afterRensa);
				//score[loop] = score1[loop];
				if (score1[loop] < AI_GAMEOVER_SCORE || s.getAllClear()) {
					//gameover or cantmove
				}
				else {
#pragma omp critical
					{
						auto nd = SimulatorNode(s, loop);
						nd.evalScore = score1[loop];
						result.push(nd);
					}
				}
#pragma omp critical
				{
					_nodeCnt++;
				}
			}
			q1.pop();
		}
	}

	int max = AI_CANT_MOVE_SCORE-1;
	int maxIndex = -1;
	while (result.empty()) {
		const auto& i = result.back();
		if (max < i.evalScore) {
			maxIndex = i._startMove;
			max = i.evalScore;
		}
		result.pop();
	}
	if (maxIndex == -1) return 2;
	return ALL_MOVEMENT[maxIndex];

	//cout << _nodeCnt << endl;
	//cout << "max rensa " << _beforeMaxrensa;
	//get max socre index
	float scoreMax = AI_CANT_MOVE_SCORE;
	for (loop = 0; loop < ALL_MOVEMENT_MAX; loop++)
		if (cnt[loop] != 0)
			score[loop] = score[loop] / cnt[loop];

	for (loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {
		if (score[loop] > scoreMax) {
			scoreMax = score[loop];
			scoreMaxIndex = loop;
			_debug = debug[loop] + " node: " + to_string(_nodeCnt);
		}
	}
	_beforeScore = scoreMax;
	_input = ALL_MOVEMENT[scoreMaxIndex];
	//cout << "score max " << scoreMax << endl;
	s.set(g._myField, g._my14remove);
	if (s.pushFast(_input, PuyoTsumo(g._myNext[0].up, g._myNext[0].down))) {
		if (_print) {
			gotoxy(0, 0);
			s.set(g._myField, g._my14remove);
			int(*sField)[6] = (int(*)[6])s.getField();
			int(*ssField)[6] = (int(*)[6])_beforeMaxS.getField();
			for (int i = 0; i < 13; i++) {
				for (int j = 0; j < 6; j++) {
					if (sField[i][j] == ssField[i][j]) {
						ssField[i][j] = 0;
					}
				}
			}
			_beforeMaxS.print();
			cout << _beforeMaxrensa << endl;
			for (loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {
				if (loop == scoreMaxIndex) {
					cout << "[" << score[loop] << "]" << " ";
				}
				else {
					cout << score[loop] << " ";
				}
			}
		}
		return _input;
	}
	else {
		return 2;
	}
}

void PPTAI::see(GameInfo g)
{
	Simulator s = Simulator(-1);
	bool isEnermyAttack = false;
	int rensa = 1;
	int score = 0;
	s.reset(0);
	s.set(g._oppField);
	s.forceFire();
	if (s.getScore() > 0) isEnermyAttack = true;
	if (g._myOjama > 0) isEnermyAttack = true;

	//enermy attack check
	if (isEnermyAttack == true && _enermyFullRensa == 0) {
		//catch full rensa size
		int rensa = 1;
		int score = 0;
		s.reset(0);
		s.set(g._oppField);
		s.forceFire(rensa, score);
		_enermyFullRensa = rensa;
		_enermyScore = score;
		if (_print)
			cout << "enermy rensa " << rensa << endl;
		if (rensa == 0) {
			_enermyFullRensa = 0;
		}
	}
	if (isEnermyAttack == true && _enermyFullRensa != 0) {
		int remainRensa = 1;
		int temp = 0;
		s.reset(0);
		s.set(g._oppField);
		s.forceFire(remainRensa, temp);// get remain rensa
		if (remainRensa <= 1 && _enermyFullRensa > 1) {//counter
			_isIncomming = true;
			_mode = MODE_SAVE;
		}
		else {
			_mode = MODE_COUNTER;
		}
		if (remainRensa == 0) {
			_enermyFullRensa = 0;
		}
	}
	else if (isEnermyAttack == false) {//normal
		_enermyFullRensa = 0;
		_enermyScore = 0;
		_isIncomming = false;
		_mode = MODE_SAVE;
	}
	if (g._oppAllClear == true) {//counter
		_mode = MODE_COUNTER;
	}
	//calculate oppnent maxscore
	{
		Detector det = Detector();
		_opponentMaxScore = 0;
		_opponentMaxRensa = 0;
		int b = 0;
		vector<Detected> dtt = det.detectMore(g._oppField, b);
		dtt.reserve(10);
		for (int i = 0; i < dtt.size(); i++) {
			_opponentMaxScore = dtt[i]._score > _opponentMaxScore ? dtt[i]._score : _opponentMaxScore;
			_opponentMaxRensa = dtt[i]._rensa > _opponentMaxRensa ? dtt[i]._rensa : _opponentMaxRensa;
		}
	}
}

int PPTAI::input()
{
	return _input;
}

void PPTAI::setDepth(int x)
{
	//_maxDepth = x;
}

void PPTAI::setNextCount(int x)
{
	//넥스트 개수 설정
	//_lastNextDepth = x;
}

void PPTAI::connectScoreAppend(int& i, int& j, int(*sf)[6], Simulator& s, float& score)
{
	bool visit[12][6];
	for (i = 0; i < 12; i++) {
		for (j = 0; j < 6; j++) {
			visit[i][j] = false;
		}
	}
	std::vector<std::vector<std::pair<int, int>>> popGroups;
	popGroups.reserve(10);
	std::vector<std::pair<int, int>> popGroup;
	popGroup.reserve(50);
	for (i = 1; i < 13; i++) {
		for (j = 0; j < 6; j++) {
			if (visit[i - 1][j] == false
				&& (!(sf[i][j] == 6 || sf[i][j] == 0))) {
				popGroup.clear();
				s.dfs(sf, i, j, sf[i][j], visit, popGroup);
				if ((int)popGroup.size() >= 2) {
					popGroups.push_back(popGroup);
				}
			}
		}
	}
	for (i = 0; i < popGroups.size(); i++) {
		switch ((int)popGroups[i].size()) {
		case 1:
			//score -= 50000;
			//score -= AI_STACK_RENSA_SCORE;
			break;
		case 2:
			score += AI_CONNECT_2_SCORE;
			break;
		case 3:
			score += AI_CONNECT_3_SCORE;
			break;
		}
	}
}

float PPTAI::testScore(Simulator& s, GameInfo g, int loop, int depth, int enermyScore, string& debug, float befScore, int& afterRensa, PuyoTsumo next) {
	if (depth == 0) debug = "";
	float score = 0;
	int i, j;
	int afterScore = 0; //drop after score;
	if (next.down == 0 && next.up == 0)
		next = g._myNext[depth];
	int(*sf)[6] = (int(*)[6])s.getField();
	Simulator backS(s);
	Simulator oppS(-1);
	oppS.set(g._oppField, g._opp14remove);
	if (s.pushFast(ALL_MOVEMENT[loop], next, afterRensa, afterScore) == true) {
		s.ojamaInput(g._myOjama - afterScore / 70);
		score -= s.getFieldHeight()[0] * 5;
		score -= s.getFieldHeight()[1] * 5;
		score -= s.getFieldHeight()[2] * 5;
		score -= s.getFieldHeight()[3] * 3;
		score -= s.getFieldHeight()[4] * 2;
		score -= s.getFieldHeight()[5] * 1;
		Simulator a = Simulator(s);
		if(g._myOjama==0)
		//a.ojamaInput(6 * 1, true);
		a.applyOjamaDrop();
		s.applyOjamaDrop();
		score += stackRensaScore(a, g);
		if (depth == 0) {
			debug = "stacking for more rensa now ";
			debug += "score  ";
			debug += to_string(_beforeMaxrensa);
			debug += "rensa\n";
		}
		if (s.getGameover() == true) {
			return AI_GAMEOVER_SCORE;
		}
		//패턴점수

		//올클리어
		//score +=
		if (s.getAllClear()) {
			return AI_ALLCLEAR_SCORE;
		}
		//공격력 점수 계산

		int myFieldOjama = s.getFieldOjama();
		int myFieldSpace = s.getFieldSpace();//가용공간
		score += (float)ai_flat_field_score * -(float)s.getFieldUnstable();
		//뭉침점수
		//if (s.getFieldSpace() < 6 * 12) {
		connectScoreAppend(i, j, sf, s, score);
		//}
		score += (float)s.getLatestChigiriLength() * AI_CHIGIRI_SCORE;


		//float rensaScore = stackRensaScore(s, g);;
		if (s.getAllClear())
			score += AI_ALLCLEAR_SCORE;
		//score += rensaScore;

			if (afterRensa > 0) {
				if (Detector::ScoreToRensa(afterScore) >= AI_LARGE_ENOUGH_CHAIN) {
					score += AI_SCORE_MAX;
				}
				if (backS.getFieldOjama() - s.getFieldOjama() > 0) {
						return AI_SCORE_MAX;
					}
					else {
					score -= AI_STACK_RENSA_SCORE;

					}
			}
			else {
				//score += rensaScore;
			}
		
		return score;
	}
	else {
		//cant move
		return AI_CANT_MOVE_SCORE;
	}
}
float PPTAI::stackRensaScore(Simulator s, GameInfo g)
{
	float score = 0;
	float maxrensa = 0;
	int maxCnt = 0;
	Detector det = Detector();
	vector<Detected> dtt;
	dtt.reserve(10);
	//dtt = det.detectMoreRand(s.getField(), s.getFieldSpace()/2, 0, g._myNext[1]);
	int b = 0;
	float va = 0;
	float vb = 0;
	float r1 = 0;
	b = 0;
	float r2 = 0;
	dtt = det.detectMore(s.getField(), b, 2, 0, g._myNext[1]);
	for (int i = 0; i < dtt.size(); i++) {
		if (dtt[i]._rensa > maxrensa) {
			maxrensa = Detector::ScoreToRensa(dtt[i]._score);
			//maxrensa = dtt[i]._rensa;
			maxCnt = 1;
			if (_beforeMaxrensa < maxrensa) {
				_beforeMaxrensa = maxrensa;
				_beforeMaxS = dtt[i]._backup;
			}
		}
		else if (dtt[i]._rensa == maxrensa) {
			maxCnt++;
		}
	}
	va = (maxrensa) * (AI_STACK_RENSA_SCORE);
	maxrensa = 0;
	dtt = det.detectMore(s.getField(), b, 1, 0, g._myNext[1]);
	for (int i = 0; i < dtt.size(); i++) {
		if (dtt[i]._rensa > maxrensa) {
			maxrensa = Detector::ScoreToRensa(dtt[i]._score);
			//maxrensa = dtt[i]._rensa;
			maxCnt = 1;
			if (_beforeMaxrensa < maxrensa) {
				_beforeMaxrensa = maxrensa;
				_beforeMaxS = dtt[i]._backup;
			}
		}
		else if (dtt[i]._rensa == maxrensa) {
			maxCnt++;
		}
	}
	vb = (maxrensa) * (AI_STACK_RENSA_SCORE);
	if (va > vb) {
		return va;
	}
	return vb;
}
string PPTAI::getDebugStr()
{
	return _debug;
}

int PPTAI::getMaxRensa()
{
	return _beforeMaxrensa;
}

void PPTAI::getParams()
{
	cout << "param start" << endl;
	cout << "int AI_FLAT_FIELD_SCORE = " << AI_FLAT_FIELD_SCORE << ";" << endl;
	cout << "int AI_CONNECT_3_SCORE = " << AI_CONNECT_3_SCORE << ";" << endl;
	cout << "int AI_CONNECT_2_SCORE = " << AI_CONNECT_2_SCORE << ";" << endl;
	cout << "int AI_STACK_RENSA_SCORE = " << AI_STACK_RENSA_SCORE << ";" << endl;
	//cout << "int AI_CHIGIRI_SCORE = " << AI_CHIGIRI_SCORE << ";" << endl;
}

void PPTAI::randParams()
{
	//cout << "rand!!" << endl;
	//+- 0~10
	srand((int)time(NULL));
	AI_FLAT_FIELD_SCORE = rand() % 100;
	AI_CONNECT_3_SCORE = rand() % 100;
	AI_CONNECT_2_SCORE = rand() % 100;
	AI_STACK_RENSA_SCORE = rand() % 100;
	//AI_CHIGIRI_SCORE = -rand() % 100;
}

void PPTAI::saveParams()
{
	_AI_FLAT_FIELD_SCORE = AI_FLAT_FIELD_SCORE;
	_AI_FLAT_FIELD_INCOMMING_SCORE = AI_FLAT_FIELD_INCOMMING_SCORE;
	_AI_SENT_ATTACK_SCORE = AI_SENT_ATTACK_SCORE;
	_AI_CONNECT_3_SCORE = AI_CONNECT_3_SCORE;
	_AI_CONNECT_2_SCORE = AI_CONNECT_2_SCORE;
	_AI_ALLCLEAR_SCORE = AI_ALLCLEAR_SCORE;
	_AI_STACK_RENSA_SCORE = AI_STACK_RENSA_SCORE;
	_AI_COUNTER_3COL_SCORE = AI_COUNTER_3COL_SCORE;
	_AI_CHIGIRI_SCORE = AI_CHIGIRI_SCORE;
}
