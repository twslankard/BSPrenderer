//////////////////////////////////////////////////////////////////////////////////////////
//	PLANE.cpp
//	function definitions for RGBA color class
//	You may use this code however you wish, but if you do, please credit me and
//	provide a link to my website in a readme file or similar
//	Downloaded from: www.paulsprojects.net
//////////////////////////////////////////////////////////////////////////////////////////

#include "PLANE.h"

void PLANE::SetFromPoints(const VECTOR3D & p0, const VECTOR3D & p1, const VECTOR3D & p2)
{
	normal=(p1-p0).CrossProduct(p2-p0);

	normal.Normalize();

	CalculateIntercept(p0);
}

void PLANE::Normalize()
{
	float normalLength=normal.GetLength();
	normal/=normalLength;
	intercept/=normalLength;
}

bool PLANE::Intersect3(const PLANE & p2, const PLANE & p3, VECTOR3D & result)//find point of intersection of 3 planes
{
	float denominator=normal.DotProduct((p2.normal).CrossProduct(p3.normal));
											//scalar triple product of normals
	if(denominator==0.0f)									//if zero
		return false;										//no intersection

	VECTOR3D temp1, temp2, temp3;
	temp1=(p2.normal.CrossProduct(p3.normal))*intercept;
	temp2=(p3.normal.CrossProduct(normal))*p2.intercept;
	temp3=(normal.CrossProduct(p2.normal))*p3.intercept;

	result=(temp1+temp2+temp3)/(-denominator);

	return true;
}

float PLANE::GetDistance(const VECTOR3D & point) const
{
	return point.x*normal.x + point.y*normal.y + point.z*normal.z + intercept;
}

int PLANE::ClassifyPoint(const VECTOR3D & point) const
{
	float distance=point.x*normal.x + point.y*normal.y + point.z*normal.z + intercept;

	if(distance > 0.01)	//==0.0f is too exact, give a bit of room
		return POINT_IN_FRONT_OF_PLANE;
	
	if(distance < -0.01)
		return POINT_BEHIND_PLANE;

	return POINT_ON_PLANE;	//otherwise
}

PLANE PLANE::lerp(const PLANE & p2, float factor)
{
	PLANE result;
	result.normal=normal*(1.0f-factor) + p2.normal*factor;
	result.normal.Normalize();

	result.intercept=intercept*(1.0f-factor) + p2.intercept*factor;

	return result;
}

bool PLANE::operator ==(const PLANE & rhs) const
{
	if(normal==rhs.normal && intercept==rhs.intercept)
		return true;

	return false;
}


