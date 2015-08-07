//***********************************************************************
//
//  LODNode
//
//  This class works as a sort of distance-activated switch.
//  When drawn, this node decides which child to draw based on the distance to the camera.
//
//***********************************************************************

#ifndef LODNODE_H
#define LODNODE_H

#include "node.h"

class LODNode : public Node
{
	public:

	Node * m_current_child;
	VECTOR3D old_camera_position;

	LODNode(Node * parent, char * name);
	virtual ~LODNode();

	double computeDistance(VECTOR3D camera_position);
	int whichChild(double distance);
	void draw(VECTOR3D camera_position);
};

#endif

