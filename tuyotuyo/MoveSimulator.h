#pragma once

enum class MoveCode
{
	left, right, down, counterClockwise, clockwise, leftjump, rightjump
};

//012,34

class MoveSimulator {
private:
	int frame;//n회 움직이면 자연으로 내려가는 동작 구현용
public:
	int _x;//회전 축 기준
	int _y;
	int _y05z = 0;
	int _y05x = 0;
	int _rot;
	int _zquickturn;
	int _xquickturn;
	int _05location;
	int _fieldMoveLayer[14][6];
	int _field[14][6];
	MoveSimulator();
	void setField(const int field[][6], bool field14row[6]);
	bool input(MoveCode c);
	void render();
	bool inputProcess(bool isMove, int spin = 0);
	void print();
};
