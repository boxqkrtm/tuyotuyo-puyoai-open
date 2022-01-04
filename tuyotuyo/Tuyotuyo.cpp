#include "Tuyotuyo.h"
#include "FormAIv2.h"

using std::cin;
using std::cout;
using std::endl;
using std::make_tuple;
using std::tuple;
using std::get;

#if !_WINDLL

int main()
{
	while (true)
	{
		//a.reset();
		//a.save();
#ifdef _WIN32
		system("chcp 65001"); //utf8
		system("cls");
#endif // WIN32
		//cout << sizeof(Duel) << endl;
		int select;

		cout << "-Tuyotuyo client Open 2.1-" << endl;
		cout << endl;
		cout << "1.AI vs AI : " << endl;
		cout << "2.AI vs nothing : " << endl;
		cout << "3.Human vs AI : " << endl;
		cout << endl;
		cout << "Select menu : ";
		cin >> select;
		switch (select)
		{
		case 1:
			DuelAIAI();
			break;
		case 2:
			DuelToko();
			break;
		case 3:
			DuelHumanAI();
			break;
		}
	}
	return 0;
}

#endif
