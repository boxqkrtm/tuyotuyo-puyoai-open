#include "CrossFunc.h"
#include <iostream>

#ifdef _WIN32

void gotoxy(int x, int y) {
	static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	std::cout.flush();
	COORD coord = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hOut, coord);
}
#else
void gotoxy(int x, int y) {
	if (x == 0 && y == 0) printf("\x1b[H");
}
#endif

#ifdef _WIN32
void textcolor(int foreground, int background)
{
	int color = foreground + background * 16;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
#else
void textcolor(int foreground, int background)
{
	//
}
#endif
