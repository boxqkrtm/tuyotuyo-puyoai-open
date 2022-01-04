#pragma once
#pragma once
#if !_WINDLL
#if !__linux__
#include "ProcessMemory.h"
#include "Simulator.h"
#include "CrossFunc.h"
#include "Duel.h"
#include "PuyoConst.h"
#include <conio.h>
#include "XboxController.h"
#include <Xinput.h>
#include <queue>

class Connector
{

public:
	Connector();
	virtual ~Connector();
	virtual bool isIngame();
	virtual void toDuel(Duel& d, bool print, bool& isInit);
	virtual void toInput(int input, GameInfo g, int pnum);
	virtual void toInputRotation(int col, int rot, const int& PPT_ROTATEDELAY, GameInfo g);
	virtual void waitToInit();
	virtual bool needInput(GameInfo g);
	virtual bool inputEmpty();
	virtual bool isReadyToInputTrue(GameInfo g);
	virtual void isReadyToInputReset();
	virtual bool isNoFloatingPuyo(GameInfo g);
	virtual bool isNoRensa(GameInfo g);
	virtual XboxController* getXbox();
	virtual int getAddr();
};

#endif
#endif
