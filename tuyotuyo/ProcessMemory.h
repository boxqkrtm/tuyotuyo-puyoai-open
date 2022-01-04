#pragma once
#if __linux__
#else
#include <Windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <psapi.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

class ProcessMemory
{
public:
    //init
    std::wstring windowName;
    std::wstring processName;
    bool TrustProcess;
    bool _opened;

    //for io
    long long _baseAddress;
    int _pid;
    HANDLE _mainProcess;

    long long getPointerOffset(std::vector<long long> p);
    ProcessMemory(std::wstring param, std::wstring param2, bool trust=false);
    ~ProcessMemory();
    bool tryGet();
    HMODULE GetModule(HANDLE IpHandle);
    HANDLE GetProcessByName(PCSTR name);
    bool ReadMemory(long long address, void* buf, int len);
    bool WriteMemory(long long* address, void* buf, int len);
};
#endif
