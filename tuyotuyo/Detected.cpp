#include "Detected.h"


Detected::Detected(int rensa, int score, int col, bool isUp, int p, Simulator backup)
{
	_rensa = rensa;
	_score = score;
	_col = col;
	_isUp = isUp;
	_p = p;
	_backup = Simulator(backup);
}
