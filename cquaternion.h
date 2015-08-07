//********************************************************************
//
// This class came from the gametutorials md3 animation tutorial.
// It's used for the spherical interpolation of joint rotations.
//
//
//
//********************************************************************
#ifndef CQUATERION_H
#define CQUATERION_H

#include <cmath>

// This is our quaternion class
class CQuaternion
{

public:

	// This is our default constructor, which initializes everything to an identity
	// quaternion.  An identity quaternion has x, y, z as 0 and w as 1.
	CQuaternion() { x = y = z = 0.0f;   w = 1.0f;  }

	// Creates a constructor that will allow us to initialize the quaternion when creating it
	CQuaternion(float X, float Y, float Z, float W)
	{
		x = X;		y = Y;		z = Z;		w = W;
	}

	// This takes in an array of 16 floats to fill in a 4x4 homogeneous matrix from a quaternion
	void CreateMatrix(float *pMatrix);

	// This takes a 3x3 or 4x4 matrix and converts it to a quaternion, depending on rowColumnCount
	void CreateFromMatrix(float *pMatrix, int rowColumnCount);

	// This returns a spherical linear interpolated quaternion between q1 and q2, according to t
	CQuaternion Slerp(CQuaternion &q1, CQuaternion &q2, float t);

private:

	// This stores the 4D values for the quaternion
	float x, y, z, w;
};

#endif

