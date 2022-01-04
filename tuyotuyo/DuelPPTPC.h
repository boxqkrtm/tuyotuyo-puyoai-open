#pragma once
#if !_WINDLL
#if __linux__

void DuelPPTPC(int ainum = 0){}
#else
#include "Connector.h"
#include "AI.h"
#include "SaveAI.h"
#include "Plan.h"
#include "PlanTest.h"
#include "fastThinker.h"



void DuelPPTPC(int ainum = 0);

void calEnermyChain(bool& enermyChain, int pnum, Simulator& befOppS, Duel& d, int& enermyScore, int rate, int& fieldfilter, Simulator& afterOppS);

void key(Connector* ppt, int& pnum, bool& retflag, bool& isstart);
#endif
#endif
