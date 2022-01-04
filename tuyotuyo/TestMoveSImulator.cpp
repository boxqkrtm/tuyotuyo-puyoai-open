#include "TestMoveSImulator.h"
#include "MoveSimulator.h"
#include <conio.h>
#include <iostream>
#include "PuyoConst.h"
#include <vector>
#include "KeyStrokeQueue.h"
#include "KeyStorkeGenerator.h"
#include "Simulator.h"
//Todo searching key stroke

void TestMoveSimulator()
{
	int testField1[13][6] = {
	{0,0,0,0,0,0},
	{0,4,0,4,0,0},
	{0,2,0,5,0,0},
	{0,5,0,3,0,0},
	{0,3,0,3,0,0},
	{0,2,0,3,0,0},
	{0,4,0,5,0,0},
	{0,3,0,2,0,0},
	{0,5,0,3,0,0},
	{0,2,0,5,0,0},
	{0,3,0,3,0,0},
	{0,5,4,2,0,0},
	{0,3,3,4,0,0}
	};

	bool field14b[] = { 1,1,1,1,1,1 };
	Simulator s(-1);
	s.set(testField1);


	MoveSimulator m;
	m.setField(testField1, field14b);
	//for (int i = 9; i < 10; i++)
	{
		s.set(testField1, field14b);
		int r = s.pushFast(24, PuyoTsumo(8, 7));
		bool isFail = true;
		KeyStrokeQueue result = KeyStorkeGenerator::gen(m, 0);
		while (true) {
			auto a = result.pop();
			if (a == -1) {
				break;
			}
			isFail = false;
			cout << KeyStrokeQueue::bitToString(a);
		}
		if (isFail == false) {
			cout << endl;
		}
		else {
			//continue;
		}
		cout << (r == true ? "true" : "((false))") << endl;
	}
	system("pause");
}
