#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <string>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <tchar.h>
#include <dos.h>
#include <stdlib.h>
#include <cstdio>
#include <tlhelp32.h>

#define BUF_SIZE 48
#define NUM_UNITS  6

using namespace std;

const string DateTimeNow();
bool IsProcessRunning(const char *processName);

int main()
{
	STARTUPINFO s[10];
	PROCESS_INFORMATION p[10];
	ifstream activate;

	TCHAR* Units[10] = //
		{TEXT("A_MemoryManager.exe"),		
		TEXT("F_GPS.exe"),
		TEXT("G_Laser.exe"),
		TEXT("C_Xbox.exe"),
		TEXT("E_TractorControl.exe"),
		TEXT("I_Steering.exe")};

	for(int i = 0; i < NUM_UNITS; i++)
	{
		if(!IsProcessRunning(Units[i]))
		{
			ZeroMemory( &s[i], sizeof(s[i]) );
			s[i].cb = sizeof(s[i]);
			ZeroMemory( &p[i], sizeof(p[i]) );
				if(!CreateProcess(0,Units[i],0,0,0,CREATE_NEW_CONSOLE /*CREATE_NO_WINDOW*/,0,0,&s[i],&p[i])) 
			return -1;   
		}
		Sleep(1000);
	}
	return 0;
}


bool IsProcessRunning(const char *processName)
{
    bool exists = false;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry))
        while (Process32Next(snapshot, &entry))
            if (!_stricmp(entry.szExeFile, processName))
                exists = true;

    CloseHandle(snapshot);
    return exists;
}

