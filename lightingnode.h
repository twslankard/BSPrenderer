//***********************************************************************
//
//  LightingNode
//
//  This class is a node which lights its children.
//  It stores only a light color and position.
//  During drawing, it enables a new GL light e.g. GL_LIGHT0,
//  recursively draws its children (like other nodes do)
//  and then disables the light for future use.
//
//  This means that any node in the scene can be lit by a maximum of eight lights,
//  because this is the limit defined by opengl.
//
//***********************************************************************

#ifndef LIGHTINGNODE_H
#define LIGHTINGNODE_H

#include "node.h"
#include <GL/gl.h>
#include <GL/glu.h>

class LightingNode : public Node
{
	public:
	LightingNode(Node * parent, char * name);

	float m_pos[4];		//light source position
	float m_color[4];	//light source color

	//setPos -- sets the position of the light source
	void setPos(float x, float y, float z, float w)
	{
		m_pos[0] = x; m_pos[1] = y; m_pos[2] = z; m_pos[3] = w;
	}

	//setColor -- sets the color of the light source
	void setColor(float r, float g, float b, float a)
	{
		m_color[0] = r; m_color[1] = g; m_color[2] = b; m_color[3] = a;
	}

	//  During drawing, this node enables a new GL light
	//  and recursively draws its children
	//  and then disables the light.
	void draw(VECTOR3D camera_position);

};

#endif

