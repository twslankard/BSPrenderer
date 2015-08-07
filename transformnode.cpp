//***********************************************************************
//  transformnode
//
//  This node is used to transform its children by translating, rotating, or scaling.
//***********************************************************************

#include <GL/gl.h>
#include <vector>
#include <cstring>
#include "transformnode.h"

TransformNode::TransformNode(Node * parent, char * name)
	: Node(parent, name)
{
	setScaling(1.0f, 1.0f, 1.0f);
	setRotation(0.0f, 0.0f, 0.0f);
	setTranslation(0.0f, 0.0f, 0.0f);
}

void TransformNode::setTranslation(float tx, float ty, float tz)
{
	m_tx = tx;
	m_ty = ty;
	m_tz = tz;
	return;
}


void TransformNode::setRotation(float rx, float ry, float rz)
{
	m_rx = rx;
	m_ry = ry;
	m_rz = rz;
	return;
}

void TransformNode::setScaling(float sx, float sy, float sz)
{
	m_sx = sx;
	m_sy = sy;
	m_sz = sz;
	return;
}

void TransformNode::draw(VECTOR3D camera_position)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	//apply the transformation
	glTranslatef(m_tx, m_ty, m_tz);
	glRotatef(m_rx, 1.0, 0.0, 0.0);
	glRotatef(m_ry, 0.0, 1.0, 0.0);
	glRotatef(m_rz, 0.0, 0.0, 1.0);
	glScalef(m_sx, m_sy, m_sz);

	//draw all of the children of this Node
	std::vector<Node *>::iterator iter = m_children.begin();
	while(iter != m_children.end())
	{
		(*iter)->draw(camera_position);
		iter++;
	}

	glPopMatrix();
}

VECTOR3D TransformNode::getTranslation(void)
{
	VECTOR3D trans;
	trans.x = m_tx;
	trans.y = m_ty;
	trans.z = m_tz;
	return trans;
}

VECTOR3D TransformNode::getRotation(void)
{
	VECTOR3D rot;
	rot.x = m_rx;
	rot.y = m_ry;
	rot.z = m_rz;
	return rot;
}

bool TransformNode::isType(char * t)
{
	if(strcmp(t, "TransformNode") == 0)
		return true;
	return false;
}

void TransformNode::computeBoundingBox()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	//apply the transformation
	glTranslatef(m_tx, m_ty, m_tz);
	glRotatef(m_rx, 1.0, 0.0, 0.0);
	glRotatef(m_ry, 0.0, 1.0, 0.0);
	glRotatef(m_rz, 0.0, 0.0, 1.0);
	glScalef(m_sx, m_sy, m_sz);

	Node::computeBoundingBox();

	glPopMatrix();
}

