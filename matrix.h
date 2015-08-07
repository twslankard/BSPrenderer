//////////////////////////////////////////////////////////////////////////////////////////
//	Matrix.h
//	Class declaration for a 4x4 matrix
//	You may use this code however you wish, but if you do, please credit me and
//	provide a link to my website in a readme file or similar
//	Downloaded from: www.paulsprojects.net
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include "VECTOR4D.h"

class Matrix
{
public:
	Matrix()
	{	LoadIdentity();	}
	Matrix(	float e0, float e1, float e2, float e3,
				float e4, float e5, float e6, float e7,
				float e8, float e9, float e10, float e11,
				float e12, float e13, float e14, float e15);
	Matrix(const float * rhs);
	Matrix(const Matrix & rhs);
	~Matrix() {}	//empty

	void SetEntry(int position, float value);
	float GetEntry(int position) const;
	VECTOR4D GetRow(int position) const;
	VECTOR4D GetColumn(int position) const;
	
	void LoadIdentity(void);
	void LoadZero(void);
	
	//binary operators
	Matrix operator+(const Matrix & rhs) const;
	Matrix operator-(const Matrix & rhs) const;
	Matrix operator*(const Matrix & rhs) const;
	Matrix operator*(const float rhs) const;
	Matrix operator/(const float rhs) const;
	friend Matrix operator*(float scaleFactor, const Matrix & rhs);

	bool operator==(const Matrix & rhs) const;
	bool operator!=(const Matrix & rhs) const;

	//self-add etc
	void operator+=(const Matrix & rhs);
	void operator-=(const Matrix & rhs);
	void operator*=(const Matrix & rhs);
	void operator*=(const float rhs);
	void operator/=(const float rhs);

	//unary operators
	Matrix operator-(void) const;
	Matrix operator+(void) const {return (*this);}
	
	//multiply a vector by this matrix
	VECTOR4D operator*(const VECTOR4D rhs) const;

	//rotate a 3d vector by rotation part
	void RotateVector3D(VECTOR3D & rhs) const
	{rhs=GetRotatedVector3D(rhs);}

	void InverseRotateVector3D(VECTOR3D & rhs) const
	{rhs=GetInverseRotatedVector3D(rhs);}

	VECTOR3D GetRotatedVector3D(const VECTOR3D & rhs) const;
	VECTOR3D GetInverseRotatedVector3D(const VECTOR3D & rhs) const;

	//translate a 3d vector by translation part
	void TranslateVector3D(VECTOR3D & rhs) const
	{rhs=GetTranslatedVector3D(rhs);}

	void InverseTranslateVector3D(VECTOR3D & rhs) const
	{rhs=GetInverseTranslatedVector3D(rhs);}
	
	VECTOR3D GetTranslatedVector3D(const VECTOR3D & rhs) const;
	VECTOR3D GetInverseTranslatedVector3D(const VECTOR3D & rhs) const;

	//Other methods
	void Invert(void);
	Matrix GetInverse(void) const;
	void Transpose(void);
	Matrix GetTranspose(void) const;
	void InvertTranspose(void);
	Matrix GetInverseTranspose(void) const;

	//Inverse of a rotation/translation only matrix
	void AffineInvert(void);
	Matrix GetAffineInverse(void) const;
	void AffineInvertTranspose(void);
	Matrix GetAffineInverseTranspose(void) const;

	//set to perform an operation on space - removes other entries
	void SetTranslation(const VECTOR3D & translation);
	void SetScale(const VECTOR3D & scaleFactor);
	void SetUniformScale(const float scaleFactor);
	void SetRotationAxis(const double angle, const VECTOR3D & axis);
	void SetRotationX(const double angle);
	void SetRotationY(const double angle);
	void SetRotationZ(const double angle);
	void SetRotationEuler(const double angleX, const double angleY, const double angleZ);
	void SetPerspective(float left, float right, float bottom, float top, float n, float f);
	void SetPerspective(float fovy, float aspect, float n, float f);
	void SetOrtho(float left, float right, float bottom, float top, float n, float f);

	//set parts of the matrix
	void SetTranslationPart(const VECTOR3D & translation);
	void SetRotationPartEuler(const double angleX, const double angleY, const double angleZ);
	void SetRotationPartEuler(const VECTOR3D & rotations)
	{
		SetRotationPartEuler((double)rotations.x, (double)rotations.y, (double)rotations.z);
	}

	//cast to pointer to a (float *) for glGetFloatv etc
	operator float* () const {return (float*) this;}
	operator const float* () const {return (const float*) this;}
	
	//member variables
	float entries[16];
};

#endif	//MATRIX4X4_H


