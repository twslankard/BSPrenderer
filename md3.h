//***********************************************************************
//
//  md3.h
//
//  MOST of this file is from the gametutorials.com MD3 animation tutorial.
//  However, I've added and changed a few things.
//  I had to modify a large portion of the animation code.
//  Lighting wasn't being used in the tutorial, so I had to convert the normals
//  stored in the files from spherical coordinates to cartesian coordinates.
//  Also I changed a few things around to allow instancing.
//
//***********************************************************************

#ifndef _MD3_H
#define _MD3_H

#define MAX_BOUNDING_BOXES 4
#define MAX_TEXTURES 100
#include <vector>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include "VECTOR2D.h"
#include "VECTOR3D.h"
#include "cquaternion.h"
#include "t3dmodel.h"
#include "boundingbox.h"
#include "Timer.h"

// These defines are used to pass into GetModel(), which is member function of CModelMD3

#define kLower	0			// This stores the ID for the legs model
#define kUpper	1			// This stores the ID for the torso model
#define kHead	2			// This stores the ID for the head model
#define kWeapon	3			// This stores the ID for the weapon model

// This enumeration stores all the animations in order from the config file (.cfg)
typedef enum
{
	// If one model is set to one of the BOTH_* animations, the other one should be too,
	// otherwise it looks really bad and confusing.

	BOTH_DEATH1 = 0,		// The first twirling death animation
	BOTH_DEAD1,				// The end of the first twirling death animation
	BOTH_DEATH2,			// The second twirling death animation
	BOTH_DEAD2,				// The end of the second twirling death animation
	BOTH_DEATH3,			// The back flip death animation
	BOTH_DEAD3,				// The end of the back flip death animation

	// The next block is the animations that the upper body performs

	TORSO_GESTURE,			// The torso's gesturing animation
	
	TORSO_ATTACK,			// The torso's attack1 animation
	TORSO_ATTACK2,			// The torso's attack2 animation

	TORSO_DROP,				// The torso's weapon drop animation
	TORSO_RAISE,			// The torso's weapon pickup animation

	TORSO_STAND,			// The torso's idle stand animation
	TORSO_STAND2,			// The torso's idle stand2 animation

	// The final block is the animations that the legs perform

	LEGS_WALKCR,			// The legs's crouching walk animation
	LEGS_WALK,				// The legs's walk animation
	LEGS_RUN,				// The legs's run animation
	LEGS_BACK,				// The legs's running backwards animation
	LEGS_SWIM,				// The legs's swimming animation
	
	LEGS_JUMP,				// The legs's jumping animation
	LEGS_LAND,				// The legs's landing animation

	LEGS_JUMPB,				// The legs's jumping back animation
	LEGS_LANDB,				// The legs's landing back animation

	LEGS_IDLE,				// The legs's idle stand animation
	LEGS_IDLECR,			// The legs's idle crouching animation

	LEGS_TURN,				// The legs's turn animation

	MAX_ANIMATIONS			// The define for the maximum amount of animations
} eAnimations;




// This class handles all of the main loading and drawing code
// I've made a few changes to it to account for instancing,
// as well as calculating the AABBs of the individual model components
class CLoadMD3
{

public:

	// This inits the data members
	CLoadMD3();								

	// This is the function that you call to load the MD3 model
	bool ImportMD3(t3DModel *pModel, char *strFileName);

	// This loads a model's .skin file
	bool LoadSkin(t3DModel *pModel, char * strSkin);

	// This loads a weapon's .shader file
	bool LoadShader(t3DModel *pModel, char * strShader);

private:


	// This reads in the data from the MD3 file and stores it in the member variables,
	// later to be converted to our cool structures so we don't depend on Quake3 stuff.
	void ReadMD3Data(t3DModel *pModel);

	// This converts the member variables to our pModel structure, and takes the model
	// to be loaded and the mesh header to get the mesh info.
	void ConvertDataStructures(t3DModel *pModel, tMd3MeshInfo meshHeader);

	// This frees memory and closes the file
	void CleanUp();
	
	// Member Variables		

	// The file pointer
	FILE *m_FilePointer;

	tMd3Header			m_Header;			// The header data

	tMd3Skin			*m_pSkins;			// The skin name data (not used)
	tMd3TexCoord			*m_pTexCoords;		// The texture coordinates
	tMd3Face			*m_pTriangles;		// Face/Triangle data
	tMd3Triangle			*m_pVertices;		// Vertex/UV indices
	tMd3Bone			*m_pBones;			// This stores the bone data (not used)
};

// This is our model class that we use to load and draw and free the Quake3 characters
class CModelMD3
{

public:

	Misc::Timer m_animation_timer;

	// These our our init and deinit() C++ functions (Constructor/Deconstructor)
	CModelMD3();
	~CModelMD3();
	
	// This loads the model from a path and name prefix.   It takes the path and
	// model name prefix to be added to _upper.md3, _lower.md3 or _head.md3.
	bool LoadModel(char * strPath, char * strModel);

	// This loads the weapon and takes the same path and model name to be added to .md3
	bool LoadWeapon(char * strPath, char * strModel);

	// This links a model to another model (pLink) so that it's the parent of that child.
	// The strTagName is the tag, or joint, that they will be linked at (I.E. "tag_torso").
	void LinkModel(t3DModel *pModel, t3DModel *pLink, char * strTagName);

	// This takes a string of an animation and sets the torso animation accordingly
	void SetTorsoAnimation(char * strAnimation);

	// This takes a string of an animation and sets the legs animation accordingly
	void SetLegsAnimation(char * strAnimation);

	// This renders the character to the screen
	void DrawModel();

	//tws -- calculates the AABBs
	void calculateBoundingBox();
	void calculateLinkBoundingBox(t3DModel * pModel);
	void calculateModelBoundingBox(t3DModel * pModel);
	
	// This frees the character's data
	void DestroyModel(t3DModel *pModel);

	// This returns a pointer to a .md3 model in the character (kLower, kUpper, kHead, kWeapon)
	t3DModel *GetModel(int whichPart);

	//tws 25.4 -- enableds or disables textures, lighting, and bounding boxes
	void setDrawMode(bool textures, bool lighting, bool bounding_boxes, bool animation);		//TWS

	//tws 27.4 -- AABBs for the individual model components, one for head, lower, upper, and weapon
	int m_current_box;
	int m_num_boxes;
	BoundingBox m_boxes[MAX_BOUNDING_BOXES];

	//tws selection--really just makes the model BB's look right
	void select();
	void unselect();

	void reLink();

	// These are are models for the character's head and upper and lower body parts
	t3DModel m_Head;
	t3DModel m_Upper;
	t3DModel m_Lower;

	// This store the players weapon model (optional load)
	t3DModel m_Weapon;

private:

	// This loads the models textures with a given path
	void LoadModelTextures(t3DModel *pModel, char * strPath);

	// This loads the animation config file (.cfg) for the character
	bool LoadAnimations(char * strConfigFile);

	// This updates the models current frame of animation, and calls SetCurrentTime()
	void UpdateModel(t3DModel *pModel);

	// This sets the lastTime, t, and the currentFrame of the models animation when needed
	void SetCurrentTime(t3DModel *pModel);

	// This recursively draws the character models, starting with the lower.md3 model
	void DrawLink(t3DModel *pModel);

	// This a md3 model to the screen (not the whole character)
	void RenderModel(t3DModel *pModel);

	// Member Variables

	// This stores the texture array for each of the textures assigned to this model
	GLuint m_Textures[MAX_TEXTURES];

	// This stores a list of all the names of the textures that have been loaded.
	std::vector<std::string> strTextures;




};


#endif
