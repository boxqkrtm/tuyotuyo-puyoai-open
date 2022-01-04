#pragma once
#include <string>
#include "GameInfo.h"
class AI
{
public:
	virtual void reset();
	virtual int think(GameInfo gi);
	virtual std::string getDebugStr();
	virtual void getParams();
	virtual void randParams();
	virtual void see(GameInfo g);
};

