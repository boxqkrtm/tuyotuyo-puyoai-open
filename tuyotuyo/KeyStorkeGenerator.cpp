#include "KeyStorkeGenerator.h"
#include "MoveSimulator.h"
#include "Simulator.h"
#include <vector>


KeyStrokeQueue KeyStorkeGenerator::gen(MoveSimulator m, int input)
{
	vector<string> all;
	vector<string> shortall;
	int b[13][6];
	Simulator s(-1);
	int msfield[13][6];

	string mchar[7] = { "<", ">", "V", "A", "B","Q","W"};
	

	//visited[m._zquickturn][m._xquickturn][m._rot][m._x][m._y] = 1;
	if (isSame(m, input) == true) {
		//cout << "found!!" << endl;
		string input = "V";
		m.input(MoveCode::down);
		//while (true) {
		//	auto r = m.input(MoveCode::down);
		//	if (r == false) break;
		//	input += ",V";
		//}
		return KeyStrokeQueue(input + ",V,");
	}
	//좌우 회전 죄 우 아래
//pragma omp parallel for
	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 7; i++) {
			if (i == 2) continue;
			if (i == j)continue;
			if (j == 0 && i == 6) continue;
			if (j == 1 && i == 5) continue;
			int visited[2][2][4][14][6] = { 0, };
			MoveSimulator mc(m);
			auto resultm = mc.input(MoveCode(i));
			if (resultm == false) continue;
			//if (visited[mc._zquickturn][mc._xquickturn][mc._rot][mc._x][mc._y] == 1) continue;
			//visited[mc._zquickturn][mc._xquickturn][mc._rot][mc._x][mc._y] = 1;
			string inputa = "";
			inputa = mchar[i];
			if (isSame(mc, input) == true) {
				string add = "";
				//while (true) {
				//	auto r = mc.input(MoveCode::down);
				//	if (r == false) break;
				//	add += ",V";
				//}
				shortall.push_back(inputa + add);
			}
			auto result = search(mc, input, 0, visited, j);
			if (result.size() == 0) {
			}
			else {
				int shortest = 999;
				string shorts = "";
				for (size_t k = 0; k < result.size(); k++)
				{
					if (result[k].length() < shortest) {
						shorts = result[k];
						shortest = result[k].length();
					}
				}
				string add = "";
				//while (true) {
				//	auto r = mc.input(MoveCode::down);
				//	if (r == false) break;
				//	add += ",V";
				//}
				shortall.push_back(inputa + "," + shorts + add);
			}
		}
	}
	if (shortall.size() == 0) {
		auto s = Simulator(-1);
		s.set(m._field);
		cout << "simulator result " << s.pushFastNoPop(input, PuyoTsumo(1, 2)) << endl;
		cout << "error key stroke generator not found input" << input;

		cout << "{" << endl;
		for (size_t i = 0; i < 13; i++)
		{
			cout << "{";
			for (size_t j = 0; j < 6; j++)
			{
				cout << m._field[i + 1][j];
				if (j != 5)cout << ",";
			}
			if (i == 12) {
				cout << "}" << endl;
			}
			else {
				cout << "}," << endl;
			}
		}
		cout << "};" << endl;
		//system("pause");
		return KeyStrokeQueue("");
	}
	else {
		//cout << "found!!" << endl;
		int shortest = 999;
		string shorts = "";
		for (size_t k = 0; k < shortall.size(); k++)
		{
			if (shortall[k].length() < shortest) {
				shorts = shortall[k];
				shortest = shortall[k].length();
			}
		}
		return KeyStrokeQueue(shorts+","+"V,");
	}
}

vector<string> KeyStorkeGenerator::search(MoveSimulator m, int input, int depth, int visited[2][2][4][14][6], int j)
{
	vector<string> all;
	string mchar[5] = { "<", ">", "V", "A", "B" };
	//좌우 회전 죄 우 아래
	for (size_t i = 0; i < 5; i++) {
		if (i == j)continue;
		if (i == 2)continue;
		MoveSimulator mc(m);
		auto resultm = mc.input(MoveCode(i));
		//if (visited[mc._zquickturn][mc._xquickturn][mc._rot][mc._x][mc._y] == 1) continue;
		//visited[mc._zquickturn][mc._xquickturn][mc._rot][mc._x][mc._y] = 1;
		if (resultm == false) continue;
		string inputa = "";
		inputa = mchar[i];
		if (isSame(mc, input) == true) {
			string add = "";
			all.push_back(inputa+add);
		}
		if (depth < maxmove) {
			auto result = search(mc, input, depth+1, visited, j);
			if (result.size() == 0) {
			}
			else {
				int shortest = 999;
				string shorts = "";
				for (size_t k = 0; k < result.size(); k++)
				{
					if (result[k].length() < shortest) {
						shorts = result[k];
						shortest = result[k].length();
					}
				}
				all.push_back(inputa + "," + shorts);
			}
		}
		else {
			return all;
		}
	}
	return all;
}

bool KeyStorkeGenerator::isSame(MoveSimulator m, int input) {
	int rot = input / 10;//rotation
	int col = input % 10;//column
	if (m._rot == rot) {
		switch (rot) {
		case 0:
			if (col == m._x) return true;
			break;
		case 1:
			if (col == m._x) return true;
			break;
		case 2:
			if (col == m._x) return true;
			break;
		case 3:
			if (col == m._x) return true;
			break;
		}
	}
	return false;
}
