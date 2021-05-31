#include "MyVehicle.hpp"

#ifdef __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <GLUT/glut.h>
	#include <unistd.h>
#elif defined(WIN32)
	#include <Windows.h>
	#include <tchar.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>

	#pragma warning(disable : 4996)
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
	#include <unistd.h>
#endif




void drawUGV(double steerAngle)
{
	const float red = .8, green = .1, blue = .2;
	const float width = .4;
	const float wheel_height = .4;
	const float wheel_width = .1;
	const float vertices[10][3] = {
		// left-hand side
		{ .5,   wheel_height -0.1,    width},
		{-.5,   wheel_height -0.1,    width},
		{-.5,   wheel_height +0.3,    width},
		{ .4,   wheel_height +0.3,    width},
		{ .5,   wheel_height +0.15,   width},
		// right-hand side
		{ .5,   wheel_height -0.1,   -width},
		{-.5,   wheel_height -0.1,   -width},
		{-.5,   wheel_height +0.3,   -width},
		{ .4,   wheel_height +0.3,   -width},
		{ .5,   wheel_height +0.15,  -width}
	};

	// quad faces
	glBegin(GL_QUADS);
		// bottom		
		glColor3f(red*.3, green, blue);
		glVertex3fv(vertices[0]);
		glVertex3fv(vertices[5]);
		glVertex3fv(vertices[6]);
		glVertex3fv(vertices[1]);

		// back
		glColor3f(red* .5, green, blue);
		glVertex3fv(vertices[1]);
		glVertex3fv(vertices[6]);
		glVertex3fv(vertices[7]);
		glVertex3fv(vertices[2]);

		// top
		glColor3f(red * .7, green, blue);
		glVertex3fv(vertices[2]);
		glVertex3fv(vertices[7]);
		glVertex3fv(vertices[8]);
		glVertex3fv(vertices[3]);

		// angled 
		glColor3f(red * .6, green, blue);
		glVertex3fv(vertices[3]);
		glVertex3fv(vertices[8]);
		glVertex3fv(vertices[9]);
		glVertex3fv(vertices[4]);

		// front
		glColor3f(red * .5, green, blue);
		glVertex3fv(vertices[4]);
		glVertex3fv(vertices[9]);
		glVertex3fv(vertices[5]);
		glVertex3fv(vertices[0]);
	glEnd();

	// left
	glBegin(GL_POLYGON);
		glColor3f(red, green, blue);
		glVertex3fv(vertices[0]);
		glVertex3fv(vertices[1]);
		glVertex3fv(vertices[2]);
		glVertex3fv(vertices[3]);
		glVertex3fv(vertices[4]);
	glEnd();

	// right
	glBegin(GL_POLYGON);
		glVertex3fv(vertices[5]);
		glVertex3fv(vertices[6]);
		glVertex3fv(vertices[7]);
		glVertex3fv(vertices[8]);
		glVertex3fv(vertices[9]);
	glEnd();
	
	
	// draw wheels
	glPushMatrix();
		glColor3f(1,1,1);
		static GLUquadric *quad = gluNewQuadric();
		glTranslatef(0, wheel_height*.5, -wheel_width*.5);

		// front
		glPushMatrix();
			glTranslatef(vertices[0][0], 0, - (width+wheel_width) );
			glPushMatrix();
				glRotatef(-steerAngle, 0, 1, 0);
				gluCylinder(quad, wheel_height*.5, wheel_height*.5, wheel_width, 12, 1);		
			glPopMatrix();
	
			glTranslatef(0, 0, (width+wheel_width) * 2);
			glPushMatrix();
				glRotatef(-steerAngle, 0, 1, 0);
				gluCylinder(quad, wheel_height*.5, wheel_height*.5, wheel_width, 12, 1);
			glPopMatrix();
		glPopMatrix();

		// back
		glPushMatrix();
			glTranslatef(vertices[1][0], 0, -(width+wheel_width));
			gluCylinder(quad, wheel_height*.5, wheel_height*.5, wheel_width, 12, 1);	

			glTranslatef(0, 0, (width+wheel_width) * 2);
			gluCylinder(quad, wheel_height*.5, wheel_height*.5, wheel_width, 12, 1);
		glPopMatrix();
	glPopMatrix();
}

MyVehicle::MyVehicle()
{
	/////////////////////////
	// Shared Memory Setup //
	/////////////////////////

	
	// Requesting a view of ProcessManagementData memory
	FileMapProcessManagement = SMOpenReadWrite(szNameProcessManagementData);
	if (FileMapProcessManagement == NULL)
	{
		_tprintf(TEXT("Could not open ProcessManagementData file view (%d).\n"), GetLastError());
		//return 1;
	}

	// Requesting access to ProcessManagementData memory
	pProcessManagement = (ProcessManagement*)SMAccessReadWrite(FileMapProcessManagement, sizeof(ProcessManagement));
	if (pProcessManagement == NULL)
	{
		_tprintf(TEXT("Could not open access to ProcessManagementData memory (%d).\n"), GetLastError());
		CloseHandle(FileMapProcessManagement);
		//return 1;
	}

	// Requesting a view of LaserData memory
	FileMapLaserData = SMOpenReadWrite(szNameLaserData);
	if (FileMapLaserData == NULL)
	{
		_tprintf(TEXT("Could not open LaserData view (%d).\n"), GetLastError());
		//return 1;
	}

	// Requesting access to pLaserData memory
	pLaserData = (LaserData*)SMAccessReadWrite(FileMapLaserData, sizeof(LaserData));
	if (FileMapLaserData == NULL)
	{
		_tprintf(TEXT("Could not open access to pLaserData (%d).\n"), GetLastError());
		CloseHandle(FileMapLaserData);
		//return 1;
	}

	//// Requesting a view of XboxData memory
	//FileMapXboxData = SMOpenReadWrite(szNameXboxData);
	//if (FileMapXboxData == NULL)
	//{
	//	_tprintf(TEXT("Could not open XboxData view (%d).\n"), GetLastError());
	//	//return 1;
	//}

	//// Requesting access to pXboxData memory
	//pXboxData = (XboxData*)SMAccessReadWrite(FileMapXboxData, sizeof(XboxData));
	//if (FileMapXboxData == NULL)
	//{
	//	_tprintf(TEXT("Could not open access to pXboxData (%d).\n"), GetLastError());
	//	CloseHandle(FileMapXboxData);
	//	//return 1;
	//}
}

MyVehicle::~MyVehicle() {
	//Cleanup on Exit.
	
	UnmapViewOfFile(pProcessManagement);

	CloseHandle(FileMapLaserData);
	CloseHandle(FileMapProcessManagement);
	//CloseHandle(FileMapXboxData);
	std::cout << "Terminating normally!" << std::endl;

	Sleep(1000);
}

void MyVehicle::draw()
{
	glPushMatrix();
	positionInGL();
	
	//drawUGV(steering, pLaserData);
	drawUGV(steering);
	//speed = pXboxData->SetSpeed;
	//steering = pXboxData->SetSteering;
	//-----------------------------------RADAR-----------------------------------//
	glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	for (int i = 0; i <= pLaserData->NumPoints; i++) {
		glVertex3f(0.5, 0.4, 0);  //0.4 is wheel height
		glVertex3f(pLaserData->Y[i], 0.4, pLaserData->X[i]);
	}
	glEnd();
	glPopMatrix();
}