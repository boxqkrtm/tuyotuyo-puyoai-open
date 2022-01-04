#pragma once

#include "Simulator.h"
#include "PuyoTsumo.h"
#include <vector>
#include <iostream>


using std::cout;
using std::pair;
using std::vector;

/*
	Simulator s(0);
	Plan p;
	p.pushShape(s, Shape::O, 3, 2, 12, false);
	p.pushShape(s, Shape::S, 1, 0, 12, false);
	p.pushShape(s, Shape::L, 2, 0, 12-3, true);
	//gtr
*/

enum class Shape {
	I,I1,I3, O, Z, Z1, S, S1, J, J1, J2, J3, L, L1, L2, L3, T, T1, T2, T3, COUNT
};

const vector<Shape> LowShape = { Shape::T,Shape::T1,Shape::T2,Shape::T3, Shape::I };
bool findOutSideSort(pair<int, int> a, pair<int, int> b);

class Plan {
private:
	/// <summary>
	/// {{dx, dy}...}
	/// </summary>
	int _shapes[(int)Shape::COUNT][4][2] = {
		{{0,0},{0,-1},{0,-1},{0,-1}},//I
		{{0,0},{1,0},{1,0},{1,0}},//I1
		{{0,0},{-1,0},{-1,0},{-1,0}},//I3
		{{0,0},{1,0},{0,-1},{-1,0}},//O
		{{0,-1},{1,0},{0,1},{1,0}},//Z
		{{0,0}, {0,-1},{1,0},{0,-1}},//Z1
		{{0,0},{1,0},{0,-1},{1,0}},//S
		{{0,0},{0,-1},{-1,0},{0,-1}},//S1
		{{0,0},{1,0},{0,-1},{0,-1}},//J
		{{0,0},{0,-1},{1,1},{1,0}},//J1
		{{0,0},{0,-1},{0,-1},{1,0}},//J2
		{{0,0},{1,0},{1,0},{0,-1}},//J3
		{{0,0},{0,-1},{0,-1},{1,2}},//L
		{{0,0},{0,-1},{1,0},{1,0}},//L1
		{{0,-2},{1,0},{0,1},{0,1}},//L2
		{{0,0},{1,0},{1,0},{0,-1}},//L3
		{{0,-1},{1,0},{1,0},{-1,1}},//T
		{{0,0},{0,-1},{0,-1},{1,1}},//T1
		{{0,0},{1,0},{1,0},{-1,-1}},//T2
		{{0,-1},{1,-1},{0,1},{0,1}}//T3
	};

	static const int SHAPE_OUTOFRANGE = -1;
	static const int SHAPE_CONFLICTPUYO = -2;
	static const int SHAPE_SUCCEED = 1;

public:
	int pushShapeTest(Simulator s, Shape sh, int x, int y);
	int pushShapeTestIngame(Simulator s, Shape sh, int x, int y);
	vector<pair<int, int>> findOutSide(Simulator& s);
	bool pushShape(Simulator& s, Shape sh, int color, int x, int y, bool removeKey, bool isNow18Ren= false, int moveDelta = 0);
	bool pushShapeIngame(Simulator& s, Shape sh, int color, int x, int y, bool removeKey, bool isNow18Ren, int moveDelta);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="field">plan field</param>
	/// <param name="sh">add rensa with shape</param>
	/// <returns></returns>
	void appendKeyPuyo(Simulator& s);
	int checkPlanRensa(Simulator s);
	int pushShapeStack(Simulator& s, Shape sh, int color, int x, int y);
};
