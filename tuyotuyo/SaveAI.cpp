#include "SaveAI.h"
#include "PuyoConst.h"
#include "GameInfo.h"
#include "Simulator.h"
#include "Detector.h"
#include <iostream>
#include <random>
#include <ctime>
#include <vector>

using std::cout;
using std::vector;

SaveAI::SaveAI()
{
	_input = -1;
}

int SaveAI::think(GameInfo g) {
	int i, loop;
	int score[ALL_MOVEMENT_MAX];
	int scoreMax = -999999999;
	int scoreMaxIndex = 2;
	{
		Simulator s = Simulator(-1);
		s.set(g._myField);
		if (s.applyGravityOnce() > 0) return _input;
	}
#pragma omp parallel for
	for (loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {//ALL_MOVEMENT_MAX
		Simulator s = Simulator(-1);
		s.set(g._myField);
#pragma omp critical
		{
		score[loop] = testScore(s, g, loop, 0);
		}
	}
	//

	for (i = 0; i < ALL_MOVEMENT_MAX; i++) {
		if (score[i] > scoreMax) {
			scoreMax = score[i];
			scoreMaxIndex = i;
		}
	}

	_input = ALL_MOVEMENT[scoreMaxIndex];
	return _input;
};

int SaveAI::input()
{
	return _input;
}

void SaveAI::setDepth(int x)
{
	_maxDepth = x;
}

int SaveAI::testScore(Simulator s, GameInfo g, int loop, int depth) {
	//parms
	const int AI_FLAT_FIELD_SCORE = 0;
	int AI_SENT_ATTACK_SCORE = 10;
	const int AI_CONNECT_3_SCORE = 30;
	const int AI_CONNECT_2_SCORE = 5;
	const int AI_ALLCLEAR_SCORE = 100000;
	int score = 0;
	int afterRensa = 1;
	int afterScore = 0;
	int i, j;

	Simulator backS(s);
	if (s.pushFast(ALL_MOVEMENT[loop], g._myNext[depth], afterRensa, afterScore) == true) {
		int rate = 70;
		long time = (clock() - _stime) / 1000;
		if (time >= 96) rate = 52;
		if (time >= 112)rate = 34;
		if (time >= 128)rate = 25;
		if (time >= 144)rate = 16;
		if (time >= 160) rate = 12;
		if (time >= 176) rate = 8;
		if (time >= 192)rate = 6;
		if (time >= 208) rate = 4;
		if (time >= 224) rate = 3;
		if (time >= 240)rate = 2;
		if (time >= 256) rate = 1;


		if(s.getAllClear())
		return AI_ALLCLEAR_SCORE;
		if (afterRensa > 0 && g._myAllClear) { s._score += 2100; }
		
		if (depth == 0) {
			int ojamas = g._myOjama;
			//if(s.getFieldSpace()-s.getFieldUnstable() <= 6*6)
				ojamas -= afterScore / rate;
				
			if (ojamas > 0) {
				ojamas = ojamas;
				s.ojamaInput(ojamas, true);
				s.applyOjamaDrop();
				s.applyOjamaDrop();
				s.applyOjamaDrop();
			}
			else if(ojamas <=0 && afterRensa == 0){
				s.ojamaInput(6, true);
				s.applyOjamaDrop();
			}
		}
		score += s.getFieldHeight()[2] >= 8 ? -1100*depth : 0;

		if (s.getGameover() == true) {
			return -999999998;
		}
		int(*sf)[6] = (int(*)[6])s.getField();
		//뭉침점수
		bool visit[12][6];
		for (i = 0; i < 12; i++) {
			for (j = 0; j < 6; j++) {
				visit[i][j] = false;
			}
		}
		std::vector<std::vector<std::pair<int, int>>> popGroups;
		popGroups.reserve(10);
		for (i = 1; i < 13; i++) {
			for (j = 0; j < 6; j++) {
				if (visit[i - 1][j] == false
					&& (!(sf[i][j] == 6 || sf[i][j] == 0))) {
					std::vector<std::pair<int, int>> popGroup;
					s.dfs(sf, i, j, sf[i][j], visit, popGroup);
					if ((int)popGroup.size() >= 2) {
						popGroups.push_back(popGroup);
					}
				}
			}
		}
		for (i = 0; i < popGroups.size(); i++) {
			switch ((int)popGroups[i].size()) {
			case 2:
				score += AI_CONNECT_2_SCORE;
				break;
			case 3:
				score += AI_CONNECT_3_SCORE;
				break;
			}
		}
		//방뿌 1개당 점수
		int attackScore = s.getScore();
		if(backS.getFieldSpaceMax1() <= 0.2 || g._myOjama > 0)
			score += (afterScore*100 / rate * AI_SENT_ATTACK_SCORE);
		else
			score += (afterScore / rate * AI_SENT_ATTACK_SCORE);

		//score += stackRensaScore(s, g, 1, false) / rate * AI_SENT_ATTACK_SCORE;
		//if (depth == 0)
		//if (afterRensa >= 5) return 9999999999+afterScore-depth*5000;
		score -= s.getFieldHeight()[0] * 1;
		score -= s.getFieldHeight()[1] * 2;
		score -= s.getFieldHeight()[2] * 3;
		score += s.getFieldHeight()[3] * 30;
		score += s.getFieldHeight()[4] * 31;
		score += s.getFieldHeight()[5] * 32;
		//if (s.getLatestChigiriLength() > 2) return score;
		//if (afterRensa > 0) return score;
	}
	else {
		//cant move
		return -999999999;
	}
	if (depth == _maxDepth-1) {
		return score;
	}
	else {
		int max = score;
		for (loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {
			auto ss =  Simulator(s);
			ss._score = s._score;
			int getScore = testScore(ss, g, loop, depth + 1);
			max = max < getScore ? getScore : max;
		}
		if(max <= -999999998) return -999999998;
		return max;
		//return max> score?max:score;
	}
}

float SaveAI::stackRensaScore(Simulator s, GameInfo g, int maxDepth, bool rush)
{
	float score = 0;
	float maxrensa = -99;
	int maxCnt = 0;
	int maxP = 0;
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
	b = 0;
	//if (_enermyShortAttack) {
		s.ojamaInput(6*2);
		s.applyOjamaDrop();
	//}
	dtt = det.detectMore(s.getField(), b, 1, 0, PuyoTsumo(0, 0));
	//dtt = det.detectMoreEx(s.getField(), b, maxDepth, 0, g._myNext[1], g._myNext[2], rush);
	for (int i = 0; i < dtt.size(); i++) {
		float nowmaxrensa = Detector::ScoreToRensa(dtt[i]._score);
		nowmaxrensa -= (4 - dtt[i]._p) * 0.25;
		if (dtt[i]._isUp == true) continue;
		auto fire = Simulator(dtt[i]._backup);
		fire.forceFire();
		if (nowmaxrensa > maxrensa) {
			maxCnt = 1;
			maxrensa = nowmaxrensa;
			maxP = dtt[i]._p;
		}
	}
	return Detector::RensaToScore(maxrensa);
}

void SaveAI::reset()
{
	_stime = clock();
}
