#include <Windows.h>
#include <XInput.h>
#include "xbox.hpp"


void XBox::UpdateStatus()
{
	if(XInputGetState(ControllerIndex,&State) == ERROR_DEVICE_NOT_CONNECTED)
	{
		Connected = 0;
		ErrorCode = CONTROLLER_NOTCONNECTED;
	}
	else
	{
		Connected = 1;
		ErrorCode = 0;
	}
}

int XBox::DZone(int Value, double range)
{
	double Data;

	if(abs(Value) <= DeadZone)
		return 0;
	else
	{
		Data = (abs(Value) - DeadZone)*range/(32768.0 - DeadZone);
		if(Value < -DeadZone)
			return -(int)Data;
		else
			return (int)Data;
	}
}

XBox::XBox()
{
	XInputEnable(true);
	if(XInputGetState(ControllerIndex,&State) == ERROR_DEVICE_NOT_CONNECTED)
	{
		Connected = 0;
		ErrorCode = CONTROLLER_NOTCONNECTED;
	}
	else
	{
		Connected = 1;
		Vibrate(0,0);
		ErrorCode = 0;
	}
	DeadZone = 0;
}

XBox::XBox(int index)
{
	ControllerIndex = index;
	XBox();
}

void XBox::SetDeadZone(int deadZone)
{
	DeadZone = deadZone;
}

void XBox::SetControllerIndex(int index)
{
	ControllerIndex = index;
}

int XBox::GetControllerIndex()
{
	return ControllerIndex;
}	

void XBox::Vibrate(int lFrequency, int rFrequency)
{
	XINPUT_VIBRATION Vibration;
	Vibration.wLeftMotorSpeed = lFrequency;
	Vibration.wRightMotorSpeed = rFrequency;
	XInputSetState(ControllerIndex,&Vibration);
}

bool XBox::IsConnected()
{
	if(XInputGetState(ControllerIndex,&State) == ERROR_DEVICE_NOT_CONNECTED)
	{
		Connected = 0;
		ErrorCode = CONTROLLER_NOTCONNECTED;
		return false;
	}
	else
		return true;
}

int XBox::BatteryLevel()
{
	XINPUT_BATTERY_INFORMATION BatteryInformation;
	XInputGetBatteryInformation(ControllerIndex,BATTERY_DEVTYPE_GAMEPAD,&BatteryInformation);

	return BatteryInformation.BatteryLevel;
}

int XBox::BatteryType()
{
	XINPUT_BATTERY_INFORMATION BatteryInformation;
	XInputGetBatteryInformation(ControllerIndex,BATTERY_DEVTYPE_GAMEPAD,&BatteryInformation);

	return BatteryInformation.BatteryType;
}

int XBox::DPadUP()
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return ((State.Gamepad.wButtons & 0x0001) > 0);
}

int XBox::DPadDOWN()
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return ((State.Gamepad.wButtons & 0x0002) > 0);
}

int XBox::DPadLEFT()
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return ((State.Gamepad.wButtons & 0x0004) > 0);
}

int XBox::DPadRIGHT()
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return ((State.Gamepad.wButtons & 0x0008) > 0);
}

int XBox::Start()
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return ((State.Gamepad.wButtons & 0x0010) > 0);
}

int XBox::Back()
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return ((State.Gamepad.wButtons & 0x0020) > 0);
}

int XBox::LeftThumb()
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return ((State.Gamepad.wButtons & 0x0040) > 0);
}

int XBox::RightThumb()
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return ((State.Gamepad.wButtons & 0x0080) > 0);
}

int XBox::LeftShoulder()
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return ((State.Gamepad.wButtons & 0x0100) > 0);
}

int XBox::RightShoulder()
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return ((State.Gamepad.wButtons & 0x0200) > 0);
}

int XBox::A()
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return ((State.Gamepad.wButtons & 0x1000) > 0);
}

int XBox::B()
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return ((State.Gamepad.wButtons & 0x2000) > 0);
}

int XBox::X()
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return ((State.Gamepad.wButtons & 0x4000) > 0);
}

int XBox::Y()
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return ((State.Gamepad.wButtons & 0x8000) > 0);
}

int XBox::LeftX(double range)
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return DZone(State.Gamepad.sThumbLX, range);
}

int XBox::LeftY(double range)
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else		
		return DZone(State.Gamepad.sThumbLY, range);
}

int XBox::RightX(double range)
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return DZone(State.Gamepad.sThumbRX, range);
}

int XBox::RightY(double range)
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return DZone(State.Gamepad.sThumbRY, range);
}

int XBox::LeftTrigger()
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return State.Gamepad.bLeftTrigger;
}

int XBox::RightTrigger()
{
	UpdateStatus();
	if (!Connected)
		return -1;
	else
		return State.Gamepad.bRightTrigger;
}

