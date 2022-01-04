#include <iostream>
#include "PuyoConst.h"
#include "Duel.h"
using std::cout;
using std::endl;

#ifdef __linux__
#define EXTERN  
#else
#define EXTERN __declspec(dllexport)

#endif

//todo memory delete

extern "C"
{
	EXTERN void DuelHiddenBagCopy(Duel *d)
	{
		//d
		//Duel(d)
	}
	EXTERN void DuelRandUnknownData(Duel* d) {
		d->RandUnknownData();
	}


	EXTERN int DuelNowBagSee(Duel *d, int index)
	{
		PuyoTsumo p = d->getSimulator(0)._bag[index];
		int result = p.up * 10 + p.down;
		return result;
	}
	EXTERN int DuelNowBagIndex(Duel *d, int player)
	{
		return d->getSimulator(player)._bagindex;
	}

	EXTERN int DuelHiddenBagIndex(Duel *d)
	{
		int a = d->getSimulator(0)._bagindex;
		int b = d->getSimulator(1)._bagindex;
		int c = a > b ? a : b;
		return c + 2;
	}

	EXTERN Duel *DuelNew(int seed)
	{
		return new Duel(seed);
	}

	EXTERN Duel *DuelCopy(Duel *d)
	{
		return new Duel(d);
	}

	EXTERN void DuelReset(Duel *d, int seed)
	{
		d->reset(seed);
	}
	EXTERN void DuelPrint(Duel *d)
	{
		d->print();
	}
	/// <summary>
	/// run the duel
	/// </summary>
	/// <param name="d">duel obejct</param>
	/// <param name="dlist">duel object list to return ( when ojama simulation )</param>
	/// <param name="player">select player for ojama simulation</param>
	/// <returns>duel state</returns>
	EXTERN int DuelRun(Duel *d)
	{
		int befFrame = d->getFrame();
		d->setIsOjamaSim(-1);
		while (true)
		{
			d->run();
			if (d->needInput(0) == true || d->needInput(1) == true || d->getWin()[0] == true || d->getWin()[1] == true)
			{
				//loop end
				break;
			}
			//if (d->getFrame() - befFrame >= 2000)
			//{
			//	cout << "duel run hang detected" << endl;
			//	break;
			//}
		}
		/*
		0 1p입력필요
		1 2p입력필요
		2 둘다 입력 필요
		3 1p승리
		4 2p승리
		5 무승부
		-1에러
		*/
		if (d->getWin()[0] == true && d->getWin()[1] == true)
		{
			return 5;
		}
		else if (d->getWin()[0] == true)
		{
			return 3;
		}
		else if (d->getWin()[1] == true)
		{
			return 4;
		}
		else if (d->needInput(0) == true && d->needInput(1) == true)
		{
			return 2;
		}
		else if (d->needInput(0) == true)
		{
			return 0;
		}
		else if (d->needInput(1) == true)
		{
			return 1;
		}
		return -1;
	}

	EXTERN int DuelStatus(Duel *d)
	{
		/*
		0 1p입력필요
		1 2p입력필요
		2 둘다 입력 필요
		3 1p승리
		4 2p승리
		5 무승부
		-1에러
		*/
		if (d->getWin()[0] == true && d->getWin()[1] == true)
		{
			return 5;
		}
		else if (d->getWin()[0] == true)
		{
			return 3;
		}
		else if (d->getWin()[1] == true)
		{
			return 4;
		}
		else if (d->needInput(0) == true && d->needInput(1) == true)
		{
			return 2;
		}
		else if (d->needInput(0) == true)
		{
			return 0;
		}
		else if (d->needInput(1) == true)
		{
			return 1;
		}
		return -1;
	}

	EXTERN int DuelRunWithOjamaSim(Duel *d, int player)
	{
		int befFrame = d->getFrame();
		d->setIsOjamaSim(player);
		while (true)
		{
			d->run();
			if (d->hasOjamaSim() == true)
			{
				return 6;
				break;
			}
			if (d->needInput(0) == true || d->needInput(1) == true || d->getWin()[0] == true || d->getWin()[1] == true)
			{
				//loop end
				break;
			}
			//if (d->getFrame() - befFrame >= 2000)
			//{
			//	cout << "duel run ojamasim hang detected" << endl;
			//	break;
			//}
		}
		/*
		0 1p입력필요
		1 2p입력필요
		2 둘다 입력 필요
		3 1p승리
		4 2p승리
		5 무승부
		6 방뿌 리스트 받기 필요
		-1에러
		*/
		if (d->getWin()[0] == true && d->getWin()[0] == true)
		{
			return 5;
		}
		else if (d->getWin()[0] == true)
		{
			return 3;
		}
		else if (d->getWin()[1] == true)
		{
			return 4;
		}
		else if (d->needInput(0) == true && d->needInput(1) == true)
		{
			return 2;
		}
		else if (d->needInput(0) == true)
		{
			return 0;
		}
		else if (d->needInput(1) == true)
		{
			return 1;
		}
		return -1;
	}

	//call duelresetojamasim when after call duelgetojama
	EXTERN Duel *DuelGetOjamaSim(Duel *d, int i)
	{
		if (d->getOjamaSim()[i] == NULL)
			return NULL;
		return new Duel(*d->getOjamaSim()[i]);
	}

	EXTERN void DuelResetOjamaSim(Duel *d)
	{
		d->resetOjamaSim();
	}

	EXTERN GameInfo *DuelGameInfo(Duel *d, int basePlayerNum)
	{
		return new GameInfo(d->getGameinfo(basePlayerNum));
	}

	EXTERN int *GameInfoGetMyField(GameInfo *g)
	{
		int *myField = new int[13 * 6];
		memcpy(myField, g->_myField, 13 * 6 * sizeof(int));
		return myField;
	}

	EXTERN int *GameInfoGetOppField(GameInfo *g)
	{
		int *oppField = new int[13 * 6];
		memcpy(oppField, g->_oppField, 13 * 6 * sizeof(int));
		return oppField;
	}

	EXTERN bool *GameInfoGetMy14remove(GameInfo *g)
	{
		return g->_my14remove;
	}

	EXTERN bool *GameInfoGetOpp14remove(GameInfo *g)
	{
		return g->_opp14remove;
	}

	EXTERN bool GameInfoGetMyAllClear(GameInfo *g)
	{
		return g->_myAllClear;
	}

	EXTERN bool GameInfoGetOppAllClear(GameInfo *g)
	{
		return g->_oppAllClear;
	}

	EXTERN int *GameInfoGetMyNext(GameInfo *g)
	{
		int *nextArr = new int[4];
		nextArr[0] = g->_myNext[0].up;
		nextArr[1] = g->_myNext[0].down;
		nextArr[2] = g->_myNext[1].up;
		nextArr[3] = g->_myNext[1].down;

		return nextArr;
	}

	EXTERN int *GameInfoGetOppNext(GameInfo *g)
	{
		int *nextArr = new int[4];
		nextArr[0] = g->_oppNext[0].up;
		nextArr[1] = g->_oppNext[0].down;
		nextArr[2] = g->_oppNext[1].up;
		nextArr[3] = g->_oppNext[1].down;
		return nextArr;
	}

	EXTERN int GameInfoGetMyOjama(GameInfo *g)
	{
		return g->_myOjama;
	}

	EXTERN int GameInfoGetOppOjama(GameInfo *g)
	{
		return g->_oppOjama;
	}

	EXTERN int GameInfoGetMyEventFrame(GameInfo *g)
	{
		return g->_myEventFrame;
	}

	EXTERN int GameInfoGetOppEventFrame(GameInfo *g)
	{
		return g->_oppEventFrame;
	}

	EXTERN void DuelInput(Duel *d, int input1p, int input2p)
	{
		int input1pRaw = -1;
		int input2pRaw = -1;
		//0~21의 값이 뿌요 시뮬레이터의 입력 형식에 맞도록 변경 -1의 경우 duel의 무시판정 적용
		if (input1p >= 0)
			input1pRaw = ALL_MOVEMENT[input1p];
		if (input2p >= 0)
			input2pRaw = ALL_MOVEMENT[input2p];
		d->input(input1pRaw, input2pRaw);
	}

	//true 입력가능 false 입력 불가능
	EXTERN bool DuelInputTest(Duel *d, int player, int input)
	{
		return d->inputTest(player, ALL_MOVEMENT[input]);
	}

	//flow
	//Duel_new->(Duel_reset)->Duel_run->Duel_info->Duel_input
	//loop: Duel_run->Duel_info->Duel_input until gameover 1p or 2p

	EXTERN void DuelDel(Duel *d)
	{

		delete d;
		d = NULL;
	}

	EXTERN void DuelSwap(Duel *d)
	{
		d->swap();
	}

	EXTERN void GameInfoDel(GameInfo *d)
	{
		delete d;
		d = NULL;
	}

	EXTERN void IntDel(int *d)
	{
		delete d;
		d = NULL;
	}

	EXTERN void floatDel(float *d)
	{
		delete d;
		d = NULL;
	}

	EXTERN float *GameInfoToGrayScale(GameInfo *g)
	{
		float ** aa = g->printGrayScale();
		float *a = new float[14 * 14];
		for (auto i = 0; i < 14; i++)
		{
			for (auto j = 0; j < 14; j++)
			{
				a[i * 14 + j] = aa[i][j];
			}
		}
		for (auto i = 0; i < 14; i++)
			delete[] aa[i];
		delete[] aa;
		return a;
	}

	EXTERN void GrayScaleDel(float *d)
	{
		delete[] d;
		d = NULL;
	}
}
