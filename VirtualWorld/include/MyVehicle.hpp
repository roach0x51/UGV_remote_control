#ifndef __MYVEHICLE_HPP__
#define __MYVEHICLE_HPP__


#include "Vehicle.hpp"

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

class MyVehicle : public Vehicle
{
private:
	HANDLE FileMapLaserData;
	HANDLE FileMapProcessManagement;
	HANDLE FileMapXboxData;

	ProcessManagement *pProcessManagement;
	LaserData *pLaserData;
	XboxData *pXboxData;
public:
	double *x;
	double *y;
	//MyVehicle(double* xpointer, double* ypointer):x(xpointer), y(ypointer) {};
	MyVehicle();
	~MyVehicle();
	virtual void draw();
};

#endif