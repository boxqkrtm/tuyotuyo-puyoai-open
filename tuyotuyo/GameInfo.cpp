#include "GameInfo.h"
#include <iostream>
#include "CrossFunc.h"
#include "Detector.h"
using std::cout;


GameInfo::GameInfo()
{
}

GameInfo::GameInfo(void* myField, void* oppField, void* my14remove, void* opp14remove, bool myAllClear, bool oppAllClear,
	PuyoTsumo* myNext, PuyoTsumo* oppNext, int myOjama, int oppOjama
	, int myEventFrame, int oppEventFrame, int time)
{
	
	memcpy(_myField, myField, sizeof(int)*13 * 6);
	memcpy(_oppField, oppField, sizeof(int) * 13 * 6);
	_myAllClear = myAllClear;
	_oppAllClear = oppAllClear;
	for (int i = 0; i < 3; i++) {
		_myNext[i] = myNext[i];
		_oppNext[i] = oppNext[i];
		//std::cout << _myNext[i].up << endl;
	}

	_myOjama = myOjama;
	_oppOjama = oppOjama;
	memcpy(_my14remove, my14remove, sizeof(bool) * 6);
	memcpy(_opp14remove, opp14remove, sizeof(bool) * 6);
	_myEventFrame = myEventFrame;
	_oppEventFrame = oppEventFrame;
	_time = time;

	/*
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 6; j++) {
			cout << _myField[i][j];
		}
		cout << endl;
	}
	cout << _myNext[0].up << endl;
	cout << _myAllClear << endl;
	*/
}

GameInfo::GameInfo(GameInfo *gi)
{
	memcpy(_myField, gi->_myField, sizeof(int) * 13 * 6);
	memcpy(_oppField, gi->_oppField, sizeof(int) * 13 * 6);
	_myAllClear = gi->_myAllClear;
	_oppAllClear = gi->_oppAllClear;
	for (int i = 0; i < 2; i++) {
		_myNext[i] = gi->_myNext[i];
		_oppNext[i] = gi->_oppNext[i];
	}

	_myOjama = gi->_myOjama;
	_oppOjama = gi->_oppOjama;
	memcpy(_my14remove, gi->_my14remove, sizeof(bool) * 6);
	memcpy(_opp14remove, gi->_opp14remove, sizeof(bool) * 6);
	_myEventFrame = gi->_myEventFrame;
	_oppEventFrame = gi->_oppEventFrame;
	_time = gi->_time;
}

GameInfo GameInfo::getReversePlayer() {
	return GameInfo(_oppField, _myField, _opp14remove, _my14remove, _oppAllClear, _myAllClear, _oppNext, _myNext, _oppOjama, _myOjama, _oppEventFrame, _myEventFrame, _time);
}



std::vector<float> GameInfo::getDlInput()
{
	std::vector<float> input;
	input.reserve(18);
	Simulator s(-1);
	Detector d;;

	//////연쇄점수 2
	s.set(_myField);
	int tmp = 0;
	auto r = d.detectMoreEx(s.getField(), tmp, 3, 0, _myNext[1], _myNext[2]);
	float rensascore = 0;
	for (const auto &a : r) {
		if (rensascore < Detector::ScoreToRensa(a._score))
			rensascore = Detector::ScoreToRensa(a._score);
	}
	input.push_back((float)rensascore / 19.0);
	s.set(_oppField);
	tmp = 0;
	r = d.detectMoreEx(s.getField(), tmp, 3, 0, _oppNext[1], _oppNext[2]);
	rensascore = 0;
	for (const auto& a : r) {
		if (rensascore < Detector::ScoreToRensa(a._score))
			rensascore = Detector::ScoreToRensa(a._score);
	}
	input.push_back((float)rensascore / 19.0);

	//발화중 남은 연쇄 2
	s.set(_myField);
	s.applyGravity();
	int temp = 0;
	int rensa = 1;
	s.applyChainPop(rensa, temp);
	input.push_back((float)rensa/19.0);
	s.set(_oppField);
	s.applyGravity();
	temp = 0;
	rensa = 1;
	s.applyChainPop(rensa, temp);
	input.push_back((float)rensa / 19.0);

	//올클 상태 2
	input.push_back(_myAllClear == true ? 1.0 : 0.0);
	input.push_back(_oppAllClear == true ? 1.0 : 0.0);

	//방뿌개수 2
	input.push_back((float)Detector::ScoreToRensa(_myOjama*70)/19.0);
	input.push_back((float)Detector::ScoreToRensa(_oppOjama*70)/19.0);

	//3연결 2연결 4
	int c2 = 0;
	int c3 = 0;
	s.set(_myField);
	int(*sf)[6] = (int(*)[6])s.getField();
	int i, j;
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
			c2 += 1;
			break;
		case 3:
			c3 += 1;
			break;
		}
	}
	input.push_back((float)Detector::ScoreToRensa(c2 / 39.0));
	input.push_back((float)Detector::ScoreToRensa(c3/ 26.0));
	c2 = 0;
	c3 = 0;
	Simulator oppS = Simulator(-1);
	oppS.set(_oppField);
	sf = (int(*)[6])s.getField();
	for (i = 0; i < 12; i++) {
		for (j = 0; j < 6; j++) {
			visit[i][j] = false;
		}
	}
	popGroups.clear();
	popGroup.clear();
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
			c2 += 1;
			break;
		case 3:
			c3 += 1;
			break;
		}
	}
	input.push_back((float)Detector::ScoreToRensa(c2 / 39.0));
	input.push_back((float)Detector::ScoreToRensa(c3 / 26.0));

	//필드남은 공간 2
	input.push_back(s.getFieldSpaceMax1());
	input.push_back(oppS.getFieldSpaceMax1());

	//평평함 2
	input.push_back(min(s.getFieldUnstable()/100.f,1));
	input.push_back(min(oppS.getFieldUnstable() / 100.f, 1));

	//3열의 높이 2
	input.push_back(s.getFieldHeight()[2] / 13.0);
	input.push_back(oppS.getFieldHeight()[2]/13.0);

	//cout << input.size() << std::endl;
	//system("pause");
	return input;
}

void GameInfo::print()
{
	textcolor(WHITE, BLACK);
	int(*p0f)[6] = (int(*)[6])_myField;
	int(*p1f)[6] = (int(*)[6])_oppField;
	int i; int j;
	std::cout << "ojama " << _myOjama;
	cout << "       ";
	std::cout << "ojama " << _oppOjama << "        " << std::endl;
	for (i = 0; i < 13; i++) {
		for (j = 0; j < 6; j++) {
			const char* p = "_";
			switch (p0f[i][j]) {
			case 0:
				textcolor(DARKGRAY, BLACK);
				p = "__";
				if (i == 1 && j == 2) {
					textcolor(RED, BLACK);
					p = "XX";
				}
				break;
			case 1:
				textcolor(RED, BLACK);
				p = "★";
				break;
			case 2:
				textcolor(GREEN, BLACK);
				p = "●";
				break;
			case 3:
				textcolor(BLUE, BLACK);
				p = "◆";
				break;
			case 4:
				textcolor(YELLOW, BLACK);
				p = "■";
				break;
			case 5:
				textcolor(MAGENTA, BLACK);
				p = "♠";
				break;
			case 6:
				textcolor(LIGHTGRAY, BLACK);
				p = "○";
				break;
			}
			std::cout << p;
		}
		cout << "       ";
		for (j = 0; j < 6; j++) {
			const char* p = "_";
			switch (p1f[i][j]) {
			case 0:
				textcolor(DARKGRAY, BLACK);
				p = "__";
				if (i == 1 && j == 2) {
					textcolor(RED, BLACK);
					p = "XX";
				}
				break;
			case 1:
				textcolor(RED, BLACK);
				p = "★";
				break;
			case 2:
				textcolor(GREEN, BLACK);
				p = "●";
				break;
			case 3:
				textcolor(BLUE, BLACK);
				p = "◆";
				break;
			case 4:
				textcolor(YELLOW, BLACK);
				p = "■";
				break;
			case 5:
				textcolor(MAGENTA, BLACK);
				p = "♠";
				break;
			case 6:
				textcolor(LIGHTGRAY, BLACK);
				p = "○";
				break;
			}
			std::cout << p;
		}
		std::cout << std::endl;
	}
	textcolor(WHITE, BLACK);
	//cout << "score " << _my;
	cout << "       ";
	//cout << "score " << _p[1].getScore() << "        " << endl;
	textcolor(WHITE, BLACK);
}

float** GameInfo::printGrayScale()
{
	float **output = NULL;
	output = new float* [14];
	for (int h = 0; h < 14; h++)
	{
		output[h] = new float[14];

		for (int w = 0; w < 14; w++)
		{
			// fill in some initial values
			output[h][w] = 0;
		}
	}


	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 6; j++)
			output[i+1][j] = _myField[i][j] * (1.0 / 6.0);

	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 6; j++)
			output[i + 1][j+7] = _oppField[i][j] * (1.0 / 6.0);

	//넥스트
	
	output[2][6] = ((float)_myNext[0].up * (1.0 / 6.0));
	output[3][6] = ((float)_myNext[0].down * (1.0 / 6.0));
	output[4][6] = ((float)_myNext[1].up * (1.0 / 6.0));
	output[5][6] = ((float)_myNext[1].down * (1.0 / 6.0));
	output[2][13] = ((float)_oppNext[0].up * (1.0 / 6.0));
	output[3][13] = ((float)_oppNext[0].down * (1.0 / 6.0));
	output[4][13] = ((float)_oppNext[1].up * (1.0 / 6.0));
	output[5][13] = ((float)_oppNext[1].down * (1.0 / 6.0));

	//올클 상태
	output[0][1] = (_myAllClear == true ? 1.0 : 0.0);
	output[0][8] = (_oppAllClear == true ? 1.0 : 0.0);

	//방뿌개수 1.0 : 6*10(값 : 실제 방뿌값)
	output[0][0] = ((float)_myOjama / 60.0);
	output[0][9] = ((float)_oppOjama / 60.0);

	return output;
}
