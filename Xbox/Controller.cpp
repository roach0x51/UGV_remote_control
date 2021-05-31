#include "controller.h"

using namespace std;

 const int Controller::buttonMask[14] = {
	XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_DPAD_DOWN,
	XINPUT_GAMEPAD_DPAD_LEFT,
	XINPUT_GAMEPAD_DPAD_RIGHT,
	XINPUT_GAMEPAD_START,
	XINPUT_GAMEPAD_BACK,
	XINPUT_GAMEPAD_LEFT_THUMB,
	XINPUT_GAMEPAD_RIGHT_THUMB,
	XINPUT_GAMEPAD_LEFT_SHOULDER,
	XINPUT_GAMEPAD_RIGHT_SHOULDER,
	XINPUT_GAMEPAD_A,
	XINPUT_GAMEPAD_B,
	XINPUT_GAMEPAD_X,
	XINPUT_GAMEPAD_Y
};


 //*************************
 //	Constructor functions
 //*************************
 Controller::Controller(){}
 Controller::Controller(DWORD controller_num){
	 if (controller_num >= 0 && controller_num <= 3){
		 index = controller_num;
	 }
	 else {
		 cout << "CONSTRUCTION_ERROR: Invalid Index." << endl;
		 Sleep(10000);
	 }
 }


 //**************************************
 //	Functions for Battery Information
 //**************************************
 void Controller::PrintBatteryInform(){
	 XInputGetBatteryInformation(index, BATTERY_DEVTYPE_GAMEPAD, &BatteryInformation);
	 switch (BatteryInformation.BatteryLevel)
	 {
	 case BATTERY_LEVEL_EMPTY: cout << "Battery level empty " << endl;
		 break;
	 case BATTERY_LEVEL_LOW: cout << "Battery level low " << endl;
		 break;
	 case BATTERY_LEVEL_MEDIUM: cout << "Battery level medium " << endl;
		 break;
	 case BATTERY_LEVEL_FULL: cout << "Battery level full" << endl;
	 }
	 Sleep(3000);
 }
 //**************************************
 int Controller::GetBatteryInform(){
	 XInputGetBatteryInformation(index, BATTERY_DEVTYPE_GAMEPAD, &BatteryInformation);
	 return BatteryInformation.BatteryLevel;
 }


 //******************************
 //	Set gamepad's status
 //******************************
 void Controller::SetPushButtons(){
	 XInputEnable(true);
	 if (XInputGetState(index, &State) == ERROR_SUCCESS){
		 for (int i = 0; i < BUTTON_SIZE; i++){
			 pushButtons[i] = State.Gamepad.wButtons & buttonMask[i];
		 }
	 }
	 else  {
		 cout << "SetPushButtons_ERROR: Controller_" << index + 1 << " is not connected." << endl;
	 }
 }


 //****************************************************
 //	Get Packet Number, use for check any in input
 //****************************************************
 DWORD Controller::GetPacketNumber(){
	 return State.dwPacketNumber;
 }


 //*************************************************
 //	Functions for getting status of push buttons
 //*************************************************
 void Controller::PrintPushButtons(){
	 for (int i = 0; i < BUTTON_SIZE; i++){
		 cout << pushButtons[i] << " ";
	 }
	 cout << endl;
 }

 //*************************************************
 bool Controller::GetPushButton(int buttonName){
	 return pushButtons[buttonName];
 }
 
 
 //********************************************
 //	Functions for getting position of thumbs
 //********************************************
 COORDINATE Controller::GetThumb(int side, bool deadZoneEnable){
	 COORDINATE coordinate;
	 if (side == LEFT_SIDE){
		 if (deadZoneEnable && DeadZoneCheck(State.Gamepad.sThumbLX, deadzoneX)){
			 coordinate.axisX = 0;
		 }
		 else {
			 coordinate.axisX = (State.Gamepad.sThumbLX > 0) ? 
				 (State.Gamepad.sThumbLX - deadzoneX) : (State.Gamepad.sThumbLX + deadzoneX);
		 }
		 if (deadZoneEnable && DeadZoneCheck(State.Gamepad.sThumbLY, deadzoneY)){
			 coordinate.axisY = 0;
		 }
		 else {
			 coordinate.axisY = (State.Gamepad.sThumbLY > 0) ?
				 (State.Gamepad.sThumbLY - deadzoneY) : (State.Gamepad.sThumbLY + deadzoneY);
		 }
	 }
	 else if (side == RIGHT_SIDE){
		 if (deadZoneEnable && DeadZoneCheck(State.Gamepad.sThumbRX, deadzoneX)){
			 coordinate.axisX = 0;
		 }
		 else {
			 coordinate.axisX = (State.Gamepad.sThumbRX > 0) ?
				 (State.Gamepad.sThumbRX - deadzoneX) : (State.Gamepad.sThumbRX + deadzoneX);
		 }
		 if (deadZoneEnable && DeadZoneCheck(State.Gamepad.sThumbRY, deadzoneY)){
			 coordinate.axisY = 0;
		 }
		 else {
			 coordinate.axisY = (State.Gamepad.sThumbRY > 0) ?
				 (State.Gamepad.sThumbRY - deadzoneY) : (State.Gamepad.sThumbRY + deadzoneY);
		 }
	 }
	 else {
		 cout << "GetThumb_ERROR: Invalid Parameter." << endl;
	 }

	 //convert the value to percentage
	 coordinate.axisX /= (double)(MAX_THUMSTICK - deadzoneX);
	 coordinate.axisY /= (double)(MAX_THUMSTICK - deadzoneX);

	 return coordinate;
 }

 //****************************************************************
 bool Controller::DeadZoneCheck(SHORT axis, int deadzone){
	 if (axis <= deadzone && axis >= -deadzone){
		 return true;
	 }
	 return false;
 }


 //*************************************
 //	Function for get value of a Trigger
 //*************************************
int Controller::GetTrigger(int side){
	int temp;
	if (side == LEFT_SIDE){
		temp = State.Gamepad.bLeftTrigger;
	}
	else if (side == RIGHT_SIDE){
		temp = State.Gamepad.bRightTrigger;
	}
	else {
		cout << "GetTrigger_ERROR: Invalid parameter." << endl;
	}
	return temp;
}


//******************
//	Set vibrations
//******************
void Controller::SetVibration(WORD leftMotor, WORD rightMotor){
	if ((leftMotor >= 0 && leftMotor <= MAX_MOTOR_SPEED) && 
		(rightMotor >= 0 && rightMotor <= MAX_MOTOR_SPEED)){
		Vibration.wLeftMotorSpeed = leftMotor;
		Vibration.wRightMotorSpeed = rightMotor;
	}
	else {
		cout << "SetVibration_ERROR: Invalid parameter." << endl;
	}
	
	if (XInputSetState(index, &Vibration) != ERROR_SUCCESS){
		cout << "SetVibration_ERROR: Controller_" << index+1 <<" is not connected." << endl;
	}
	/*
	Vibration.wLeftMotorSpeed = leftMotor;
	Vibration.wRightMotorSpeed = rightMotor;
	XInputSetState(index, &Vibration);
	*/
}


//*********************************************
//	Function to get Gamepad index
//*********************************************
int Controller::GetIndex(){
	return index;
}

//*********************************************
void Controller::SetDeadzone(int axisX, int axisY){
	deadzoneX = abs(axisX);
	deadzoneY = abs(axisY);
}



