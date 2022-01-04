#pragma once
#include "Simulator.h"
#include <utility>
using std::pair;
class Detected {
public:
	int _rensa;
	int _score;
	int _col;
	bool _isUp;
	int _wastePuyo;
	int _p;
	bool isExpanded = false;
	vector<pair<int, int>> addedCoord;
	Simulator _backup = Simulator(-1);
	Detected(int rensa, int score, int col, bool isUp, int p, Simulator backup = Simulator(-1));
};
