#pragma once

#include "PuyoTsumo.h"
#include <vector>
#include <string.h>
#include <string>
using std::vector;
using std::string;
/*
--example usage--
int seed = 123;
//p1
Simulator p1 = Simulator(seed); //create sim p1
p1.pushFast(0, PuyoTsumo(1, 2)); //  1
p1.pushFast(1, PuyoTsumo(1, 2)); //  2
p1.pushFast(2, PuyoTsumo(1, 2)); // 222  => stacking like this
p1.pushFast(21, PuyoTsumo(1, 2));// 111
//p1.print();//2chain and allclear(200 + 2100)
int p1attack = p1.getDeltaScore() / 70;//2 calculate garbage

//p2
Simulator p2 = Simulator(seed); //create sim p2
p2.ojamaInput(p1attack, true);//get ojama and ojamaReady
p2.pushFast(2, p2.getNowPuyo());//drop puyo(now bag) and drop ojama
p2.pushFast(2, p2.getNowPuyo());//drop puyo(now bag)
p2.print();//print field
*/

class Simulator {
private:
	//m_var
	int _puyoField[13][6];
	int _fieldHeight[6];
	int _beforeScore;
	int _scoredelta;//for ojama calculation
	int _ojama;
	bool _ojamaReady;
	bool _14remove[6];
	int _seed;//bag and ojama drop
	int _latestChigiriLength;
	int _latestWaste;
	

	//internal functions
	bool pushInternal(int input, PuyoTsumo pt);

public:
	int _score;
	int _latestPopSize = 0;
	void applyChainPop(int& rensa, int& score);
	PuyoTsumo _bag[128];//puyo bag
	int _bagindex;
	Simulator(Simulator *s);
	Simulator(int seed);
	void reset(int seed);
	void set(int puyoField[13][6]);
	void set(const int puyoField[13][6]);
	void set(int puyoField[13][6], bool* c14remove);
	void ojamaInput(int ojama, bool isRensaEnd);
	void ojamaInput(int ojama);

	//internal functions
	void applyOjamaDrop();
	int applyOjamaDropOnce();
	/// <summary>
	/// apply ojama drop once with custom fixed location(not random)
	/// </summary>
	/// <param name="dropLoc">drop location</param>
	/// <returns></returns>
	int applyOjamaDropOnce(bool dropLoc[6]);
	void applyGravity();
	int applyGravityFix();
	int applyGravityOnce();
	bool ismemerror();
	bool applyAntiGravityTargetOnce(int x, int y); //for insert
	void removeFloationg();
	static bool isSame(Simulator a, Simulator b);
	int isSameCnt(Simulator a, Simulator b);
	int getColorCount();
	vector<int> getColors();
	bool operator==(Simulator& s);
	int hash();
	int applyPop(int rensa); //return score
	bool checkFireAbleReal(); //연쇄모형이 실제 게임에서 발화 가능한 행동인지 검사
	bool checkFieldIndex(int nowX, int nowY);
	static void dfs(int puyoField[][6], int i, int j, int c, bool visit[][6], std::vector<std::pair<int, int>>& popGroup);

	//control field
	/*
		input = A*10 + B
		A=rotation clockwise 0~3
		B=column location 0~5
	*/
	bool push(int input, PuyoTsumo pt);
	bool pushFast(int input, PuyoTsumo pt);
	bool pushFastNoPop(int input, PuyoTsumo pt);
	bool pushFastFloat(int input, PuyoTsumo pt);
	bool pushFast(int input, PuyoTsumo pt, int& rensa, int& score);

	//push and apply rensa and drop ojama for fast simulation
	//drop puyo and fire and drop ojama
	bool forceFire();//force rensa start, useful when after set field function
	bool forceFire(int& rensa, int& score);
	bool forceFireOnce(int& rensa);//for debugging
	void print();

	//util
	static string numToPuyo(int num);
	static int inputToIndex(int input);
	static void puyoInputPreview(int input, PuyoTsumo p);

	//get variable
	int getDeltaScore();
	bool getGameover();
	int getScore();
	vector<float> getDlInput();
	bool getAllClear(); //check all clear status, "after execute applyGrabvity()"
	void* getField();
	void* get14Field();
	int getOjama();
	bool getOjamaReady();
	PuyoTsumo getNowPuyo();//get puyo from bag and index++;
	PuyoTsumo* getNext();//just get 2next puyo
	int* getFieldHeight();
	int getLatestChigiriLength();
	int getLatestWaste();
	bool _latestHachiich = false;
	int getFieldSpace();
	float getFieldSpaceMax1();
	int getFieldOjama();
	bool checkNeedJump(int input);
	int getFieldUnstable();
	int getFieldUnstableSq();
	float getAvgHeight();
	float getFieldBelly();
	int getFieldUnstableSq33();
	//i열->(i+1)열의 높이차이가 날 때 마다 1씩 증가하는 필드 안정성

	//set variable
	void setScore(int val);
	void setNext(PuyoTsumo* pt);
	void appendScore(int val);
};
