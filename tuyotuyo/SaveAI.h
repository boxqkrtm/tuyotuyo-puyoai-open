#pragma once
#include "PuyoTsumo.h"
#include "Simulator.h"
#include "GameInfo.h"
#include "AI.h"

class SaveAI : public AI{
private:
	int _input;
	int _maxDepth = 3;
	long _stime;
public:
	SaveAI();
	int think(GameInfo g);
	int input();
	void setDepth(int x);
	int testScore(Simulator s, GameInfo g, int loop, int depth);
	float stackRensaScore(Simulator s, GameInfo g, int maxDepth, bool rush);
	void reset();
	//next[0] = now puyo
	//next[1] = right side next queue's first puyo
};
