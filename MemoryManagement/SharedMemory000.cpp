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
#include "sharedmemory.h"
#include "sharedmemoryfcn.h"

#define BUF_SIZE 48
#define ERR_CREATE_FILEMAP 4
#define ERR_MAP_FILEVIEW   5


using namespace std;

const string DateTimeNow();
bool IsProcessRunning(const char *processName);

int main()
{  
	// File handles	
	HANDLE FileMapLaserData;
	HANDLE FileMapXboxData;
	HANDLE FileMapUGVControl;
	HANDLE FileMapProcessManagement;

	ProcessManagement *pProcessManagement;


	/////////////////////////
	// Shared Memory Setup //
	/////////////////////////




	// Create shared memory for LaserData
	FileMapLaserData = SMCreateReadWrite(sizeof(LaserData), szNameLaserData);
	if (FileMapLaserData == NULL) 
	{ 
		_tprintf(TEXT("Could not create shared memory for LaserData (%d).\n"), GetLastError());
		_getch();

		return 1;
	}

	// Create shared memory for XboxData
	FileMapXboxData = SMCreateReadWrite(sizeof(XboxData), szNameXboxData);
	if (FileMapXboxData == NULL) 
	{ 
		_tprintf(TEXT("Could not create shared memory for XboxData (%d).\n"), GetLastError());
		_getch();

		return 1;
	}

	// Create shared memory for UGVControl
	FileMapUGVControl = SMCreateReadWrite(sizeof(UGVControl), szNameUGVControlData);
	if (FileMapUGVControl == NULL) 
	{ 
		_tprintf(TEXT("Could not create shared memory for UGVControl (%d).\n"), GetLastError());
		_getch();

		return 1;
	}

	// Create shared memory for ProcessManagementData
	FileMapProcessManagement = SMCreateReadWrite(sizeof(ProcessManagement), szNameProcessManagementData);
	if (FileMapProcessManagement == NULL) 
	{ 
		_tprintf(TEXT("Could not create shared memory for ProcessManagementData (%d).\n"), GetLastError());
		_getch();

		return 1;
	}
	//Shared Memory Setup END ------------------------------
	// Requesting a view of Modules memory
	FileMapProcessManagement = SMOpenReadWrite(szNameProcessManagementData);
	if (FileMapProcessManagement == NULL) 
	{ 
		_tprintf(TEXT("Could not open Modules file view (%d).\n"), GetLastError());
		return 1;
	} 

	// Requesting access to Modules memory
	pProcessManagement = (ProcessManagement*)SMAccessReadWrite(FileMapProcessManagement, sizeof(ProcessManagement));
	if (pProcessManagement == NULL) 
	{ 
		_tprintf(TEXT("Could not open access to Modules memory (%d).\n"), GetLastError()); 
		CloseHandle(FileMapProcessManagement);
		return 1;
	}
	//Module execution start
	cout<<"Memory Setup Complete"<<endl;

	//Module execution begins.
	//Flag all programs permission to run their loops
	pProcessManagement->ShutDown.Status = 0x0000;
	//Set heartbeats to be true to begin with
	pProcessManagement->HeartBeat.Status = 0xFFFF;

	pProcessManagement->ShutDown.Flags.SharedMemory = 0;
	while(!pProcessManagement->ShutDown.Flags.SharedMemory)
	{

		if(_kbhit())
			break;
		pProcessManagement->HeartBeat.Flags.SharedMemory = 1;  
		Sleep(500);
	}

	//Cleanup on Exit.

	UnmapViewOfFile(pProcessManagement);

	CloseHandle(FileMapLaserData);
	CloseHandle(FileMapXboxData);
	CloseHandle(FileMapUGVControl);
	CloseHandle(FileMapProcessManagement);
	cout << "Terminating normally!" << endl;
	Sleep(1000);

	return 0;
}


const string DateTimeNow()
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	//tstruct = *localtime(&now);
	localtime_s(&tstruct, &now);

	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);
	return buf;
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
