#include "KeyStrokeQueue.h"

KeyStrokeQueue::KeyStrokeQueue(string inputStr)
{
	int bitdata = 0;
	for (int i = 0; i < inputStr.size(); i++)
	{
		if (inputStr[i] == '<')
			bitdata |= 1;
		if (inputStr[i] == '>')
			bitdata |= 2;
		if (inputStr[i] == 'V')
			bitdata |= 4;
		if (inputStr[i] == 'A')
			bitdata |= 8;
		if (inputStr[i] == 'B')
			bitdata |= 16;
		if (inputStr[i] == 'Q')
			bitdata |= 32;
		if (inputStr[i] == 'W')
			bitdata |= 64;
		if (inputStr[i] == ',' || inputStr[i] == '\0')
		{
			input.push_back(bitdata);
			bitdata = 0;
		}
	}
}

int KeyStrokeQueue::pop()//fifo
{
	if (input.size() == 0) return -1;
	int result = input[0];
	input.erase(input.begin());
	return result;
}

int KeyStrokeQueue::viewNextNext() {
	if (input.size() == 0) return -1;
	int result = -1;
	for (int i = 0; i < input.size(); i++)
	{
		if (KeyStrokeQueue::bitToString(input[i]) == "<" || KeyStrokeQueue::bitToString(input[i]) == ">") {
			result = input[i];
			input.erase(input.begin() + i);
			break;
		}

	}
	return result;

}

bool KeyStrokeQueue::isPressed(int bitdata, string key)
{
	if ((stringToBit(key) & bitdata) == stringToBit(key))
	{
		return true;
	}
	else
	{
		return false;
	}
}

string KeyStrokeQueue::bitToString(int bitdata)
{
	string str = "";
	if ((bitdata & 1) == 1)
		str += "<";
	if ((bitdata & 2) == 2)
		str += ">";
	if ((bitdata & 4) == 4)
		str += "V";
	if ((bitdata & 8) == 8)
		str += "A";
	if ((bitdata & 16) == 16)
		str += "B";
	if ((bitdata & 32) == 32)
		str += "Q";
	if ((bitdata & 64) == 64)
		str += "W";
	if (str == "")return " ";
	return str;
}

int KeyStrokeQueue::stringToBit(string inputStr)
{
	int bitdata = 0;
	if (inputStr[0] == '<')
		bitdata |= 1;
	if (inputStr[0] == '>')
		bitdata |= 2;
	if (inputStr[0] == 'V')
		bitdata |= 4;
	if (inputStr[0] == 'A')
		bitdata |= 8;
	if (inputStr[0] == 'B')
		bitdata |= 16;
	if (inputStr[0] == 'Q')
		bitdata |= 32;
	if (inputStr[0] == 'W')
		bitdata |= 64;
	return bitdata;
}
