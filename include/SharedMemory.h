#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <windows.h>
#include <tchar.h>
extern TCHAR szNameLaserData[];
extern TCHAR szNameXboxData[];
extern TCHAR szNameUGVControlData[];
extern TCHAR szNameProcessManagementData[];

//Define Shared Memory Structures---------------
struct LaserData
{
	double X[10000];
	double Y[10000];
	int NumPoints;
};

struct XboxData
{
	double SetSpeed;
	double SetSteering;
};

struct UGVControl
{
	double SetSpeed;
	double SetSteering;
};

struct UnitFlags
{
	unsigned short	SharedMemory : 1,
		XBox : 1,
		UGVControl : 1,
		Laser : 1,
		ProcessManagement : 1,
		Reserved : 12;
};


union ExecFlags
{
	unsigned short	Status;
	UnitFlags Flags;
};

struct ProcessManagement
{
	ExecFlags HeartBeat;
	ExecFlags ShutDown;
	double UGVTimeStamp;
};
//Define Shared Memory Structures END------------
#endif