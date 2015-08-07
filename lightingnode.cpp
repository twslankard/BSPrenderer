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

#include "lightingnode.h"
#include <iostream>

//***********************************************************************
//  LightingNode
//***********************************************************************
LightingNode::LightingNode(Node * parent, char * name)
	: Node(parent, name)
{
	m_pos[0] = 0.0f;m_pos[1] = 0.0f;m_pos[2] = 0.0f;m_pos[3] = 1.0f;
	m_color[0]=1.0f;m_color[1]=1.0f;m_color[2]=1.0f;m_color[3]=1.0f;
}

//***********************************************************************
//  draw
//
//  This function enables a new GL light e.g. GL_LIGHT0,
//  recursively draws the node's children
//  and then disables the light for future use.
//
//  Note that the view frustum doesn't cull this node,
//  because lighting outside of the view frustum
//  may affect nodes inside of the view frustum
//***********************************************************************
void LightingNode::draw(VECTOR3D camera_position)
{

	//draw a point to show where the light is
	if(m_light_positions)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glPointSize(20.0);
		glColor3f(m_color[0], m_color[1], m_color[2]);
		glBegin(GL_POINTS);
		glVertex3f(m_pos[0], m_pos[1], m_pos[2]);
		glEnd();
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
	}

	//make sure we have enough lights available.
	//opengl allows a max of 8 lights, so we set our max light "height" to 8.
	//in other words, the most lights allowed in a particular subtree is 8.
	if(m_num_lights < 8)
	{
		//enable the GL light
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);		//tws 26.4, 2
		glLightfv(0x4000 + m_num_lights, GL_POSITION, m_pos);		//tws 26.4, 2
		glLightfv(0x4000 + m_num_lights, GL_DIFFUSE, m_color);		//tws 26.4, 2
		glEnable(0x4000 + m_num_lights);			//the light is m_num_lights + the offset for the GL enumeration

		//decrease our available pool of lights by 1
		m_num_lights++;
	}
	else
	{
		std::cerr << "Error, number of lights exceeded in this subtree." << std:: endl;
	}

	//draw all of the children of this Node
	std::vector<Node *>::iterator iter = m_children.begin();
	while(iter != m_children.end())
	{
		(*iter)->m_num_lights = m_num_lights;
		(*iter)->draw(camera_position);
		iter++;
	}

	//compute the BB of this node
	computeBoundingBox();

	//if BB's are enabled, then show the BB of this node too
	if(m_bounding_boxes)
		drawBoundingBox();

	//now disable the light, we're done with the affected part of the scene graph
	glDisable(0x4000 + m_num_lights - 1);

	//free up an additional light
	m_num_lights--;

	return;
}

