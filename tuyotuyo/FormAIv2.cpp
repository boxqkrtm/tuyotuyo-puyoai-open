#include "FormAIv2.h"

FormAIv2::FormAIv2()
{
	reset();
	q.reserve(10000);
	q2.reserve(10000);
}

FormAIv2::FormAIv2(FormAIv2* fa)
{
	_befOppOjama = 0;
	_enermyShortAttack = false;
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
}

void FormAIv2::reset()
{
	_enermyShortAttack = false;
	_isEnermyAttack = false;
	_bagindex = -1;
	_vbag.clear();
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
void FormAIv2::thinkthread(float scores[], GameInfo g, int loop, int depth, int _enermyScore, string* debug
	, int _opponentMaxRensa, int _opponentMaxScore, int _remainRensa, vector<PuyoTsumo> _vbag, bool _enermyShortAttack) {
}

int FormAIv2::think(GameInfo g) {
	Simulator sc(-1);
	sc.set(g._myField, g._my14remove);
	if (sc.ismemerror()) return -1;
	int i, j;
	if (AI_SEE) see(g);//응시

	Simulator s = Simulator(-1);

	_bagindex++;
	_vbag.push_back(g._myNext[0]);
	//read bag color and write to table
	if (_init == true) {
		_init = false;
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
	queue<int> a;
	int cnt[ALL_MOVEMENT_MAX];
	for (int i = 0; i < ALL_MOVEMENT_MAX; i++)
		cnt[i] = 0;
	float score[ALL_MOVEMENT_MAX] = { 0 };
	string debug[ALL_MOVEMENT_MAX];

	_nodeCnt = 0;
	//depth0
	if (s.getAllClear()) _mode = MODE_STACK;
	if (g._myOjama > 0 || g._oppOjama > 0) _mode = MODE_SAVE;

	_beforeMaxrensa = 0;
	_beforeMaxS.reset(-1);

	q.clear();
	int ncnt = 0;
	for (int depth = 0; depth < _maxDepth; depth++) {
		if (depth == 0) {
			//첫 노드
#pragma omp parallel for
			for (int loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {
				Simulator s(-1);
				s.set(g._myField, g._my14remove);
				int afterRensa = 1;
				float beforeScore = 0;
				auto sn = testScore(s, g, loop, depth, _enermyScore, &debug[loop], beforeScore, afterRensa, _opponentMaxRensa, _opponentMaxScore, _remainRensa, _vbag, _enermyShortAttack);
				sn._startMove = loop;
#pragma omp critical
				{
					q.push_back(sn);
				}
			}
		}
		else {
			//이후 노드
			q2.clear();
			//q2.reserve(10000);
			sort(q.begin(), q.end(), [](SimulatorNode a, SimulatorNode b) -> bool {return a.evalScore-(a.isSearched?100000:0) > b.evalScore - (b.isSearched ? 100000 : 0); });

			for (int i = 0; i < q.size(); i++) {
				auto& qq = q[i];
				if (i < 22) {
					
					if (qq.isSearched) {
#pragma omp critical
						{
							q2.push_back(qq);
						}
					}
					else {
#pragma omp parallel for
						for (int loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {
							Simulator s(qq._s);
							int afterRensa = 1;
							float beforeScore = qq.befScore;
							auto sn = testScore(s, g, loop, depth, qq._enermyScore, &debug[loop], beforeScore, afterRensa, _opponentMaxRensa, _opponentMaxScore, qq._remainRensa, _vbag, _enermyShortAttack);
							sn._startMove = qq._startMove;
#pragma omp critical
							{
								q2.push_back(sn);
							}
							ncnt++;
						}
					}
				}
#pragma omp critical
				{
					qq.isSearched = true;
					q2.push_back(qq);
				}
			}
			q = q2;
		}
	}
	//thread thinks[22];
	//int threadCnt = 22;
	//int threadp = 0;
	//for (; threadp < ALL_MOVEMENT_MAX; threadp += threadCnt) {
	//	for (int loop = threadp; (loop < threadp+threadCnt) && loop < ALL_MOVEMENT_MAX; loop++) {

	//		Simulator s(-1);
	//		s.set(g._myField, g._my14remove);
	//		int afterRensa = 1;
	//		int beforeScore = 0;
	//		thinks[loop] = thread(FormAIv2::testScore, );
	//	}
	//	for (int loop = threadp; (loop < threadp + threadCnt) && loop < ALL_MOVEMENT_MAX; loop++) {
	//		thinks[loop].join();
	//	}
	//}

//cout << _nodeCnt << endl;
//cout << "max rensa " << _beforeMaxrensa;
//get max socre index
	float scoreMax = AI_CANT_MOVE_SCORE;
	int scoreMaxIndex = 2;
	while (!q.empty()) {
		const auto& f = q.back();
		if (f.evalScore >= scoreMax) {
			if(f.evalScore == scoreMax){
				if(f._startMove < scoreMaxIndex)
					scoreMaxIndex = f._startMove;
			}
			else {
				scoreMax = f.evalScore;
				scoreMaxIndex = f._startMove;
			}
			
		}
		if (f.evalScore >= score[f._startMove]) {
			score[f._startMove] = f.evalScore;
		}
		q.pop_back();
	}
	//cout << scoreMax << endl;

	//for (int loop = 0; loop < ALL_MOVEMENT_MAX; loop++) {
	//	cout << score[loop] << " ";
	//}
	//cout << endl;


	_input = ALL_MOVEMENT[scoreMaxIndex];
	//cout << "score max " << scoreMax << endl;
	s.set(g._myField, g._my14remove);

	//_beforeMaxS.print();
	//system("pause");
	_debug = debug[scoreMaxIndex];
	if (_print) {
		//system("cls");
		cout << debug[scoreMaxIndex] << endl;
	}
	if (s.pushFast(_input, PuyoTsumo(g._myNext[0].up, g._myNext[0].down))) {
		if (true) {
			//gotoxy(0, 0);

			//s.set(g._myField, g._my14remove);
			//int(*sField)[6] = (int(*)[6])s.getField();
			//int(*ssField)[6] = (int(*)[6])_beforeMaxS.getField();
			//for (int i = 0; i < 13; i++) {
			//	for (int j = 0; j < 6; j++) {
			//		if (sField[i][j] == ssField[i][j]) {
			//			ssField[i][j] = 0;
			//		}
			//	}
			//}
			/*if (_beforeMaxrensa >= 7) {
				_beforeMaxS.print();
				system("pause");
			}
			*/
		}
		return _input;
	}
	else {
		return 2;
	}
}

void FormAIv2::see(GameInfo g)
{

	int rate = 70;
	long time = g._time;
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
	//if (_print)
	//	system("cls");
	Simulator s = Simulator(-1);

	//남은 연쇄수 계산
	_remainRensa = 1;
	int tmpscore = 0;
	s.set(g._oppField);
	if (!s.ismemerror()) {
		s.forceFire(_remainRensa, tmpscore);
	}
	

	//총 연쇄 수 계산

	if (_remainRensa > 0)
		_enermyScore = g._enermyScore;
	else {
		_enermyScore = g._myOjama * rate;
	}


	//연쇄 점수 계산
	//_enermyScore = (g._myOjama+g._oppOjama) * rate;

	//calculate oppnent maxscore
	Detector det = Detector();
	int b = 0;
	_enermyShortAttack = false;
	for (int j = 1; j < 6; j++) {
		for (int k = 1; k < 6; k++) {
			auto dtt = det.detectMore(s.getField(), b, 1, 0, PuyoTsumo(k, j));
			for (int i = 0; i < dtt.size(); i++) {
				float nowmaxrensa = Detector::ScoreToRensa(dtt[i]._score);
				nowmaxrensa = dtt[i]._rensa;
				if (_opponentMaxScore < dtt[i]._score) {
					_opponentMaxScore = dtt[i]._score;
					_opponentMaxRensa = dtt[i]._rensa;
				}
				if ((dtt[i]._rensa == 2 && dtt[i]._score >= rate * 6 * 3)) {
					_enermyShortAttack = true;
				}
			}
		}
	}
	
	auto dtt = det.detectMore(s.getField(), b, 1, 0, PuyoTsumo(0, 0));
	for (int i = 0; i < dtt.size(); i++) {
		float nowmaxrensa = Detector::ScoreToRensa(dtt[i]._score);
		nowmaxrensa = dtt[i]._rensa;
		if (_opponentMaxScore < dtt[i]._score) {
			_opponentMaxScore = dtt[i]._score;
			_opponentMaxRensa = dtt[i]._rensa;
		}
		if ((dtt[i]._rensa == 2 && dtt[i]._score >= rate * 6 * 3)) {
			_enermyShortAttack = true;
		}
	}

	if (_print) {
		//cout << "attack detect" << _remainRensa << "remain rensa" << " " << _enermyScore << " score" << endl;
		//cout << "opp max" << _opponentMaxRensa << endl;
		//if (_enermyShortAttack) {
		//	cout << "enermy has quick attack" << endl;
		//}
		////s.print();
	}

}

int FormAIv2::input()
{
	return _input;
}

void FormAIv2::setDepth(int x)
{
	//_maxDepth = x;
}

void FormAIv2::setNextCount(int x)
{
	//넥스트 개수 설정
	//_lastNextDepth = x;
}

float FormAIv2::connectScore(Simulator s, GameInfo g)
{
	int(*sf)[6] = (int(*)[6])s.getField();
	int i, j;
	float score = 0;
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
				if (popGroup.size() == 0) popGroup.push_back(std::pair<int, int>(i, j));
				if ((int)popGroup.size() >= 0) {
					popGroups.push_back(popGroup);
				}
			}
		}
	}
	int dx = 0;
	int dy = 0;
	bool leftb = false;
	bool rightb = false;
	bool upb = false;
	for (i = 0; i < popGroups.size(); i++) {
		int h[6] = { 0 };
		switch ((int)popGroups[i].size()) {
		case 1:
			//맨밑
			/*if (popGroups[i][0].first == 12)
				score -= 1;*/
			//if (popGroups[i][0].first == 12) {
			//	//좌우고립
			//	leftb = false;
			//	rightb = false;
			//	upb = false;
			//	if (!s.checkFieldIndex(popGroups[i][0].second, popGroups[i][0].first + 1)
			//		|| sf[popGroups[i][0].first + 1][popGroups[i][0].second] != 0) {
			//		leftb = true;
			//	}
			//	if (!s.checkFieldIndex(popGroups[i][0].second, popGroups[i][0].first - 1)
			//		|| sf[popGroups[i][0].first - 1][popGroups[i][0].second] != 0) {
			//		rightb = true;
			//	}
			//	if (!s.checkFieldIndex(popGroups[i][0].second-1, popGroups[i][0].first)
			//		|| sf[popGroups[i][0].first][popGroups[i][0].second-1] != 0) {
			//		upb = true;
			//	}
			//	if (leftb && rightb && upb) score -=3;
			//}
			break;
		case 2:
			for (int l = 0; l < 6; l++) h[l] = 0;
			dx = 0;
			dy = 0;
			for (j = 0; j < popGroups[i].size() - 1; j++) {
				dy += abs(popGroups[i][j].first - popGroups[i][j + 1].first);
				dx += abs(popGroups[i][j].second - popGroups[i][j + 1].second);
				h[popGroups[i][j].second]++;
			}
				score += 35;
			break;
		case 3:
			for (int l = 0; l < 6; l++) h[l] = 0;
			dx = 0;
			dy = 0;
			for (j = 0; j < popGroups[i].size() - 1; j++) {
				dy += abs(popGroups[i][j].first - popGroups[i][j + 1].first);
				dx += abs(popGroups[i][j].second - popGroups[i][j + 1].second);
				h[popGroups[i][j].second]++;
			}
			score += 70;
			break;
		}
	}
	return score;
}
SimulatorNode FormAIv2::testScore(Simulator& s, GameInfo g, int loop, int depth, int enermyScore, string* debug, float befScore, int& afterRensa, int _opponentMaxRensa, int _opponentMaxScore, int _remainRensa, vector<PuyoTsumo> _vbag, bool _enermyShortAttack, PuyoTsumo next) {
	int rate = 70;
	long time = g._time;
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

	*debug = "";
	float score = 0;
	int i, j;
	int afterScore = 0; //drop after score;
	if (next.down == 0 && next.up == 0)
		next = g._myNext[depth];
	int(*sf)[6] = (int(*)[6])s.getField();
	Simulator backS(s);
	Simulator oppS(-1);
	oppS.set(g._oppField, g._opp14remove);
	GameInfo gi = GameInfo(g);
	if (s.pushFast(ALL_MOVEMENT[loop], next, afterRensa, afterScore) == true) {
		//_remainRensa -= 1;
		s.applyOjamaDrop();
	float p = 0;
	p = s.getFieldUnstableSq() * 10;
	score -= p;
		//field score
		//befScore -= s.getFieldHeight()[1] >= 12 ? 100000 : 0;
		befScore -= s.getFieldHeight()[2] >= 11 ? 100000 : 0;
		//befScore -= s.getFieldHeight()[3] >= 12 ? 100000 : 0;
		//befScore -= s.getFieldHeight()[4] >= 12 ? 100000 : 0;
		befScore -= s.getLatestChigiriLength() * 10;
		//score -= s.getLatestChigiriLength() * 0.01;
		
		score += connectScore(s, g);

		int rscore = 0;
			score -= s.getFieldBelly() * 20;
		rscore = Detector::RensaToScore(stackRensaScore(s, g, 3, _enermyShortAttack));
		befScore += (backS.getFieldOjama() - s.getFieldOjama()) * 10;
		if (s.getGameover() == true) {
			*debug = "gameover";
			auto sn = SimulatorNode(s, loop);
			sn.evalScore = AI_GAMEOVER_SCORE;
			sn.isSearched = true;
			return sn;
		}

		if (s.getAllClear() && afterRensa <= 3 && AI_SURPRISE_ATTACK) {
			*debug = "all clear";
			befScore += AI_SCORE_MAX + afterScore - depth * 1000;
		}
		
		_remainRensa -= afterRensa;
		if (s.getLatestChigiriLength() > 4) _remainRensa -= 1;
		if (afterRensa > 0) _remainRensa -= afterRensa;
		//




		
		//공격력 점수 계산
		int myFieldOjama = s.getFieldOjama();
		int myFieldSpace = s.getFieldSpace();//가용공간
		int largee = AI_LARGE_ENOUGH_CHAIN;
		if (g._oppAllClear) {
			largee = 4;
			befScore -= s.getFieldHeight()[0] * 100;
			befScore -= s.getFieldHeight()[1] * 200;
			befScore -= s.getFieldHeight()[2] * 9999;
			befScore += s.getFieldHeight()[3] * 500;
			befScore += s.getFieldHeight()[4] * 209;
			befScore += s.getFieldHeight()[5] * 108;
			if (afterScore >= 2100) befScore += AI_SCORE_MAX;
			*debug = "stacking puyo counter";
		}
		else {
			*debug = "stacking puyo flat";
		}


		if (AI_SURPRISE_ATTACK && g._myAllClear && afterRensa > 0) {
			*debug = "ac attack";
			gi._myAllClear = false;
			befScore += AI_SCORE_MAX-depth * 1000;
		}

		//대응//대응
		if (afterRensa > 0) {
			*debug = "fire damage " + to_string(afterRensa - enermyScore);
		}
		if (AI_SEE && enermyScore > rate &&
			enermyScore < afterScore) {
			int bonus = (afterScore - enermyScore) / rate;
			if (bonus >= 12) bonus = 12;
			befScore += AI_SCORE_MAX+bonus;
		}
		enermyScore -= afterScore/rate*rate;
		if (AI_SEE && enermyScore > rate && _remainRensa <= 1) {
			s.ojamaInput(enermyScore / rate, true);
			s.applyOjamaDrop();
			enermyScore = 1;
			rscore = Detector::RensaToScore(stackRensaScore(s, g, 3, false));
		}

		////묻기

		//asf aefasdfqeasf mode


		if (enermyScore <= 0 && AI_SURPRISE_ATTACK) {
			//난전
			if (afterRensa == 2 && afterScore >= rate * 6*4) {
				befScore += AI_SCORE_MAX;
			}
			/*if (s.getFieldOjama() >= 6 * 2
				|| oppS.getFieldOjama() >= 6 * 2) {
				if (afterScore >= _opponentMaxScore && afterRensa <= 3) {
					befScore += AI_SCORE_MAX;
				}
				
				
			}*/
			//확정발화
			/*if (Detector::ScoreToRensa(afterScore) >= Detector::ScoreToRensa(_opponentMaxScore)
				+ min(afterRensa/2, oppS.getFieldSpace() / 4)
				&& oppS.getFieldSpace() <= 6*4) {
				befScore += AI_SCORE_MAX;
			}*/
			
		}
		if (
			(Detector::ScoreToRensa(afterScore) - backS.getFieldOjama() / 4.0f >= 12)
			//(afterRensa-backS.getFieldOjama()/4.0f >= 12)
			) {
			//return afterScore * param[6];
			auto sn = SimulatorNode(s, loop);
			//sn.evalScore = score + Detector::ScoreToRensa(afterScore);
			sn.evalScore = afterScore;
			//Detector::RensaToScore(
			//	Detector::ScoreToRensa(afterScore)
			//	//+ connectScore(backS, g)
			//)
			//- depth * 10000;
			//;
			sn.isSearched = true;
			return sn;
		}
		else {
			score += rscore;
		}
		
		//if (AI_SEE && enermyScore > rate &&
		//	enermyScore < afterScore) {
		//	befScore += 10;
		//	enermyScore = 0;
		//}
		//enermyScore -= afterScore;
		//if (AI_SEE && enermyScore > rate && _remainRensa <= 1) {
		//	s.ojamaInput(enermyScore / rate, true);
		//	enermyScore = 0;
		//	s.applyOjamaDrop();
		//}
		if (depth >= _maxDepth - 1 || true) {
			auto sn = SimulatorNode(s, loop);
			sn.evalScore = score + befScore;
			sn.befScore = befScore;
			sn._enermyScore = _enermyScore;
			sn._remainRensa = _remainRensa;
			if (afterRensa > 3) sn.isSearched = true;
			return sn;
		}
	}
	else {
		//cant move
		auto sn = SimulatorNode(s, loop);
		sn.evalScore = AI_CANT_MOVE_SCORE;
		sn.isSearched = true;
		return sn;
	}
}
float FormAIv2::stackRensaScore(Simulator s, GameInfo g, int maxDepth, bool rush)
{
	float maxrensa = -19;
	int maxi = -1;
	Detector det = Detector();
	vector<Detected> dtt;
	int b = 0;

	//int aaa[13][6] = {
	//{0,0,0,0,0,0},
	//{0,0,0,0,0,0},
	//{0,0,0,0,0,0},
	//{0,0,0,0,0,0},
	//{0,0,0,0,0,0},
	//{0,0,0,0,0,0},
	//{0,0,0,0,0,0},
	//{0,0,0,0,0,0},
	//{1,0,0,0,0,0},
	//{1,0,0,0,0,0},
	//{2,0,0,0,0,0},
	//{1,2,3,4,0,0},
	//{1,2,3,4,0,0}
	//};
	//s.set(aaa);

	//float pp = s.getFieldUnstableSq() * 0.2f;
	//float rs = (1 - s.getFieldSpaceMax1());
	int hhh = s.hash();
	if (rensaCache[hhh] != 0) {
		//return rensaCache[s.hash()];
	}



	dtt = det.detectMore(s.getField(), b, 1, 0, PuyoTsumo(0, 0), rush);
	for (int i = 0; i < dtt.size(); i++) {
		//float nowmaxrensa = dtt[i]._rensa;
		float nowmaxrensa = Detector::ScoreToRensa(dtt[i]._score);
		nowmaxrensa -= dtt[i]._p * 0.25f;
		//nowmaxrensa -= dtt[i]._isUp ? 0.5 : 0;
	
		//nowmaxrensa -= pp;
		//nowmaxrensa -= dtt[i]._isUp ? 1 : 0 * 1.f;
		if (nowmaxrensa > maxrensa) {
			maxrensa = nowmaxrensa; maxi = i;
		}
	}
	//if (true) {
	//	if (maxi != -1)
	//		dtt[maxi]._backup.print();
	//	cout << "score " << maxrensa << endl;
	//	system("pause");
	//}
	//rensaCache[s.hash()] = maxrensa;
	return (maxrensa);
}
string FormAIv2::getDebugStr()
{
	return _debug;
}

int FormAIv2::getMaxRensa()
{
	return _beforeMaxrensa;
}

void FormAIv2::getParams()
{
	//cout << "param start" << endl;
	int paramcnt = 9;
	cout << "float param[" << paramcnt << "] = {";
	for (int j = 0; j < paramcnt; j++) {
		cout << param[j];
		if (j != paramcnt - 1)cout << ",";
	}
	cout << "};";
	cout << endl;

}

void FormAIv2::randParams()
{
	for (int j = 0; j < 9; j++) {
		param[j] = (rand() % 10000) / 100.f;
	}

}

void FormAIv2::saveParams()
{

}

void FormAIv2::revertParams()
{

}

void FormAIv2::permutation(int arr[4], int start, int end, vector<vector<int>>& result) {
	if (start == end) {
		vector<int> tmp(arr, arr + end + 1);
		result.push_back(tmp);
	}
	for (int i = start; i <= end; i++) {
		swap(arr[start], arr[i]);
		permutation(arr, start + 1, end, result);
		swap(arr[start], arr[i]);
	}
};
