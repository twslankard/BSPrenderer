#include "camera.h"

void Camera::set(VECTOR3D newPosition, float newAngleYaw, float newAnglePitch)
{
	m_position.x = newPosition.x;
	m_position.y = newPosition.y;

	m_position.z = newPosition.z;
	m_yaw = newAngleYaw;
	m_pitch = newAnglePitch;

	updateDirectionVector();
}

void Camera::moveForward(float amount)
{
			//zero pitch = move forward
	m_position.x += (float)cos(m_pitch * M_PI / 180) * (float)sin(m_yaw * M_PI / 180) * amount;
	m_position.z -= (float)cos(m_pitch * M_PI / 180) * (float)cos(m_yaw * M_PI / 180) * amount;

	m_position.y -= (float)sin(m_pitch * M_PI / 180) * amount;
}

void Camera::moveBackward(float amount)
{
			//zero pitch = move forward
	m_position.x -= (float)cos(m_pitch * M_PI / 180) * (float)sin(m_yaw * M_PI / 180) * amount;
	m_position.z += (float)cos(m_pitch * M_PI / 180) * (float)cos(m_yaw * M_PI / 180) * amount;

	m_position.y += (float)sin(m_pitch * M_PI / 180) * amount;
}

void Camera::turnLeft(float amount)
{
	m_yaw -= amount;

	updateDirectionVector();
}

void Camera::turnRight(float amount)
{
	m_yaw += amount;

	updateDirectionVector();
}

void Camera::sidestepLeft(float amount)
{
	m_position.x -= (float)cos(m_yaw * M_PI/180) * amount;
	m_position.z -= (float)sin(m_yaw * M_PI/180) * amount;
}

void Camera::sidestepRight(float amount)
{
	m_position.x += (float)cos(m_yaw * M_PI/180) * amount;
	m_position.z += (float)sin(m_yaw * M_PI/180) * amount;
}

void Camera::lookUp(float amount)
{
	m_pitch += amount;
	if(m_pitch < -90)
		m_pitch = -90;
	if(m_pitch > 90)
		m_pitch = 90;

	updateDirectionVector();
}

void Camera::lookDown(float amount)
{
	m_pitch -= amount;
	if(m_pitch < -90)
		m_pitch = -90;
	if(m_pitch > 90)
		m_pitch = 90;

	updateDirectionVector();
}

void Camera::updateDirectionVector(void)
{
	m_direction = VECTOR3D(-cos(m_pitch * M_PI / 180) * sin(m_yaw * M_PI / 180), sin(m_pitch * M_PI / 180), cos(m_pitch * M_PI / 180) * cos(m_yaw * M_PI / 180));
	m_binormal = VECTOR3D(cos(m_yaw * M_PI / 180), -cos(m_pitch * M_PI / 180), sin(m_yaw * M_PI / 180));
}


