//***********************************************************************
//
//  BoundingBox
//
//  This class isn't interesting.. all it does is store a pair of points
//  which represent the min and max coordinates of an axis-aligned bounding box
//
//***********************************************************************
#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "VECTOR3D.h"

class BoundingBox
{
	public:
	VECTOR3D min_point;
	VECTOR3D max_point;


	void setBoundingBox(VECTOR3D a, VECTOR3D b)
	{
		min_point = a;
		max_point = b;
	}

	void setBoundingBox(float minx, float miny, float minz, float maxx, float maxy, float maxz)
	{
		min_point.x = minx;
		min_point.y = miny;
		min_point.z = minz;
		max_point.x = maxx;
		max_point.y = maxy;
		max_point.z = maxz;

	}
};

#endif

