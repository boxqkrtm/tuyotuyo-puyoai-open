#include "DuelAIAI.h"
#include "DlAI.h"
#include "MSVAI.h"
#include "FormAIv2.h"

void DuelAIAI()
{
	int start = clock();
	int pw[2];
	int seed;
	srand((unsigned)time(NULL));
	seed = rand();
	Duel d = Duel(seed);
	pw[0] = 0;
	pw[1] = 0;
	bool watch = true;
	int ft = 50;
	AI* ai1 = new FormAIv2();
	AI* ai2 = new FormAIv2();
	//float param[9] = { 3.22,5.85,0.21,56.99,35.57,79.26,78.92,48.68,50.75 };
	//float param[7] = { 3.28,26.46,0.12,43.89,88.75,84.33,26 };

	//for (int i = 0; i < 9; i++)
	//	((FormAIv2*)ai1)->param[i] = param[i];;
	//gen5


	while (pw[0] != ft && pw[1] != ft)
	{
		int i1 = 0;
		int i2 = 0;
		//d.input(3, 4);
		//d.run();
		//d.run();
		//while (d.needInput(0)==false)d.run();
		while (true)
		{
			if (d.needInput(0) == true) i1 = ai1->think(d.getGameinfo(0));
			//if (d.needInput(0) == true) i1 = x.think(d.getGameinfo(0));
			if (d.needInput(1) == true) i2 = ai2->think(d.getGameinfo(1));
			d.input(i1, i2);
			bool r = d.run();
			if ( watch == true)
			{
				gotoxy(0, 2);
				d.print();
				cout << pw[0] << ":" << pw[1] << "seed : " << (seed) << endl;
			    Sleep(16);
				//Sleep(1);
			}

			if (r == false)
			{
				pw[0] += d.getWin()[0];
				pw[1] += d.getWin()[1];
				seed++;
				d.reset(seed);
				system("cls");
				cout << pw[0] << ":" << pw[1] << "seed : " << (seed) << endl;
				break;
			}
		}
	}
	cout << pw[0] << ":" << pw[1] << endl;
	cout << "완료 " << ((float)(clock() - start) / CLOCKS_PER_SEC) << "초" << endl;
	system("PAUSE");
	system("PAUSE");
}
