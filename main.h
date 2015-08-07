#ifndef MAIN_H
#define MAIN_H

#include <GL/gl.h>	//tws
#include <GL/glu.h>	//tws
#include <GL/glut.h>
#include <GL/glext.h>	//tws
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <string>

#include "VECTOR3D.h"
#include "camera.h"
#include "main.h"
#include "BSP.h"
#include "frustum.h"
#include "node.h"
#include "md3node.h"
#include "lodnode.h"
#include "transformnode.h"
#include "lightingnode.h"
#include "Timer.h"

//keyboard callbacks
void regular_up(unsigned char theKey, int mouseX, int mouseY);
void regular_keys(unsigned char theKey, int mouseX, int mouseY);
void special_keys(int theKey, int mouseX, int mouseY);

//mouse callbacks
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void passive_motion(int x, int y);

//draw callbacks
void reshape(int w, int h);
void redraw(void);
void idle(void);

//menu callbacks
void selection(int hidden_node_ptr);
void viewOptions(int);

//other routines
bool initialize();
void createSelectionMenu(void);
void pauseGame();
void unpauseGame();

#endif	//MAIN_H



