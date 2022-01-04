#include "ProcessMemory.h"

long long ProcessMemory::getPointerOffset(std::vector<long long> p)
{
    {
        long long nowAddr = _baseAddress;

        for (int i = 0; i < p.size() - 1; i++)
        {
            nowAddr += p[i];
            long long buf = 0;
            if (ReadMemory(nowAddr, &nowAddr, sizeof(nowAddr)))
            {
                //cout << "find addr " << hex << nowAddr << endl;
                //succeed
            }
            else {
                //cout << "fail " << hex << nowAddr << endl;
                return 0;
            }
        }
        return nowAddr + p.back();
    }
}

ProcessMemory::ProcessMemory(std::wstring param, std::wstring param2, bool trust)
{
    windowName = param;
    processName = param2;
    TrustProcess = trust;
    _baseAddress = 0;
    tryGet();
}

ProcessMemory::~ProcessMemory()
{
    CloseHandle(_mainProcess);
}

bool ProcessMemory::tryGet()
{
    //get PID
    HWND WindowHandle = FindWindow(nullptr, windowName.c_str());
    DWORD PID;
    GetWindowThreadProcessId(WindowHandle, &PID);
    _pid = PID;

    //get handle
    _mainProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _pid);
    //get baseAddress
    PVOID hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    HMODULE Module = GetModule(hProcess);

    if (Module == NULL) printf("NULL!!");
    DWORD_PTR BaseAddress = (DWORD_PTR)Module;
    _baseAddress = (long long)Module;
    //printf("connect pid:%d hex:%llx", _pid, _baseAddress);
    return true;
}

HMODULE ProcessMemory::GetModule(HANDLE IpHandle)
{
    HMODULE hMods[1024];
    HANDLE pHandle = IpHandle;
    DWORD cbNeeded;
    unsigned int i;

    if (EnumProcessModules(pHandle, hMods, sizeof(hMods), &cbNeeded))
    {
        for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
        {
            TCHAR szModName[MAX_PATH];
            if (GetModuleFileNameEx(pHandle, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
            {

                std::wstring tmp = szModName;
                std::wstring wstrModName;
                wstrModName.assign(tmp.begin(), tmp.end());
                //you will need to change this to the name of the exe of the foreign process
                std::wstring lowerName = processName;
                transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char c) { return std::tolower(c); });
                //std::wcout << processName << std::endl;
                //std::wstring wstrModContain = lowerName + L".exe";
                std::wstring wstrModContain = processName;
                if (wstrModName.find(wstrModContain) != std::string::npos)
                {
                    CloseHandle(pHandle);
                    return hMods[i];
                }
            }
        }
    }
    return nullptr;
}

HANDLE ProcessMemory::GetProcessByName(PCSTR name)
{
    DWORD_PTR pid = 0;

    // Create toolhelp snapshot.
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    ZeroMemory(&process, sizeof(process));
    process.dwSize = sizeof(process);

    // Walkthrough all processes.
    if (Process32First(snapshot, &process))
    {
        do
        {
            // Compare process.szExeFile based on format of name, i.e., trim file path
            // trim .exe if necessary, etc.
            if (std::string((char*)process.szExeFile) == std::string(name))
            {
                pid = process.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &process));
    }

    CloseHandle(snapshot);
    if (pid != 0)
    {
        _pid = pid;
        //get pid succeed
        return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    }
    // Not found
    return NULL;
}

bool ProcessMemory::ReadMemory(long long address, void* buf, int len)
{
    return ReadProcessMemory(_mainProcess, (LPCVOID)address, buf, len, NULL);
}

bool ProcessMemory::WriteMemory(long long* address, void* buf, int len)
{
    //return WriteProcessMemory(_mainProcess, (LPCVOID)address, buf, len, NULL);
    return false;
}
