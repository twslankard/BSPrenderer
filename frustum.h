//////////////////////////////////////////////////////////////////////////////////////////
//	FRUSTUM.h
//	class declaration for frustum for frustum culling
//	You may use this code however you wish, but if you do, please credit me and
//	provide a link to my website in a readme file or similar
//	Downloaded from: www.paulsprojects.net
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "PLANE.h"
#include "matrix.h"

//planes of frustum
enum FRUSTUM_PLANES
{
	LEFT_PLANE=0,
	RIGHT_PLANE,
	TOP_PLANE,
	BOTTOM_PLANE,
	NEAR_PLANE,
	FAR_PLANE
};

enum FRUSTUM_CLASSIFICATION
{
	OUTSIDE_FRUSTUM=0,
	IN_FRUSTUM
};

class Frustum
{
public:
	void Update();
	bool IsPointInside(const VECTOR3D & point);
	bool IsBoundingBoxInside(const VECTOR3D * vertices);
	
protected:
	PLANE planes[6];
};

#endif	//FRUSTUM_H

