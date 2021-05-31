//
//Need to implement the xbox disconnect ditect funtion,
//when the xbox is disconnected, all xbox data in shared memory need to be overwrited to "0".
//
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
#include "controller.h"

#define BUF_SIZE 48
#define ERR_CREATE_FILEMAP 4
#define ERR_MAP_FILEVIEW   5


using namespace std;

int main()
{
	// File handles	
//	HANDLE FileMapLaserData;
	HANDLE FileMapXboxData;
	HANDLE FileMapUGVControl;
	HANDLE FileMapProcessManagement;

	ProcessManagement *pProcessManagement;
	XboxData *pXboxData;
	UGVControl *pUGVControlData;

	Controller Xbox(0);

	double XboxTimeStamp;
	__int64 Frequency, Counter;

	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_SHOW);

	QueryPerformanceFrequency((LARGE_INTEGER *) &Frequency);
	/////////////////////////
	// Shared Memory Setup //
	/////////////////////////


	// Requesting a view of ProcessManagementData memory
	FileMapProcessManagement = SMOpenReadWrite(szNameProcessManagementData);
	if (FileMapProcessManagement == NULL) 
	{ 
		_tprintf(TEXT("Could not open ProcessManagementData file view (%d).\n"), GetLastError());
		return 1;
	} 

	// Requesting access to ProcessManagementData memory
	pProcessManagement = (ProcessManagement*)SMAccessReadWrite(FileMapProcessManagement, sizeof(ProcessManagement));
	if (pProcessManagement == NULL) 
	{ 
		_tprintf(TEXT("Could not open access to ProcessManagementData memory (%d).\n"), GetLastError()); 
		CloseHandle(FileMapProcessManagement);
		return 1;
	}

	// Requesting a view of XboxData memory
	FileMapXboxData = SMOpenReadWrite(szNameXboxData);
	if (FileMapXboxData == NULL) 
	{ 
		_tprintf(TEXT("Could not open XboxData view (%d).\n"), GetLastError());
		return 1;
	} 

	// Requesting access to pXboxData memory
	pXboxData = (XboxData*)SMAccessReadWrite(FileMapXboxData, sizeof(XboxData));
	if (FileMapXboxData == NULL) 
	{ 
		_tprintf(TEXT("Could not open access to pXboxData (%d).\n"), GetLastError()); 
		CloseHandle(FileMapXboxData);
		return 1;
	}


	// Requesting a view of UGVControlData memory
	FileMapUGVControl = SMOpenReadWrite(szNameUGVControlData);
	if (FileMapUGVControl == NULL) 
	{ 
		_tprintf(TEXT("Could not open UGVControlData view (%d).\n"), GetLastError());
		return 1;
	} 

	// Requesting access to UGVControlData memory
	pUGVControlData = (UGVControl*)SMAccessReadWrite(FileMapUGVControl, sizeof(UGVControl));
	if (FileMapUGVControl == NULL) 
	{ 
		_tprintf(TEXT("Could not open access to UGVControlData (%d).\n"), GetLastError()); 
		CloseHandle(FileMapUGVControl);
		return 1;
	}

	
	XInputEnable(true);
	Xbox.SetDeadzone(7000,7000);
	int Count = 0;
	pProcessManagement->ShutDown.Flags.XBox = 0;
	while(!pProcessManagement->ShutDown.Flags.XBox)
	{
		Xbox.SetPushButtons();
		pXboxData->SetSpeed = Xbox.GetThumb(LEFT_SIDE, true).axisY;
		pXboxData->SetSteering = 40.0*Xbox.GetThumb(RIGHT_SIDE, true).axisX;
		cout << pXboxData->SetSteering << "\t" <<pXboxData->SetSpeed << endl;
		if(_kbhit())
			break;
		pProcessManagement->HeartBeat.Flags.XBox = 1;
		if(Xbox.GetPushButton(BUTTON_B))
			pProcessManagement->ShutDown.Status = 0xFFFF;
		QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
		XboxTimeStamp = (double)(Counter)/(double)Frequency*1000;
		cout << "Time Difference "  << fabs(pProcessManagement->UGVTimeStamp - XboxTimeStamp) << endl;
		//Sleep(500);
	}

	//Cleanup on Exit.

	UnmapViewOfFile(pProcessManagement);

	CloseHandle(FileMapXboxData);
	CloseHandle(FileMapUGVControl);
	CloseHandle(FileMapProcessManagement);
	cout << "Terminating normally!" << endl;
	Sleep(1000);

	return 0;
}