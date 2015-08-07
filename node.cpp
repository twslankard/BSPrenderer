//***********************************************************************
//
//  Node
//
//  This class is the basic node class for the scenegraph.
//***********************************************************************

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <cstring>
#include "node.h"

//**********************************************************************************
//  Node
//
//  initializes this Node, and sets its parent to "parent", and automatically
//  adds itself as a child
//**********************************************************************************
Node::Node(Node * parent, char * name)
{
	m_selected = false;
	m_num_lights = 0;
	m_name = name;
	m_parent = NULL;
	m_parent = parent;
	m_visible = true;

	if(m_parent != NULL)
		m_parent->addChild(this);
}

//**********************************************************************************
//  ~Node
//
//  does nothing at the moment
//**********************************************************************************
Node::~Node()
{

}

//**********************************************************************************
//  addChild
//
//  pushes node_to_add onto this Node's list of children
//  and makes this the parent of node_to_add
//**********************************************************************************
void Node::addChild(Node * node_to_add)
{
	node_to_add->setParent(this);		//make this the parent of the new child
	m_children.push_back(node_to_add);
	return;
}

//**********************************************************************************
//  removeChild
//
//  find the occurrence of node_to_remove in the list of children, and remove it
//**********************************************************************************
void Node::removeChild(Node * node_to_remove)
{

	std::vector<Node *>::iterator iter = m_children.begin();
	while(iter != m_children.end())
	{
		if(*iter == node_to_remove)
		{
			(*iter)->setParent(NULL);
			m_children.erase(iter);
			break;
		}
		else
		{
			iter++;
		}
	}
}

//**********************************************************************************
//  getChild
//
//  returns the child at "index_of_node_to_get"
//**********************************************************************************
Node * Node::getChild(int index_of_node_to_get)
{
	return m_children[index_of_node_to_get];
}

//**********************************************************************************
//  draw
//
//  draws all children of this node
//**********************************************************************************
void Node::draw(VECTOR3D camera_position)
{
	
	//draw all of the children of this Node
	std::vector<Node *>::iterator iter = m_children.begin();
	while(iter != m_children.end())
	{
		(*iter)->m_num_lights = m_num_lights;
		(*iter)->draw(camera_position);
		//(*iter)->computeBoundingBox();

		//if(m_bounding_boxes)
		//(*iter)->drawBoundingBox();
		iter++;
	}

	//compute the BB of this node
	//computeBoundingBox();

	//if BB's are enabled, then show the BB of this node too
	if(m_bounding_boxes)
		drawBoundingBox();

	return;
}

//**********************************************************************************
//  computeBoundingBox
//
//  computes the bounding boxes of all children, and then computes the bounding
//  box of this Node using the bounding boxes of the children
//**********************************************************************************
void Node::computeBoundingBox()
{
	bool first = true;
	float min_x = 0.0f, min_y = 0.0f, min_z = 0.0f, max_x = 0.0f, max_y = 0.0f, max_z = 0.0f;

	//compute the bounding boxes of all children and keep track of the min and max extents
	std::vector<Node *>::iterator iter = m_children.begin();
	while(iter != m_children.end())
	{
		//compute the bounding box of the child		//called by draw
		(*iter)->computeBoundingBox();

		//if it's the first child, initialize the min and max extents to the min and max points of the first BB
		if(first)
		{
			min_x = (*iter)->m_bounding_box.min_point.x;
			min_y = (*iter)->m_bounding_box.min_point.y;
			min_z = (*iter)->m_bounding_box.min_point.z;

			max_x = (*iter)->m_bounding_box.max_point.x;
			max_y = (*iter)->m_bounding_box.max_point.y;
			max_z = (*iter)->m_bounding_box.max_point.z;

			first = false;
		}
		else
		{
			//update min and max
			if((*iter)->m_bounding_box.min_point.x < min_x)
				min_x = (*iter)->m_bounding_box.min_point.x;
			if((*iter)->m_bounding_box.min_point.y < min_y)
				min_y = (*iter)->m_bounding_box.min_point.y;
			if((*iter)->m_bounding_box.min_point.z < min_z)
				min_z = (*iter)->m_bounding_box.min_point.z;

			if((*iter)->m_bounding_box.max_point.x > max_x)
				max_x = (*iter)->m_bounding_box.max_point.x;
			if((*iter)->m_bounding_box.max_point.y > max_y)
				max_y = (*iter)->m_bounding_box.max_point.y;
			if((*iter)->m_bounding_box.max_point.z > max_z)
				max_z = (*iter)->m_bounding_box.max_point.z;
		}

		iter++;
	}

	//store this node's BB
	m_bounding_box.setBoundingBox(min_x, min_y, min_z, max_x, max_y, max_z);

	return;
}

//**********************************************************************************
//  setParent
//
//  sets the parent of this node
//  one should never need to use this as a public method--that's why it's a private one
//**********************************************************************************
void Node::setParent(Node * new_parent)
{
	m_parent = new_parent;
	return;
}


//**********************************************************************************
//  setDrawMode
//
//  sets the draw mode of this node and all of its children
//**********************************************************************************
void Node::setDrawMode(bool textures, bool lighting, bool bounding_boxes, bool light_positions, bool animation)
{
	//set the draw mode of this node
	m_textures = textures;
	m_lighting = lighting;
	m_bounding_boxes = bounding_boxes;
	m_light_positions = light_positions;
	m_animation = animation;

	//set the draw mode of all child nodes
	std::vector<Node *>::iterator iter = m_children.begin();
	while(iter != m_children.end())
	{
		(*iter)->setDrawMode(textures, lighting, bounding_boxes, light_positions, animation);
		iter++;
	}

	return;
}

//**********************************************************************************
//  drawBoundingBox
//
//  draws the bounding box... note that the box must be calculated first
//**********************************************************************************
void Node::drawBoundingBox()
{
	//load a fresh matrix, since the boxes are relative to the eye--not the models
	glPushMatrix();
	glLoadIdentity();

	//Don't cull the AABB faces, we want to see all of them
	glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	//if the node is selected, change the color of the BB
	glColor3f(0.0f, 0.75f, 0.0f);
	if(m_selected)
		glColor3f(0.75f, 0.0f, 0.0f);

	glBegin(GL_QUADS);
	glVertex3f(m_bounding_box.min_point.x, m_bounding_box.min_point.y, m_bounding_box.min_point.z);
	glVertex3f(m_bounding_box.max_point.x, m_bounding_box.min_point.y, m_bounding_box.min_point.z);
	glVertex3f(m_bounding_box.max_point.x, m_bounding_box.min_point.y, m_bounding_box.max_point.z);
	glVertex3f(m_bounding_box.min_point.x, m_bounding_box.min_point.y, m_bounding_box.max_point.z);

	glVertex3f(m_bounding_box.min_point.x, m_bounding_box.max_point.y, m_bounding_box.min_point.z);
	glVertex3f(m_bounding_box.max_point.x, m_bounding_box.max_point.y, m_bounding_box.min_point.z);
	glVertex3f(m_bounding_box.max_point.x, m_bounding_box.max_point.y, m_bounding_box.max_point.z);
	glVertex3f(m_bounding_box.min_point.x, m_bounding_box.max_point.y, m_bounding_box.max_point.z);

	glVertex3f(m_bounding_box.min_point.x, m_bounding_box.min_point.y, m_bounding_box.min_point.z);
	glVertex3f(m_bounding_box.max_point.x, m_bounding_box.min_point.y, m_bounding_box.min_point.z);
	glVertex3f(m_bounding_box.max_point.x, m_bounding_box.max_point.y, m_bounding_box.min_point.z);
	glVertex3f(m_bounding_box.min_point.x, m_bounding_box.max_point.y, m_bounding_box.min_point.z);

	glVertex3f(m_bounding_box.min_point.x, m_bounding_box.min_point.y, m_bounding_box.max_point.z);
	glVertex3f(m_bounding_box.max_point.x, m_bounding_box.min_point.y, m_bounding_box.max_point.z);
	glVertex3f(m_bounding_box.max_point.x, m_bounding_box.max_point.y, m_bounding_box.max_point.z);
	glVertex3f(m_bounding_box.min_point.x, m_bounding_box.max_point.y, m_bounding_box.max_point.z);
	glEnd();

	/*glPointSize(7.0);
	//glBegin(GL_POINTS);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(m_bounding_box.min_point.x, m_bounding_box.min_point.y, m_bounding_box.min_point.z);
	glColor3f(0.0f, 0.75f, 0.0f);
	glVertex3f(m_bounding_box.max_point.x, m_bounding_box.min_point.y, m_bounding_box.min_point.z);
	glVertex3f(m_bounding_box.max_point.x, m_bounding_box.max_point.y, m_bounding_box.min_point.z);
	glVertex3f(m_bounding_box.min_point.x, m_bounding_box.max_point.y, m_bounding_box.min_point.z);

	glVertex3f(m_bounding_box.min_point.x, m_bounding_box.min_point.y, m_bounding_box.max_point.z);
	glVertex3f(m_bounding_box.max_point.x, m_bounding_box.min_point.y, m_bounding_box.max_point.z);
	glVertex3f(m_bounding_box.max_point.x, m_bounding_box.max_point.y, m_bounding_box.max_point.z);
	glVertex3f(m_bounding_box.min_point.x, m_bounding_box.max_point.y, m_bounding_box.max_point.z);
	glEnd();

*/
	glPopMatrix();

	return;
}

//**********************************************************************************
//  bbClick
//
//  returns true if the user clicked on the node's BB
//**********************************************************************************
Node * Node::bbClick(int x, int y)
{
	GLdouble m_projection_matrix[16];
	GLint m_viewport[4];

	//get the viewport and projection matrix
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	glGetDoublev(GL_PROJECTION_MATRIX, m_projection_matrix);

	GLdouble bb_screen_x;
	GLdouble bb_screen_y;
	GLdouble bb_screen_z;

	GLdouble identitymatrix[16] = {	1.0, 0.0, 0.0, 0.0,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					0.0, 0.0, 0.0, 1.0	};

	//find the screen coordinates
	gluProject(     m_bounding_box.min_point.x,
                        m_bounding_box.min_point.y,
                        m_bounding_box.min_point.z,
                        identitymatrix,
                        m_projection_matrix,
                        m_viewport,
                        &bb_screen_x,
                        &bb_screen_y,
                        &bb_screen_z
                                );

	//std::cerr << "(" << x << ", " << y << ") - (" << bb_screen_x << ", " << H - bb_screen_y << ")" << std::endl;



	std::vector<Node *>::iterator iter = m_children.begin();
	while(iter != m_children.end())
	{
		if( (*iter)->bbClick(x, y) != NULL)
			return (*iter);
		iter++;
	}

	if(fabs((float)x - bb_screen_x - 2.0f) < 4.0f)
	if(fabs((float)y - (m_viewport[3] - bb_screen_y) - 2.0f) < 4.0f)
	{
		std::cerr << "Clicked on " << m_name << std::endl;
		return this;
	}
 	return NULL;
}

//**********************************************************************************
//  selectAll
//
//  selects this node and its children
//**********************************************************************************
void Node::selectAll()
{
	m_selected = true;

	std::vector<Node *>::iterator iter = m_children.begin();
	while(iter != m_children.end())
	{
		(*iter)->selectAll();
		iter++;
	}
}

//**********************************************************************************
//  unselectAll
//
//  unselects this node and its children
//**********************************************************************************
void Node::unselectAll()
{
	m_selected = false;

	std::vector<Node *>::iterator iter = m_children.begin();
	while(iter != m_children.end())
	{
		(*iter)->unselectAll();
		iter++;
	}
}

//**********************************************************************************
//  createMenu
//
//  adds items to a glut menu for this subtree
//**********************************************************************************
int Node::createMenu(int index, int level)
{
	std::string label;

	int i;
	for(i = 0; i < level; i++)
	{
		label.append("-");
	}
	label.append(m_name);
	glutAddMenuEntry(label.c_str(), (long)this);

	std::vector<Node *>::iterator iter = m_children.begin();
	while(iter != m_children.end())
	{
		(*iter)->createMenu(0, level + 1);
		iter++;
	}

	return 0;
}

//**********************************************************************************
//  Node::isType
//
//  returns true if t is "Node".
//**********************************************************************************
bool Node::isType(char * t)
{
	if(strcmp(t, "Node") == 0)
		return true;
	return false;
}

//**********************************************************************************
//  calculateVisibleNodes
//
//  marks this node and all children as visible or not visible, depending whether
//  or not they are in the view frustum
//**********************************************************************************
void Node::calculateVisibleNodes(Frustum view_frustum)
{
	VECTOR3D min = m_bounding_box.min_point;
	VECTOR3D max = m_bounding_box.max_point;
	VECTOR3D this_nodes_bounding_box[8];
	this_nodes_bounding_box[0] = VECTOR3D(min.x, min.y, min.z);
	this_nodes_bounding_box[1] = VECTOR3D(max.x, min.y, min.z);
	this_nodes_bounding_box[2] = VECTOR3D(min.x, max.y, min.z);
	this_nodes_bounding_box[3] = VECTOR3D(max.x, max.y, min.z);
	this_nodes_bounding_box[4] = VECTOR3D(min.x, min.y, max.z);
	this_nodes_bounding_box[5] = VECTOR3D(max.x, min.y, max.z);
	this_nodes_bounding_box[6] = VECTOR3D(min.x, max.y, max.z);
	this_nodes_bounding_box[7] = VECTOR3D(max.x, max.y, max.z);

	if(view_frustum.IsBoundingBoxInside(this_nodes_bounding_box))
	//if(view_frustum.IsPointInside(m_bounding_box.min_point) || view_frustum.IsPointInside(m_bounding_box.max_point))
	{
		//mark this node as visible
		m_visible = true;
	}
	else
	{
		m_visible = false;
	}

	//check to see which children are visible
	std::vector<Node *>::iterator iter = m_children.begin();
	while(iter != m_children.end())
	{
		(*iter)->calculateVisibleNodes(view_frustum);
		iter++;
	}
}

//**********************************************************************************
//  visible
//
//  returns true iff m_visible is true
//**********************************************************************************
bool Node::visible()
{
	return m_visible;
}





