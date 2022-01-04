#pragma once
#include "Simulator.h"
class SimulatorNode {
public:
	SimulatorNode(Simulator s, int startMove) {
		_s = s;
		_startMove = startMove;
	}
	Simulator _s = Simulator(-1);
	int _startMove = -1;
	float evalScore = 0;
	float befScore = 0;
	int afterRensa = 0;
	int afterScore = 0;
	bool isSearched = false;
	bool isGameover = false;
	int depth = 0;
	int visitCount = 0;
	int _remainRensa = 0;
	int _enermyScore = 0;
};
