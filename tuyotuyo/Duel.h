#pragma once

#include "Simulator.h"
#include "GameInfo.h"
#include "CrossFunc.h"
#include <iostream>
#include "PuyoConst.h"
using std::cout;
using std::endl;

/* example usage
int i;
int pw[2];
srand(time(NULL));

Duel d = Duel(rand());
pw[0] = 0;
pw[1] = 0;

for (i = 0; i < 1;) {
	d.input(rand() % 3 * 10 + rand() % 6, rand() % 3 * 10 + rand() % 6);
	bool r = d.run();

	d.print();
	if (r == false) {
		pw[d.getWin()]++;
		d.reset(rand());
		i++;
		cout << pw[0] << ":" << pw[1] << endl;
	}
	Sleep(16);

}
cout << "end " << pw[0] << ":" << pw[1] << endl;
Sleep(100000);
*/

class Duel {
private:
	//init
	
	int _frame;
	int _pframe[2];//1p or 2p를 -1입력으로 쉬게 만들었을 때 다른 한쪽의 frameEvent(다음 수를 뽑는 프레임)값을 원할하게 하기 위한 변수
	int _frameEvent[2];
	int _pDelay[2];//n player's remain delay
	int _pBeforeDelay[2];
	int _inputTable[2];
	int _pRensa[2];
	bool _pIsPop[2];
	bool _pWin[2];
	bool _isGameEnd;
	int _boardState[2];
	bool _pIsOjamaDrop[2];
	int _pDroppedOjama[2];
	bool _pIsAllClear[2];
	bool _pIsSoftDrop[2];
	int _pSoftdropBonus[2];//softdrop bonus allclaer etc
	int _pPendingOjama[2];
	int const _allClearBonus = 2100;
	int _isOjamaSim = -1;
	bool _isToko;
	int _enermyScore[2] = { 0 };
	Duel *ojamaSim[20];//3=20
	//delay(frame)
	int _nowRensaOjamaCount[2]; //현재 연쇄로 보낼 방뿌의 양(상대가 하나놓으면 바로 맞을 양)
	static const int DELAY_FREEFALL = 4;
	static const int DELAY_MOVE = 1;
	static const int DELAY_ROTATE = 2;

	static const int DELAY_DROP = 2;
	static const int DELAY_CHIGIRI = 4;
	static const int DELAY_PLACE = 16;
	static const int DELAY_POP = 32;

	void delayToBoardState(int i);
	void stateDropPuyo(int i);
	void statePopPuyo(int i, int& retflag);
	void createOjamaSim(int nowOjama, int& i, int  a[6], int& ojamaSimIndex);
	void stateDropOjama(int i, int& retflag);
	void statePlacePuyo(int i, int& retflag);
	void stateDelayPuyo(int i);
public:
	int _seed;
	Simulator _p[2] = { Simulator(0),Simulator(0) };
	~Duel();
	int getState(int player);
	Duel(GameInfo g);
	void RandUnknownData();
	Duel(Duel* d);
	Duel(int seed);
	void input(int p1, int p2);
	int _maxRensa = 0;

	//same as simulator's push
	bool run();
	void print();
	void reset(int seed);
	void set(int player, int puyoField[13][6]);
	void set(int player, int puyoField[13][6], bool remove14[6]);
	bool* getWin();
	GameInfo getGameinfo(int playerNum);
	int getFrame();
	bool needInput(int player);
	void setToko();
	void setScore(int player, int score);
	int getFrameEvent(int player);
	bool inputTest(int player, int input);
	Simulator getSimulator(int player);
	void setOjama(int player, int ojama);
	void setNext(int player, PuyoTsumo next1[2]);
	void setAllClear(int player, bool isAllClear);
	void setIsOjamaSim(int player);
	bool hasOjamaSim();
	Duel** getOjamaSim();
	void resetOjamaSim();
	void swap();
	void intSwap(int& a, int& b);
	void boolSwap(bool& a, bool& b);
};
//getState -> think -> input -> run
