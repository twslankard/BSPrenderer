//***********************************************************************
//
//  Node
//
//  This class is the basic node class for the scenegraph.
//***********************************************************************

#ifndef NODE_H
#define NODE_H

#include <vector>
#include "VECTOR3D.h"
#include "frustum.h"
#include "boundingbox.h"


class Node
{
	protected:

	//true, if some part of the node is within the view frustum
	bool m_visible;

	//a list of children of this node
	std::vector<Node *> m_children;

	//these variables tell us what we should be drawing
	bool m_textures, m_lighting, m_bounding_boxes, m_light_positions, m_animation;

	private:

	//the parent of this node, which should only be NULL for the root node
	Node * m_parent;

	public:
	bool m_selected;
	void setParent(Node * new_parent);	//sets the parent of this node to new_parent
	BoundingBox m_bounding_box;		//the axis aligned bounding box of this node, which contains all of the AABBs of its children
	char * m_name;				//the name of this node.. the name is used for debugging, I left it in for convenience
	unsigned short m_num_lights;		//the current number of lights being used during the drawing process


	Node(Node * parent, char * name);
	virtual ~Node();
	void addChild(Node * node_to_add);	//adds "node_to_add" to the list of children
	void removeChild(Node * node_to_remove);//removes "node_to_remove" from the list of children
	Node * getChild(int index_of_node_to_get);//returns the child at "index_of_node_to_get" in children

	virtual bool isType(char *);
	virtual void selectAll();		//unselects this node and its children
	virtual void unselectAll();		//unselects this node and its children
	bool visible(void);


	//calculateVisibleNodes:
	//marks all children in the view frustum as visible
	//and all children out of the frustum as not visible
	virtual void calculateVisibleNodes(Frustum view_frustum);

	

	virtual void draw(VECTOR3D camera_position);			//draws this node and its children
	virtual void drawBoundingBox();		//draws this node's bounding box, which first must be calculated using computeBoundingBox
	virtual void computeBoundingBox();	//calculates the bounding box of this node, given the bounding boxes of the children
	virtual Node * bbClick(int x, int y);	//returns a pointer to node highest up in the tree whose bounding box is clicked on
	virtual int createMenu(int, int);
	//sets what should be drawn (i.e. whether textures should be displayed)
	virtual void setDrawMode(bool textures, bool lighting, bool bounding_boxes, bool light_positions, bool animation);
};

#endif
