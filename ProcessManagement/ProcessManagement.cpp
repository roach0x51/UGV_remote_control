#include <windows.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <string>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <tchar.h>
#include <dos.h>
#include <stdlib.h>
#include <cstdio>
#include <tlhelp32.h>
#include "sharedmemory.h"
#include "sharedmemoryfcn.h"

#define BUF_SIZE 48
#define ERR_CREATE_FILEMAP 4
#define ERR_MAP_FILEVIEW   5
#define NUM_UNITS  5
// Defines the critical routines list
#define EXEC_MASK 0x001d // 1 1101
#define XBOX_MASK 0x0002 // 0 0010

/*
SharedMemory:1,
XBox:1,
UGVControl:1,
Laser:1,
ProcessManagement:1,
UGVModel:1
Matlab:1
*/

using namespace std;
bool IsProcessRunning(const char *processName);

int main()
{
	STARTUPINFO s[10];
	PROCESS_INFORMATION p[10];

	// Start up sequence
	TCHAR* Units[10] = //
	{
		TEXT("MemoryManagement.exe"),
		TEXT("UGVControl.exe"),
		TEXT("VirtualWorld.exe"),
		TEXT("Xbox.exe"),
		TEXT("Laser.exe"),
	};

	// File handles	
	HANDLE FileMapLaserData;
	HANDLE FileMapXboxData;
	HANDLE FileMapUGVControl;
	HANDLE FileMapProcessManagement;

	ProcessManagement *pProcessManagement;
	XboxData *pXboxData;
	UGVControl *pUGVControlData;
	LaserData *pLaserData;

	int HeartbeatCounter = 0;

	__int64 Frequency;

	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_SHOW);

	QueryPerformanceFrequency((LARGE_INTEGER *)&Frequency);

	//Run all other processes =======
	for (int i = 0; i < NUM_UNITS; i++)
	{
		// Check if each process is running
		if (!IsProcessRunning(Units[i]))
		{
			ZeroMemory(&s[i], sizeof(s[i]));
			s[i].cb = sizeof(s[i]);
			ZeroMemory(&p[i], sizeof(p[i]));
			// Start the child processes. 

			if (!CreateProcess(NULL,   // No module name (use command line)
				Units[i],        // Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				CREATE_NEW_CONSOLE,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory 
				&s[i],            // Pointer to STARTUPINFO structure
				&p[i])           // Pointer to PROCESS_INFORMATION structure
				)
			{
				printf("%s failed (%d).\n", Units[i], GetLastError());
				_getch();
				return -1;
			}
		}
		std::cout << "Started: " << Units[i] << endl;
		Sleep(1000);
	}

	/////////////////////////
	// Shared Memory Setup //
	/////////////////////////


	// Requesting a view of ProcessManagementData memory
	FileMapProcessManagement = SMOpenReadWrite(szNameProcessManagementData);
	if (FileMapProcessManagement == NULL)
	{
		_tprintf(TEXT("Could not open ProcessManagementData file view (%d).\n"), GetLastError());
		_getch();
		return 1;
	}

	// Requesting access to ProcessManagementData memory
	pProcessManagement = (ProcessManagement*)SMAccessReadWrite(FileMapProcessManagement, sizeof(ProcessManagement));
	if (pProcessManagement == NULL)
	{
		_tprintf(TEXT("Could not open access to ProcessManagementData memory (%d).\n"), GetLastError());
		_getch();
		CloseHandle(FileMapProcessManagement);
		return 1;
	}

	// Requesting a view of LaserData memory
	FileMapLaserData = SMOpenReadWrite(szNameLaserData);
	if (FileMapLaserData == NULL)
	{
		_tprintf(TEXT("Could not open LaserData view (%d).\n"), GetLastError());
		_getch();
		return 1;
	}

	// Requesting access to pLaserData memory
	pLaserData = (LaserData*)SMAccessReadWrite(FileMapLaserData, sizeof(LaserData));
	if (FileMapLaserData == NULL)
	{
		_tprintf(TEXT("Could not open access to pLaserData (%d).\n"), GetLastError());
		_getch();
		CloseHandle(FileMapLaserData);
		return 1;
	}

	// Requesting a view of XboxData memory
	FileMapXboxData = SMOpenReadWrite(szNameXboxData);
	if (FileMapXboxData == NULL)
	{
		_tprintf(TEXT("Could not open XboxData view (%d).\n"), GetLastError());
		_getch();
		return 1;
	}

	// Requesting access to XboxData memory
	pXboxData = (XboxData*)SMAccessReadWrite(FileMapXboxData, sizeof(XboxData));
	if (FileMapXboxData == NULL)
	{
		_tprintf(TEXT("Could not open access to pXboxData (%d).\n"), GetLastError());
		_getch();
		CloseHandle(FileMapXboxData);
		return 1;
	}


	// Requesting a view of UGVControlData memory
	FileMapUGVControl = SMOpenReadWrite(szNameUGVControlData);
	if (FileMapUGVControl == NULL)
	{
		_tprintf(TEXT("Could not open UGVControlData view (%d).\n"), GetLastError());
		_getch();
		return 1;
	}

	// Requesting access to UGVControlData memory
	pUGVControlData = (UGVControl*)SMAccessReadWrite(FileMapUGVControl, sizeof(UGVControl));
	if (FileMapUGVControl == NULL)
	{
		_tprintf(TEXT("Could not open access to UGVControlData (%d).\n"), GetLastError());
		_getch();
		CloseHandle(FileMapUGVControl);
		return 1;
	}



	pProcessManagement->ShutDown.Flags.ProcessManagement = 0;
	//Main loop
	while (!pProcessManagement->ShutDown.Flags.ProcessManagement)
	{
		if (_kbhit()) {
			pProcessManagement->ShutDown.Status = 0xFFFF;
			break;
		}
		pProcessManagement->HeartBeat.Flags.ProcessManagement = 1;
		if ((pProcessManagement->HeartBeat.Status & EXEC_MASK) != EXEC_MASK) {
			cout << (pProcessManagement->HeartBeat.Status & EXEC_MASK) << endl;
			HeartbeatCounter++;
		}
		else
			HeartbeatCounter = 0;

		// Initiate shutdown if count exceeds
		if (HeartbeatCounter > 25)
		{
			pProcessManagement->ShutDown.Status = 0xFFFF;
			break;
		}


		if (!pProcessManagement->HeartBeat.Flags.XBox) {
			pXboxData->SetSpeed = 0;
			pXboxData->SetSteering = 0;

			// Restart noncritical child processes
			if (!IsProcessRunning("Xbox.exe"))
			{
				ZeroMemory(&s[0], sizeof(s[0]));
				s[0].cb = sizeof(s[0]);
				ZeroMemory(&p[0], sizeof(p[0]));
				if (!CreateProcess(NULL,   // No module name (use command line)
					"XBox.exe",        // Command line
					NULL,           // Process handle not inheritable
					NULL,           // Thread handle not inheritable
					FALSE,          // Set handle inheritance to FALSE
					CREATE_NEW_CONSOLE,              // No creation flags
					NULL,           // Use parent's environment block
					NULL,           // Use parent's starting directory 
					&s[0],            // Pointer to STARTUPINFO structure
					&p[0])           // Pointer to PROCESS_INFORMATION structure
					)
				{
					printf("XBox.exe failed (%d).\n", GetLastError());
					_getch();
					return -1;
				}
				else
					std::cout << "Restarted XBox.exe " << endl;
			}
		}

		cout << pProcessManagement->HeartBeat.Status << " " << HeartbeatCounter << endl;
		pProcessManagement->HeartBeat.Status = 0;
		Sleep(500);
	}// End of main loop

	 //Cleanup on Exit.

	UnmapViewOfFile(pProcessManagement);
	UnmapViewOfFile(pXboxData);
	UnmapViewOfFile(pUGVControlData);
	UnmapViewOfFile(pLaserData);

	CloseHandle(FileMapXboxData);
	CloseHandle(FileMapUGVControl);
	CloseHandle(FileMapProcessManagement);
	CloseHandle(FileMapLaserData);
	std::cout << "Terminating normally!" << endl;
	Sleep(1000);

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