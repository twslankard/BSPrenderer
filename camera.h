#ifndef CAMERA_H
#define CAMERA_H

#include "VECTOR3D.h"
#include <iostream>
using namespace std;

class Camera
{
public:

	float m_yaw, m_pitch;
	VECTOR3D m_position;
	VECTOR3D m_direction;
	VECTOR3D m_binormal;
	void set(VECTOR3D newPosition, float newAngleYaw, float newAnglePitch);
	void moveForward(float amount);
	void moveBackward(float amount);
	void turnLeft(float amount);
	void turnRight(float amount);
	void sidestepLeft(float amount);
	void sidestepRight(float amount);
	void lookUp(float amount);
	void lookDown(float amount);
	void updateDirectionVector(void);
};

#endif	//WALKING_CAMERA_H



