#include "DuelOptimizer.h"
#include "FormAIv2.h"
bool resultCompare(pair<AI*, int> a, pair<AI*, int> b) {
	return a.second > b.second ? true : false;
}

int battle(AI *bestAI, AI *challengerAI) {
	int start = clock();
	int pw[2];
	int seed = rand();
	//seed = 0;
	Duel d = Duel(seed);
	pw[0] = 0;
	pw[1] = 0;
	bool watch = true;
	int ft = 1;
	int succeedCnt = 0;
	{
		//if (succeedCnt > 5000) { watch = true; }
		while (pw[0] != ft && pw[1] != ft)
		{
			bestAI->reset();
			challengerAI->reset();
			int i1 = 0;
			int i2 = 0;
			while (true)
			{
				if (d.needInput(0) == true)
				{
					i1 = bestAI->think(d.getGameinfo(0));
					//i1 = save->think(d.getGameinfo(0));
				}
				if (d.needInput(1) == true)
				{
					i2 = challengerAI->think(d.getGameinfo(1));
				}
				d.input(i1, i2);
				bool r = d.run();

				/*if (d.getFrame() % 600 == 0)
				{
					gotoxy(0, 2);
					d.print();
					cout << pw[0] << ":" << pw[1] << endl;
					Sleep(1);
				}*/

				if (r == false || d.getFrame() >= 60*60*5)
				{
					//gotoxy(0, 2);
					//d.print();
					pw[0] += d.getWin()[0];
					pw[1] += d.getWin()[1];
					if (d.getWin()[0] == 0 && d.getWin()[1] == 0) {
						pw[0]++;
						pw[1]++;
					}
					d.reset(rand());
					//cout << pw[0] << ":" << pw[1] << endl;
					break;
				}
			}
			//경기 end

		}
		//cout << pw[0] << ":" << pw[1] << endl;
		//cout << "완료 " << ((float)(clock() - start) / CLOCKS_PER_SEC) << "초" << endl;

	}
	if (pw[0] < pw[1])
	{
		return 0;
	}
	else if (pw[0] > pw[1])
	{
		return 1;
	}
	else {
		return -1;
	}
	pw[0] = 0;
	pw[1] = 0;
}

int toko(FormAIv2* challengerAI, bool view = false) {

	//srand(time(NULL));
	int ascore = 0;
	for (int i = 0; i < 10; i++) {
		challengerAI->reset();
		challengerAI->_print = false;
		challengerAI->AI_SEE = false;
		challengerAI->AI_SURPRISE_ATTACK = false;
		//cout << ".";
		Duel d = Duel(10372 + i);
		//Duel d = Duel(rand());
		d.setToko();
		while (1) {
			if (d.needInput(0) == true) {
				int a = challengerAI->think(d.getGameinfo(0));
				d.input(a, 0);
			}
			if (d.run() == false || d.getSimulator(0)._bagindex >= 70 || d._maxRensa>=5) {
				if (true) {
					//gotoxy(0, 0);
					//d.print();
				}
				//if (d.getSimulator(0).getScore() > ascore)
				/*int tmp = 0;
				cout << "input score : ";
				cin >> tmp;
				ascore += tmp;*/
				ascore += d.getSimulator(0).getScore();
				
				
				break;
			}
		}
	}
	//cout << "input score : ";
	//cin >> ascore;
	//if (ascore <= 0) ascore = 1;
	return ascore/10;
}

int rand100() {
	int s = 0;
	for (int i = 0; i < 100; i++) s += rand();
	return s; 
}
void DuelOptimazer()
{
	vector<pair<AI*, int>> ais;
	int unitcnt = 7;

	//첫 후보들 랜덤생성..
	for (int i = 0; i < unitcnt; i++) {
		pair<AI*, int> tmp;
		tmp.first = new FormAIv2();
		//((FormAIv2*)tmp.first)->_multi = false;
		if (i != 0)
			tmp.first->randParams();
		//tmp.second = 100;
		tmp.second = 0;
		ais.push_back(tmp);
	}

	int gen = 0;

	while (1) {
		vector<pair<AI*, int>> ais2;
		//cout << "gen" << gen << " start" << endl;
		//대전
		//cout << endl;
//#pragma omp parallel for
			for (int i = 0; i < ais.size(); i++) {
				if (ais[i].second == 0) {
					ais[i].second = toko(static_cast<FormAIv2*>(ais[i].first));
					cout << "score : " << ais[i].second << endl;
				}

				//for (int j = i+1; j < ais.size(); j++) {
				//	int r = battle(ais[i].first, ais[j].first);
				//	float winrate1 = 1 / (pow(10, (ais[j].second - ais[i].second) / 400.f) + 1);
				//	float winrate2 = 1 - winrate1;
				//		if (r == 1) {
				//			//win
				//			ais[i].second = ais[i].second + 40 * (1 - winrate1);
				//			ais[j].second = ais[j].second + 40 * (0 - winrate2);
				//		}
				//		else if (r == 0) {
				//			//lose
				//			ais[i].second = ais[i].second + 40 * (0 - winrate1);
				//			ais[j].second = ais[j].second + 40 * (1 - winrate2);
				//		}
				//		else {
				//			//draw
				//		}
				//	}
				//cout << endl << i + 1 << "/" << ais.size() << endl;
			}
		//cout << endl;
		//cout << "battle complete" << endl;

		//정렬..
		sort(ais.begin(), ais.end(), resultCompare);

		int allscore = 0;
		for (const auto& a : ais) allscore += a.second;

		//자식생성
		//cout << "make children" << endl;
		for (int i = 0; i < unitcnt; i++) {
			pair<AI*, int> tmp;
			tmp.first = new FormAIv2();

			//확률로 돌연변이
			int muatation = 5;


			int p1 = -1;
			int p2 = -1;

			int ran = (rand100()) % allscore;
			int tmpallscore = 0;
			for (int j = 0; j < ais.size(); j++) {
				tmpallscore += ais[j].second;
				//룰렛 선택
				if (tmpallscore > ran) {
					p1 = j;
					break;
				}
			}
			p2 = p1;

			while (p1 == p2) {
				ran = (rand100()) % allscore;
				tmpallscore = 0;
				for (int j = 0; j < ais.size(); j++) {
					tmpallscore += ais[j].second;
					//룰렛 선택
					if (tmpallscore > ran) {
						p2 = j;
						break;
					}
				}
			}

			//cout << "parent " << p1 << " " << p2 << endl;



			for (int j = 0; j < 9; j++) {
				if (rand() % muatation == 0) {
					((FormAIv2*)tmp.first)->param[j] = (rand() % 10000) / 100.f;
				}
				else {
					int ran = rand() % allscore;
					int tmpallscore = 0;
					if (rand() % 2 == 0) ((FormAIv2*)tmp.first)->param[j] = ((FormAIv2*)ais[p1].first)->param[j];
					else ((FormAIv2*)tmp.first)->param[j] = ((FormAIv2*)ais[p2].first)->param[j];
				}
			}
			//tmp.second = (ais[p1].second + ais[p2].second) / 2;
			tmp.second = 0;
			ais2.push_back(tmp);
		}

		//세대교체(상위 그대로 사용)
		cout << "gen " << gen << " best " << "score:" << ais[0].second << endl;
		int top =3;
		ais[0].first->getParams();
		for (int i = top; i < ais.size(); i++) {
			delete ais[i].first;
		}
		for (int i = 0; i < top; i++) {
			ais[i].second = 0;
			ais2.push_back(ais[i]);
		}

		ais = ais2;
		gen++;
	}
}
