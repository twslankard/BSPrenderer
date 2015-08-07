#include <iostream>
#include "lodnode.h"
#include "VECTOR3D.h"

using namespace std;

LODNode::LODNode(Node * parent, char * name)
	:Node(parent, name)
{
	m_current_child = NULL;
}

LODNode::~LODNode()
{

}

double LODNode::computeDistance(VECTOR3D camera_position)
{
	if(m_current_child == NULL)
		return 0.0;

	VECTOR3D child_position = m_current_child->m_bounding_box.min_point;
	//child_position.lerp(m_current_child->m_bounding_box.max_point, 0.5);

	return child_position.GetLength();
}

int LODNode::whichChild(double distance)
{


	//this'll never happen, but it's "good code" 
	if(distance < 0.0)
		distance = -distance;

	unsigned int child = (unsigned int)floor((float)distance / 10.0 * m_children.size());
	if(child >= m_children.size())
		child = m_children.size() - 1;

	if(m_children.size() == 0)
		return -1;

	return child;
}

void LODNode::draw(VECTOR3D camera_position)
{
	//get the change in position since last time
	VECTOR3D delta = camera_position - old_camera_position;

	//if there wasn't much change, then don't change the current child... this prevents artifacts due to rapid change in detail level
	int which = whichChild(computeDistance(camera_position));
	
	if(which != -1)
	{
		if(m_current_child == NULL || delta.GetSquaredLength() > 0.001)
			m_current_child = m_children[which];
		m_current_child->draw(camera_position);
	}

	old_camera_position = camera_position;

	return;
}

