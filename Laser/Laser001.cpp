#define _USE_MATH_DEFINES
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <math.h>
#include "sharedmemory.h"
#include "sharedmemoryfcn.h"

#include "LMS151.h"

using namespace std;


#define DEFAULT_PORT "23000"
#define IP_ADDRESS "192.168.1.2"//"10.1.1.21"// //"192.168.1.111"//"192.168.5.150"

int main() 
{
	HANDLE FileMapLaserData;
	HANDLE FileMapXboxData;
	HANDLE FileMapUGVControl;
	HANDLE FileMapProcessManagement;

	ProcessManagement *pProcessManagement;
	LaserData *pLaserData;
	
	LMS151 Laser(IP_ADDRESS, DEFAULT_PORT);
	
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
	
	// Requesting a view of LaserData memory
	FileMapLaserData = SMOpenReadWrite(szNameLaserData);
	if (FileMapLaserData == NULL)
	{
		_tprintf(TEXT("Could not open LaserData view (%d).\n"), GetLastError());
		return 1;
	}

	// Requesting access to pLaserData memory
	pLaserData = (LaserData*)SMAccessReadWrite(FileMapLaserData, sizeof(LaserData));
	if (FileMapLaserData == NULL)
	{
		_tprintf(TEXT("Could not open access to pLaserData (%d).\n"), GetLastError());
		CloseHandle(FileMapLaserData);
		return 1;
	}

	if (Laser.ConnectToServer() == 0)
		cout << "Connected to Laser. " << endl;
	
	Laser.Login();
	
	Laser.Configure(45, 135, HALF_DEGREE);

	pProcessManagement->ShutDown.Flags.Laser = 0;
	while (!pProcessManagement->ShutDown.Flags.Laser) {
		Laser.GetRangeCoords();
		pLaserData->NumPoints = Laser.NumPoints;
		for (int i = 0; i <= Laser.NumPoints; i++) {
			pLaserData->X[i] = Laser.RangeData[i][0];
			pLaserData->Y[i] = Laser.RangeData[i][1];
		}
		if (_kbhit())
			break;
		pProcessManagement->HeartBeat.Flags.Laser = 1;
	}
	
	
	Laser.Logout();
	
	
	//_getch();

	cout << "out of the loop" << endl;
	UnmapViewOfFile(pProcessManagement);

	CloseHandle(FileMapLaserData);
	//CloseHandle(FileMapXboxData);
	//CloseHandle(FileMapUGVControl);
	CloseHandle(FileMapProcessManagement);
	cout << "Terminating normally!" << endl;
	Sleep(1000);
	return 0;
}