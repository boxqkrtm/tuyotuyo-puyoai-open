#pragma once
#include <random>
#include <ctime>
#include "Duel.h"
#include "PuyoConst.h"
#include "Plan.h"
#include "CrossFunc.h"
#include "SaveAI.h"

#ifdef __linux__
#else
#include <conio.h>
using std::cout;
using std::endl;
void DuelHumanAI();
#endif
