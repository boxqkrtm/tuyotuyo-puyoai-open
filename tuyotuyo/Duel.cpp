#include "Duel.h"
#include "Detector.h"

#define STATE_DROP_PUYO 0
#define STATE_PLACE_PUYO 1
#define STATE_POP_PUYO 2
#define STATE_DEALAY_PUYO 3
#define STATE_OJAMA_DROP 4

Duel::~Duel()
{
	resetOjamaSim();
}

int Duel::getState(int player)
{
	return _boardState[player];
}

Duel::Duel(GameInfo g)
{
	//init
	int sed = rand();
	reset(sed);
	Simulator tmp(sed);
	_enermyScore[0] = 0;
	_enermyScore[1] = 0;
	//1p
	tmp.setNext(g._myNext);
	tmp.set(g._myField, g._my14remove);
	_boardState[0] = tmp.applyGravityOnce() > 0 ? STATE_DROP_PUYO : STATE_POP_PUYO;
	tmp.set(g._myField, g._my14remove);
	_p[0] = Simulator(tmp);

	//2p
	tmp.setNext(g._oppNext);
	tmp.set(g._oppField, g._opp14remove);
	_boardState[0] = tmp.applyGravityOnce() > 0 ? STATE_DROP_PUYO : STATE_POP_PUYO;
	tmp.set(g._oppField, g._opp14remove);
	_p[1] = Simulator(tmp);

	_seed = 0;
	_frame = 0;
	_pPendingOjama[0] = g._myOjama;
	_pPendingOjama[1] = g._oppOjama;
}

void Duel::RandUnknownData()
{
	//모르는 색패정보 섞음
#ifdef __linux__
	int maxIndex = std::max((int)_p[0]._bagindex,(int) _p[1]._bagindex);
#else
	int maxIndex = max((int)_p[0]._bagindex, (int)_p[1]._bagindex);
#endif
	if (maxIndex + 2 >= 128)
		return;

	//shuffle
	for (int j = 0; j < 2; j++)
	{
		for (int i = maxIndex + 2; i < 128; i++)
		{
			int temp;
			int r = maxIndex + 2 + (rand() % (128 - (maxIndex + 2)));
			temp = _p[j]._bag[i].up;
			_p[j]._bag[i].up = _p[j]._bag[r].down;
			_p[j]._bag[r].down = temp;
		}
	}
	//TODO
}

Duel::Duel(Duel *d)
{
	//init
	_p[0] = Simulator(d->getSimulator(0));
	_p[1] = Simulator(d->getSimulator(1));
	_seed = d->_seed;
	_frame = d->_frame;
	_pframe[0] = _pframe[0];
	_pframe[1] = _pframe[1];
	memcpy(_frameEvent, d->_frameEvent, sizeof(int) * 2);
	memcpy(_pDelay, d->_pDelay, sizeof(int) * 2);
	memcpy(_pBeforeDelay, d->_pBeforeDelay, sizeof(int) * 2);
	memcpy(_inputTable, d->_inputTable, sizeof(int) * 2);
	memcpy(_pRensa, d->_pRensa, sizeof(int) * 2);
	memcpy(_pIsPop, d->_pIsPop, sizeof(bool) * 2);
	memcpy(_pWin, d->_pWin, sizeof(bool) * 2);
	_isGameEnd = d->_isGameEnd;
	memcpy(_boardState, d->_boardState, sizeof(int) * 2);
	memcpy(_pIsOjamaDrop, d->_pIsOjamaDrop, sizeof(bool) * 2);
	memcpy(_pDroppedOjama, d->_pDroppedOjama, sizeof(int) * 2);
	memcpy(_pIsAllClear, d->_pIsAllClear, sizeof(bool) * 2);
	memcpy(_pIsSoftDrop, d->_pIsSoftDrop, sizeof(bool) * 2);
	memcpy(_pSoftdropBonus, d->_pSoftdropBonus, sizeof(int) * 2);
	memcpy(_pPendingOjama, d->_pPendingOjama, sizeof(int) * 2);
	_isToko = d->_isToko;
	memcpy(_nowRensaOjamaCount, d->_nowRensaOjamaCount, sizeof(int) * 2);
	for (int i = 0; i < 20; i++)
	{
		ojamaSim[i] = NULL;
	}
}

Duel::Duel(int seed)
{
	_isToko = false;
	reset(seed);
}

void Duel::input(int p1, int p2)
{
	_inputTable[0] = p1;
	_inputTable[1] = p2;
}

void Duel::delayToBoardState(int i)
{
	if (_pDelay[i] == 0)
	{ //if no delay droppuyo or poppuyo
		_boardState[i] = STATE_DROP_PUYO;
	}
	else if (_pDelay[i] > 0)
	{ //if delay wait
		_boardState[i] = STATE_DEALAY_PUYO;
	}
	else if (_pDelay[i] == -1)
	{ //if need input
		_boardState[i] = STATE_PLACE_PUYO;
	}
	if (_pIsOjamaDrop[i] == true)
	{
		_boardState[i] = STATE_OJAMA_DROP;
	}
}

void Duel::stateDropPuyo(int i)
{
	int dropcount = _p[i].applyGravityOnce();
	if (dropcount > 0)
	{
		//normal softdrop
		if (_pIsSoftDrop[i] == true && dropcount == 2)
		{
			_pDelay[i] = DELAY_DROP;
			_pSoftdropBonus[i] += 1;
			_p[i].appendScore(1);
		}
		//chigiri drop
		else if (_pIsSoftDrop[i] == true && dropcount == 1)
		{
			_pDelay[i] = DELAY_CHIGIRI;
		}
		else
		{
			_pDelay[i] = DELAY_DROP;
		}
	}
	else
	{ //has no floating puyos
		if (_pIsSoftDrop[i] == true)
		{											   //save frame at placed
			_frameEvent[i] = _pframe[i] + DELAY_PLACE; //놓고 난 후 다음 뽑을 때의 프레임
			_pDelay[i] = DELAY_PLACE;
		}
		_pIsSoftDrop[i] = false;
		if (_pDelay[i] == 0)
		{
			_boardState[i] = STATE_POP_PUYO;
		}
	}
}

void Duel::statePopPuyo(int i, int &retflag)
{
	int rate = 70;
	long time = _frame / 60;
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

	retflag = 1;
	//chaining puyo
	if ( _pRensa[i] == 0) //1연쇄 확인 후 다 터지고 난 후 프레임 미리 계산 시뮬레이션
	{
		Duel dd(_seed);
		dd.set(0, (int(*)[6])_p[i].getField());
		dd.setToko();
		while (dd.needInput(0) == false && _isToko == false)
		{
			if (dd.run() == false)
				break;
		}
		_frameEvent[i] = _pframe[i] + dd.getFrame();
		_enermyScore[(i + 1) % 2] = dd.getSimulator(i).getScore() - _p[i].getOjama() * rate;
	}
	int getScore = _p[i].applyPop(++_pRensa[i]);
	_p[i].appendScore(getScore);
	
	if (getScore == 0)
	{ //end pop
		_enermyScore[(i + 1) % 2] = _p[(i + 1) % 2].getOjama()*rate;
		if (_p[i].getGameover() == true)
		{
			_pWin[(i + 1) % 2] = true;
			_pIsPop[i] = false;
			_pDelay[i] = -2;
			retflag = 3;
			return;
		}
		//result rensa (_pRensa[i]-1)
		_pIsPop[i] = false;
		if (_pRensa[i] >= 1 + 1)
		{										 // 1rensa or above
			_p[(i + 1) % 2].ojamaInput(0, true); //end oajama input
			//_pPendingOjama[(i + 1) % 2] += _nowRensaOjamaCount[i];//this is will drop after place

			_pPendingOjama[(i + 1) % 2] = _p[(i + 1) % 2].getOjama(); //this is will drop after place
			_nowRensaOjamaCount[i] = 0;
			_pIsAllClear[i] = _p[i].getAllClear(); //allclear check
			if (_pIsAllClear[i] == true)
			{
				_p[i].appendScore(_allClearBonus);
				_pSoftdropBonus[i] += _allClearBonus;
			}
		}
		_maxRensa = _maxRensa < (_pRensa[i] - 1) ? _pRensa[i] - 1 : _maxRensa;
		_pRensa[i] = 0;
		//placed after
		if (_pPendingOjama[i] > 0)
		{
			Duel dd(_seed);
			dd.set(0, (int(*)[6])_p[i].getField());	  //필드복사
			dd._pIsOjamaDrop[i] = _pIsOjamaDrop[i];	  //방뿌 상태복사
			dd._pDroppedOjama[i] = _pDroppedOjama[i]; //방뿌 상태복사
			dd._pPendingOjama[i] = _pPendingOjama[i]; //방뿌 상태복사
			dd.setToko();
			while (dd.needInput(0) == false && _isToko == false)
			{
				if (dd.run() == false)
					break;
			}
			_frameEvent[i] = _pframe[i] + dd.getFrame();
			_pIsOjamaDrop[i] = true;
			_boardState[i] = STATE_OJAMA_DROP;
		}
		else
		{
			_pDelay[i] = -1; //to STATE_PLACE_PUYO nextframe;
		}
	}
	else if (getScore > 0)
	{	//more pop
		//calculate end rensa frame

		int attack = (getScore + _pSoftdropBonus[i]) / rate;
		_pSoftdropBonus[i] = 0;
		_pIsPop[i] = true;
		//Offset rule
		if (_p[i].getOjama() >= attack)
		{
			_p[i].ojamaInput(-attack);
			_pPendingOjama[i] += -attack;
		}
		else if (_p[i].getOjama() < attack)
		{
			attack -= _p[i].getOjama();
			_pIsOjamaDrop[i] = false;
			_pPendingOjama[i] = 0;
			_p[i].ojamaInput(-_p[i].getOjama());
			_nowRensaOjamaCount[i] += attack;
			_p[(i + 1) % 2].ojamaInput(attack, false);
		}
		_pDelay[i] = DELAY_POP;
	}
}

void Duel::createOjamaSim(int nowOjama, int &i, int a[6], int &ojamaSimIndex)
{
	bool dropLoc[6] = {false, false, false, false, false, false};
	for (int ocnt = 0; ocnt < nowOjama; ocnt++)
	{
		dropLoc[a[ocnt]] = true;
	}
	Duel *d = new Duel(*this);
	d->_p[i].applyOjamaDropOnce(dropLoc);
	ojamaSim[ojamaSimIndex++] = d;
}

void Duel::stateDropOjama(int i, int &retflag)
{
	retflag = 1;
	if (_pDelay[i] == 0)
	{
		int gravityCount = _p[i].applyGravityOnce();
		if (gravityCount > 0)
		{
			_pDelay[i] = DELAY_DROP;
		}
		if (_p[i].getOjama() == 0 //when zero ojama due to offset rule before ojama state
			|| _pDroppedOjama[i] == 30 || _pPendingOjama[i] == 0)
		{ //end drop
			if (gravityCount == 0)
			{
				_pIsOjamaDrop[i] = false;
				_pDroppedOjama[i] = 0;
				_pDelay[i] = -1; //to STATE_PLACE_PUYO next frame
					//game over check
				if (_p[i].getGameover() == true)
				{
					_pWin[(i + 1) % 2] = true;
					_pDelay[i] = -2;
					_pIsPop[i] = false;
					{
						retflag = 3;
						return;
					};
				}
			}
		}
		else
		{
			_p[i].ojamaInput(0, true);
			if (_isOjamaSim == i && _p[i].getOjama() <= 5 && _p[i].getOjama() >= 1)
			{
				int nowOjama = _p[i].getOjama();
				int ojamaSimIndex = 0;
				//for loop all ojama cobination
				int a[6] = {6, 6, 6, 6, 6, 6};
				for (a[0] = 0; a[0] < a[1] && nowOjama >= 1; a[0]++)
				{
					if (nowOjama == 1)
					{
						createOjamaSim(nowOjama, i, a, ojamaSimIndex);
						continue;
					}
					for (a[1] = a[0] + 1; a[1] < a[2] && nowOjama >= 2; a[1]++)
					{
						if (nowOjama == 2)
						{
							createOjamaSim(nowOjama, i, a, ojamaSimIndex);
							continue;
						}
						for (a[2] = a[1] + 1; a[2] < a[3] && nowOjama >= 3; a[2]++)
						{
							if (nowOjama == 3)
							{
								createOjamaSim(nowOjama, i, a, ojamaSimIndex);
								continue;
							}
							for (a[3] = a[2] + 1; a[3] < a[4] && nowOjama >= 4; a[3]++)
							{
								if (nowOjama == 4)
								{
									createOjamaSim(nowOjama, i, a, ojamaSimIndex);
									continue;
								}
								for (a[4] = a[3] + 1; a[4] < a[5] && nowOjama >= 5; a[4]++)
								{
									if (nowOjama == 5)
									{
										createOjamaSim(nowOjama, i, a, ojamaSimIndex);
										continue;
									}
								}
							}
						}
					}
				}
			}

			int droppedOjama = _p[i].applyOjamaDropOnce();
			_pPendingOjama[i] -= droppedOjama;
			_pDroppedOjama[i] += droppedOjama;
		}
	}
	else
	{
		if (_pDelay[i] > 0)
		{
			_pDelay[i]--;
			{
				retflag = 3;
				return;
			};
		}
	}
}

void Duel::statePlacePuyo(int i, int &retflag)
{
	retflag = 1;
	if (_isToko == true && i == 1)
	{
		retflag = 3;
		return;
	};
	//game over check
	//place puyo
	PuyoTsumo now = _p[i].getNowPuyo();
	if (_inputTable[i] == -2)
	{
		vector<int> move;
		for (int test = 0; test < 22; test++)
		{
			move.push_back(test);
		}
		for (int test = 0; test < 22; test++)
		{
			int random = rand() % move.size();
			_inputTable[i] = move[random];
			int r = _p[i].push(_inputTable[i], now);
			move.erase(move.begin() + random);
			if (r == false)
				continue;
			else
				break;
		}
	}
	else
	{
		int r = _p[i].push(_inputTable[i], now);
		if (r == false)
		{
			_p[i].push(2, now);
			cout << "error dual input error get" << _inputTable[i] << endl;
			system("pause");
		}
	}

	_pDelay[i] = 0;
	_pIsSoftDrop[i] = true;
}

void Duel::stateDelayPuyo(int i)
{
	//STATE DEALAY PUYO
	if (_pDelay[i] > 0)
	{
		_pDelay[i]--;
	}
}

bool Duel::run()
{
	int i;
	if (_isGameEnd == true)
		return false;
	_frame++;
	if (_inputTable[0] != -1)
		_pframe[0]++;
	if (_inputTable[1] != -1)
		_pframe[1]++;
	for (i = 0; i < 3; i++)
	{ //0=1p, 1=2p, 3=무승부 판정 확인
		if (i <= 1 && _inputTable[i] == -1)
		{
			continue; //인풋이 -1이면 보드를 연산하지 않고 스킵함
		}
		if (i == 2)
		{
			if ((_pWin[0] == true || _pWin[1] == true))
			{
				_isGameEnd = true;
				return false;
			}
			break;
		}

		delayToBoardState(i);

		//Set action
		if (_boardState[i] == STATE_DROP_PUYO)
		{
			stateDropPuyo(i);
		}

		if (_boardState[i] == STATE_POP_PUYO)
		{
			int retflag;
			statePopPuyo(i, retflag);
			if (retflag == 3)
				continue;
		}

		if (_boardState[i] == STATE_OJAMA_DROP)
		{
			int retflag;
			stateDropOjama(i, retflag);
			if (retflag == 3)
				continue;
		}

		if (_boardState[i] == STATE_PLACE_PUYO)
		{
			int retflag;
			statePlacePuyo(i, retflag);
			if (retflag == 3)
				continue;
		}

		stateDelayPuyo(i);
	}
	return !_isGameEnd;
}

void Duel::print()
{
	textcolor(WHITE, BLACK);
	int(*p0f)[6] = (int(*)[6])_p[0].getField();
	int(*p1f)[6] = (int(*)[6])_p[1].getField();
	int i;
	int j;
	std::cout << "ojama " << _p[0].getOjama();
	//std::cout << "pojama " << _pPendingOjama[0];
	cout << "       ";
	std::cout << "ojama " << _p[1].getOjama() << "        " << std::endl;
	for (i = 0; i < 13; i++)
	{
		for (j = 0; j < 6; j++)
		{
			const char *p = "_";
			switch (p0f[i][j])
			{
			case 0:
				textcolor(DARKGRAY, BLACK);
				p = "_";
				if (i == 1 && j == 2)
				{
					textcolor(RED, BLACK);
					p = "X";
				}
				break;
			case 1:
				textcolor(RED, BLACK);
				p = "R";
				break;
			case 2:
				textcolor(GREEN, BLACK);
				p = "G";
				break;
			case 3:
				textcolor(BLUE, BLACK);
				p = "B";
				break;
			case 4:
				textcolor(YELLOW, BLACK);
				p = "Y";
				break;
			case 5:
				textcolor(MAGENTA, BLACK);
				p = "P";
				break;
			case 6:
				textcolor(LIGHTGRAY, BLACK);
				p = "O";
				break;
			}
			std::cout << p;
		}
		cout << "       ";
		for (j = 0; j < 6; j++)
		{
			const char *p = "_";
			switch (p1f[i][j])
			{
			case 0:
				textcolor(DARKGRAY, BLACK);
				p = "_";
				if (i == 1 && j == 2)
				{
					textcolor(RED, BLACK);
					p = "X";
				}
				break;
			case 1:
				textcolor(RED, BLACK);
				p = "R";
				break;
			case 2:
				textcolor(GREEN, BLACK);
				p = "G";
				break;
			case 3:
				textcolor(BLUE, BLACK);
				p = "B";
				break;
			case 4:
				textcolor(YELLOW, BLACK);
				p = "Y";
				break;
			case 5:
				textcolor(MAGENTA, BLACK);
				p = "P";
				break;
			case 6:
				textcolor(LIGHTGRAY, BLACK);
				p = "O";
				break;
			}
			std::cout << p;
		}
		std::cout << std::endl;
	}
	textcolor(WHITE, BLACK);
	cout << "score " << _p[0].getScore();
	cout << "       ";
	cout << "score " << _p[1].getScore() << "        " << endl;

	if (_isGameEnd == true)
	{
		if (_pWin[0] == true && _pWin[1] == true)
		{
			cout << "draw" << endl;
		}
		else if (_pWin[0] == true)
		{
			cout << "1p"
				 << " win" << endl;
		}
		else if (_pWin[1] == true)
		{
			cout << "2p"
				 << " win" << endl;
		}
	}
	textcolor(WHITE, BLACK);
}

void Duel::reset(int seed)
{
	int i;
	_seed = seed;
	_frame = 0;
	for (i = 0; i < 2; i++)
	{
		_pDelay[i] = 0;
		_pframe[i] = 0;
		_pBeforeDelay[i] = -1;
		_inputTable[i] = 2;
		_pRensa[i] = 0;
		_pIsPop[i] = false;
		_p[i].reset(seed);
		_pIsOjamaDrop[i] = false;
		_pDroppedOjama[i] = 0;
		_pIsAllClear[i] = false;
		_pSoftdropBonus[i] = 0;
		_pIsSoftDrop[i] = false;
		_pPendingOjama[i] = 0;
		_pWin[i] = false;
		_frameEvent[i] = 0;
		_nowRensaOjamaCount[i] = 0;
	}
	_isGameEnd = false;
	_isOjamaSim = -1;
	//_p[1].set(CHAINFORM_GTR);
	for (int i = 0; i < 20; i++)
	{
		ojamaSim[i] = NULL;
	}
}

void Duel::set(int player, int puyoField[13][6])
{
	_p[player].set(puyoField);
}

void Duel::set(int player, int puyoField[13][6], bool remove14[6])
{
	_p[player].set(puyoField, remove14);
}

bool *Duel::getWin()
{
	return _pWin;
}

GameInfo Duel::getGameinfo(int playerNum)
{
	PuyoTsumo *p0next = _p[0].getNext();
	PuyoTsumo *p1next = _p[1].getNext();
	GameInfo g;
	switch (playerNum)
	{
	case 0:
		if (_isToko == true)
		{
			static Simulator temp = Simulator(0);
			return GameInfo(_p[0].getField(), temp.getField(), _p[0].get14Field(), temp.get14Field(), _pIsAllClear[0], _pIsAllClear[1], p0next, p1next, _p[0].getOjama(), _p[1].getOjama(), _frameEvent[0], _frameEvent[1], _frame/60);
		}
		else
		{
			g = GameInfo(_p[0].getField(), _p[1].getField(), _p[0].get14Field(), _p[1].get14Field(), _pIsAllClear[0], _pIsAllClear[1], p0next, p1next, _p[0].getOjama(), _p[1].getOjama(), _frameEvent[0], _frameEvent[1], _frame / 60);
			g._enermyScore = _enermyScore[0];
			return g;
		}
		break;
	case 1:
		g= GameInfo(_p[1].getField(), _p[0].getField(), _p[1].get14Field(), _p[0].get14Field(), _pIsAllClear[1], _pIsAllClear[0], p1next, p0next, _p[1].getOjama(), _p[0].getOjama(), _frameEvent[1], _frameEvent[0], _frame / 60);
		g._enermyScore = _enermyScore[1];
		return g;
		break;
	default:
		return GameInfo(_p[0].getField(), _p[1].getField(), _p[0].get14Field(), _p[1].get14Field(), _pIsAllClear[0], _pIsAllClear[1], p0next, p1next, _p[0].getOjama(), _p[1].getOjama(), _frameEvent[0], _frameEvent[1], _frame / 60);
		break;
	}
	delete p0next;
	delete p1next;
}

int Duel::getFrame()
{
	return _frame;
}

bool Duel::needInput(int player)
{
	if (player == 0 && _inputTable[0] == -1)
		return false; //1p 유저가 스킵상태이면 입력이 필요 없음
	if (player == 1 && _inputTable[1] == -1)
		return false; //2p 유저가 스킵상태이면 입력 필요없음
	if (_pDelay[player] == -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Duel::setToko()
{
	_isToko = true;
}

void Duel::setScore(int player, int score)
{
	_p[player].setScore(score);
}

int Duel::getFrameEvent(int player)
{
	return _frameEvent[player];
}

bool Duel::inputTest(int player, int input)
{
	Simulator s(-1);
	s.set((int(*)[6])_p[player].getField(), (bool *)_p[player].get14Field());
	return s.pushFast(input, PuyoTsumo(1, 1));
}

Simulator Duel::getSimulator(int player)
{
	return _p[player];
}

void Duel::setOjama(int player, int ojama)
{
	_p[player].ojamaInput(-_p[player].getOjama());
	_p[player].ojamaInput(ojama);
}

void Duel::setNext(int player, PuyoTsumo next[2])
{
	_p[player].setNext(next);
}

void Duel::setAllClear(int player, bool isAllClear)
{
	_pIsAllClear[player] = isAllClear;
}

void Duel::setIsOjamaSim(int player)
{
	_isOjamaSim = player;
}

bool Duel::hasOjamaSim()
{
	if (ojamaSim[0] != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Duel **Duel::getOjamaSim()
{
	return ojamaSim;
}

void Duel::resetOjamaSim()
{
	for (int i = 0; i < 20; i++)
	{
		if (ojamaSim[i] != NULL)
		{
			ojamaSim[i]->~Duel();
		}
		ojamaSim[i] = NULL;
	}
}

void Duel::intSwap(int &a, int &b)
{
	int tmp;
	tmp = a;
	a = b;
	b = tmp;
}

void Duel::boolSwap(bool &a, bool &b)
{
	int tmp;
	tmp = a;
	a = b;
	b = tmp;
}

void Duel::swap()
{
	int _seed;
	int _frame;
	intSwap(_pframe[0], _pframe[1]);
	intSwap(_frameEvent[0], _frameEvent[1]);
	intSwap(_pDelay[0], _pDelay[1]);
	intSwap(_pBeforeDelay[0], _pBeforeDelay[1]);
	intSwap(_inputTable[0], _inputTable[1]);
	intSwap(_pRensa[0], _pRensa[1]);
	boolSwap(_pIsPop[0], _pIsPop[1]);
	boolSwap(_pWin[0], _pWin[1]);
	intSwap(_boardState[0], _boardState[1]);
	boolSwap(_pIsOjamaDrop[0], _pIsOjamaDrop[1]);
	intSwap(_pDroppedOjama[0], _pDroppedOjama[1]);
	boolSwap(_pIsAllClear[0], _pIsAllClear[1]);
	boolSwap(_pIsSoftDrop[0], _pIsSoftDrop[1]);
	intSwap(_pSoftdropBonus[0], _pSoftdropBonus[1]);
	intSwap(_pPendingOjama[0], _pPendingOjama[1]);
	intSwap(_nowRensaOjamaCount[0], _nowRensaOjamaCount[1]);
	auto tmp = _p[0];
	_p[0] = _p[1];
	_p[1] = tmp;
}
