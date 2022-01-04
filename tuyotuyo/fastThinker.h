#pragma once
#include <thread>
#include <iostream>
#include "AI.h"
#include "Duel.h"

using std::thread;
using std::cout;
using std::endl;

class fastThinker {
private:
	std::thread th;
public:
	void fastThread(AI* ai, Duel& d, int& pnum);
	void s(AI* ai, Duel& d, int& pnum);
	int get();
};

