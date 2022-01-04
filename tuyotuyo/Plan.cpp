#include "Plan.h"
#include <algorithm>
using std::endl;

int Plan::pushShapeTest(Simulator s, Shape sh, int x, int y)
{
	int(*nowShape)[2] = _shapes[(int)sh];
	int(*sField)[6] = (int(*)[6])s.getField();
	//move test
	int nowX = x;
	int nowY = y;
	for (int i = 0; i < 4; i++)
	{
		nowX += nowShape[i][0];
		nowY += nowShape[i][1];
		if (nowX < 0 || nowX >5 || nowY < 0 || nowY > 13) return SHAPE_OUTOFRANGE;
	}
	nowX = x;
	nowY = y;
	for (int i = 0; i < 4; i++)
	{
		nowX += nowShape[i][0];
		nowY += nowShape[i][1];
		if (sField[nowY][nowX] != 0) {
			return SHAPE_CONFLICTPUYO;
		}
	}
	return SHAPE_SUCCEED;
}

int Plan::pushShapeTestIngame(Simulator s, Shape sh, int x, int y)
{
	int(*nowShape)[2] = _shapes[(int)sh];
	int(*sField)[6] = (int(*)[6])s.getField();
	int color = sField[y][x];
	//move test
	int nowX = x;
	int nowY = y;
	for (int i = 0; i < 4; i++)
	{
		nowX += nowShape[i][0];
		nowY += nowShape[i][1];
		if (nowX < 0 || nowX >5 || nowY < 0 || nowY > 13) return SHAPE_OUTOFRANGE;
	}
	nowX = x;
	nowY = y;
	for (int i = 0; i < 4; i++)
	{
		nowX += nowShape[i][0];
		nowY += nowShape[i][1];
		if (sField[nowY][nowX] != 0 && sField[nowY][nowX] != color) {
			return SHAPE_CONFLICTPUYO;
		}
	}
	return SHAPE_SUCCEED;
}

//x, y
vector<pair<int, int>> Plan::findOutSide(Simulator& s)
{
	vector<pair<int, int>> result;
	result.reserve(64);
	int(*sField)[6] = (int(*)[6])s.getField();
	if (s.getAllClear() == true)
	{
		for (int i = 0; i < 6; i++)
		{
			result.push_back(pair<int,int>(i, 12));
		}
		return result;
	}
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 13; j++)
		{
			//search up to down
			if (sField[j][i] == 0) {
				//search both right and left
				if (i - 1 >= 0 && sField[j][i - 1] != 0) {
					//left
					result.push_back(pair<int, int>(i - 1, j));
				}
				if ((i + 1) < 6) {
					//right
					if (sField[j][(i + 1) % 6] != 0) result.push_back(pair<int, int>(i + 1, j));
				}
			}
			else {
				//bottom
				result.push_back(pair<int, int>(i, j));
				break;
			}
			if (j == 12) {
				result.push_back(pair<int, int>(i, j));
				break;
			}
		}
	}
	//sort des y
	//sort(result.begin(), result.end(), findOutSideSort);
	//unique(result.begin(), result.end(), findOutSideSort);
	for (int i = 0; i < result.size()-1; i++)
	{
		for (int j = i+1; j < result.size(); j++) {
			if (result[i].first == result[j].first
				&& result[i].second == result[j].second) {
				result.erase(result.begin() + i);
			}
		}
	}
	return result;
}

//sandwitch or tailing
bool Plan::pushShape(Simulator& s, Shape sh, int color, int x, int y, bool removeKey, bool isNow18Ren, int moveDelta)
{
	int mdx[] = { 0,0,1,0,-1 };
	int mdy[] = { 0,1,0,-1,0 };
	int(*nowShape)[2] = _shapes[(int)sh];
	int(*sField)[6] = (int(*)[6])s.getField();
			vector<pair<int, int>> shapeHistory;
			shapeHistory.reserve(10);
	int nowX, nowY;
	//move test
	bool isLoop = true;
	bool isSafe;
		switch (Plan::pushShapeTest(s, sh, x + mdx[moveDelta], y + mdy[moveDelta])) {
		case SHAPE_OUTOFRANGE://범위 벗어남, 컬러겹침, 불가능한 늘리기
			return false;
		case SHAPE_CONFLICTPUYO:
			nowX = x + mdx[moveDelta];
			nowY = y + mdy[moveDelta];
			for (int i = 0; i < 4; i++)
			{
				nowX += nowShape[i][0];
				nowY += nowShape[i][1];
				if (sField[nowY][nowX] != 0) {
					if (s.applyAntiGravityTargetOnce(nowX, nowY) == true) {
						//push to up
					}
					else {
						return false;
					}
				}
				sField[nowY][nowX] = color;
			}
			if (s.applyGravityOnce() > 0) return false; // 공중에 뜬 경우 취소 다른 모양과 중복됨
			//if (s.checkFireAbleReal() == false) return false;

			break;
		case SHAPE_SUCCEED://1연쇄만 허용 2연쇄부터는 끼우기 필수
			Simulator test = Simulator(s);
			test.forceFire();
			//apply
			nowX = x + mdx[moveDelta];
			nowY = y + mdy[moveDelta];
			shapeHistory.clear();
			for (int i = 0; i < 4; i++)
			{
				nowX += nowShape[i][0];
				nowY += nowShape[i][1];
				sField[nowY][nowX] = color;
				shapeHistory.push_back(pair<int, int>(nowX, nowY));
			}
			if (s.applyGravityOnce() > 0)
			{
				return false; // 공중에 뜬 경우 취소 다른 모양과 중복됨
			}
			//필수는 아니지만 더 좋은 모형을 위함
			if (s.checkFireAbleReal() == false)
			{
				//cout << "Cant fire" << endl;
				return false;
			}

			if (removeKey == true) 
			{
				//키뿌요를 때어낼 곳은
				//지금 끼워넣은것에서 바깥쪽에 해당하는것을 때어냄
				//때어냈는데 뿌요가 공중에 떠 있다면 불가능한 수
				for (auto a : shapeHistory) {
					for (auto b : Plan::findOutSide(s)) {
						if (a.first == b.first
							&& a.second == b.second) {
							sField[a.second][a.first] = 0;
							if (s.applyGravityOnce() > 0) return false;
							else return true;
						}
					}
				}
				return false; //error
			}
			isLoop = false;
			break;
		}
	if (isNow18Ren==false&&s.getGameover() == true) return false;
	return true;
}

bool Plan::pushShapeIngame(Simulator& s, Shape sh, int color, int x, int y, bool removeKey, bool isNow18Ren, int moveDelta)
{
	int mdx[] = { 0,0,1,0,-1 };
	int mdy[] = { 0,1,0,-1,0 };
	int(*nowShape)[2] = _shapes[(int)sh];
	int(*sField)[6] = (int(*)[6])s.getField();
	vector<pair<int, int>> shapeHistory;
	shapeHistory.reserve(10);
	int nowX, nowY;
	//move test
	bool isLoop = true;
	bool isSafe;
	switch (Plan::pushShapeTest(s, sh, x + mdx[moveDelta], y + mdy[moveDelta])) {
	case SHAPE_OUTOFRANGE://범위 벗어남, 컬러겹침, 불가능한 늘리기
		return false;
	case SHAPE_CONFLICTPUYO:
		nowX = x + mdx[moveDelta];
		nowY = y + mdy[moveDelta];
		for (int i = 0; i < 4; i++)
		{
			nowX += nowShape[i][0];
			nowY += nowShape[i][1];
			if (sField[nowY][nowX] != 0) {
				if (s.applyAntiGravityTargetOnce(nowX, nowY) == true) {
					//push to up
				}
				else {
					return false;
				}
			}
			sField[nowY][nowX] = color;
		}
		if (s.applyGravityOnce() > 0) return false; // 공중에 뜬 경우 취소 다른 모양과 중복됨
		//if (s.checkFireAbleReal() == false) return false;

		break;
	case SHAPE_SUCCEED://1연쇄만 허용 2연쇄부터는 끼우기 필수
		Simulator test = Simulator(s);
		test.forceFire();
		//apply
		nowX = x + mdx[moveDelta];
		nowY = y + mdy[moveDelta];
		shapeHistory.clear();
		for (int i = 0; i < 4; i++)
		{
			nowX += nowShape[i][0];
			nowY += nowShape[i][1];
			sField[nowY][nowX] = color;
			shapeHistory.push_back(pair<int, int>(nowX, nowY));
		}
		if (s.applyGravityOnce() > 0)
		{
			return false; // 공중에 뜬 경우 취소 다른 모양과 중복됨
		}
		//필수는 아니지만 더 좋은 모형을 위함
		if (s.checkFireAbleReal() == false)
		{
			//cout << "Cant fire" << endl;
			return false;
		}

		if (removeKey == true)
		{
			//키뿌요를 때어낼 곳은
			//지금 끼워넣은것에서 바깥쪽에 해당하는것을 때어냄
			//때어냈는데 뿌요가 공중에 떠 있다면 불가능한 수
			for (auto a : shapeHistory) {
				for (auto b : Plan::findOutSide(s)) {
					if (a.first == b.first
						&& a.second == b.second) {
						sField[a.second][a.first] = 0;
						if (s.applyGravityOnce() > 0) return false;
						else return true;
					}
				}
			}
			return false; //error
		}
		isLoop = false;
		break;
	}
	if (isNow18Ren == false && s.getGameover() == true) return false;
	return true;
}

bool findOutSideSort(pair<int, int> a, pair<int, int> b)
{
	return a.second > b.second ? true : false;
}

void Plan::appendKeyPuyo(Simulator& s) {
	int(*sField)[6] = (int(*)[6])s.getField();
	auto outside = findOutSide(s);
	int index = 0;
	int keyLocation = -1;
	int maxIndex = -1;
	int maxRensa = -1;
	int maxColor = 0;
	for (auto elem : outside) {
		//copy
		Simulator sTemp = Simulator(-1);
		sTemp.set(sField);
		int(*tempField)[6] = (int(*)[6])sTemp.getField();
		//test keypuyo ( highest rensa )
		int color = sField[elem.second][elem.first];//get keypuyo color
		if (sField[elem.second - 1][elem.first] == 0) {
			tempField[elem.second - 1][elem.first] = color;
			keyLocation = 0;
		}
		else if (sField[elem.second][elem.first - 1] == 0) {
			tempField[elem.second][elem.first - 1] = color;
			keyLocation = 1;
		}
		else if (sField[elem.second][elem.first + 1] == 0) {
			tempField[elem.second][elem.first + 1] = color;
			keyLocation = 2;
		}
		int rensa = checkPlanRensa(sTemp);
		if (maxRensa < rensa) {
			maxRensa = rensa;
			maxIndex = index;
			maxColor = color;
		}
		index++;
	}
	if (maxColor != 0) {
		auto elem = outside[maxIndex];
		if (keyLocation == 0) {
			sField[elem.second - 1][elem.first] = maxColor;
			keyLocation = 0;
		}
		else if (keyLocation == 1) {
			sField[elem.second][elem.first - 1] = maxColor;
			keyLocation = 1;
		}
		else if (keyLocation == 2) {
			sField[elem.second][elem.first + 1] = maxColor;
		}
	}
	else {
		//if no connect 3 puyo just skip appending key puyo
	}
	s.applyGravity();
}

int Plan::checkPlanRensa(Simulator s) {
	int rensa = 1;
	int score = 0;
	s.forceFire(rensa, score);
	return rensa;
}

//for real stacking (without keypuyo)
int Plan::pushShapeStack(Simulator& s, Shape sh, int color, int x, int y)
{
	//backup
	int(*sField)[6] = (int(*)[6])s.getField();
	Simulator sBackup = Simulator(-1);
	sBackup.set((int(*)[6])s.getField());

	//appendKeyPuyo for plan rensa
	appendKeyPuyo(s);
	int beforeRensa = checkPlanRensa(s);
	//push shape without key
	bool result = pushShape(s, sh, color, x, y, true);
	//fail
	if (result == false) {
		//if fail push rollback keypuyo and return -1
		s.set((int(*)[6])sBackup.getField());
		return -1;
	}
	else if (result == true) {
		//fail2
		if (checkPlanRensa(s) > 0) {
			s.set((int(*)[6])sBackup.getField());
			return -1;
		}
		//if succeed return rensa
		Simulator sTemp = Simulator(-1);
		sTemp.set(sField);
		appendKeyPuyo(sTemp);
		if (beforeRensa < checkPlanRensa(sTemp)) {
			return checkPlanRensa(sTemp);
		}
		else {
			//plan fail
			return -1;
		}
	}
	return -1;
}


