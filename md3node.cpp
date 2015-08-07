//***********************************************************************
//  MD3Node
//
//  This class is a wrapper, which adapts a CModelMD3 to the scenegraph
//***********************************************************************

#include "md3node.h"
#include <iostream>
#include <cstring>

MD3Node::MD3Node(Node * parent, char * name)
	: Node(parent, name)
{
	m_visible = true;

}

MD3Node::MD3Node(Node * parent, char * name, MD3Node * instance_of)
	: Node(parent, name)
{
	//make a complete copy of the original
	*this = *instance_of;

	//except change the name
	m_name = name;

	//and set the parent to the correct parent
	setParent(parent);

	//allocate a new link array for each piece of the model,
	//or else we'll have a bunch of clones walking around with the same animation
	m_model.m_Lower.pLinks = new t3DModel*[m_model.m_Lower.numOfTags];
	m_model.m_Upper.pLinks = new t3DModel*[m_model.m_Upper.numOfTags];
	m_model.m_Head.pLinks =  new t3DModel*[m_model.m_Head.numOfTags];
	m_model.m_Weapon.pLinks = new t3DModel*[m_model.m_Weapon.numOfTags];

	//update the link arrays for the model, so that they contain the correct information for the new instance
	m_model.reLink();
}

MD3Node::MD3Node(Node * parent, char * name, char * model_to_load, char * weapon_to_load)
	: Node(parent, name)
{
	//load the character model
	m_model.LoadModel(model_to_load, model_to_load);

	// Load the gun and attach it to our character
	m_model.LoadWeapon(model_to_load, weapon_to_load);
}

void MD3Node::draw(VECTOR3D camera_postion)
{
	if(m_visible == false)
	{
		//cerr << "not drawing lara" << endl;
		return;
	}

	//cerr << "Drawing lara" << endl;

	m_model.DrawModel();
	//computeBoundingBox();			//tws 27.4 -- this is called by the parent?

	//draw the bounding box if necessary	//tws 27.4 -- this is called by the parent?
	if(m_bounding_boxes)
	{
		drawBoundingBox();
	}

	return;
}

void MD3Node::setAnimation(char * torso_animation, char * legs_animation)
{
	m_model.SetTorsoAnimation(torso_animation);
	m_model.SetLegsAnimation(legs_animation);
	return;
}

void MD3Node::setDrawMode(bool textures, bool lighting, bool bounding_boxes, bool light_positions, bool animation)
{
	Node::setDrawMode(textures, lighting, bounding_boxes, light_positions, animation);
	m_model.setDrawMode(textures, lighting, bounding_boxes, animation);
}

void MD3Node::computeBoundingBox(void)
{
	int current_box;
	float min_x, min_y, min_z, max_x, max_y, max_z;

	m_model.calculateBoundingBox();

	min_x = m_model.m_boxes[0].min_point.x;
	min_y = m_model.m_boxes[0].min_point.y;
	min_z = m_model.m_boxes[0].min_point.z;

	max_x = m_model.m_boxes[0].max_point.x;
	max_y = m_model.m_boxes[0].max_point.y;
	max_z = m_model.m_boxes[0].max_point.z;

	for(current_box = 0; current_box <= m_model.m_num_boxes; current_box++)
	{
		if(m_model.m_boxes[current_box].min_point.x < min_x)
			min_x = m_model.m_boxes[current_box].min_point.x;
		if(m_model.m_boxes[current_box].min_point.y < min_y)
			min_y = m_model.m_boxes[current_box].min_point.y;
		if(m_model.m_boxes[current_box].min_point.z < min_z)
			min_z = m_model.m_boxes[current_box].min_point.z;

		if(m_model.m_boxes[current_box].max_point.x > max_x)
			max_x = m_model.m_boxes[current_box].max_point.x;
		if(m_model.m_boxes[current_box].max_point.y > max_y)
			max_y = m_model.m_boxes[current_box].max_point.y;
		if(m_model.m_boxes[current_box].max_point.z > max_z)
			max_z = m_model.m_boxes[current_box].max_point.z;

	}

	m_bounding_box.setBoundingBox(min_x, min_y, min_z, max_x, max_y, max_z);
}

//**********************************************************************************
//  selectAll
//
//  selects this node and its children
//**********************************************************************************
void MD3Node::selectAll()
{
	//std::cerr << "selecting model" << std::endl;

	m_model.select();
	Node::selectAll();
}

//**********************************************************************************
//  unselectAll
//
//  unselects this node and its children
//**********************************************************************************
void MD3Node::unselectAll()
{
	m_model.unselect();
	Node::unselectAll();
}

bool MD3Node::isType(char * t)
{
	if(strcmp(t, "MD3Node") == 0)
		return true;
	return false;
}


