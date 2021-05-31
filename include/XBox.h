#ifndef XBOX_H
#define XBOX_H

#include <xinput.h>

#define CONTROLLER_NOTCONNECTED -1

class XBox
{
private:
	XINPUT_STATE State;
	int ControllerIndex;
	int Connected;
	int ErrorCode;
	int DeadZone;

private:
	void UpdateStatus();
	int DZone(int Value, double range);

public:
	XBox();
	XBox(int index);
	void SetDeadZone(int deadZone);
	void SetControllerIndex(int index);
	int GetControllerIndex();
	void Vibrate(int lFrequency, int rFrequency);
	bool IsConnected();
	int BatteryLevel();
	int BatteryType();
	int DPadUP();
	int DPadDOWN();
	int DPadLEFT();
	int DPadRIGHT();
	int Start();
	int Back();
	int LeftThumb();
	int RightThumb();
	int LeftShoulder();
	int RightShoulder();
	int A();
	int B();
	int X();
	int Y();
	int LeftX(double range);
	int LeftY(double range);
	int RightX(double range);
	int RightY(double range);
	int LeftTrigger();
	int RightTrigger();
};
#endif