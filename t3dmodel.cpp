#include <GL/gl.h>
#include "t3dmodel.h"

void t3DModel::drawAABB()
{
	glColor3f(0.0f, 0.75f, 0.0f);
	if(m_selected)
		glColor3f(0.75f, 0.0f, 0.0f);


	glBegin(GL_QUADS);
	glVertex3f(min_x, min_y, min_z);
	glVertex3f(max_x, min_y, min_z);
	glVertex3f(max_x, min_y, max_z);
	glVertex3f(min_x, min_y, max_z);

	glVertex3f(min_x, max_y, min_z);
	glVertex3f(max_x, max_y, min_z);
	glVertex3f(max_x, max_y, max_z);
	glVertex3f(min_x, max_y, max_z);

	glVertex3f(min_x, min_y, min_z);
	glVertex3f(max_x, min_y, min_z);
	glVertex3f(max_x, max_y, min_z);
	glVertex3f(min_x, max_y, min_z);

	glVertex3f(min_x, min_y, max_z);
	glVertex3f(max_x, min_y, max_z);
	glVertex3f(max_x, max_y, max_z);
	glVertex3f(min_x, max_y, max_z);
	glEnd();
}
