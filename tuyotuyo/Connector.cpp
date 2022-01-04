#include "Connector.h"

Connector::Connector()
{
}

Connector::~Connector()
{
}

bool Connector::isIngame()
{
	return false;
}

void Connector::toDuel(Duel& d, bool print, bool& isInit)
{
}

void Connector::toInput(int input, GameInfo g, int pnum)
{
}

void Connector::toInputRotation(int col, int rot, const int& PPT_ROTATEDELAY, GameInfo g)
{
}

void Connector::waitToInit()
{
}

bool Connector::needInput(GameInfo g)
{
	return false;
}

bool Connector::inputEmpty()
{
	return false;
}

bool Connector::isReadyToInputTrue(GameInfo g)
{
	return false;
}

void Connector::isReadyToInputReset()
{
}

bool Connector::isNoFloatingPuyo(GameInfo g)
{
	return false;
}

bool Connector::isNoRensa(GameInfo g)
{
	return false;
}

XboxController* Connector::getXbox()
{
	return NULL;
}

int Connector::getAddr() {
	return NULL;
}
