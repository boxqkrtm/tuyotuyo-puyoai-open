#pragma once
#include "Plan.h"
#include "Simulator.h"
#include "CrossFunc.h"
#include <iostream>
#include <fstream>
#include <omp.h>
#include <time.h>

using std::vector;
using std::cout;
using std::endl;

bool createForm(Plan& p, Simulator s, int befRen, int targetRen, vector<Shape> &Queue);
void plantest();
