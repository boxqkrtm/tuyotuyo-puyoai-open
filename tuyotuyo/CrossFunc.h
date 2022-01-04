#pragma once
//sleep function and gotoxy for cross

#define BLACK 0 
#define BLUE 9	
#define GREEN 2 
#define RED 12
#define MAGENTA 13
#define LIGHTGRAY 7 
#define YELLOW 14 
#define WHITE 15 
#define DARKGRAY 8
#ifdef __linux__ 
#define DLLOC "tuyo/"
#include <unistd.h>
#define Sleep(x) usleep(x*1000)
#elif _WIN32
#define DLLOC "C:\\tuyo\\"
#include <Windows.h>
#else
#endif
void gotoxy(int x, int y);
void textcolor(int foreground, int background);
