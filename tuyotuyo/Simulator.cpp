//
#include <iostream>
#include <vector>
#include <ctime>
#include <random>

//
#include "Simulator.h"
#include "PuyoTsumo.h"
#include "PuyoConst.h"
#include "CrossFunc.h"
#include "KeyStorkeGenerator.h"

//승룡 비허용 여부
//TODO 한쪽으로 올린 뒤 다른 한쪽으로 빠르게 이동하는 승룡 허용
//TODO 승룡 및 이동에 관한 최단거리 키 스트로크 얻기 기능 추가
//false 허용
//true 비허용
#define OVER_FIELD_DISABLE true

//
using std::cout;
using std::endl;
using std::vector;

void Simulator::applyOjamaDrop()
{
	if (_ojamaReady == false) return;
	int i, loop;
	bool dropLoc[6] = { false, false, false, false, false, false };

	if (_ojama > 0) {
		if (_ojama >= 30) {
			//drop 30(5line)
			for (loop = 0; loop < 5; loop++) {
				for (i = 0; i < 6; i++) {
					if (_puyoField[0][i] == 0) {
						_puyoField[0][i] = 6;
					}
				}
				applyGravity();
			}
			_ojama -= 30;
			//drop 30 end
		}
		else {
			//drop <30
			//drop line(>=10)
			for (loop = 0; loop < _ojama / 6; loop++) {
				for (i = 0; i < 6; i++) {
					if (_puyoField[0][i] == 0) {
						_puyoField[0][i] = 6;
					}
				}
				applyGravity();
			}
			//drop dot(<6)
			for (i = 0; i < _ojama % 6; i++) {
				dropLoc[i] = true;
			}

			//shuffle drop loc
			srand(time(NULL));//real random
			for (i = 0; i < _ojama % 6; i++) {
				int r = rand() % 6;
				int temp;
				temp = dropLoc[i];
				dropLoc[i] = dropLoc[r];
				dropLoc[r] = temp;
			}

			//drop
			for (i = 0; i < 6; i++) {
				if (_puyoField[0][i] == 0 && dropLoc[i] == true) {
					_puyoField[0][i] = 6;
				}
			}
			applyGravity();
			_ojama = 0;
			//all ojama drop end
		}
	}
}

int Simulator::applyOjamaDropOnce()
{
	if (_ojamaReady == false)return 0;
	int i;
	bool dropLoc[6] = { false, false, false, false, false, false };
	if (_ojama >= 6) {
		for (i = 0; i < 6; i++) {
			if (_puyoField[0][i] == 0) {
				_puyoField[0][i] = 6;
			}
		}
		_ojama -= 6;
		if (_ojama == 0) {
			_ojamaReady = false;
		}
		return 6;
	}
	if (_ojama < 6) {
		//drop dot(<6)
		for (i = 0; i < _ojama % 6; i++) {
			dropLoc[i] = true;
		}

		//shuffle drop loc
		for (i = 0; i < _ojama % 6; i++) {
			int r = rand() % 6;
			int temp;
			temp = dropLoc[i];
			dropLoc[i] = dropLoc[r];
			dropLoc[r] = temp;
		}

		//drop
		for (i = 0; i < 6; i++) {
			if (_puyoField[0][i] == 0 && dropLoc[i] == true) {
				_puyoField[0][i] = 6;
			}
		}
		int temp = _ojama;
		_ojama = 0;
		_ojamaReady = false;
		return temp;
	}
	return 0;
}


int Simulator::applyOjamaDropOnce(bool dropLoc[6])
{
	if (_ojamaReady == false)return 0;
	int i;
	if (_ojama >= 6) {
		for (i = 0; i < 6; i++) {
			if (_puyoField[0][i] == 0) {
				_puyoField[0][i] = 6;
			}
		}
		_ojama -= 6;
		if (_ojama == 0) {
			_ojamaReady = false;
		}
		return 6;
	}
	if (_ojama < 6) {
		//drop
		for (i = 0; i < 6; i++) {
			if (_puyoField[0][i] == 0 && dropLoc[i] == true) {
				_puyoField[0][i] = 6;
			}
		}
		int temp = _ojama;
		_ojama = 0;
		_ojamaReady = false;
		return temp;
	}
	return 0;
}


void Simulator::applyGravity()
{
	while (applyGravityOnce() != 0);
}

//임의의 7번뿌요를 놓아 아무것도 떨어지지 않도록 함
int Simulator::applyGravityFix()
{
	int i, j;
	int moveCount = 0;
	for (i = 0; i < 13-1; i++) {
		for (j = 0; j < 6; j++) {
			if (_puyoField[i + 1][j] == 0 && _puyoField[i][j] != 0) {
				_puyoField[i + 1][j] = 7;
				moveCount++;
			}
		}
	}
	return moveCount;
}

int Simulator::applyGravityOnce()
{
	int i, j;
	int moveCount = 0;
	for (i = 11; i >= 0; i--) {
		for (j = 0; j < 6; j++) {
			if (_puyoField[i + 1][j] == 0 && _puyoField[i][j] != 0) {
				_puyoField[i + 1][j] = _puyoField[i][j];
				_puyoField[i][j] = 0;
				moveCount++;
			}
		}
	}
	return moveCount;
}

bool Simulator::ismemerror() {
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 6; j++) {
			if (_puyoField[i][j] >= 7 || _puyoField[i][j] <= -1) {
				return true;
			}
		}
	}
	//또는 모든 필드가 한색으로 도배됨
	auto co = getColors();
	for (int j = 1; j <= 6; j++) {
		if (co[j] == 13 * 6) return true;
	}
	return false;
}

int Simulator::applyPop(int rensa)
{
	_latestPopSize = 0;
	int i, j;
	bool visit[12][6]; //13열 탐색안함
	std::vector<std::vector<std::pair<int, int>>> popGroups;
	popGroups.reserve(50);
	for (i = 0; i < 12; i++) {
		for (j = 0; j < 6; j++) {
			visit[i][j] = false;
		}
	}
	//탐색
	std::vector<std::pair<int, int>> popGroup;
	popGroup.reserve(50);
	for (i = 1; i < 13; i++) {
		for (j = 0; j < 6; j++) {
			//방뿌거나 공기가 아니면서 방문하지 않은곳 탐색
			if (visit[i - 1][j] == false
				&& (!(_puyoField[i][j] >= 6 || _puyoField[i][j] == 0))) {
				popGroup.clear();
				dfs(_puyoField, i, j, _puyoField[i][j], visit, popGroup);
				if ((int)popGroup.size() >= POP_COUNT) {
					popGroups.push_back(popGroup);
					_latestWaste += popGroup.size() - 4;

				}
			}
		}
	}

	if (rensa == 1) {
		_latestHachiich = true;
		for (auto a : popGroups) {
			for (auto b : a) {
				int x = b.second;
				int y = b.first;
				if (checkFieldIndex(x,y-1)) {
					if (_puyoField[y-1][x] == 0) _latestHachiich = false;
				}
				//if (checkFieldIndex(x+1, y)) {
				//	if (_puyoField[y][x+1] == 0) _latestHachiich = false;
				//}
				//if (checkFieldIndex(x - 1, y)) {
				//	if (_puyoField[y][x - 1] == 0) _latestHachiich = false;
				//}
			}
		}
	}

	if (popGroups.size() == 0) {
		return 0;
	}

	int puyoCnt = 0;
	int colorCnt = 0;
	bool colorList[10] = { 0 };
	int GB = 0;//group bonus
	int CB;//color bonus
	int RB;//rensa bonus
	int bonus;
	int score;
	const int CBtable[] = { 0,0,3,6,12,24 };//color bonus table
	const int GBtable[] = { 0,0,0,0,0,2,3,4,5,6,7 };//group bonus table
	const int RBtable[19] = { 0,8,16,32,64,96,128,160,192,224,256,288,320,352,384,416,448,480,512 };
	RB = RBtable[rensa - 1];

	for (auto elem : popGroups) {
		//한번에 터지는 뿌요양 계산
		puyoCnt += (int)elem.size();
		if (elem.size() > 10) {
			GB += 10;
		}
		else {
			GB += GBtable[(int)elem.size()];
		}
		for (auto e : elem)
		{
			colorList[_puyoField[e.first][e.second]] = true;
		}
		//뿌요 삭제 주변 방뿌까지
		for (auto elem2 : elem) {
			_puyoField[elem2.first][elem2.second] = 0;
			int di[] = { -1,0,1,0 };
			int dj[] = { 0,1,0,-1 };
			for (i = 0; i < 4; i++) {
				int y = elem2.first + di[i];
				int x = elem2.second + dj[i];
				//터지면서 상하좌우로 방뿌 있을 경우 같이 삭제
				if (y >= 0 && y < 13 && x >= 0 && x < 6) {
					if (_puyoField[y][x] == 6)	_puyoField[y][x] = 0;
				}
			}
		}
	}

	//뿌요 컬러 수 계산
	for (i = 0; i < 10; i++) { colorCnt += colorList[i]; }

	//점수계산
	CB = CBtable[colorCnt];//color bonus
	bonus = (RB + CB + GB);
	if (bonus < 1) {
		bonus = 1;
	}
	else if (bonus > 999) {
		bonus = 999;
	}
	//cout << (10 * puyoCnt) << "x" << bonus << endl;
	//cout << RB << " " << CB << " " << GB << "colorcnt" << colorCnt << endl;
	score = ((10 * puyoCnt) * bonus);
	_latestPopSize += puyoCnt;
	return score;
}

bool Simulator::checkFireAbleReal()
{
	//print();
	//발화색이 하치이치 발화 가능(뿌요 한개 아래)
	//발화색이 필드 외부 노출인 경우(뿌요 노출)
	//발화점 하기전에 죽는경우 안됨
	//실제 게임에서 발화가 가능함
	vector<std::pair<int, int>> popGroup;
	popGroup.clear();
	bool visit[12][6]; //13열 탐색안함
	for (int i = 0; i < 12; i++) 
		for (int j = 0; j < 6; j++) 
			visit[i][j] = false;
	bool breakAll = false;
	popGroup.reserve(10);
	int connect4 = 0;
	for (int i = 1; i < 13; i++) {
		for (int j = 0; j < 6; j++) {
			//방뿌거나 공기가 아니면서 방문하지 않은곳 탐색
			if (visit[i - 1][j] == false
				&& (!(_puyoField[i][j] == 6 || _puyoField[i][j] == 0))) {
				dfs(_puyoField, i, j, _puyoField[i][j], visit, popGroup);
				if ((int)popGroup.size() >= POP_COUNT) {
					connect4++;
					if (popGroup.size() >= POP_COUNT+1) return false;
					break;
				}
				else {
					popGroup.clear();
				}
			}
		}
	}
	if (connect4 != 1)return false;
	Simulator temp(-1);
	temp.set(_puyoField);
	int(*tempField)[6] = ((int(*)[6])temp.getField());
	bool isOpened = false;
	for (int i = 0; i < popGroup.size(); i++)
	{
		int dx[] = { 1,-1,0,0 };
		int dy[] = { 0,0,-1,-2 };
		for (int j = 0; j < 4; j++)
		{
			if (checkFieldIndex(popGroup[i].second + dx[j], popGroup[i].first + dy[j] + 1) == false)continue;
			if (tempField[popGroup[i].first + dy[j] + 1][popGroup[i].second+dx[j]] == 0)
			{
				isOpened = true;
				break;
			}
		}
	}
	if (isOpened) {
		//마지막으로 발화하기전에 죽는지 체크
		//필드에 발화 뿌요 제거 후 사망 체크
		bool isDropAble = false;
		for (int i = 0; i < popGroup.size(); i++) {
			if ((tempField[0][1] == 0) && (popGroup[i].second < 1)) isDropAble = true;
			if ((tempField[0][3] == 0) && (popGroup[i].second > 3)) isDropAble = true;
			if (popGroup[i].second == 2) isDropAble = true;
		}
		for (int i = 0; i < popGroup.size(); i++) {
			tempField[popGroup[i].first][popGroup[i].second] = 0;
		}
		temp.applyGravity();
		if (isDropAble == false) { return false; }
		if (temp.getGameover() == true)return false;
		else return true;
	}
	return false;
}

bool Simulator::checkFieldIndex(int nowX, int nowY)
{
	//y범위 0~12 (13개)
	if (nowX >= 0 && nowX <= 5 && nowY >= 0 && nowY <= 12) return true;
	else return false;
}

void Simulator::dfs(int puyoField[][6], int i, int j, int c, bool visit[][6], std::vector<std::pair<int, int>>& popGroup) {
	int di[] = { -1,0,1,0 };
	int dj[] = { 0,1,0,-1 };
	int a;
	for (a = 0; a < 4; a++) {
		int y = i + di[a];
		int x = j + dj[a];
		if (x >= 0 && x < 6 && y>0 && y <= 12
			&& visit[y - 1][x] == false) {
			if (puyoField[y][x] == c) {
				popGroup.push_back(std::pair<int, int>(y, x));
				visit[y - 1][x] = true;
				dfs(puyoField, y, x, c, visit, popGroup);
			}
		}
	}
}

void Simulator::applyChainPop(int& rensa, int& score)
//need default
//int rensa = 1;
//int score = 0;
{
	_latestWaste = 0;
	int deltaScore = 0;
	applyGravity();
	do {
		deltaScore = applyPop(rensa);
		if (deltaScore != 0) {
			applyGravity();
			score += deltaScore;
			rensa++;
		}
	} while (deltaScore != 0);
	rensa--;
	_scoredelta = _score + score - _beforeScore;
	_beforeScore = _score + score;
}

bool Simulator::getAllClear()
{
	//check floor 1 puyos
	int i;
	for (i = 0; i < 6; i++) {
		if (_puyoField[12][i] != 0) {
			return false;
		}
	}
	return true;
}

void* Simulator::getField()
{
	return _puyoField;
}
void* Simulator::get14Field()
{
	return _14remove;
}

int Simulator::getOjama()
{
	return _ojama;
}

bool Simulator::getOjamaReady()
{
	return _ojamaReady;
}

void Simulator::setScore(int val)
{
	_score = val;
}

void Simulator::setNext(PuyoTsumo* pt)
{
	_bag[0] = pt[0];
	_bag[1] = pt[1];
	_bagindex = 0;
}

void Simulator::appendScore(int val)
{
	_score += val;
}

Simulator::Simulator(Simulator* s)
{
	memcpy(_puyoField, s->_puyoField, sizeof(int) * 13 * 6);
	memcpy(_fieldHeight, s->_fieldHeight, sizeof(int) * 6);
	_score = s->_score;
	_beforeScore = s->_beforeScore;
	_scoredelta = s->_scoredelta;
	_ojama = s->_ojama;
	_ojamaReady = s->_ojamaReady;
	memcpy(_14remove, s->_14remove, sizeof(bool) * 6);
	_seed = s->_seed;
	_latestChigiriLength = s->_latestChigiriLength;
	memcpy(_bag, s->_bag, sizeof(PuyoTsumo) * 128);
	_bagindex = s->_bagindex;
}

Simulator::Simulator(int seed)
{
	int i;
	reset(seed);
}

void Simulator::reset(int seed)
{
	int i, j, r;
	for (i = 0; i < 13; i++) {
		for (j = 0; j < 6; j++) {
			_puyoField[i][j] = 0;
		}
	}

	for (i = 0; i < 6; i++) {
		_fieldHeight[i] = 0;
	}
	for (i = 0; i < 6; i++)
		_14remove[i] = true;
	_score = 0;
	_ojama = 0;
	_ojamaReady = false;
	_seed = seed % 65535;
	_bagindex = 0;
	_beforeScore = 0;
	_scoredelta = 0;
	_latestChigiriLength = 0;
	if(seed!=-1)srand(_seed);
	//set puyobag(5color in 4color 1~5)
	bool puyoColor[5] = { true, true, true, true, true };
	int puyoColorQ[5] = { 64,64,64,64,64 };
	int puyos[256];
	r = rand() % 5;
	puyoColor[r] = false;
	puyoColorQ[r] = 0;


	if (seed == -1) return;//시드 -1의 경우 속도를 위해 가방 생성안함
	//fill puyos
	//fill puyos 0~3index (lower or equal than 3color)
	int c[3];
	for (i = 0; i < 3; i++) {
		r = rand() % 5;
		while (1) {
			if (puyoColor[r] == false) {
				r = (r + 1) % 5;
			}
			else {
				break;
			}
		}
		c[i] = r;
	}
	r = rand() % 3;
	switch (r) {
	case 0:
		puyos[0] = c[0];
		puyos[1] = c[0];
		puyos[2] = c[1];
		puyos[3] = c[2];
		break;
	case 1:
		puyos[0] = c[1];
		puyos[1] = c[0];
		puyos[2] = c[0];
		puyos[3] = c[2];
		break;
	case 2:
		puyos[0] = c[1];
		puyos[1] = c[2];
		puyos[2] = c[0];
		puyos[3] = c[0];
		break;
	}
	puyoColorQ[c[0]] -= 2;
	puyoColorQ[c[1]] -= 1;
	puyoColorQ[c[2]] -= 1;

	//fill normal
	int puyosIndex = 4;
	for (i = 0; i < 5; i++) {
		for (; puyoColorQ[i] > 0; puyoColorQ[i]--) {
			puyos[puyosIndex++] = i;
		}
	}

	//shuffle
	for (i = 4; i < 256; i++) {
		int temp;
		r = rand() % (256 - 4) + 4;
		temp = puyos[i];
		puyos[i] = puyos[r];
		puyos[r] = temp;
	}

	//push to puyobag
	for (i = 0; i < 128; i++) {
		int index = i * 2;//puyos index
		_bag[i].up = puyos[index] + 1;
		_bag[i].down = puyos[index + 1] + 1;
	}
}

void Simulator::set(int puyoField[13][6])
{
	memcpy(_puyoField, puyoField, 13 * 6 * sizeof(int));
}

void Simulator::set(const int puyoField[13][6])
{
	memcpy(_puyoField, puyoField, 13 * 6 * sizeof(int));
}
void Simulator::set(int puyoField[13][6], bool* c14remove)
{
	memcpy(_puyoField, puyoField, 13 * 6 * sizeof(int));
	for (int i = 0; i < 6; i++)
	{
		_14remove[i] = c14remove[i];
	}
}

void Simulator::ojamaInput(int ojama, bool isRensaEnd)
{
	_ojama += ojama;
	_ojamaReady = isRensaEnd;
}

void Simulator::ojamaInput(int ojama)
{
	_ojama += ojama;
}

PuyoTsumo Simulator::getNowPuyo()
{
	return _bag[(_bagindex++) % 128];
}

PuyoTsumo* Simulator::getNext()
{
	int i;
	PuyoTsumo* next = new PuyoTsumo[3];
	for (i = 0; i < 3; i++) {
		next[i] = _bag[(_bagindex + i) % 128];
	}
	return next;
}

int* Simulator::getFieldHeight()
{
	int i, j;
	for (i = 0; i < 6; i++) {
		_fieldHeight[i] = 0;
	}
	for (j = 0; j < 6; j++) {
		bool found = false;
		for (i = 0; i < 13; i++) {
			if (_puyoField[i][j] != 0) {
				_fieldHeight[j] = 13 - i;//set height 0~13
				found = true;
				break;
			}
		}
		if (found == false) {
			_fieldHeight[j] = 0;
		}
	}
	return _fieldHeight;
}

int Simulator::getLatestChigiriLength()
{
	return _latestChigiriLength;
}

int Simulator::getLatestWaste()
{
	return _latestWaste;
}

int Simulator::getFieldSpace()
{
	int space = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 1; j < 12; j++) {
			if (_puyoField[j][i] == 0) {
				space++;
			}
		}
	}
	return space;
}

float Simulator::getFieldSpaceMax1()
{
	int space = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 1; j < 13; j++) {
			if (_puyoField[j][i] == 0) {
				space++;
			}
		}
	}
	return (float)space/(12.0*6.0);
}

int Simulator::getFieldOjama()
{
	int space = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 13; j++) {
			if (_puyoField[j][i] == 6) {
				space++;
			}
			else {
				break;
			}
		}
	}
	return space;
}

bool Simulator::checkNeedJump(int input) {
	int rot = input / 10;//rotation
	int col = input % 10;//column
	//돌리기 기술 승룡 테스트
	if (getFieldHeight()[1] >= 12 && getFieldHeight()[3] >= 12) {
		//가운데 갇힌상태로 넘기기
		return true;
	}
	return false;
}     

int Simulator::getFieldUnstable()
{
	int fieldUnstable = 0;
	for (int i = 0; i < 5; i++) {
		int val = pow(abs(getFieldHeight()[i] - getFieldHeight()[i + 1]), 1);
		fieldUnstable += val;
	}
	return fieldUnstable;
}

int Simulator::getFieldUnstableSq()
{
	int fieldUnstable = 0;
	for (int i = 0; i < 6; i++)	if (getFieldHeight()[i] == 0) fieldUnstable += 1;
	for (int i = 0; i < 5; i++) {
		 {
			int val = pow(abs(getFieldHeight()[i] - getFieldHeight()[i + 1]), 2);
			if(val > 0)
			fieldUnstable += val;
		}
	}
	return fieldUnstable;
}

float Simulator::getAvgHeight() {
	float sum = 0;
	for (int i = 0; i < 6; i++) {
		sum += getFieldHeight()[i];
	}
	return sum / 6.0f;
}

float Simulator::getFieldBelly()
{
	float fieldUnstable = 0;


	fieldUnstable += getFieldHeight()[1] - getFieldHeight()[0];
	fieldUnstable += getFieldHeight()[2] - getFieldHeight()[1];
	fieldUnstable += getFieldHeight()[3] - getFieldHeight()[4];
	fieldUnstable += getFieldHeight()[4] - getFieldHeight()[5];

	return fieldUnstable;
}


int Simulator::getFieldUnstableSq33()
{
	int fieldUnstable = 0;
	for (int i = 0; i < 3; i++) {
		int val = pow(abs(getFieldHeight()[i] - getFieldHeight()[i + 1]), 2);
		fieldUnstable += val;
	}
	for (int i = 3; i < 6; i++) {
		int val = pow(abs(getFieldHeight()[i] - getFieldHeight()[i + 1]), 2);
		fieldUnstable += val;
	}
	return fieldUnstable;
}

bool Simulator::pushInternal(int input, PuyoTsumo pt) {
	int i, j;
	for (i = 0; i < 6; i++) {
		_fieldHeight[i] = 0;
	}
	for (j = 0; j < 6; j++) {
		bool found = false;
		for (i = 0; i < 13; i++) {
			if (_puyoField[i][j] != 0) {
				_fieldHeight[j] = 13 - i;//set height 0~13
				found = true;
				break;
			}
		}
		if (found == false) {
			_fieldHeight[j] = 0;
		}
	}

	//MoveSimulator m;
	//m.setField(_puyoField, (int*)_14remove);
	//KeyStrokeQueue r = KeyStorkeGenerator::gen(m, input);
	//if (r.pop() == -1) return false;

	int rot = input / 10;//rotation
	int col = input % 10;//column
	if (col == 2 && (rot == 0 || rot == 2)) {  }
	else{
		if (col > 5 || col < 0) {
			return false;
		}
		//돌리기 기술 승룡 테스트
		if (getFieldHeight()[1] >= 12 && getFieldHeight()[3] >= 12) {
			//가운데 갇힌상태로 넘기기
			if (((col == 0) || (col == 1 && rot == 3)) && ((_fieldHeight[1] >= 13 || _fieldHeight[0] >= 13))) {
				return false;
			}
			if (((col == 1) || (col == 2 && rot == 3)) && (_fieldHeight[1] >= 13)) {
				return false;
			}
			if (((col == 3) || (col == 2 && rot == 1)) && (_fieldHeight[3] >= 13)) {
				return false;
			}
			if (((col == 4) || (col == 3 && rot == 1)) && ((_fieldHeight[3] >= 13) || (_fieldHeight[4] >= 13))) {
				return false;
			}
			if (((col == 5) || (col == 4 && rot == 1)) && ((_fieldHeight[3] >= 13) || (_fieldHeight[4] >= 13) || (_fieldHeight[5] >= 13))) {
				return false;
			}
		}
		else {
			//그 이외의 돌려 넘겨놓기 판정 시뮬레이션
			//start h=11,x=2
			int h = 11;//바닥기준 뿌요의 바닥에 있을 뿌요 높이
			if (col < 2)
			{
				/*for (int x = 2; x <= 5; x++) {
					if (_fieldHeight[x] == 13) break;
					if (_fieldHeight[x] == h) {
						h++;
					}
				}*/
				for (int x = 2; x >= col; x--) {
					//바닥이 있고 옆과 한칸 차이면 오르기 가능
					if (_fieldHeight[x] == 13) return false;
					if (_fieldHeight[x] == h) {
						h++;
					}
					//if (x + 1 <= 5 && rot == 1 && _fieldHeight[x + 1] > h) return false;
					if (x - 1 >= 0 && rot == 3 && _fieldHeight[x - 1] > h) return false;
					if (_fieldHeight[x] <= h)
					{
						//pass
					}
					else {
						return false;
						//cant
					}
				}
			}
			else if (col > 2)
			{
				//for (int x = 2; x >= 0; x--) {
				//	//바닥이 있고 옆과 한칸 차이면 오르기 가능
				//	if (_fieldHeight[x] == 13) break;
				//	if (_fieldHeight[x] == h) {
				//		h++;
				//	}
				//}
				for (int x = 2; x <= col; x++) {
					if (_fieldHeight[x] == 13) return false;
					if (_fieldHeight[x] == h) {
						h++;
					}
					if (x + 1 <= 5 && rot == 1 && _fieldHeight[x + 1] > h) return false;
					//if (x - 1 >= 0 && rot == 3 && _fieldHeight[x - 1] > h) return false;
					if (_fieldHeight[x] <= h)
					{
						//pass
					}
					else {
						return false;
						//cant
					}
				}
			}
			else if (col == 2) {
				if (rot == 1) {
					if (h >= _fieldHeight[3]) {

					}
					else {
						return false;
					}
				}
				if (rot == 3) {
					if (h >= _fieldHeight[1]) {

					}
					else {
						return false;
					}
				}
			}
		}
		//MoveSimulator m;
		//m.setField(_puyoField, (int*)_14remove);
		//if (KeyStorkeGenerator::gen(m, input).pop() == -1) return false;
		//else return true;

	}
	switch (rot) {
	case 0:
		if (_fieldHeight[col] <= 11) {
			_puyoField[0][col] = pt.up;
			_puyoField[1][col] = pt.down;
			return true;
		}
		else if (_fieldHeight[col] == 12 && _14remove[col] == true && !OVER_FIELD_DISABLE) {
			//over field
			_puyoField[0][col] = pt.down;
			_14remove[col] = false;
			return true;
		}
		else {
			return false;
		}
		break;
	case 1://clockwise rotation once
		if (col >= 5) {
			//over field right side
			return false;
		}
		_latestChigiriLength = abs(_fieldHeight[col] - _fieldHeight[col + 1]);
		if (_fieldHeight[col] <= 12 && _fieldHeight[col + 1] <= 12) {
			_puyoField[0][col] = pt.down;
			_puyoField[0][col + 1] = pt.up;
			return true;

		}
		else if (_fieldHeight[col] == 13 || _fieldHeight[col + 1] == 13) {
			//fail case
			if ((_fieldHeight[col] == 13) && (_fieldHeight[col + 1] == 13)) {
				return false;
			}

			//succeed case
			if (_fieldHeight[col] != 13 && _14remove[col] == true && !OVER_FIELD_DISABLE) {
				_puyoField[0][col] = pt.down;
				_14remove[col] = false;
			return true;
			}

			if (_fieldHeight[col + 1] != 13 && _14remove[col + 1] == true) {
				_puyoField[0][col + 1] = pt.up;
				_14remove[col + 1] = false;
			return true;
			}
		}
		break;
	case 2://clockwise rotation twice
		if (_fieldHeight[col] <= 11) {
			_puyoField[0][col] = pt.down;
			_puyoField[1][col] = pt.up;
			return true;
		}
		else if (_fieldHeight[col] == 12 && _14remove[col] == true && !OVER_FIELD_DISABLE) {
			//over field
			_puyoField[0][col] = pt.up;
			_14remove[col] = false;
			return true;
		}
		else {
			return false;
		}
		break;
	case 3://counter clockwise rotation once
		if (col == 0) {
			//over field right side
			return false;
		}
		_latestChigiriLength = abs(_fieldHeight[col] - _fieldHeight[col - 1]);
		if (_fieldHeight[col] <= 12 && _fieldHeight[col - 1] <= 12) {
			_puyoField[0][col] = pt.down;
			_puyoField[0][col - 1] = pt.up;
			return true;
		}
		else if (_fieldHeight[col] == 13 || _fieldHeight[col - 1] == 13) {
			//fail case
			if ((_fieldHeight[col] == 13) && (_fieldHeight[col - 1] == 13)) {
				return false;
			}

			//succeed case
			if (_fieldHeight[col] != 13 && _14remove[col] == true && !OVER_FIELD_DISABLE) {
				_puyoField[0][col] = pt.down;
				_14remove[col] = false;
				return true;
			}

			if (_fieldHeight[col - 1] != 13 && _14remove[col - 1] == true && !OVER_FIELD_DISABLE) {
				_puyoField[0][col - 1] = pt.up;
				_14remove[col - 1] = false;
				return true;
			}
		}
	}
	return false;
}

bool Simulator::push(int input, PuyoTsumo pt)
{
	bool r = pushInternal(input, pt);
	if (r == false) {
		//cout << "warn: wrong input(Simulator::push)" << endl;
		return false;
	}
	return true; // succeed
}

bool Simulator::pushFast(int input, PuyoTsumo pt)
{
	bool r = pushInternal(input, pt);
	if (r == false) {
		return false;
	}
	int rensa = 1;
	int score = 0;
	applyChainPop(rensa, score);
	applyOjamaDrop();
	_score += score;
	return true; // succeed
}
bool Simulator::pushFastNoPop(int input, PuyoTsumo pt)
{
	bool r = pushInternal(input, pt);
	applyGravity();
	if (r == false) {
		return false;
	}
	return true; // succeed
}

bool Simulator::pushFastFloat(int input, PuyoTsumo pt)
{
	bool r = pushInternal(input, pt);
	if (r == false) {
		return false;
	}
	int rensa = 1;
	int score = 0;
	_score += score;
	return true; // succeed
}


bool Simulator::pushFast(int input, PuyoTsumo pt, int& rensa, int& score)
{
	bool r = pushInternal(input, pt);
	if (r == false) {
		return false;
	}
	applyChainPop(rensa, score);
	applyOjamaDrop();
	_score += score;
	return true; // succeed
}

bool Simulator::forceFire(int& rensa, int& score)
{
	applyChainPop(rensa, score);
	_score += score;
	if (rensa == 0) {
		return false;
	}
	else {
		return true;
	}
}

bool Simulator::forceFireOnce(int& rensa)
{
	applyGravity();
	int popScore = applyPop(rensa);
	_score += popScore;
	if (popScore == 0) {
		rensa--;
	}
	if (rensa == 0) {
		return false;
	}
	else {
		return true;
	}
}

bool Simulator::forceFire()
{
	int rensa = 1;
	int score = 0;
	applyChainPop(rensa, score);
	_score += score;
	if (rensa == 0) {
		return false;
	}
	else {
		return true;
	}
}

void Simulator::print()
{
	textcolor(WHITE, BLACK);
	int i; int j;
	std::cout << "ojama " << _ojama << std::endl;
	for (i = 0; i < 13; i++) {
		for (j = 0; j < 6; j++) {
			string p = "";
			if (i == 1 && j == 2 && _puyoField[i][j] ==0) {
				textcolor(RED, BLACK);
				p = "X";
			}
			else {
				p = numToPuyo(_puyoField[i][j]);
				//p = std::to_string(_puyoField[i][j]);
			}
			cout << p;
		}
		std::cout << std::endl;
	}
	textcolor(WHITE, BLACK);
	std::cout << "score " << _score << std::endl;
}

string Simulator::numToPuyo(int num) {
	textcolor(WHITE, BLACK);
	string p = "";
	switch (num) {
	case 0:
		textcolor(WHITE, BLACK);
		p = "_";
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
	case 7:
		textcolor(LIGHTGRAY, BLACK);
		p = "△";
		break;
	case 8:
		textcolor(LIGHTGRAY, BLACK);
		p = "▲";
		break;
	default:
		p = "E";
		break;
	}

	return p;
}

int Simulator::inputToIndex(int input)
{
	int result = -1;
	for (int i = 0; i < ALL_MOVEMENT_MAX; i++) {
		if (ALL_MOVEMENT[i] == input) {
			result = i;
			break;
		}
	}
	return result;
}

void Simulator::puyoInputPreview(int input, PuyoTsumo p)
{
	int col = input % 10;
	int rot = input / 10;
	
	//up
	for (int i = 0; i < 6; i++) {
		if (col == i && rot == 0) {
			cout << numToPuyo(p.up);
			textcolor(WHITE, BLACK);
		}
		else if (col == i && rot == 2) {
			cout << numToPuyo(p.down);
			textcolor(WHITE, BLACK);
		}
		else {
			cout << numToPuyo(0);
			textcolor(WHITE, BLACK);
		}
	}
	cout << endl;

	//down
	if (rot == 0 || rot == 2) {
		for (int i = 0; i < 6; i++) {
			if (col == i && rot == 0) {
				cout << numToPuyo(p.down);
				textcolor(WHITE, BLACK);
			}
			else if (col == i && rot == 2) {
				cout << numToPuyo(p.up);
				textcolor(WHITE, BLACK);
			}
			else {
				cout << numToPuyo(0);
				textcolor(WHITE, BLACK);
			}
		}
	}
	else if (rot == 1 || rot == 3) {
		for (int i = 0; i < 6; i++) {
			if (col == i && rot == 1) {
				cout << numToPuyo(p.down);
				textcolor(WHITE, BLACK);
			}
			else if (col + 1 == i && rot == 1) {
				cout << numToPuyo(p.up);
				textcolor(WHITE, BLACK);
			}
			else if (col - 1 == i && rot == 3) {
				cout << numToPuyo(p.up);
				textcolor(WHITE, BLACK);
			}
			else if (col == i && rot == 3) {
				cout << numToPuyo(p.down);
				textcolor(WHITE, BLACK);
			}
			else {
				cout << numToPuyo(0);
				textcolor(WHITE, BLACK);
			}
		}
	}
	cout << endl;
}

int Simulator::getDeltaScore()
{
	int result = 0;
	if (_scoredelta != 0) {
		result = _scoredelta;
		_scoredelta = 0;
	}
	return result;
}

bool Simulator::getGameover()
{
	if (_puyoField[1][2] != 0) {
		return true;
	}
	return false;
}

int Simulator::getScore()
{
	return _score;
}

vector<float> Simulator::getDlInput() {
	vector<float> input;
	input.reserve(13 * 6);
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 6; j++) {
			//puyo 0~6 //AIR R G B Y P GARBAGE
			//0~6 to 0~1
			input.push_back((float)_puyoField[i][j] * (1 / 6));
		}
	}
	PuyoTsumo *next = getNext();
	input.push_back(next[0].up);
	input.push_back(next[0].down);
	input.push_back(next[1].up);
	input.push_back(next[1].down);
	delete next;
	return input;
}

bool Simulator::applyAntiGravityTargetOnce(int x, int y) {
	int i = 0;
	int moveCount = 0;
	for (i = 1; i <= y; i++) {
		if (_puyoField[i - 1][x] == 0 && _puyoField[i][x] != 0) {
			_puyoField[i - 1][x] = _puyoField[i][x];
			_puyoField[i][x] = 0;
			moveCount++;
		}
		else {
			continue;
		}
	}
	return moveCount > 0 ? true : false;
}

void Simulator::removeFloationg()
{
	int i, j;
	int backupF[13][6];
	memcpy(backupF, _puyoField, 13 * 6 * sizeof(int));
	int moveCount = 0;
	int moveCountInter = 0;
	_puyoField[0][2] = 0;
	_puyoField[1][2] = 0;
	//_puyoField[2][2] = 0;


	do {
		moveCountInter = 0;
		for (i = 11; i >= 0; i--) {
			for (j = 0; j < 6; j++) {
				if (_puyoField[i + 1][j] == 0 && _puyoField[i][j] != 0) {
					_puyoField[i + 1][j] = 0;
					_puyoField[i][j] = 0;
					moveCount++;
					moveCountInter++;
				}
			}
		}
	} while (moveCountInter != 0);
	if (moveCount > 2) {
		memcpy(_puyoField, backupF, 13 * 6 * sizeof(int));
	}
}

bool Simulator::isSame(Simulator a, Simulator b) {
	int (*aa)[6] = (int(*)[6])a.getField();
	int(*bb)[6] = (int(*)[6])b.getField();
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 6; j++) {
			if (aa[i][j] != bb[i][j]) {
				return false;
			}
		}
	}
	return true;
}

int Simulator::isSameCnt(Simulator a, Simulator b) {
	int aaa = 0;
	int(*aa)[6] = (int(*)[6])a.getField();
	int(*bb)[6] = (int(*)[6])b.getField();
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 6; j++) {
			if (aa[i][j] != bb[i][j]) {
				aaa++;
			}
		}
	}
	return aaa;
}

int Simulator::getColorCount() {
	int colorTable[10] = {0};
	for (int i = 0; i < 13; i++)
	{
		for (size_t j = 0; j < 6; j++)
		{
			colorTable[_puyoField[i][j]] = 1;
		}
	}
	int ccnt = 0;
	for (int i = 1; i <= 5; i++)
		ccnt += colorTable[i];
	return ccnt;
}

vector<int> Simulator::getColors() {
	int colorTable[10] = { 0 };
	for (int i = 0; i < 13; i++)
	{
		for (size_t j = 0; j < 6; j++)
		{
			colorTable[_puyoField[i][j]] += 1;
		}
	}
	vector<int> ccnt;
	for (int i = 0; i <= 6; i++)
		ccnt.push_back(colorTable[i]);

	return ccnt;
}

bool Simulator::operator ==(Simulator &s) {
	return isSame(this, s);
}

int Simulator::hash() {
	long long hh[6] = { 0 };

		for (int j = 0; j < 6; j++) {
			for (int i = 0; i < 13; i++) {

			hh[j] += (_puyoField[i][j]+1)*pow(7, i);
		}
	}

	long long h = hh[0];
	for (int i = 1; i < 6; i++) {
		h+= hh[i];
	}
	return h%1000000;
}
