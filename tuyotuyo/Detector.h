#pragma once
#include <vector>
#include <queue>
#include <math.h>
#include <utility>	
#include "Detected.h"
#include "Plan.h"
#include "Simulator.h"
#include "PuyoConst.h"
#include "SimulatorNode.h"
#include "Plan.h"

using std::vector;
using std::endl;
using std::cout;
using std::queue;
using std::pair;
using std::make_pair;

class ChildNode {
public:
	ChildNode(Simulator ss, int a, double b, int d, vector<pair<int, int>> e) {
		s = Simulator(ss);
		addedPuyoCnt = a;
		rensa = b;
		score = d;
		fireCoord = e;
	}
	Simulator s = Simulator(-1);
	int addedPuyoCnt;
	double rensa;
	int score;
	vector<pair<int, int>> fireCoord;//발화점 좌표
	bool isSearched = false;
};

class Detector
{

public:
	vector<Detected> detect(void* field);
	int detectMoreMaxrensa(Simulator s);

	pair<vector<SimulatorNode>, pair<int, int>> detectMoreMoreGetNode(vector<SimulatorNode> v, int noColor, PuyoTsumo next, int depth = 0);
	float getFireHeight(Simulator s);
	float getFireX(Simulator s);
	SimulatorNode* detectMoreMore(Simulator s, int noColor, PuyoTsumo* next, int startdepth = 0, int maxDepth = 12, bool fullnext = false);
	bool createForm(Plan& p, Simulator inputS, int befRen, int targetRen, vector<Shape>& queue);
	double detectMoreMoreMSV(Simulator s, Simulator ori);

	void testChildNode(ChildNode n, int depth, vector<ChildNode>& childs, Simulator ori);

	static double RensaToScore(double rensa);

	static double GetRensaScore(int score, double addedPuyoCount);

	static double ValueFunc(double x);

	static double childNodeScore(ChildNode c, Simulator original);

	static double ScoreToRensa(double score);

	vector<Detected> detectMoreRand(void* field, int maxDepth = 1, int depth = 0, PuyoTsumo pt = PuyoTsumo(0, 0), int befMax = 0);

	vector<Detected> detectMore(void* field, int& befMax, int maxDepth = 1, int depth = 0, PuyoTsumo pt = PuyoTsumo(0, 0), bool rush = false);

	vector<Detected> detectMoreEx(void* field, int& befMax, int maxDepth, int depth, PuyoTsumo pt, PuyoTsumo pt2, bool rush = false);

	vector<Simulator> getFireReadySimulators(bool visit[][6], Simulator s, int& needPuyoCnt, int startx, int starty, int color, Simulator ori, vector<pair<int, int>>& addedCoord);

	Simulator getFireReadySimulator(bool visit[][6], Simulator s, int& needPuyoCnt, int startx, int starty, int color, Simulator ori, vector<pair<int,int>> &addedCoord);
	Simulator getFireReadySimulatorfast(bool visit[][6], Simulator s, int& needPuyoCnt, int startx, int starty, int color, Simulator ori, vector<pair<int, int>>& addedCoord);
	float getConnectScore(Simulator s);

	int detectMaxrensa(Simulator s);

};
