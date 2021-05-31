#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <windows.h>
#include <iostream>
#include <Xinput.h>
#include <math.h>

#define BUTTON_SIZE		14
#define THUMB_DEADZONE	8269
#define MAX_MOTOR_SPEED 65500
#define LEFT_SIDE		0
#define RIGHT_SIDE		1

#define UP			0
#define DOWN		1
#define LEFT		2
#define RIGHT		3
#define START		4
#define BACK		5
#define	L_THUMB		6
#define	R_THUMB		7
#define	L_SHOULDER	8
#define R_SHOULDER	9
#define	BUTTON_A	10
#define BUTTON_B	11 
#define BUTTON_X	12
#define BUTTON_Y	13

#define MAX_THUMSTICK 32767

typedef struct _COORDINATE {
	double axisX;
	double axisY;
} COORDINATE;


class Controller{
private:
	DWORD index;
	XINPUT_BATTERY_INFORMATION BatteryInformation;
	XINPUT_STATE State;
	XINPUT_VIBRATION Vibration;
	bool pushButtons[BUTTON_SIZE];
	static const int buttonMask[14];
	int deadzoneX = 0, deadzoneY = 0;

public:
	//	Constructor functions
	Controller();
	Controller(DWORD controller_num);

	//	Functions for Battery Information
	void PrintBatteryInform();
	int GetBatteryInform();

	//	Set gamepad's status
	void SetPushButtons();
	
	//	Get Packet Number, use for check any in input
	DWORD GetPacketNumber();

	//	Functions for getting status of push buttons
	void PrintPushButtons();
	bool GetPushButton(int buttonName);
	
	//	Functions for getting position of thumbs
	COORDINATE GetThumb(int side, bool deadZoneEnable);
	bool DeadZoneCheck(SHORT axis, int deadzone);
	
	//	Function for get value of a Trigger
	int GetTrigger(int side);

	//	Set vibrations
	void SetVibration(WORD leftMotor, WORD rightMotor);

	//
	int GetIndex();

	//
	void SetDeadzone(int axisX, int axisY);
	
	
};


#endif 