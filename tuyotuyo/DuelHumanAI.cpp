#include "DuelHumanAI.h"
#include "FormAIv2.h"


void DuelHumanAI()
{
	int pw[2];
	int seed = (unsigned)time(NULL);
	//seed = 3;
	srand(seed++);
	Duel d = Duel(rand());
	pw[0] = 0;
	pw[1] = 0;
	bool watch = true;
	int ft = 1;
	FormAIv2 ai2 = FormAIv2();
	ai2._print = false;
	ai2.AI_SEE = false;
	ai2.AI_SURPRISE_ATTACK = false;
	//SaveAI ai2 = SaveAI();
	while (1) {
		for (; pw[0] != ft && pw[1] != ft;) {
			ai2.reset();
			int i1 = 0;
			int i2 = 0;
			while (true) {
				if (d.needInput(0) == true) {
					i2 = ai2.think(d.getGameinfo(0));

					int ans = 2;
					while (1) {
						system("cls");
						gotoxy(25, 0);
						cout << "ai choice" << i2 << endl;
						gotoxy(0, 0);
						Simulator::puyoInputPreview(ans, d.getGameinfo(0)._myNext[0]);
						gotoxy(20, 0);
						cout << Simulator::numToPuyo(d.getGameinfo(0)._myNext[1].up) << " ";
						cout << Simulator::numToPuyo(d.getGameinfo(0)._myNext[2].up) << endl;
						textcolor(WHITE, BLACK);
						gotoxy(20, 1);
						cout << Simulator::numToPuyo(d.getGameinfo(0)._myNext[1].down) << " ";
						cout << Simulator::numToPuyo(d.getGameinfo(0)._myNext[2].down) << endl;
						textcolor(WHITE, BLACK);
						gotoxy(0, 2);
						d.print();
						int key = _getch();
						if (key == 75) {
							//left arrow
							if (ans % 10 > 0) {
								ans -= 1;
							}
							if (ans / 10 == 3 && ans % 10 == 0) {
								//wall kick
								ans += 1;
							}
						}
						else if (key == 77) {
							//right arrow
							if (ans % 10 < 5) {
								ans += 1;
							}
							if (ans / 10 == 1 && ans % 10 == 5) {
								//wall kick
								ans -= 1;
							}
						}
						else if (key == 'z') {
							//left rotation
							ans -= 10;
							if (ans < 0) {
								ans += 40;
							}
							if (ans / 10 == 3 && ans % 10 == 0) {
								//wall kick
								ans += 1;
							}
							if (ans / 10 == 1 && ans % 10 == 5) {
								//wall kick
								ans -= 1;
							}
						}
						else if (key == 'x') {
							//right rotation
							ans += 10;
							if (ans >= 40) {
								ans -= 40;
							}
							if (ans / 10 == 1 && ans % 10 == 5) {
								//wall kick
								ans -= 1;
							}
							if (ans / 10 == 3 && ans % 10 == 0) {
								//wall kick
								ans += 1;
							}
						}
						else if (key == 32) {
							break;
						}
						else if (key == 'a') {
							ans = i2;
							break;
						}
						else {
							//cout << "press" << key << "  " << endl;
						}
					}
					i1 = ans;
				}
				else {
				}
				if (d.needInput(0) == true) {
					cout << "request" << endl;
					i2 = ai2.think(d.getGameinfo(0));
				}
				d.input(i1, -1);
				bool r = d.run();

				if (d.getFrame() % 1 == 0 && watch == true) {
					gotoxy(0, 2);
					auto pre = d.getSimulator(0);
					//i2 = ai2.think(d.getGameinfo(1));
					pre.pushFast(i2, d.getGameinfo(0)._myNext[0]);
					d.set(1, (int(*)[6])pre.getField());
					d.print();
					cout << pw[0] << ":" << pw[1] << endl;
					cout << "        " << endl;
					cout << ai2.getDebugStr();
					cout << "frame" << d.getFrame() << "         " << endl;
					//cout << "eframe1p" << d.getFrameEvent(0) << " " << "eframe2p" << d.getFrameEvent(1) << "         " << endl;
					cout << "                                                                            " << endl;

					Sleep(1);
				}
				if (r == false) {
					pw[0] += d.getWin()[0];
					pw[1] += d.getWin()[1];
					srand(seed++);
					d.reset(rand());
					system("cls");
					cout << pw[0] << ":" << pw[1] << endl;
					cout << "last seed" << seed - 1 << endl;
					break;
				}
			}
		}
	}
}
