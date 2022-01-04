#include "DuelToko.h"
#include "MSVAI.h"
#include "FormAIv2.h"

void DuelToko()
{
	system("cls");
	srand(time(NULL));
	int seed = rand();
	//seed = 6016;
	Duel d = Duel(seed);
	d.setToko();
	FormAIv2 *a = new FormAIv2();
	a->AI_SURPRISE_ATTACK = false;
	a->AI_SEE = false;
	//a->_multi = false;
	int ai1 = -1;
	int frame = 0;
	cout << "seed" << seed;
	while (1) {
		if (d.needInput(0) == true) {
			ai1 = a->think(d.getGameinfo(0));
			d.input(ai1, 0);
		}
		if (d.run() == false) {
			d.reset(rand());
			a->reset();
		}
		double min = d.getFrame() / 60. / 60.;
		gotoxy(0, 0);
		if (true) {
			d.print();
		}
		cout << "bag index:" << d.getSimulator(0)._bagindex << endl;
		cout << "min : " << min << endl;
		cout << "state: " << d.getState(0) << endl;
		cout << "debug : " << ai1 << " " << a->getDebugStr() << endl;
		cout << "maxrensa : " << d._maxRensa << endl;
		cout << "seed :" << d._seed << endl;
		//Sleep(1);
		
		frame++;
	}
}
