//***********************************************************************
//
//  MD3Node
//
//  This class is a wrapper, which adapts a CModelMD3 to the scenegraph
//***********************************************************************

#ifndef MD3NODE_H
#define MD3NODE_H

#include <iostream>
#include "boundingbox.h"
#include "node.h"
#include "md3.h"

using namespace std;

class MD3Node : public Node
{
	public:
	CModelMD3 m_model;
	MD3Node(Node * parent, char * name);
	MD3Node(Node * parent, char * name, MD3Node * instance_of);
	MD3Node(Node * parent, char * name, char * model_to_load, char * weapon_to_load);
	void draw(VECTOR3D camera_postion);
	void computeBoundingBox();
	void setAnimation(char * torso_animation, char * legs_animation);
	void setDrawMode(bool textures, bool lighting, bool bounding_boxes, bool light_positions, bool animation);
	void selectAll(void);
	void unselectAll(void);
	bool isType(char *);
};

#endif


