//***********************************************************************
//  transformnode
//
//  This node is used to transform its children by translating, rotating, or scaling.
//***********************************************************************

#ifndef TRANSFORMNODE_H
#define TRANSFORMNODE_H

#include "node.h"

class TransformNode : public Node
{
	private:
	float m_tx, m_ty, m_tz;
	float m_rx, m_ry, m_rz;
	float m_sx, m_sy, m_sz;

	public:
	TransformNode(Node *, char *);
	void draw(VECTOR3D camera_position);
	void setTranslation(float tx, float ty, float tz);
	VECTOR3D getTranslation(void);
	void setRotation(float rx, float ry, float rz);
	VECTOR3D getRotation(void);
	void setScaling(float sx, float sy, float sz);
	bool isType(char *);
	void computeBoundingBox();
};

#endif

