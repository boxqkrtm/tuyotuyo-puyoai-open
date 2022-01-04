#include "fastThinker.h"

int fastinput = -1;
void fastThinker::fastThread(AI* ai, Duel& d, int& pnum)
{
	fastinput = ai->think(d.getGameinfo(pnum));
}

void fastThinker::s(AI* ai, Duel& d, int& pnum) {
	th = std::thread(&fastThinker::fastThread, fastThinker(), std::ref(ai), std::ref(d), std::ref(pnum));
	th.detach();
}

int fastThinker::get() {
	int r = fastinput;
	//cout << fastinput << endl;
	fastinput = -1;
	return r;
}
