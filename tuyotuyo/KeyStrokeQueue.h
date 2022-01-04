#pragma once
#include <vector>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;
using std::vector;

class KeyStrokeQueue
{
	//00~53까지의 입력을 위해서 키 입력을 저장함 <vector>사용
	//
public:
	vector<int> input;
	//bit style 2^(n-1), 1=left 2=right 3=down, 4=leftspin, 5=rightspin
	//5bit 00000

	//string -> bitdata
	//<, >, V ,A < B(V is alphabet V)
	//no space with comma (multiple press, max 2)
	//ex) <A,V,V,V,V
	KeyStrokeQueue(string inputStr);

	int pop();

	int viewNextNext();

	//키를 받아서 사용 할 때 비트값에 따라 이 키가 눌러지고 있는지 반환
	static bool isPressed(int bitdata, string key);

	static string bitToString(int bitdata);

	static int stringToBit(string inputStr);
};
