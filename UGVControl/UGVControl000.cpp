#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


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
#include "EthernetClient.h"

#define BUF_SIZE 48
#define ERR_CREATE_FILEMAP 4
#define ERR_MAP_FILEVIEW   5

#define STUDENTNUM "3438901\n"

#define MOTION_PORT "25000"
#define IP_ADDRESS "192.168.1.2"

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

	EthernetClient Motion(IP_ADDRESS, MOTION_PORT);

	double UGVTimeStamp;
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

	// Requesting access to XboxData memory
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

	if (Motion.ConnectToServer() == 0)
		cout << "Connected to Motion Server. " << endl;

	Motion.SendStudentNum(STUDENTNUM);

	char ControlData[50];
	bool flage = 0;
	

	pProcessManagement->ShutDown.Flags.UGVControl = 0;
	while(!pProcessManagement->ShutDown.Flags.UGVControl)
	{
		cout << pXboxData->SetSpeed <<"\t"<< pXboxData->SetSteering<< endl;
		sprintf(ControlData, "# %f %f %d #", -1*pXboxData->SetSteering, pXboxData->SetSpeed, flage);
		
		//sending control string to server
		Motion.SendData(ControlData, strlen(ControlData));
		flage = !flage;
		if(_kbhit())
			break;
		pProcessManagement->HeartBeat.Flags.UGVControl = 1;
		QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
		UGVTimeStamp = (double)(Counter)/(double)Frequency*1000;
		pProcessManagement->UGVTimeStamp = UGVTimeStamp;
		Sleep(200);
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