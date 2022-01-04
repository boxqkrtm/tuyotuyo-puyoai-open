#include "PlanTest.h"

using std::fstream;

bool createForm(Plan& p, Simulator inputS, int befRen, int targetRen, vector<Shape> &queue)
{
	{
		int max = -99;
		int min = 99;
		for (int i = 0; i < 6; i++) {
			max = inputS.getFieldHeight()[i] > max ? inputS.getFieldHeight()[i] : max;
			min = inputS.getFieldHeight()[i] < min ? inputS.getFieldHeight()[i] : min;
		}
		//if (max - min > 8) {
		if (max - min > 8) {
			return false;
		}
	}

	//test
	//#pragma omp parallel for
	for (int sh = 0; sh < queue.size(); sh++) {
		//if (rand() % 3 == 0)continue;
		Simulator s = Simulator(inputS);
		Shape sha = queue[sh];
		for (auto i : p.findOutSide(s)) {
		//if (rand() % 3 == 0)continue;
			for (int num = 1; num <= 4; num++) {
				Simulator temp(s);//놓을 정보 보관(다음 depth로 넘김
				if (p.pushShape(temp, sha, num, i.first, i.second, false, befRen == 18 ? true : false, 0) == false)
				{
					continue;
				}
				else
				{
					//성공 다음 연쇄 탐색
					if (befRen < p.checkPlanRensa(temp))
					{
						if (befRen + 1 == targetRen) {
							if (temp.checkFireAbleReal() == true) {
								temp.print();
#if(0)
		char filename[] = "out.txt";
		fstream uidlFile(filename, std::fstream::in | std::fstream::out | std::fstream::app);
		uidlFile << "<hr>";
		int(* pField)[6] = (int(*)[6])inputS.getField();
		for (int i = 0; i < 13; i++)
		{
			for (int j = 0; j < 6; j++)
			{
				uidlFile << "<span class = \"c" << pField[i][j] << "\">●</span>";
			}
			uidlFile << "<br>";
		}
		uidlFile << "<hr>";
#endif
								return true;
							}
							return false;
						}
						if (createForm(p, temp, p.checkPlanRensa(temp), targetRen, queue) == true) {
							return true;
							break;
						}
						//create depth
						if (befRen == targetRen - 1 ? true : false == true) continue;
					}
				}
			}
		}
	}
	return false;
}
	
void plantest() {
	srand(time(NULL));
	Simulator s = Simulator(-1);
	Plan p;
	int num = 1;
	int befRen = 0;

	vector<Shape> shapes = { Shape::I, Shape::I1, Shape::I3, Shape::O,
		Shape::Z, Shape::Z1, Shape::S, Shape::S1, Shape::J,
		Shape::J1, Shape::J2, Shape::J3, Shape::L, Shape::L1,
		Shape::L2, Shape::L3, Shape::T, Shape::T1,
		Shape::T2, Shape::T3 };
	createForm(p, s, befRen, 15, shapes);
}

