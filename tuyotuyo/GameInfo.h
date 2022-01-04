#pragma once
#include <vector>
#include "PuyoTsumo.h"
#include "Simulator.h"
class GameInfo {
public:
	int _myField[13][6];
	int _oppField[13][6];
	bool _myAllClear;
	bool _oppAllClear;
	PuyoTsumo _myNext[5];
	PuyoTsumo _oppNext[5];
	int _myOjama;
	int _oppOjama;
	bool _my14remove[6];
	bool _opp14remove[6];
	/// <summary>
	/// EventFrame is (last placing frame) or (frame when after fire)
	/// </summary>
	int _myEventFrame;
	int _oppEventFrame;
	int _time;
	int _enermyScore = 0; 
	GameInfo();
	GameInfo(void* myField, void* oppField, void* my14remove, void* opp14remove, bool myAllClear, bool oppAllClear,
		PuyoTsumo* myNext, PuyoTsumo* oppNext, int myOjama, int oppOjama
		, int myEventFrame, int oppEventFrame, int time);
	GameInfo(GameInfo *gi);
	GameInfo getReversePlayer();
	std::vector<float> getDlInput();
	void print();
	float** printGrayScale();
};
