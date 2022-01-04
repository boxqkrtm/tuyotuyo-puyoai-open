#include "MoveSimulator.h"

#include <iostream>
#include "CrossFunc.h"
#include "Simulator.h"
#include <string>

#define GRAVITY_PER_MOVECOUNT 5
#define COLORA 7
#define COLORB 8

using std::cout;
using std::endl;

MoveSimulator::MoveSimulator() {
	//init array to 0
	for (int i = 0; i < 14; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			_field[i][j] = 0;
			_fieldMoveLayer[i][j] = 0;
		}
	}
	_rot = 0;
	_x = 2;
	_y = 2;
	_zquickturn = 0;
	_xquickturn = 0;
	_05location = -1;
	_fieldMoveLayer[_y][_x] = COLORA;
	switch (_rot)
	{
	case 0:
		_fieldMoveLayer[_y - 1][_x] = COLORB;
		break;
	case 1:
		_fieldMoveLayer[_y][_x + 1] = COLORB;
		break;
	case 2:
		_fieldMoveLayer[_y + 1][_x] = COLORB;
		break;
	case 3:
		_fieldMoveLayer[_y][_x - 1] = COLORB;
		break;
	}
}

void MoveSimulator::setField(const int field[][6], bool field14row[6])//14 true = blank
{
	for (int j = 0; j < 6; j++)
	{
		_field[0][j] = field14row[j]==true?0:1;
	}
	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			_field[i+1][j] = field[i][j];
		}
	}
	//memcpy(_field, field, 13 * 6 * sizeof(int));
}


bool MoveSimulator::input(MoveCode c)
{
	frame++;
	int beforeRot = _rot;
	switch (c)
	{
	case MoveCode::left:
		_x -= 1;
		if (inputProcess(true) == false)
		{
			_x += 1;
			return false;
		}
		render();
		return true;
		break;

	case MoveCode::right:
		_x += 1;
		if (inputProcess(true) == false)
		{
			_x -= 1;
			return false;
		}
		render();
		return true;
		break;
	case MoveCode::down:
		if (_rot == 0)
		{
			if (_y + 1 > 13) return false;
			//세로 바닥만 없으면 가능
			if (_field[_y + 1][_x] == 0)
			{
				_y += 1;
				render();
				return true;
			}
			return false;
		}
		else if (_rot == 2)
		{
			if (_y + 2 > 13) return false;
			//세로 바닥만 없으면 가능
			if (_field[_y + 2][_x] == 0)
			{
				_y += 1;
				render();
				return true;
			}
			return false;
		}
		else
		{
			if (_y + 1 > 13) return false;
			//가로 양쪽 바닥이 비어있어야만 가능
			if (_field[_y + 1][_x] == 0
				&&
				(
					(_rot == 1 && _field[_y + 1][_x+1] == 0)
					||
					(_rot == 3 && _field[_y + 1][_x-1] == 0)
				)
				)
			{
				_y += 1;
				render();
				return true;
			}
			return false;
		}
		break;
	case MoveCode::counterClockwise:
		if (_zquickturn == 1) {
			_zquickturn = 0;
			_rot += 2;
			_y05z += 1;
			_rot = _rot % 4;
			if (inputProcess(false, 0) == false)
			{
				_rot = beforeRot;
				return false;
			}
			render();
			return true;
		}
		else {
			_rot -= 1;
			if (_rot < 0) _rot = 3;
			if (inputProcess(false, 0) == false)
			{
				_rot = beforeRot;
				return false;
			}
			render();
			return true;
		}
		break;
	case MoveCode::clockwise:
		if (_xquickturn == 1) {
			_y05x += 1;
			_xquickturn = 0;
			_rot += 2;
			_rot = _rot % 4;
			if (inputProcess(false ,1) == false)
			{
				_rot = beforeRot;
				return false;
			}
			render();
			return true;
		}
		else {
			_rot += 1;
			if (_rot > 3) _rot = 0;
			if (inputProcess(false, 1) == false)
			{
				_rot = beforeRot;
				return false;
			}
			render();
			return true;
		}
		break;
	case MoveCode::leftjump:
		if ((_x == 5 || _field[_y][_x + 1]) != 0 && (_x == 0 || _field[_y][_x - 1] != 0)) {
			_y--;
			input(MoveCode::left);
			input(MoveCode::left);
			return true;
		}
		return false;
		//양옆이 막힘
		break;
	case MoveCode::rightjump:
		if ((_x == 5 || _field[_y][_x + 1]) != 0 && (_x == 0 || _field[_y][_x - 1] != 0)) {
			_y--;
			input(MoveCode::right);
			input(MoveCode::right);
			input(MoveCode::right);
			return true;
		}
		return false;
		break;
	default:
		cout << "MoveSimulator::input invalid input" << endl;
		return false;
		break;
	}
	return false;
}
void MoveSimulator::render()
{
	for (int i = 0; i < 14; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			_fieldMoveLayer[i][j] = 0;
		}
	}
	//render
	_fieldMoveLayer[_y][_x] = COLORA;
	switch (_rot)
	{
	case 0:
		_fieldMoveLayer[_y-1][_x] = COLORB;
		break;
	case 1:
		_fieldMoveLayer[_y][_x + 1] = COLORB;
		break;
	case 2:
		_fieldMoveLayer[_y + 1][_x] = COLORB;
		break;
	case 3:
		_fieldMoveLayer[_y][_x - 1] = COLORB;
		break;
	}
}

//0spin counter.... 1spin clockwise
bool MoveSimulator::inputProcess(bool isMove, int spin)
{
	_zquickturn = 0;
	_xquickturn = 0;
	//wall kick etc...
	if (_field[_y][_x] != 0) return false;
	if (_x < 0 || _x >= 6 || _y < -1 || _y >= 14) return false;
	switch (_rot)
	{
	case 0:
		if (_field[_y - 1][_x] != 0 || _y == 0) return false;
		if (_field[_y - 1][_x] == 0) return true;
		break;
	case 1:
		//오른쪽이 비어있음
		if (_field[_y][_x + 1] == 0 && _x != 5) return true;
		//오른쪽이 막혔지만 왼쪽이 비어있음
		if ((_x == 5 || _field[_y][_x + 1] != 0) && _field[_y][_x - 1] == 0 && !(_x - 1 < 0)) {
			//오른쪽 벽 월킥
			_x -= 1;
			return true;
		}
		else if ((_x == 5||_field[_y][_x + 1]) != 0 && (_x == 0 || _field[_y][_x - 1] != 0)) {
			//양옆이 막힘
			//quickturn count += 1
			if(spin == 0){
				_rot += 1;
				if (_rot >= 3) _rot = 0;
				_zquickturn = 1;
			}	
			if (spin == 1) {
				_rot -= 1;
				if (_rot < 0) _rot = 3;
				_xquickturn = 1;
			}
			return true;
		}
		break;
	case 2:
		//
		if ((_field[_y + 1][_x] != 0 || _y >= 13)) {
			if (_y == 0) return false;//15row impossible
			if (isMove == true)return false;
			_y -= 1;
			return true;
		}
		if (_field[_y + 1][_x] == 0) return true;
		break;
	case 3:
		//맵밖
		//왼쪽 비어있음
		if (_field[_y][_x - 1] == 0 && _x != 0) return true;
		if ((_x == 0 || _field[_y][_x - 1] != 0) && _field[_y][_x + 1] == 0 && !(_x + 1 >= 6)) {
			//왼쪽 벽 월킥
			_x += 1; return true;
		}
		else if ((_x==5 || _field[_y][_x + 1] != 0)
			&& (_x ==0 || _field[_y][_x - 1] != 0)) {
			//양옆이 막힘
			if (spin == 0) {
				_rot += 1;
				if (_rot >= 3) _rot = 0;
				_zquickturn = 1;
			}
			if (spin == 1) {
				_rot -= 1;
				if (_rot < 0) _rot = 3;
				_xquickturn = 1;
			}
			return true;
		}
		break;
	default:
		//error
		cout << "MoveSimulator::moveLayerRender invalid _rot";
		return false;
		break;
	}
	return false;
}

void MoveSimulator::print()
{
	//print field with fieldMoveLayer
	textcolor(WHITE, BLACK);
	int i; int j;
	for (i = 1; i < 14; i++) {
		for (j = 0; j < 6; j++) {
			string p = "";
			if (i == 2 && j == 2 && _field[i][j] == 0 && _fieldMoveLayer[i][j] == 0) {
				textcolor(RED, BLACK);
				p = "XX";
			}
			else {
				if (_fieldMoveLayer[i][j] != 0) {
					//print move layer
					p = Simulator::numToPuyo(_fieldMoveLayer[i][j]);
				}
				else
				{
					//print field when move layer is 0
					p = Simulator::numToPuyo(_field[i][j]);
				}
			}
			cout << p;
		}
		std::cout << std::endl;
	}
	textcolor(WHITE, BLACK);
}
