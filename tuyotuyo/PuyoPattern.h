#pragma once
#include <vector>

using std::vector;
class PuyoPattern {
public:
	PuyoPattern(vector<vector<int>> shape, int score, bool isCorner) {
		this->score = score;
		this->shape = shape;
		this->isCorner = isCorner;
	};
	//puyo pattern shape 0=anyother puyo or air, 1=samepuyo
	vector<vector<int>> shape;
	int score;
	bool isCorner;
};
