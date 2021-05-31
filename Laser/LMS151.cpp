#define _USE_MATH_DEFINES
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <string>
#include <iostream>
#include <sstream>
#include <math.h>
#include "LMS151.h"

#define STUDENTNUM "3438901\n"

using namespace std;

LMS151::LMS151():EthernetClient()
{
	StartAngle = -45;
	EndAngle = 225;
	Resolution = 0.5;
}

LMS151::LMS151(char * portaddress, char* port):EthernetClient(portaddress, port)
{
	StartAngle = -45;
	EndAngle = 225;
	Resolution = 0.5;
}

LMS151::~LMS151()
{
}

void LMS151::Login()
{
	string Fragments;
	string LoginResp;
	int Return;
	char Login[256] = "sMN SetAccessMode 03 F4724744";
	
	//send students Num
	SendStudentNum(STUDENTNUM);
	//check if OK\n is received
	while (RxData == "OK\n")
		recv(ConnectSocket, RxData, (int)strlen(RxData), 0);

	SendData(Login,strlen(Login));
	ReceiveData();
	LoginResp = RxData;
	istringstream is(LoginResp);
	is >> Fragments >> Fragments >> hex >> Return;
	if(Return == 0)
		Error = LOGIN_FAILED;
	else
		Error = NOERROR;
	//std::cout << Error << std::endl;
}

void LMS151::Logout()
{
	string Fragments;
	string LogoutResp;
	int Return;
	char Logout[256] = "sMN Run";
	SendData(Logout,strlen(Logout));
	ReceiveData();
	LogoutResp = RxData;
	istringstream is(LogoutResp);
	is >> Fragments >> Fragments >> hex >> Return;
	if(Return == 0)
		Error = LOGOUT_FAILED;
	else
		Error = NOERROR;
	//std::cout << Error << std::endl;
}

void LMS151::Configure(int startAngle, int endAngle, double resolution)
{
	string Fragments;
	string ConfigResp;
	int Return;
	char SetFreqRange[256];// = "sMN mLMPsetscancfg +5000 +1 +5000 +450000 +135000";
	StartAngle = startAngle;
	EndAngle = endAngle;
	Resolution = resolution;
	sprintf_s(SetFreqRange,"sMN mLMPsetscancfg +5000 +1 %+4d %+d %+d",resolution, startAngle*10000, endAngle*10000);
	SendData(SetFreqRange, strlen(SetFreqRange));
	ReceiveData();
	ConfigResp = RxData;
	istringstream is(ConfigResp);
	is >> Fragments >> Fragments >> hex >> Return;
	if(Return == 0)
		Error = SCAN_CONFIG_ERROR;
	else
		Error = NOERROR;
}


void LMS151::GetRangeCoords()
{
//	int NumPoints;
	double Angle = StartAngle;
	int Range;
	string Fragments;
	string ScanResp;
	char ScanOnce[256] = "sRN LMDscandata";
	SendData(ScanOnce, strlen(ScanOnce));
	ReceiveData();
	ScanResp = RxData;
	istringstream is(ScanResp);
	//cout << ScanResp << endl;
	for(int i = 0; i < 25; i++)
		is >> Fragments;
	is >> hex >> NumPoints;
	//cout << NumPoints << endl;
	for(int i = 0; i < NumPoints; i++)
	{
		is >> hex >> Range;
		RangeData[i][0] = (double)Range/1000.0*cos(Angle*M_PI/180.0);
		RangeData[i][1] = (double)Range/1000.0*sin(Angle*M_PI/180.0);
		Angle += Resolution/10000.0;
//		cout << RangeData[i][0] << " " << RangeData[i][1] << endl;
	}
}
