#include "Detector.h"

vector<Detected> Detector::detect(void* field)
{
	int(*f)[6] = (int(*)[6])field;
	Simulator s = Simulator(-1);
	int i, j, k, l;
	int rensa;
	int score;
	vector<Detected> result;
	result.reserve(10);
	s.set(f);

	if (s.applyGravityOnce() > 0) { return result; } // if puyo floated
	if (s.applyPop(0) > 0) { return result; } //if puyo in chaining state
	for (i = 0; i < 6; i++) { // input
		for (j = 1; j < 6; j++) { //puyo color
			rensa = 1;
			score = 0;
			s.set(f);
			bool r = s.pushFast(i, PuyoTsumo(0, j), rensa, score);
			if (r == false) {
				continue;

			}
			int w = 13 * 6 - s.getFieldSpace() + s.getLatestWaste();
			if (score >= 70 * 6) {
				result.push_back(Detected(rensa, score, i, false, w));
			}
		}
	}
	return result;

}

vector<Detected> Detector::detectMoreRand(void* field, int maxDepth, int depth, PuyoTsumo pt, int befMax)
{

	//srand(time(NULL));
	vector<Detected> result;
	if (depth == maxDepth) return result;
	int(*f)[6] = (int(*)[6])field;
	Simulator s = Simulator(-1);
	int i, j, k, l;
	int rensa;
	int score;
	result.reserve(10);
	s.set(f);

	//no next
	if (pt.up == 0) {
		for (int loop = 0; loop < (int)1; loop++) {
			i = rand() % 22;//for (i = 0; i < 22; i++) { // input
			j = rand() % 5 + 1;
			k = rand() % 5 + 1;
			//cout << depth << endl;
			rensa = 1;
			score = 0;
			s.set(f);
			Simulator backup(s);
			backup.pushFastNoPop(ALL_MOVEMENT[i], PuyoTsumo(j, k));
			bool r = s.pushFast(ALL_MOVEMENT[i], PuyoTsumo(j, k), rensa, score);
			if (s.getColorCount() == 5) {
				continue;
			}
			if (s.getGameover()) continue;
			if (r != false) {
				//int w = 13 * 6 - s.getFieldSpace() + s.getLatestWaste();
				int w = 0;
				if (rensa > 0) {
					result.push_back(Detected(rensa, score, i, false, w, backup));
				}
				else {
					for (auto& e : detectMoreRand(s.getField(), maxDepth, depth + 1, PuyoTsumo(0, 0))) {
						result.push_back(e);
					}
				}
			}
			else {
				//loop--;
				continue;
			}
		}
		//has next
	}
	else {
		for (int loop = 0; loop < (int)(200); loop++) {
			i = rand() % 22;
			rensa = 1;
			score = 0;
			s.set(f);
			Simulator backup(s);
			backup.pushFastNoPop(ALL_MOVEMENT[i], pt);
			bool r = s.pushFast(ALL_MOVEMENT[i], pt, rensa, score);
			if (r != false) {
				//int w = 13 * 6 - s.getFieldSpace() + s.getLatestWaste();
				int w = 0;
#pragma omp critical
				if (rensa > 0) {
					result.push_back(Detected(rensa, score, i, false, w, backup));
				}
				else {
					for (auto& e : detectMoreRand(s.getField(), maxDepth, depth + 1, PuyoTsumo(0, 0))) {
						result.push_back(e);
					}
				}
			}
		}
	}
	return result;
}

vector<Detected> Detector::detectMoreEx(void* field, int& befMax, int maxDepth, int depth, PuyoTsumo pt, PuyoTsumo pt2, bool rush)
{
	vector<Detected> result;
	int(*f)[6] = (int(*)[6])field;
	Simulator s = Simulator(-1);
	int i, j, k, l;
	int rensa;
	int score;
	result.reserve(10);
	s.set(f);

	for (i = 0; i < 22; i++) { // input
		rensa = 1;
		score = 0;
		s.set(f);
		Simulator backup(s);
		backup.pushFastNoPop(ALL_MOVEMENT[i], pt);
		bool r = s.pushFast(ALL_MOVEMENT[i], pt, rensa, score);
		if (s.getAllClear()) score += 2100;
		//if (s.getFieldUnstable() >= 10) continue;
		if (r == false) {
			continue;
		}
		int w = 13 * 6 - s.getFieldSpace() + s.getLatestWaste();
		//#pragma omp critical
		if (rensa > 0) {
			befMax++;
			result.push_back(Detected(rensa, score, i, true, w, backup));
		}
		else {
			befMax++;
			for (auto& e : detectMore(s.getField(), befMax, maxDepth, depth + 1, pt2, rush)) {
				result.push_back(e);
			}
		}
	}
	return result;
}

vector<Simulator> Detector::getFireReadySimulators(bool visit[][6], Simulator s, int& needPuyoCnt, int startx, int starty, int color, Simulator ori, vector<pair<int, int>>& addedCoord) {
	vector<Simulator> re;
	int(*f)[6] = (int(*)[6])s.getField();

	int di[] = { 1,0,-1,0 };
	int dj[] = { 0,1,0,-1 };
	int y = starty;
	int x = startx;
	//cout << y << " " << x << "searching" << endl;

	if (needPuyoCnt > 0) {
		for (int k = 0; k < 4; k++) {
			if (s.checkFieldIndex(x + dj[k], y + di[k]) == false)continue;
			if (visit[y + di[k]][x + dj[k]] == true) continue;
			visit[y + di[k]][x + dj[k]] = true;
			if (f[y + di[k]][x + dj[k]] == 0) {
				//if (s.checkFieldIndex(x + dj[k], y + di[k] + 1) == true)
					//if (f[y + di[k] + 1][x + dj[k]] == 0)continue;//공중 안뜸
				f[y + di[k]][x + dj[k]] = color;
				if (Simulator(s).pushFastNoPop(x + dj[k], PuyoTsumo(0, 0)) == false) continue;
				//if (true) {//다 안붙였는데 옆뿌요랑 충돌
				//	//상하좌우에 오리지널이 아닌 동색뿌요
				//	bool c = false;
				//	int(*of)[6] = (int(*)[6])ori.getField();
				//	for (int l = 0; l < 4; l++) {
				//		if (s.checkFieldIndex(x + dj[k] + dj[l], y + di[k] + di[l]) == false)continue;
				//		if (f[y + di[k] + di[l]][x + dj[k] + dj[l]] == color && of[y + di[k] + di[l]][x + dj[k] + dj[l]] != color)
				//			c = true;
				//	}
				//	if (c) {
				//		f[y + di[k]][x + dj[k]] = 0;
				//		continue;
				//	}
				//}

				//needpuyocnt 갱신
				int npcold = needPuyoCnt;
				bool visit3[13][6] = { false };
				std::vector<std::pair<int, int>> popGroup;
				int i = y + di[k];
				int j = x + dj[k];
				if (visit3[i - 1][j] == false
					&& (!(f[i][j] == 6 || f[i][j] == 0))) {
					s.dfs(f, i, j, f[i][j], visit3, popGroup);
					if (popGroup.size() == 0) popGroup.push_back(std::pair<int, int>(i, j));
					needPuyoCnt = 4 - popGroup.size();
				}
				if (needPuyoCnt < 0) {
					f[y + di[k]][x + dj[k]] = 0;
					needPuyoCnt = npcold;
					continue;
				}

				if (needPuyoCnt == 0) {
					re.push_back(s);
					f[y + di[k]][x + dj[k]] = 0;
					needPuyoCnt = npcold;
				}
				else {
					int nc = needPuyoCnt;
					bool visit2[13][6] = { false };
					auto r = getFireReadySimulators(visit2, s, nc, x + dj[k], y + di[k], color, s, addedCoord);
					for (auto& e : r)
						re.push_back(e);
					f[y + di[k]][x + dj[k]] = 0;
					needPuyoCnt = npcold;
				}

			}
			else if (f[y + di[k]][x + dj[k]] == color) {
				int nc = needPuyoCnt;
				vector<pair<int, int>> addedCoord2;
				auto r = getFireReadySimulators(visit, s, nc, x + dj[k], y + di[k], color, ori, addedCoord);
				for (auto& e : r)
					re.push_back(e);
			}
			else {
				continue;
			}
		}
	}

	return re;
}

Simulator Detector::getFireReadySimulator(bool visit[][6], Simulator s, int& needPuyoCnt, int startx, int starty, int color, Simulator ori, vector<pair<int, int>>& addedCoord) {
	int(*f)[6] = (int(*)[6])s.getField();

	int di[] = { 1,0,-1,0 };
	int dj[] = { 0,1,0,-1 };
	int y = starty;
	int x = startx;
	//cout << y << " " << x << "searching" << endl;

	if (needPuyoCnt > 0) {
		for (int k = 0; k < 4; k++) {
			if (s.checkFieldIndex(x + dj[k], y + di[k]) == false)continue;
			if (visit[y + di[k]][x + dj[k]] == true) continue;
			visit[y + di[k]][x + dj[k]] = true;
			if (f[y + di[k]][x + dj[k]] == 0) {
				//if (s.checkFieldIndex(x + dj[k], y + di[k] + 1) == true)
					//if (f[y + di[k] + 1][x + dj[k]] == 0)continue;//공중 안뜸
				if (Simulator(s).pushFastNoPop(x + dj[k], PuyoTsumo(0, 0)) == false) continue;
				f[y + di[k]][x + dj[k]] = color;

				//if (true) {//다 안붙였는데 옆뿌요랑 충돌
				//	//상하좌우에 오리지널이 아닌 동색뿌요
				//	bool c = false;
				//	int(*of)[6] = (int(*)[6])ori.getField();
				//	for (int l = 0; l < 4; l++) {
				//		if (s.checkFieldIndex(x + dj[k] + dj[l], y + di[k] + di[l]) == false)continue;
				//		if (f[y + di[k] + di[l]][x + dj[k] + dj[l]] == color && of[y + di[k] + di[l]][x + dj[k] + dj[l]] != color)
				//			c = true;
				//	}
				//	if (c) {
				//		f[y + di[k]][x + dj[k]] = 0;
				//		continue;
				//	}
				//}

				//needpuyocnt 갱신

				bool visit3[13][6] = { false };
				std::vector<std::pair<int, int>> popGroup;
				int i = y + di[k];
				int j = x + dj[k];
				if (visit3[i - 1][j] == false
					&& (!(f[i][j] == 6 || f[i][j] == 0))) {
					s.dfs(f, i, j, f[i][j], visit3, popGroup);
					if (popGroup.size() == 0) popGroup.push_back(std::pair<int, int>(i, j));
					needPuyoCnt = 4 - popGroup.size();
				}


				addedCoord.push_back(make_pair(y + di[k], x + dj[k]));
				bool visit2[13][6] = { false };
				vector<pair<int, int>> addedCoord2;
				int nc = needPuyoCnt;
				auto r = getFireReadySimulator(visit2, s, nc, x + dj[k], y + di[k], color, s, addedCoord2);
				if (nc == 0) {
					needPuyoCnt = 0;
					for (auto& e : addedCoord2)
						addedCoord.push_back(e);
					return r;
				}

			}
			else if (f[y + di[k]][x + dj[k]] == color) {
				int nc = needPuyoCnt;
				vector<pair<int, int>> addedCoord2;
				auto r = getFireReadySimulator(visit, s, nc, x + dj[k], y + di[k], color, ori, addedCoord2);
				if (nc == 0) {
					needPuyoCnt = 0;
					for (auto& e : addedCoord2)
						addedCoord.push_back(e);
					return r;
				}
			}
			else {
				continue;
			}

		}

	}
	else if (needPuyoCnt == 0) {

		return s;
	}

	return s;
}

Simulator Detector::getFireReadySimulatorfast(bool visit[][6], Simulator s, int& needPuyoCnt, int startx, int starty, int color, Simulator ori, vector<pair<int, int>>& addedCoord) {
	int(*f)[6] = (int(*)[6])s.getField();

	int di[] = { 1,0,-1,0 };
	int dj[] = { 0,1,0,-1 };
	int y = starty;
	int x = startx;
	//cout << y << " " << x << "searching" << endl;

	if (needPuyoCnt > 0) {
		for (int k = 0; k < 4; k++) {
			if (s.checkFieldIndex(x + dj[k], y + di[k]) == false)continue;
			if (visit[y + di[k]][x + dj[k]] == true) continue;
			visit[y + di[k]][x + dj[k]] = true;
			if (f[y + di[k]][x + dj[k]] == 0) {
				//if (s.checkFieldIndex(x + dj[k], y + di[k] + 1) == true)
					//if (f[y + di[k] + 1][x + dj[k]] == 0)continue;//공중 안뜸
				if (Simulator(s).pushFastNoPop(x + dj[k], PuyoTsumo(0, 0)) == false) continue;
				f[y + di[k]][x + dj[k]] = color;

				//if (true) {//다 안붙였는데 옆뿌요랑 충돌
				//	//상하좌우에 오리지널이 아닌 동색뿌요
				//	bool c = false;
				//	int(*of)[6] = (int(*)[6])ori.getField();
				//	for (int l = 0; l < 4; l++) {
				//		if (s.checkFieldIndex(x + dj[k] + dj[l], y + di[k] + di[l]) == false)continue;
				//		if (f[y + di[k] + di[l]][x + dj[k] + dj[l]] == color && of[y + di[k] + di[l]][x + dj[k] + dj[l]] != color)
				//			c = true;
				//	}
				//	if (c) {
				//		f[y + di[k]][x + dj[k]] = 0;
				//		continue;
				//	}
				//}

				//needpuyocnt 갱신

				needPuyoCnt -= 1;


				addedCoord.push_back(make_pair(y + di[k], x + dj[k]));
				bool visit2[13][6] = { false };
				vector<pair<int, int>> addedCoord2;
				int nc = needPuyoCnt;
				auto r = getFireReadySimulatorfast(visit2, s, nc, x + dj[k], y + di[k], color, s, addedCoord2);
				if (nc == 0) {
					needPuyoCnt = 0;
					for (auto& e : addedCoord2)
						addedCoord.push_back(e);
					return r;
				}

			}
			else if (f[y + di[k]][x + dj[k]] == color) {
				int nc = needPuyoCnt;
				vector<pair<int, int>> addedCoord2;
				auto r = getFireReadySimulatorfast(visit, s, nc, x + dj[k], y + di[k], color, ori, addedCoord2);
				if (nc == 0) {
					needPuyoCnt = 0;
					for (auto& e : addedCoord2)
						addedCoord.push_back(e);
					return r;
				}
			}
			else {
				continue;
			}

		}

	}
	else if (needPuyoCnt == 0) {

		return s;
	}

	return s;
}

vector<Detected> Detector::detectMore(void* field, int& befMax, int maxDepth, int depth, PuyoTsumo pt, bool rush)
{
	vector<Detected> result;
	if (depth == maxDepth) return result;
	int(*f)[6] = (int(*)[6])field;
	Simulator s = Simulator(-1);
	int i, j, k, l;
	int rensa;
	int score;
	//result.reserve(10000);
	s.set(f);
	if (pt.up == 0) {
		if (true) {
			int i, j;
			bool visit[12][6];
			for (i = 0; i < 12; i++) {
				for (j = 0; j < 6; j++) {
					visit[i][j] = false;
				}
			}
			int(*sf)[6] = (int(*)[6])s.getField();
			std::vector<std::vector<std::pair<int, int>>> popGroups;
			popGroups.reserve(10);
			std::vector<std::pair<int, int>> popGroup;
			popGroup.reserve(50);
			Plan p;

			for (auto out : p.findOutSide(s)) {
				i = out.second;
				j = out.first;
				if (visit[i - 1][j] == false
					&& (!(sf[i][j] == 6 || sf[i][j] == 0))) {
					popGroup.clear();
					s.dfs(sf, i, j, sf[i][j], visit, popGroup);
					int popcnt = 0;
					popcnt = 1;
					if (popGroup.size() == 0) popGroup.push_back(std::pair<int, int>(i, j));
					if ((int)popGroup.size() >= popcnt) {
						popGroups.push_back(popGroup);
						//first only head
					}
				}
			}
			{
				for (i = 0; i < popGroups.size(); i++) {

					//1단계 그냥 발화 필드 양산
					int needpuyocnt = 4 - popGroups[i].size();
					int color = sf[popGroups[i][0].first][popGroups[i][0].second];

					int y = popGroups[i][0].first;
					int x = popGroups[i][0].second;
					//cout << x << " " << y << endl;
					//cout << "color " << color << endl;
					int nc = needpuyocnt;
					bool visit2[13][6] = { false };
					vector<pair<int, int>> addedCoord;
					bool heavy = false;
					if (heavy) {
						auto fires = getFireReadySimulators(visit2, s, nc, x, y, color, s, addedCoord);
						for (auto& fire : fires) {

							needpuyocnt = addedCoord.size();
							int oriren = 1;
							int sco = 0;
							Simulator backup(fire);
							fire.applyChainPop(oriren, sco);
							if (fire._latestHachiich == true) continue;
							auto dd = Detected(oriren, sco, i, false, needpuyocnt, backup);
							dd.addedCoord = addedCoord;
							result.push_back(dd);

							//2단계 개별뿌요 추가

							bool visit3[12][6] = { false };
							std::vector<std::pair<int, int>> puyos;
							int(*sfire)[6] = (int(*)[6])backup.getField();
							s.dfs(sfire, y, x, color, visit3, puyos);

							for (auto& puyo : puyos) {
								int y = puyo.first;
								int x = puyo.second;
								int colortable[10] = { 0 };
								colortable[0] = 1;
								colortable[color] = 1;
								colortable[6] = 1;
								//add keys ( only 1)
								for (int k = y + 1; k < 13; k++) {
									if (s.checkFieldIndex(x, k) && colortable[sf[k][x]] == 0) {
										Simulator ss(backup);
										colortable[sf[k][x]]++;
										if (ss.pushFastNoPop(x, PuyoTsumo(0, sf[k][x])) == false) continue;
										//ss.print();
										bool visit2[13][6] = { false };
										int nc = needpuyocnt;
										vector<pair<int, int>> addedCoord;
										//auto fire3 = getFireReadySimulator(visit2, ss, nc, x, y, color, ss, addedCoord);
										auto fire3 = ss;
										needpuyocnt = addedCoord.size();
										auto backup2 = Simulator(fire3);
										int ren = 1;
										int sco = 0;
										fire3.applyChainPop(ren, sco);
										if (fire3._latestHachiich == true) continue;
										auto dd2 = Detected(ren, sco, i, true, needpuyocnt + 1, backup2);
										dd2.addedCoord = addedCoord;//todo addedcoord bug test not using now
										result.push_back(dd2);
									}
								}
							}
						}
					}
					else {
						auto fire = getFireReadySimulatorfast(visit2, s, nc, x, y, color, s, addedCoord);
						needpuyocnt = addedCoord.size();
						int oriren = 1;
						int sco = 0;
						Simulator backup(fire);
						fire.applyChainPop(oriren, sco);
						if (fire._latestHachiich == true) continue;
						auto dd = Detected(oriren, sco, i, false, needpuyocnt, backup);
						dd.addedCoord = addedCoord;
						result.push_back(dd);
						//2단계 개별뿌요 추가

						/*bool visit3[12][6] = { false };
						std::vector<std::pair<int, int>> puyos;
						int(*sfire)[6] = (int(*)[6])backup.getField();
						s.dfs(sfire, y, x, color, visit3, puyos);*/
						std::vector<std::pair<int, int>> puyos;
						puyos = popGroups[i];
						for (auto& puyo : puyos) {
							int y = puyo.first;
							int x = puyo.second;
							int colortable[10] = { 0 };
							colortable[0] = 1;
							colortable[color] = 1;
							colortable[6] = 1;
							//add keys ( only 1)
							for (int k = y + 1; k < 13; k++) {
								if (s.checkFieldIndex(x, k) && colortable[sf[k][x]] == 0) {
									Simulator ss(backup);
									colortable[sf[k][x]]++;
									if (ss.pushFastNoPop(x, PuyoTsumo(0, sf[k][x])) == false) continue;
									//ss.print();
									auto backup2 = Simulator(ss);
									int ren = 1;
									int sco = 0;
									ss.applyChainPop(ren, sco);
									if (ss._latestHachiich == true) continue;
									auto dd2 = Detected(ren, sco, i, true, needpuyocnt + 1, backup2);
									result.push_back(dd2);
								}
							}
						}
					}
				}
			}
		}
		//2단계 만들어진 필드들로 연쇄 확장
		//vector<Detected> result2;
		//for(int loop = 0; loop < 2; loop++){
		//	for (int in = 0; in < result.size(); in++) {
		//		auto& a = result[in];
		//		if (a.isExpanded == true) continue;
		//		if (a.addedCoord.size() == 0) continue;
		//		auto colors = a._backup.getColors();
		//		Plan p;
		//		for (int i = 1; i <= 5; i++) {
		//			auto news = Simulator(a._backup);
		//			if (p.pushShape(news, Shape::T, i, a.addedCoord[0].second, a.addedCoord[0].first, false, false, 0)) {
		//				auto backup3 = Simulator(news);
		//				int ren = 1;
		//				int sco = 0;
		//				news.applyChainPop(ren, sco);
		//				//backup3.print();
		//				auto dd2 = Detected(ren, sco, i, true, a._p + 4, backup3);
		//				result2.push_back(dd2);
		//			}
		//		}
		//		a.isExpanded = true;
		//	}
		//	for (auto& t : result2) result.push_back(t);
		//}
		return result;

	}
	else {
		for (i = 0; i < 22; i++) { // input
			rensa = 1;
			score = 0;
			s.set(f);
			Simulator backup(s);
			backup.pushFastNoPop(ALL_MOVEMENT[i], pt);
			bool r = s.pushFast(ALL_MOVEMENT[i], pt, rensa, score);
			if (s.getAllClear()) score += 2100;
			//if (s.getFieldUnstable() >= 10) continue;
			if (r == false) {
				continue;
			}
			int w = 13 * 6 - s.getFieldSpace() + s.getLatestWaste();
			//#pragma omp critical
			if (rensa > 0) {
				befMax++;
				result.push_back(Detected(rensa, score, i, true, w, backup));
			}
			else {
				befMax++;
				for (auto& e : detectMore(s.getField(), befMax, maxDepth, depth + 1, PuyoTsumo(0, 0))) {
					result.push_back(e);
				}
			}
		}
	}
	return result;
}

int Detector::detectMaxrensa(Simulator s)
{
	int(*f)[6] = (int(*)[6])s.getField();
	Simulator a = Simulator(s);
	int i, j, k, l;
	int rensa;
	int score;
	int maxRensa = 0;

	s.applyGravity();
	for (i = 0; i < 6; i++) { // input
		for (j = 1; j < 6; j++) { //puyo color
			rensa = 1;
			score = 0;
			auto a = Simulator(s);
			bool r = a.pushFast(i, PuyoTsumo(j, 0), rensa, score);
			if (r == false) {
				continue;
			}
			if (maxRensa > rensa) {
				maxRensa = rensa;
			}
		}
	}
	return maxRensa;
}


int Detector::detectMoreMaxrensa(Simulator s)
{
	int(*f)[6] = (int(*)[6])s.getField();
	Simulator a = Simulator(s);
	int i, j, k, l;
	int rensa;
	int score;
	int maxRensa = 0;

	s.applyGravity();
	for (i = 0; i < 22; i++) { // input
		for (j = 1; j < 6; j++) { //puyo color
			for (k = j; k < 6; k++) { //puyo color
				rensa = 1;
				score = 0;
				auto a = Simulator(s);
				bool r = a.pushFast(i, PuyoTsumo(j, k), rensa, score);
				if (r == false) {
					continue;
				}
				if (maxRensa > rensa) {
					maxRensa = rensa;
				}
			}
		}
	}
	return maxRensa;
}

pair<vector<SimulatorNode>, pair<int, int>> Detector::detectMoreMoreGetNode(vector<SimulatorNode> v, int noColor, PuyoTsumo next, int depth) {
	int i, j, k, l;
	vector<SimulatorNode> d2;
	int maxRensa = 0;
	int maxRensaIndex = -1;
	int width = 50;//15
	if (v.size() == 0) return pair<vector<SimulatorNode>, pair<int, int>>(v, make_pair(maxRensa, maxRensaIndex));
	sort(v.begin(), v.end(), [](SimulatorNode a, SimulatorNode b) -> bool
		{return a.evalScore > b.evalScore; });

	if (v.size() <= width) {
		width = v.size();
	}
	if (v.size() >= width) {
		v.erase(v.begin() + width, v.end());
	}
	for (int i = 0; (i < width); i++) {
		if (v[i].isSearched) continue;
		v[i].isSearched = true;
		v[i].visitCount++;
		//cout << v[i].evalScore << endl;
		//v[i]._s.print();
		//system("pause");
		if (next.up != 0) {
			for (l = 0; l < 22; l++) { // input
				//j = next.up;
				//k = next.down;
				//Simulator a = Simulator(v[i]._s);
				//int rensa = 1;
				//int score = 0;
				//Simulator back = Simulator(a);
				//back.pushFastNoPop(ALL_MOVEMENT[l], PuyoTsumo(j, k));
				//bool r = a.pushFast(ALL_MOVEMENT[l], PuyoTsumo(j, k), rensa, score);

				//if (r == false || a.getGameover() == true || back.checkFireAbleReal()==false) continue;
				//auto sn = SimulatorNode(a, v[i]._startMove);
				//if (a.getAllClear()) sn.afterScore += 2100;
				//sn.afterRensa = rensa;
				//sn.afterScore = score;
				//sn._s = back;
				//sn.evalScore = 0;
				//sn.evalScore += a.getAllClear() ? 2000 : 0;
				//sn.evalScore += ScoreToRensa(sn.afterScore) * 100;
				////sn.evalScore -= a.getFieldUnstable() * 5;
				////sn.evalScore -= a._latestHachiich ? 10000 : 0;
				//sn.depth = v[i].depth + 1;
				//v.push_back(sn);
			}
		}
		else {
			for (l = 0; l < 22; l++) { // input
				//for (j = 1; j < 6; j++)
				{
					//for (k = j; k < 6; k++)
					{

						j = rand() % 5 + 1;
						k = rand() % 5 + 1;
						if (j == noColor || k == noColor) {
							l--;
							continue;
						}
						Simulator a = Simulator(v[i]._s);
						int rensa = 1;
						int score = 0;
						Simulator back = Simulator(a);
						back.pushFastNoPop(ALL_MOVEMENT[l], PuyoTsumo(j, k));

						bool r = a.pushFast(ALL_MOVEMENT[l], PuyoTsumo(j, k), rensa, score);

						if (r == false || a.getGameover() == true || back.checkFireAbleReal() == false) continue;
						auto sn = SimulatorNode(a, v[i]._startMove);
						if (a.getAllClear()) sn.afterScore += 2100;
						sn.afterRensa = rensa;
						sn.afterScore = score;
						sn._s = back;
						sn.evalScore = 0;
						//sn.evalScore += a.getAllClear() ? 1000 : 0;
						///cout << (back.getFieldHeight()[ALL_MOVEMENT[l] % 10] / 13.0) << endl;
						sn.evalScore += ScoreToRensa(sn.afterScore) * 100.f;
						if (rensa > 0) {
							sn.evalScore -= 13.f - getFireHeight(back) * 40.f;
						}
						sn.depth = v[i].depth + 1;
						//sn.evalScore -= sn.depth * 25;
						//sn.evalScore += getConnectScore(back);
						//sn.evalScore -= a.getFieldUnstable() * 5;
						//sn.evalScore -= a._latestHachiich ? 10000 : 0;

						v.push_back(sn);
					}
				}
			}
		}
	}
	return pair<vector<SimulatorNode>, pair<int, int>>(v, make_pair(maxRensa, maxRensaIndex));
}

float Detector::getFireHeight(Simulator s) {
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
				if ((int)popGroup.size() >= 4) {
					popGroups.push_back(popGroup);
				}
			}
		}
	}
	float cnt = 0;
	float sum = 0;
	for (i = 0; i < popGroups.size(); i++) {
		for (j = 0; j < popGroups[i].size(); j++) {
			sum += popGroups[i][j].first;
			cnt += 1.0f;
		}
	}
	if (cnt != 0)
		return sum / cnt;
	else
		return 0;
}

float Detector::getFireX(Simulator s) {
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
				if ((int)popGroup.size() >= 3) {
					popGroups.push_back(popGroup);
				}
			}
		}
	}
	float cnt = 0;
	float sum = 0;
	for (i = 0; i < popGroups.size(); i++) {
		for (j = 0; j < popGroups[i].size(); j++) {
			sum += popGroups[i][j].second;
			cnt += 1.0f;
		}
	}
	if (cnt != 0)
		return sum / cnt;
	else
		return 0;
}

float Detector::getConnectScore(Simulator s) {
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
				if ((int)popGroup.size() >= 2) {
					popGroups.push_back(popGroup);
				}
			}
		}
	}
	for (i = 0; i < popGroups.size(); i++) {
		switch ((int)popGroups[i].size()) {
		case 1:
			//score -= 1;
			//score -= 50000;
			//score -= AI_STACK_RENSA_SCORE;
			break;
		case 2:
			score += 1;
			break;
		case 3:
			score += 1;
			break;
		}
	}
	return score;
}

SimulatorNode* Detector::detectMoreMore(Simulator s, int noColor, PuyoTsumo* next, int startdepth, int maxDepth, bool fullnext)
{
	//int beamWidth = 5;
	Simulator ori = Simulator(s);
	int i, j, k, l;
	float maxRensa = 0;
	int maxRensaIndex = 0;

	vector<SimulatorNode> d0;
	vector<SimulatorNode> d1;
	//depth 0
	if (startdepth == 0) {

		//for (i = 0; i < 22; i++) {
		//	Simulator a = Simulator(ori);
		//	int rensa = 1;
		//	int score = 0;
		//	bool r = a.pushFast(ALL_MOVEMENT[i], PuyoTsumo(next[0].up, next[0].down), rensa, score);
		//	auto sn = SimulatorNode(a, i);
		//	sn.afterRensa = rensa;
		//	sn.afterScore = score;
		//	if (r == false || a.getGameover() == true) continue;
		//	d0.push_back(sn);
		//	if (rensa > maxRensa) {
		//		maxRensa = rensa;
		//	}
		//}

		////depth 1

		//while (!d0.empty()) {
		//	SimulatorNode n = d0.back();
		//	for (i = 0; i < 22; i++) {
		//		Simulator a = Simulator(n._s);
		//		int rensa = 1;
		//		int score = 0;
		//		Simulator back = Simulator(a);
		//		back.pushFastNoPop(ALL_MOVEMENT[i], PuyoTsumo(next[1].up, next[1].down));
		//		bool r = a.pushFast(ALL_MOVEMENT[i], PuyoTsumo(next[1].up, next[1].down), rensa, score);
		//		if (r == false || a.getGameover() == true) continue;
		//		auto sn = SimulatorNode(a, n._startMove);
		//		if (a.getAllClear()) sn.afterScore += 2100;
		//		sn.afterRensa = rensa;
		//		sn.afterScore = score;
		//		sn._s = back;
		//		sn.evalScore = 0;
		//		sn.evalScore += a.getAllClear() ? 0 : 0;
		//		sn.evalScore -= back.getFieldUnstable() * 1000;
		//		sn.evalScore += getConnectScore(a);
		//		sn.evalScore += sn.afterRensa*1000;
		//		//sn.evalScore -= a._latestHachiich ? 10000 : 0;
		//		//sn.evalScore -= a.getLatestWaste() / 4.0 * 2000;
		//		for (int aa = 0; aa < 6; aa++)
		//			if (a.getFieldHeight()[aa] >= 12) sn.evalScore -= 100;
		//		d1.push_back(sn);
		//		if (rensa > maxRensa) {
		//			maxRensa = rensa;
		//			maxRensaIndex = sn._startMove;
		//		}
		//	}
		//	d0.pop_back();

		//}
	}
	else {
		for (i = 0; i < 22; i++) {
			Simulator a = Simulator(ori);
			int rensa = 1;
			int score = 0;
			Simulator back = Simulator(a);
			back.pushFastNoPop(ALL_MOVEMENT[i], PuyoTsumo(next[1].up, next[1].down));
			bool r = a.pushFast(ALL_MOVEMENT[i], PuyoTsumo(next[1].up, next[1].down), rensa, score);
			if (r == false || a.getGameover() == true) continue;
			auto sn = SimulatorNode(a, i);
			if (a.getAllClear()) sn.afterScore += 2100;
			sn.afterRensa = rensa;
			sn.afterScore = score;
			sn._s = back;
			sn.evalScore = 0;
			//sn.evalScore -= a.getFieldUnstable() * 50;
			//sn.evalScore += a.getAllClear() ? 1000 : 0;
			sn.evalScore += ScoreToRensa(sn.afterScore) * 100;
			//sn.evalScore += getConnectScore(back);
			//sn.evalScore -= a._latestHachiich ? 10000 : 0;
			d1.push_back(sn);
		}

	}
	SimulatorNode* max = NULL;
	for (auto& k : d1) {
		if (max == NULL) {
			max = new SimulatorNode(k);
		}
		if (k.evalScore > max->evalScore) {
			delete max;
			max = new SimulatorNode(k);
		}
		if (k.afterRensa > 0) k.evalScore = -9999999;//넥스트로 바로 발화한경우 미래 탐색 제외
	}

	//depth 2~
	if (max == NULL) return NULL;
	pair<vector<SimulatorNode>, pair<int, int>>r;
	r.first = d1;
	for (int i = 0; i < maxDepth; i++) {
		if (fullnext)
			r = detectMoreMoreGetNode(r.first, noColor, next[2 + i]);
		else
			r = detectMoreMoreGetNode(r.first, noColor, PuyoTsumo(0, 0), 2 + i);
		for (auto& k : r.first) {
			if (k.evalScore > max->evalScore) {
				delete max;
				max = new SimulatorNode(k);
			}
			//if (k.afterRensa > 0) k.evalScore = -9999999;//넥스트로 바로 발화한경우 미래 탐색 제외
		}
	}
	if (max != NULL)
		max->evalScore /= 100;
	return max;
}

bool Detector::createForm(Plan& p, Simulator inputS, int befRen, int targetRen, vector<Shape>& queue)
{
	{
		int max = -99;
		int min = 99;
		for (int i = 0; i < 6; i++) {
			max = inputS.getFieldHeight()[i] > max ? inputS.getFieldHeight()[i] : max;
			min = inputS.getFieldHeight()[i] < min ? inputS.getFieldHeight()[i] : min;
		}
		//if (max - min > 8) {
		if (max - min > 8) {
			return false;
		}
	}

	//test
	//#pragma omp parallel for
	for (int sh = 0; sh < queue.size(); sh++) {
		//if (rand() % 3 == 0)continue;
		Simulator s = Simulator(inputS);
		Shape sha = queue[sh];
		for (auto i : p.findOutSide(s)) {
			//if (rand() % 3 == 0)continue;
			for (int num = 1; num <= 4; num++) {
				Simulator temp(s);//놓을 정보 보관(다음 depth로 넘김
				if (p.pushShape(temp, sha, num, i.first, i.second, false, befRen == 18 ? true : false, 0) == false)
				{
					continue;
				}
				else
				{
					//성공 다음 연쇄 탐색
					if (befRen < p.checkPlanRensa(temp))
					{
						if (befRen + 1 == targetRen) {
							if (temp.checkFireAbleReal() == true) {
#if(0)
								char filename[] = "out.txt";
								fstream uidlFile(filename, std::fstream::in | std::fstream::out | std::fstream::app);
								uidlFile << "<hr>";
								int(*pField)[6] = (int(*)[6])inputS.getField();
								for (int i = 0; i < 13; i++)
								{
									for (int j = 0; j < 6; j++)
									{
										uidlFile << "<span class = \"c" << pField[i][j] << "\">●</span>";
									}
									uidlFile << "<br>";
								}
								uidlFile << "<hr>";
#endif
								return true;
							}
							return false;
						}
						if (createForm(p, temp, p.checkPlanRensa(temp), targetRen, queue) == true) {
							break;
						}
						//create depth
						if (befRen == targetRen - 1 ? true : false == true) continue;
					}
				}
			}
		}
	}
	return false;
}

double Detector::detectMoreMoreMSV(Simulator s, Simulator ori)
{
	double score = 0;
	vector<ChildNode> childs;
	childs.reserve(20000);
	auto tmp = vector<pair<int, int>>();
	double maxs = -99;
	ChildNode* maxn = NULL;
	for (int i = 0; i < 20; i++) {
		testChildNode(ChildNode(s, 0, 0, 0, tmp), 0, childs, ori);
		if (false) {
			//#pragma omp critical
			{
				for (int i = 0; i < childs.size(); i++) {
					double rensaScore = (childs[i].rensa) - (double)childs[i].addedPuyoCnt / 2 / 4.0;
					if (maxs < rensaScore) {
						maxs = rensaScore;
						maxn = &childs[i];
					}
				}
				/*if (maxn != NULL && maxs > 14) {
					maxn->s.print();
					system("pause");
				}*/
			}
		}
	}

	double maxAddedMaxRensaScore[20] = { 0 };//1~20
	//연쇄점수 = 연쇄수-(추가된뿌요/4)
	for (int i = 0; i < childs.size(); i++) {
		//double rensaScore = (double)ScoreToRensa(childs[i].score) - (double)childs[i].addedPuyoCnt / 4.0 / 2.;
		double rensaScore = (childs[i].rensa) - (double)childs[i].addedPuyoCnt / 2 / 4.0;
		if (maxAddedMaxRensaScore[childs[i].addedPuyoCnt - 1] < rensaScore)
			maxAddedMaxRensaScore[childs[i].addedPuyoCnt - 1] = rensaScore;
	}

	vector <pair<double, double>> scores;
	for (int i = 0; i < 11; i++) {
		if (maxAddedMaxRensaScore[i] == 0) continue;
		scores.push_back(make_pair(i + 1, maxAddedMaxRensaScore[i]));
	}

	for (int i = scores.size() - 1; i >= 1; i--) {
		//더 많이 추가한게 더 적게 추가한것 보다 적으면 삭제
		for (int j = 0; j < scores.size(); j++) {
			if (scores[i].first > scores[j].first) {
				if (scores[i].second < scores[j].second) {
					scores.erase(scores.begin() + i);
					break;
				}
			}
		}
	}

	//get max rensa
	double C1_4_VALUE_POLICY = 1.5;
	double standardAddCount = log(ori.getFieldSpace()) * C1_4_VALUE_POLICY;
	for (int i = 0; i < scores.size(); i++) {
		double prevScore = i == 0 ? 0 : scores[i - 1].second;
		double dScore = scores[i].second - prevScore;
		double addValue = (double)(scores[i].first) / standardAddCount / 2 / 2;
		score += ValueFunc(addValue) * dScore; // i+1 = addedPuyoCount
	}
	return score;
}


//thanks for 마소빌

//original node, depth, childs array
void Detector::testChildNode(ChildNode n, int depth, vector<ChildNode>& childs, Simulator ori) {
	//노출된 뿌요 뭉치
	Plan plan;
	Simulator s = Simulator(-1);
	ChildNode* maxNode = &n;
	bool isOriNode = true;
	if (childs.size() != 0) {
		if (true) {
			double maxNodeScore = -9999;
			for (int i = 0; i < childs.size(); i++)
			{
				double score = childNodeScore(childs[i], ori);
				if (childs[i].isSearched == false && score > maxNodeScore) {
					maxNodeScore = score;
					maxNode = &childs[i];
					isOriNode = false;
				}
			}
			if (maxNode->isSearched == true) cout << "searched" << endl;
		}
		else {
			while (maxNode->isSearched == true)
				maxNode = &childs[rand() % childs.size()];
		}
	}

	s = maxNode->s;
	maxNode->isSearched = true;
	s.setScore(maxNode->score);

	int(*oriField)[6] = (int(*)[6])ori.getField();
	int(*maxField)[6] = (int(*)[6])maxNode->s.getField();
	int visitField[13][6] = { 0 };
	memcpy(visitField, oriField, 13 * 6 * sizeof(int));

	int heightLimit[] = { 13,13,11,13,13,13 };

	auto r = plan.findOutSide(s);//뿌요 뭉치 바깥 좌표
	//발화점 찾기
	if (isOriNode == true) {
		for (int i = 0; i < r.size(); i++) {
			//int i = rand() % r.size();
			//노출된 뿌요 뭉치(개수와 좌표)
			bool visit[13][6] = { false };
			int x = r[i].first;
			int y = r[i].second;
			int orix = x;
			int oriy = y;
			int color = maxField[y][x];
			if (visitField[y][x] == 0) continue;
			if (color == 0 || color >= 6) continue;
			std::vector<std::pair<int, int>> popGroup;
			Simulator::dfs(maxField, y, x, color, visit, popGroup);
			int needPuyoCnt = 4 - popGroup.size();
			for (int l = 0; l < popGroup.size(); l++) {
				if (s.checkFieldIndex(popGroup[l].second, popGroup[l].first) == false) cout << " error " << endl;
				visitField[popGroup[l].first][popGroup[l].second] = 0;
			}

			//뿌요 추가
			int di[] = { -1,0,1,0 };
			int dj[] = { 0,1,0,-1 };

			switch (needPuyoCnt) {
			case 1:
				for (int a = 0; a < 4; a++) {
					x = orix;
					y = oriy;
					Simulator ss(s);
					int(*ssf)[6] = (int(*)[6])ss.getField();
					if (x + dj[a] >= 0 && x + dj[a] < 6 && y + di[a]>0 && y + di[a] <= 12) {

						if (ssf[y + di[a]][x + dj[a]] == 0) {
							ssf[y + di[a]][x + dj[a]] = color;
						}
						else {
							//끼워넣기 오리지널 필드가 아니면
							if (false && oriField[y + di[a]][x + dj[a]] == 0) {
								ss.applyAntiGravityTargetOnce(x + dj[a], y + di[a]);
								ssf[y + di[a]][x + dj[a]] = color;
							}
							else {
								continue;
							}
						}
						int needBottom = 0;
						needBottom = ss.applyGravityFix();
						if (needBottom >= 4) continue; // 바닥 4개 이상 금지


						int* h = ss.getFieldHeight();//높이 넘으면 불가
						//하치이치 전용 발화점 만들기 금지
						bool isHeightLimitOver = false;
						for (int k = 0; k < 6; k++) {
							if (h[k] >= heightLimit[k]) {
								isHeightLimitOver = true;
								break;
							}
						}
						if (isHeightLimitOver == true) continue;
						ssf[y + di[a]][x + dj[a]] = 0;
						if (ss.pushFastNoPop(x + dj[a], PuyoTsumo(0, color)) == false) continue;//놓기 불가 금지
						Simulator backup = Simulator(ss);
						int rensa = 1;
						int score = 0;
						ss.applyChainPop(rensa, score);
						ss.appendScore(score);
						vector<pair<int, int>> fireCoord = { make_pair(x + dj[a], y + di[a]) };
						auto result = ChildNode(backup, maxNode->addedPuyoCnt + needPuyoCnt * 2 + needBottom, rensa, ss.getScore(), fireCoord);
						childs.push_back(result);
					}
				}
				break;
			case 2:
				for (int a = 0; a < 4; a++) {
					x = orix + dj[a];
					y = oriy + di[a];
					Simulator ss(s);
					int(*ssf)[6] = (int(*)[6])ss.getField();
					if (x >= 0 && x < 6 && y>0 && y <= 12) {
						if (ssf[y][x] == 0) {
							ssf[y][x] = color;
						}
						else {
							//끼워넣기 오리지널 필드가 아니면
							if (oriField[y][x] == 0) {
								ss.applyAntiGravityTargetOnce(x, y);
								ssf[y][x] = color;
							}
							else {
								continue;
							}
						}
						for (int b = 0; b < 4; b++) {
							x = orix + dj[a] + dj[b];
							y = oriy + di[a] + di[b];
							Simulator sss(ss);
							int(*sssf)[6] = (int(*)[6])sss.getField();
							if (x >= 0 && x < 6 && y>0 && y <= 12) {
								if (ssf[y][x] == 0) {
									ssf[y][x] = color;
								}
								else {
									//끼워넣기 오리지널 필드가 아니면
									if (false && oriField[y][x] == 0) {
										ss.applyAntiGravityTargetOnce(x, y);
										ssf[y][x] = color;
									}
									else {
										continue;
									}
								}
								int rensa = 1;
								int score = 0;
								int needBottom = 0;
								needBottom = sss.applyGravityFix();
								if (needBottom >= 4) continue;
								int* h = sss.getFieldHeight();//높이 넘으면 불가
								bool isHeightLimitOver = false;
								for (int k = 0; k < 6; k++) {
									if (h[k] >= heightLimit[k]) {
										isHeightLimitOver = true;
										break;
									}
								}
								if (isHeightLimitOver == true) continue;
								sssf[y][x] = 0;
								if (sss.pushFastNoPop(x, PuyoTsumo(0, color)) == false) continue;//놓기 불가 금지
								Simulator backup = Simulator(sss);
								sss.applyChainPop(rensa, score);
								sss.appendScore(score);
								vector<pair<int, int>> fireCoord = { make_pair(orix + dj[a], oriy + di[a]), make_pair(orix + dj[a] + dj[b], oriy + di[a] + di[b]) };
								auto result = ChildNode(backup, maxNode->addedPuyoCnt + needPuyoCnt * 2 + needBottom, rensa, sss.getScore(), fireCoord);
								childs.push_back(result);
							}
						}
					}
				}
				break;
			case 4: //
				for (int a = 0; a < 4; a++) {
					x = orix + dj[a];
					y = oriy + di[a];
					Simulator ss(s);
					int(*ssf)[6] = (int(*)[6])ss.getField();
					if (x >= 0 && x < 6 && y>0 && y <= 12) {
						if (ssf[y][x] == 0) {
							ssf[y][x] = color;
						}
						else {
							//끼워넣기 오리지널 필드가 아니면
							if (false && oriField[y][x] == 0) {
								ss.applyAntiGravityTargetOnce(x, y);
								ssf[y][x] = color;
							}
							else {
								continue;
							}
						}
						for (int b = 0; b < 4; b++) {
							x = orix + dj[a] + dj[b];
							y = oriy + di[a] + di[b];
							Simulator sss(ss);
							int(*sssf)[6] = (int(*)[6])sss.getField();
							if (x >= 0 && x < 6 && y>0 && y <= 12) {
								if (ssf[y][x] == 0) {
									ssf[y][x] = color;
								}
								else {
									//끼워넣기 오리지널 필드가 아니면
									if (oriField[y][x] == 0) {
										ss.applyAntiGravityTargetOnce(x, y);
										ssf[y][x] = color;
									}
									else {
										continue;
									}
								}
								for (int c = 0; c < 4; c++) {
									x = orix + dj[a] + dj[b] + dj[c];
									y = oriy + di[a] + di[b] + di[c];
									Simulator ssss(sss);
									int(*ssssf)[6] = (int(*)[6])ssss.getField();
									if (x >= 0 && x < 6 && y>0 && y <= 12) {
										if (ssf[y][x] == 0) {
											ssf[y][x] = color;
										}
										else {
											//끼워넣기 오리지널 필드가 아니면
											if (oriField[y][x] == 0) {
												ss.applyAntiGravityTargetOnce(x, y);
												ssf[y][x] = color;
											}
											else {
												continue;
											}
										}
										int rensa = 1;
										int score = 0;
										int needBottom = 0;
										needBottom = ssss.applyGravityFix();
										if (needBottom >= 4) continue; // 바닥 4개 이상 금지
										int* h = ssss.getFieldHeight();//높이 넘으면 불가
										bool isHeightLimitOver = false;
										for (int k = 0; k < 6; k++) {
											if (h[k] >= heightLimit[k]) {
												isHeightLimitOver = true;
												break;
											}
										}
										if (isHeightLimitOver == true) continue;
										ssssf[y][x] = 0;
										if (ssss.pushFastNoPop(x, PuyoTsumo(0, color)) == false) continue;//놓기 불가 금지
										Simulator backup = Simulator(ssss);
										ssss.applyChainPop(rensa, score);
										ssss.appendScore(score);
										vector<pair<int, int>> fireCoord = { make_pair(orix + dj[a], oriy + di[a]), make_pair(orix + dj[a] + dj[b], oriy + di[a] + di[b])
	, make_pair(orix + dj[a] + dj[b] + dj[c], oriy + di[a] + di[b] + di[c]) };
										auto result = ChildNode(backup, maxNode->addedPuyoCnt + 3 * 2 + needBottom, rensa, ssss.getScore(), fireCoord);
										childs.push_back(result);
									}
								}
							}
						}
					}
				}
				break;
			}
		}

	}
	return;
}

double Detector::RensaToScore(double rensa)
{
	bool flag = false;

	double STANDARD_SCORE[] = { 0, 40, 360, 1000, 2280, 4840, 8680, 13800, 20200, 27880,
				36840, 47080, 58600, 71400, 85480, 100840, 117480, 135400, 154600, 175080, 196840 };

	if (rensa < 0)
	{
		flag = true;
		rensa *= -1;
	}

	for (int i = 1; i < 21; i++)
	{
		if (rensa < i)
			if(flag)
				return -(STANDARD_SCORE[i - 1] + (STANDARD_SCORE[i] - STANDARD_SCORE[i - 1]) * (rensa + 1 - i));
			else
				return STANDARD_SCORE[i - 1] + (STANDARD_SCORE[i] - STANDARD_SCORE[i - 1]) * (rensa + 1 - i);
	}

	return STANDARD_SCORE[20];
}

double Detector::GetRensaScore(int score, double addedPuyoCount) {
	return ScoreToRensa(score) - addedPuyoCount / 2. / 4;
}

double Detector::ValueFunc(double x) {
	return exp(atan(x) - x);
}

double Detector::childNodeScore(ChildNode c, Simulator original) {
	if (false) {
		return 10. + c.rensa - (double)c.addedPuyoCnt * ((double)c.s.getFieldSpaceMax1());
	}
	else {
		double standardRensaScore = 0;
		double rensaScore = (c.rensa) - (double)c.addedPuyoCnt / 2.0 / 4.0;

		if (original.getColorCount() > 0)
		{
			double C1_4_VALUE_POLICY = 1.5;
			double oriStandardAddCount = log(original.getFieldSpace()) * C1_4_VALUE_POLICY;
			double sAddCount = oriStandardAddCount;
			double sEfficiency = 0.9;
			double maxAddScore = RensaToScore(c.rensa + (double)c.s.getColorCount() / (double)4) - RensaToScore(c.rensa);
			double maxRensaScore = ScoreToRensa((double)c.score + maxAddScore) - (double)c.addedPuyoCnt / (double)4. / 2.;

			if (maxRensaScore < 0.1)
				standardRensaScore = 0;
			else if (rensaScore / maxRensaScore >= sEfficiency)
				standardRensaScore = rensaScore * ValueFunc((double)c.addedPuyoCnt / sAddCount);
			else
			{
				double ratio = (double)c.s.getColorCount() / (double)original.getColorCount();
				double c1 = ratio * maxRensaScore * (1 - sEfficiency) / (maxRensaScore - rensaScore);
				double k1 = 1;
				double x = (double)c.addedPuyoCnt + pow(c1, -1 / k1) - pow(ratio, -1.0 / k1);
				standardRensaScore = maxRensaScore * sEfficiency * ValueFunc(x / sAddCount);

			}
		}

		return standardRensaScore;
	}
}

double Detector::ScoreToRensa(double score)
{
	double STANDARD_SCORE[] = { 0, 40, 360, 1000, 2280, 4840, 8680, 13800, 20200, 27880,
				36840, 47080, 58600, 71400, 85480, 100840, 117480, 135400, 154600, 175080, 196840 };

	if (score < STANDARD_SCORE[0]) return 0;

	for (int i = 1; i < 21; i++)
	{
		if (score < STANDARD_SCORE[i]) {
			return i - 1 + (score - STANDARD_SCORE[i - 1]) / (STANDARD_SCORE[i] - STANDARD_SCORE[i - 1]);
		}
	}

	return 20;
}
