//includes
#include <cstring>
#include "main.h"

//global defines
#define MOVEMENT_SPEED 0.12
#define LOOK_SPEED 2.4 

using namespace std;

//global vars
BSP bsp;					//quake 3 bsp
Frustum frustum;				//the view frustum
Camera camera;					//the camera
MD3Node * gun;
MD3Node * model;
TransformNode * gun_transform;
Node * root;					//root of the scenegraph
Node * select_node = NULL;			//the currently selected node in the scenegraph
int mpos_x, mpos_y;				//the current mouse position
int W = 800, H = 600;				//width and height of the window
int KeyDown[256];				//stores which keys are down
bool paused = false;				//true if the game is paused
Misc::Timer timer;				//limits the player movement
bool lighting = true, textures = true, bounding_boxes = false, animation = true, light_positions = false;
int view_options_menu;
int selection_menu;
string msg;
bool debug = true;
bool is_attacking = false;
bool is_moving = false;
bool was_moving = false;
float bob = 0.0;
//**********************************************************************************
//  main
//
//  sets up a window and menus, and calls various functions to initialize the game
//**********************************************************************************
int main(int argc, char * argv[])
{
	//initialize glut
	glutInitWindowSize(W, H);
	glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	//option "-w" starts the game in a window
	if (argc >= 2 && strcmp(argv[1], "-w") == 0)
	{
		//create a window
		glutCreateWindow("OMG H4X!");
	}
	else
	{
		//enter full screen mode
		glutGameModeString("800x600:32@100");
		glutEnterGameMode();
	}

	//set up draw callbacks
	glutReshapeFunc(reshape);
	glutDisplayFunc(redraw);
        glutIdleFunc(idle);

	//set up keyboard
	memset( KeyDown, 0, sizeof( KeyDown ) );
	glutKeyboardUpFunc( regular_up );
	glutKeyboardFunc(regular_keys);
	glutSpecialFunc(special_keys);

	//enable functions for handling the mouse, for rotation
	glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(passive_motion);

	//create a menu
	view_options_menu = glutCreateMenu(viewOptions);
	glutAddMenuEntry("Toggle Textures", 1);
	glutAddMenuEntry("Toggle Lighting", 2);
	glutAddMenuEntry("Toggle Bounding Boxes", 3);
 	glutAddMenuEntry("Toggle Light Positions", 4);
	glutAddMenuEntry("Toggle Animation", 5);
	glutAddMenuEntry("Quit", 6);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	//find out how many multitexturing units we have
	//
	//originally, I planned to encorporate multitexturing for the BSP,
	//because BSPs use light maps. However I couldn't get the multitexturing functionality to work.
	int num_multitexture_units = -1;
	glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, &num_multitexture_units);
	cerr << "Number of multitexture units available: " << num_multitexture_units << endl;

	//load the map and initialize the scene
	initialize();

	//create the selection menu
	createSelectionMenu();

	//start timer
	cerr << "Starting..." << endl;
	timer.elapse();
	unpauseGame();
	glutMainLoop();
        return 0;

}

//**********************************************************************************
//  viewOptions
//
//  handles menu toggling for view options of the entire scene
//**********************************************************************************
void viewOptions(int value)
{
	switch (value)
	{
		case 1:
			if(textures)
				textures = false;
			else
				textures = true;
			break;
		case 2:
			if(lighting)
				lighting = false;
			else
				lighting = true;
			break;
		case 3:
			if(bounding_boxes)
				bounding_boxes = false;
			else
				bounding_boxes = true;

			break;
		case 4:
			if(light_positions)
				light_positions = false;
			else
				light_positions = true;

			break;
		case 5:
			if(animation)
				animation = false;
			else
				animation = true;

			break;
		case 6:
			exit(0);
			break;
	}
	root->setDrawMode(textures, lighting, bounding_boxes, light_positions, animation);
	glutPostRedisplay();
}


//**********************************************************************************
//  selection
//
//  this function is the callback for the menu created by createSelectionMenu.
//  The menu item argument is passed as an int in GLUT, but is treated as a Node *
//  so that we have direct access to the selected item, without having to search for it.
//**********************************************************************************
void selection(int hidden_node_ptr)
{
	root->unselectAll();
	select_node = (Node *)hidden_node_ptr;
	std::cerr << "Selected " << select_node->m_name << std::endl;
	std::cerr << select_node << std::endl;
	select_node->selectAll();
}

//**********************************************************************************
//  createSelectionMenu
//
//  this calls "createMenu" on the root of the scenegraph,
//  which traverses the entire scenegraph and adds a menu item for each node
//**********************************************************************************
void createSelectionMenu(void)
{
	if(root == NULL)
	{
		cerr << "No scene initialized!" << endl;
		return;
	}

	selection_menu = glutCreateMenu(selection);
	root->createMenu(0, 0);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
}

//**********************************************************************************
//  initialize
//
//  this function loads a quake BSP level
//  and sets up a scenegraph with characters
//**********************************************************************************
bool initialize()
{
	char levelName[256];
	int curveTesselation;
	VECTOR3D cameraPosition;
	float angleYaw, anglePitch;

	//read in the map name etc from config.txt
	FILE * configFile=fopen("config.txt", "rt");
	if(!configFile)
	{
		cerr<< "Cannot open map config file." << endl;
		return false;
	}

	//read the config file
	fscanf(configFile, "Map: %s\n", levelName);
	fscanf(configFile, "Curve Tesselation: %d\n", &curveTesselation);
	fscanf(configFile, "Camera Position: (%f %f %f)\n", &cameraPosition.x, &cameraPosition.y, &cameraPosition.z);
	fscanf(configFile, "Camera Orientation: %f %f\n", &angleYaw, &anglePitch);
	fclose(configFile);

	//load the level
	if(!bsp.Load(levelName, curveTesselation))
		return false;

	//set the camera position
	camera.set(cameraPosition, angleYaw, anglePitch);

	//set up a scene
	root = new Node(NULL, "root");
	LightingNode * bluelight = new LightingNode(root, "bluelight");
	LightingNode * redlight = new LightingNode(bluelight, "redlight");
	LightingNode * greenlight = new LightingNode(redlight, "greenlight");
	LightingNode * yellowlight = new LightingNode(greenlight, "yellowlight");
	LightingNode * greenlight2 = new LightingNode(yellowlight, "greenlight2");
	LightingNode * yellowlight2 = new LightingNode(greenlight2, "yellowlight2");

	bluelight->setColor(0.3, 0.3, 1.0, 1.0);
	bluelight->setPos(-2.3, 1.5, -2.3, 1.0);
	redlight->setColor(1.0, 0.3, 0.3, 1.0);
	redlight->setPos(-2.3, 1.5, 2.3, 1.0);
	greenlight->setColor(0.3, 0.8, 0.3, 1.0);
	greenlight->setPos(2.3, 1.5, 2.3, 1.0);
	yellowlight->setColor(0.8, 0.8, 0.3, 1.0);
	yellowlight->setPos(2.3, 1.5, -2.3, 1.0);
	greenlight2->setColor(0.3, 0.8, 0.3, 1.0);
	greenlight2->setPos(0.0, 2.0, -12.5, 1.0);
	yellowlight2->setColor(0.8, 0.8, 0.3, 1.0);
	yellowlight2->setPos(10.0, 2.0, 0.0, 1.0);

	TransformNode * translate1 = new TransformNode(yellowlight2, "scale2");
	translate1->setTranslation(0.00, 0.4, 0.00);
	TransformNode * translate2 = new TransformNode(yellowlight2, "scale2");
	translate2->setTranslation(2.00, 0.4, 2.00);
	translate2->setRotation(0.0, 120.0, 0.0);
	TransformNode * scale = new TransformNode(translate1, "scale");
	scale->setScaling(0.015, 0.015, 0.015);
	//scale->setTranslation(0.00, 0.40, 0.00);
	TransformNode * scale2 = new TransformNode(translate2, "scale2");
	scale2->setScaling(0.015, 0.015, 0.015);


	//scale2->setTranslation(3.00, 0.40, 3.00);
	//gun_transform = new TransformNode(yellowlight2, "scale");
	//gun_transform->setScaling(0.015, 0.015, 0.015);
	//gun_transform->setTranslation(0.00, 0.40, 0.00);
	//gun = new MD3Node(gun_transform, "lara", "data/lara", "data/lara/railgun");
	//gun->setAnimation("TORSO_STAND", "LEGS_IDLE");

	//set up a level of detail node, with two children
	LODNode * level_of_detail_node = new LODNode(scale, "lod");
        model = new MD3Node(level_of_detail_node, "lara", "data/lara", "data/lara/railgun");
	MD3Node * model2 = new MD3Node(level_of_detail_node, "strooper", "data/strooper", "data/strooper/railgun");

	MD3Node * model3 = new MD3Node(scale2, "strooper", "data/strooper", "data/strooper/railgun");

        //set the animation of the models
        model->setAnimation("TORSO_STAND", "LEGS_IDLE");
	model2->setAnimation("TORSO_STAND", "LEGS_IDLE");
	model3->setAnimation("TORSO_STAND", "LEGS_RUN");
	root->setDrawMode(textures, lighting, bounding_boxes, light_positions, animation);

	return true;
}


//**********************************************************************************
//  reshape
//
//  this function responds to screen resize events, but also initializes all of
//  the necessary OpenGL functions we'll be using
//**********************************************************************************
void reshape(int w, int h)
{
	cerr << "New window size: (" << w << ", " << h << ")" << endl;

	if(h == 0)
		h = 1;
	W = w;
	H = h;
	glViewport(0, 0, w, h);					//reset viewport

	//set up projection matrix
	glMatrixMode(GL_PROJECTION);							//select projection matrix
	glLoadIdentity();										//reset
	gluPerspective(75.0f, (GLfloat)w/(GLfloat)h, 0.1f, 100.0f);

	//load identity modelview
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//set up the various GL options we need
	glShadeModel(GL_SMOOTH);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);

	//make sure lights are set up
	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.015);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.015);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.015);
	glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0.015);
	glLightf(GL_LIGHT4, GL_QUADRATIC_ATTENUATION, 0.015);
	glLightf(GL_LIGHT5, GL_QUADRATIC_ATTENUATION, 0.015);
	glLightf(GL_LIGHT6, GL_QUADRATIC_ATTENUATION, 0.015);
	glLightf(GL_LIGHT7, GL_QUADRATIC_ATTENUATION, 0.015);
	glEnable(GL_AUTO_NORMAL);
}

//**********************************************************************************
//  output
//
//  prints a message on the screen
//**********************************************************************************
void output(float x, float y, const char * text, void* font)
{

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, (GLfloat)W, 0.0, (GLfloat)H, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glRasterPos2f(x, y);

	int lines = 0;
	int i;
	int length = strlen(text);
	for (i = 0; i < length; i++)
	{
		if(text[i] == '\n')
		{
			glRasterPos2f(x, y+(lines*18));
			lines++;
		}
		else
		{
			glutBitmapCharacter(font, text[i]);
		}
	}
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

}

//**********************************************************************************
//  idle
//
//  This function continuously checks for user input, updates the camera position,
//  and calls redisplay
//**********************************************************************************
void idle(void)
{
	VECTOR3D start, end, collision_location;

	//save the camera start position
	start = camera.m_position;

	is_moving = false;

	//check to see if enough time has passed.
	//we use this timer to limit the player speed.
	if(!paused && timer.peekTime() > 0.02)
	{
		//move the camera according to which keys are pressed
		if(KeyDown['w'])
		{
			is_moving = true;
			camera.moveForward(MOVEMENT_SPEED);
		}
		if(KeyDown['a'])
		{
			is_moving = true;
			//camera.sidestepLeft(MOVEMENT_SPEED);
			camera.turnLeft(LOOK_SPEED);
		}
		if(KeyDown['s'])
		{
			is_moving = true;
			camera.moveBackward(MOVEMENT_SPEED);
		}
		if(KeyDown['d'])
		{
			is_moving = true;
			//camera.sidestepRight(MOVEMENT_SPEED);
			camera.turnRight(LOOK_SPEED);
		}
		/*if(KeyDown['r'])
		{
			camera.m_position.y += MOVEMENT_SPEED;
		}
		if(KeyDown['f'])
		{
			camera.m_position.y -= MOVEMENT_SPEED;
		}*/

		//if(is_moving)
		//{
		//	bob += 0.3;
		//	camera.m_position.y += 0.02*sin(bob);
		//}

		//get the camera's end position
		end = camera.m_position;

		//find the collision in each direction (slow, ugly) -- BUT IT SEEMS TO WORK!
		//I do have plans to optimize this
		VECTOR3D colx = bsp.getCollision(start, VECTOR3D(end.x, start.y, start.z));
		VECTOR3D coly = bsp.getCollision(start, VECTOR3D(start.x, end.y, start.z));
		VECTOR3D colz = bsp.getCollision(start, VECTOR3D(start.x, start.y, end.z));

		//and set the camera position based on the individual collision
		camera.m_position.x = colx.x;
		camera.m_position.y = coly.y;
		camera.m_position.z = colz.z;

		//change the look direction depending on the mouse position
		//camera.turnRight((mpos_x - W/2) / LOOK_SPEED);
		//camera.lookUp((mpos_y - H/2) / LOOK_SPEED);

		//set the mouse postion to the center of the screen
		//this allows the user to keep panning the view around
		//without having the cursor hit the edge of the screen
		glutWarpPointer(W/2, H/2);

		//reset the movement timer
		timer.elapse();
	}

	if(is_moving && !was_moving)
	{
		//gun->setAnimation("TORSO_STAND", "LEGS_WALK");
		was_moving = true;
	}

	if(!is_moving && was_moving)
	{
		//gun->setAnimation("TORSO_STAND", "LEGS_IDLE");
		was_moving = false;
	}

	//continuously redraw the screen
	glutPostRedisplay();
}

//**********************************************************************************
//  redraw
//
//  applys the camera transformation,
//  calculates the visible set of faces in the BSP, draws the BSP,
//  calculates the visible set of objects in the scenegraph, and draws the scenegraph
//**********************************************************************************
void redraw(void)
{
	//Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	//reset modelview matrix
        glLoadIdentity();

	//update the frustum using an identity modelview matrix because the AABB's are in eye space
	frustum.Update();

	//now we can update the visible set of nodes in the scenegraph
	root->calculateVisibleNodes(frustum);

	//apply our camera transformation
        glRotatef(camera.m_pitch, 1.0f, 0.0f, 0.0f);
        glRotatef(camera.m_yaw, 0.0f, 1.0f, 0.0f);
        glTranslatef(-camera.m_position.x, -camera.m_position.y, -camera.m_position.z);
        frustum.Update();

	//determine the visible set of faces in the BSP
        bsp.CalculateVisibleFaces(camera.m_position, frustum);

	//save all GL attributes
        glPushAttrib(GL_ALL_ATTRIB_BITS);

	//draw the world, which uses back face culling
	glDisable(GL_LIGHTING);
	glCullFace(GL_BACK);
	glPushMatrix();
	bsp.Draw();
	glPopMatrix();

	//draw the scene graph models, which use front face culling
	glCullFace(GL_FRONT);

	//update all of the AABBs in the scenegraph
	glPushMatrix();
	root->computeBoundingBox();
	glPopMatrix();

	//draw the visible nodes
	glPushMatrix();
	root->draw(camera.m_position);
	glPopMatrix();

	//show messages
	if(paused)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0, 0.0, 0.0);
		output(W/2.0 - 9.0*strlen("GAME IS PAUSED.") / 2.0 , H/2.0, "GAME IS PAUSED.", GLUT_BITMAP_9_BY_15);
	}

	//debug messages
	if(debug)
	{
		//msg.clear();
		char cam_pos[128];
		//sprintf(cam_pos, "\nCamera position: (%.1f, %.1f, %.1f)", camera.m_position.x, camera.m_position.y, camera.m_position.z);
		sprintf(cam_pos, "\nCamera direction: (%.1f, %.1f, %.1f)", camera.m_direction.x, camera.m_direction.y, camera.m_direction.z);
		//msg.append(cam_pos);

	/*	if(select_node != NULL)
		{
			msg.append("\nCurrent selection is ");
			msg.append(select_node->m_name);
			msg.append(".");
		}
		if(model->visible())
		{
			//msg.append("\nLara is visible.");
		}
		else
		{
			//msg.append("\nLara is not visible.");
		}*/
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0, 0.0, 0.0);
		output(10.0, 20.0, cam_pos, GLUT_BITMAP_9_BY_15);
	}


	if(is_attacking)
	{
	/*	//cerr << "Attack!" << endl;
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
		glColor4f(1.0, 0.0, 0.0, 0.5);

		glPointSize(5.0);
		glBegin(GL_LINES);

		VECTOR3D shot_start = camera.m_position + camera.m_binormal * 0.1;

		VECTOR3D shot_end = camera.m_position - camera.m_direction * 20.0;
		bsp.TraceRay(shot_start, shot_end);
		shot_end = bsp.outputEnd;

		glVertex3f(shot_start.x, shot_start.y, shot_start.z);
		glVertex3f(shot_end.x, shot_end.y, shot_end.z);
		//glVertex3f(shot_end.x, shot_end.y, shot_end.z);
		//glVertex3f(shot_start.x, shot_start.y, 0.0 + shot_start.z);



		glEnd();
		glBegin(GL_POINTS);
		glVertex3f(shot_end.x, shot_end.y, shot_end.z);
		glEnd();*/
	}


	//restore attributes
        glPopAttrib();

	//swap the buffers
	glutSwapBuffers();
}

//**********************************************************************************
//  mouse
//
//  handles mouse clicks
//**********************************************************************************
void mouse(int button, int state, int x, int y)
{
	if(!is_attacking && state == GLUT_DOWN)
	{
		//gun->setAnimation("TORSO_ATTACK", "LEGS_IDLE");
		is_attacking = true;
	}

	if(is_attacking && state == GLUT_UP)
	{
		was_moving = false;
		//gun->setAnimation("TORSO_STAND", "LEGS_IDLE");
		is_attacking = false;
	}
}

//**********************************************************************************
//  motion
//
//  handles moving the mouse when buttons are down
//**********************************************************************************
void motion(int x, int y)
{
	//save the mouse position for mouse looking
	mpos_x = x;
	mpos_y = y;
}


//**********************************************************************************
//  passive_motion
//
//  handles moving the mouse
//**********************************************************************************
void passive_motion(int x, int y)
{
	//save the mouse position for mouse looking
	mpos_x = x;
	mpos_y = y;
}

//**********************************************************************************
//  regular_keys
//
//  upon a key-down event, flags the key as being pressed
//**********************************************************************************
void regular_keys(unsigned char theKey, int mouseX, int mouseY)
{
	KeyDown[theKey] = 1;

	switch(theKey)
	{
		case 27:
			usleep(100);
			if(paused)
			{
				//halt gameplay
				unpauseGame();
			}
			else
			{
				//resume gameplay
				pauseGame();
			}
		break;
		case 'z':
			usleep(100);
			if(debug)
				debug = false;
			else
				debug = true;

		break;
	}

	//if a node is selected
	if(select_node ==NULL)
		return;

	if(select_node->isType("TransformNode"))
	{
		TransformNode * tfnode = (TransformNode *)select_node;
		VECTOR3D rot = tfnode->getRotation();

		switch (theKey)
		{
			case ',':
				usleep(100);
				tfnode->setRotation(rot.x, rot.y - 5.1, rot.z);
				break;
			case '.':
				usleep(100);
				tfnode->setRotation(rot.x, rot.y + 5.1, rot.z);
				break;
			default:
				break;
		}
	}
	glutPostRedisplay();
}

//**********************************************************************************
//  regular_up
//
//  upon a key-up event, flags the key as not being pressed
//**********************************************************************************
void regular_up(unsigned char theKey, int mouseX, int mouseY)
{
	KeyDown[theKey] = 0;
	return;
}



//**********************************************************************************
//  special_keys
//
//  this function responds to the arrow keys being pressed
//**********************************************************************************
void special_keys(int theKey, int mouseX, int mouseY)
{
	if(select_node ==NULL)
		return;

	if(select_node->isType("TransformNode"))
	{
		TransformNode * tfnode = (TransformNode *)select_node;
		VECTOR3D trans = tfnode->getTranslation();

		usleep(100);
		switch (theKey)
		{
			case GLUT_KEY_UP:

				tfnode->setTranslation(trans.x + 1, trans.y, trans.z);
				break;
			case GLUT_KEY_DOWN:
				tfnode->setTranslation(trans.x - 1, trans.y, trans.z);
				break;
			case GLUT_KEY_LEFT:
				tfnode->setTranslation(trans.x, trans.y, trans.z - 1);
				break;
			case GLUT_KEY_RIGHT:
				tfnode->setTranslation(trans.x, trans.y, trans.z + 1);
				break;
			default:
				break;
		}
	}
}



//**********************************************************************************
//  pause
//
//  halts gameplay, makes the cursor visible, and returns cursor control to the user
//**********************************************************************************
void pauseGame(void)
{
 	glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

	//enable menus
	glutSetMenu(selection_menu);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
	glutSetMenu(view_options_menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	//turn off animation
	root->setDrawMode(textures, lighting, bounding_boxes, light_positions, false);

	//mark the game as paused
	paused = true;
}

//**********************************************************************************
//  unpause
//
//  resumes gameplay and returns cursor control to the game
//**********************************************************************************
void unpauseGame(void)
{
	//unselect the current node
	select_node = NULL;

	//hide the cursor
	glutSetCursor(GLUT_CURSOR_NONE);

	//disable menus
	glutDetachMenu(GLUT_MIDDLE_BUTTON);
	glutDetachMenu(GLUT_RIGHT_BUTTON);

	//turn animation on
	//root->setDrawMode(textures, lighting, bounding_boxes, light_positions, true);

	//mark the game as unpaused
	paused = false;
}



