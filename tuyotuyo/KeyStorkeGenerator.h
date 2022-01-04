#pragma once
#include "KeyStrokeQueue.h"
#include "MoveSimulator.h"
#include <vector>

class KeyStorkeGenerator
{
private:

public:
	static inline int maxmove = 7;
	//a to b
	static KeyStrokeQueue gen(MoveSimulator m, int input);
	static vector<string> search(MoveSimulator m, int input, int depth, int visited[2][2][4][14][6], int j);
	static bool isSame(MoveSimulator m, int input);
};

