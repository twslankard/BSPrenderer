//***********************************************************************
//
//  md3.cpp
//
//  MOST of this file is from the gametutorials.com MD3 animation tutorial.
//  However, I've added and changed a few things.
//  I had to modify a large portion of the animation code.
//  Lighting wasn't being used in the tutorial, so I had to convert the normals
//  stored in the files from spherical coordinates to cartesian coordinates.
//  Also I changed a few things around to allow instancing.
//
//***********************************************************************

#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "image.h"
#include "md3.h"
#include "Timer.h"

bool IsInString(std::string strString, std::string strSubString)
{
	// Make sure both of these strings are valid, return false if any are empty
	if(strString.length() <= 0 || strSubString.length() <= 0) return false;

	// grab the starting index where the sub string is in the original string
	unsigned int index = strString.find(strSubString);

	// Make sure the index returned was valid
	if(index >= 0 && index < strString.length())
		return true;

	// The sub string does not exist in strString.
	return false;
}


CModelMD3::CModelMD3()
{
	// Here we initialize all our mesh structures for the character
	memset(&m_Head,  0, sizeof(t3DModel));
	memset(&m_Upper, 0, sizeof(t3DModel));
	memset(&m_Lower, 0, sizeof(t3DModel));
	memset(&m_Weapon, 0, sizeof(t3DModel));

	//tws set the default draw mode
	setDrawMode(true, true, false, true);
	unselect();
	m_animation_timer.elapse();
}

CModelMD3::~CModelMD3()
{
	// Here we free all of the meshes in our model
	DestroyModel(&m_Head);
	DestroyModel(&m_Upper);
	DestroyModel(&m_Lower);
	DestroyModel(&m_Weapon);


}


void CModelMD3::DestroyModel(t3DModel *pModel)
{
	// Go through all the objects in the model
	for(int i = 0; i < pModel->numOfObjects; i++)
	{
		// Free the faces, normals, vertices, and texture coordinates.
		if(pModel->pObject[i]->pFaces)		delete [] pModel->pObject[i]->pFaces;		//tws 27.4 - 3
		if(pModel->pObject[i]->pNormals)	delete [] pModel->pObject[i]->pNormals;		//tws 27.4 - 3
		if(pModel->pObject[i]->pVerts)		delete [] pModel->pObject[i]->pVerts;		//tws 27.4 - 3
		if(pModel->pObject[i]->pTexVerts)	delete [] pModel->pObject[i]->pTexVerts;	//tws 27.4 - 3
	}

	// Free the tags associated with this model
	if(pModel->pTags)		delete [] pModel->pTags;

	// Free the links associated with this model (We use free because we used malloc())
	if(pModel->pLinks)		delete [] pModel->pLinks;
}


t3DModel *CModelMD3::GetModel(int whichPart)
{
	// Return the legs model if desired
	if(whichPart == kLower) 
		return &m_Lower;

	// Return the torso model if desired
	if(whichPart == kUpper) 
		return &m_Upper;

	// Return the head model if desired
	if(whichPart == kHead) 
		return &m_Head;

	// Return the weapon model
	return &m_Weapon;
}



bool CModelMD3::LoadModel(char * strPath, char * strModel)
{
	char strLowerModel[255];	// This stores the file name for the lower.md3 model
	char strUpperModel[255];	// This stores the file name for the upper.md3 model
	char strHeadModel[255];	// This stores the file name for the head.md3 model
	char strLowerSkin[255];	// This stores the file name for the lower.md3 skin
	char strUpperSkin[255];	// This stores the file name for the upper.md3 skin
	char strHeadSkin[255];	// This stores the file name for the head.md3 skin
	CLoadMD3 loadMd3;				// This object allows us to load each .md3 and .skin file

	// Make sure valid path and model names were passed in
	if(!strPath || !strModel) return false;

	// Store the correct files names for the .md3 and .skin file for each body part.
	// We concatinate this on top of the path name to be loaded from.
	sprintf(strLowerModel, "%s/lower.md3", strPath);
	sprintf(strUpperModel, "%s/upper.md3", strPath);
	sprintf(strHeadModel,  "%s/head.md3",  strPath);
	
	// Get the skin file names with their path
	sprintf(strLowerSkin, "%s/lower_default.skin", strPath);
	sprintf(strUpperSkin, "%s/upper_default.skin", strPath);
	sprintf(strHeadSkin,  "%s/head_default.skin",  strPath);
	
	// Load the head mesh (*_head.md3) and make sure it loaded properly
	if(!loadMd3.ImportMD3(&m_Head,  strHeadModel))
	{
		// Display an error message telling us the file could not be found
		//MessageBox(g_hWnd, "Unable to load the HEAD model!", "Error", MB_OK);
		std::cerr << "Unable to load the HEAD model!" << std::endl;
		return false;
	}

	// Load the upper mesh (*_head.md3) and make sure it loaded properly
	if(!loadMd3.ImportMD3(&m_Upper, strUpperModel))
	{
		// Display an error message telling us the file could not be found
		//MessageBox(g_hWnd, "Unable to load the UPPER model!", "Error", MB_OK);
		std::cerr << "Unable to load the UPPER model!" << std::endl;
		return false;
	}

	// Load the lower mesh (*_lower.md3) and make sure it loaded properly
	if(!loadMd3.ImportMD3(&m_Lower, strLowerModel))
	{
		// Display an error message telling us the file could not be found
		//MessageBox(g_hWnd, "Unable to load the LOWER model!", "Error", MB_OK);
		std::cerr << "Unable to load the LOWER model!" << std::endl;
		return false;
	}

	// Load the lower skin (*_upper.skin) and make sure it loaded properly
	if(!loadMd3.LoadSkin(&m_Lower, strLowerSkin))
	{
		// Display an error message telling us the file could not be found
		//MessageBox(g_hWnd, "Unable to load the LOWER skin!", "Error", MB_OK);
		std::cerr << "Unable to load the LOWER skin!" << std::endl;
		return false;
	}

	// Load the upper skin (*_upper.skin) and make sure it loaded properly
	if(!loadMd3.LoadSkin(&m_Upper, strUpperSkin))
	{
		// Display an error message telling us the file could not be found
		//MessageBox(g_hWnd, "Unable to load the UPPER skin!", "Error", MB_OK);
		std::cerr << "Unable to load the UPPER SKIN!" << std::endl;
		return false;
	}

	// Load the head skin (*_head.skin) and make sure it loaded properly
	if(!loadMd3.LoadSkin(&m_Head,  strHeadSkin))
	{
		// Display an error message telling us the file could not be found
		//MessageBox(g_hWnd, "Unable to load the HEAD skin!", "Error", MB_OK);
		std::cerr << "Unable to load the HEAD skin!" << std::endl;
		return false;
	}

	// Load the lower, upper and head textures.
	LoadModelTextures(&m_Lower, strPath);
	LoadModelTextures(&m_Upper, strPath);
	LoadModelTextures(&m_Head,  strPath);



	// We added to this function the code that loads the animation config file

	// This stores the file name for the .cfg animation file
	char strConfigFile[255];

	// Add the path and file name prefix to the animation.cfg file
	sprintf(strConfigFile,  "%s/animation.cfg",  strPath);

	// Load the animation config file (*_animation.config) and make sure it loaded properly
	if(!LoadAnimations(strConfigFile))
	{
		// Display an error message telling us the file could not be found
//		MessageBox(g_hWnd, "Unable to load the Animation Config File!", "Error", MB_OK);
		std::cerr << "Unable to load the animation config file '" << strConfigFile << "'." << std::endl;
		return false;
	}

	// Link the lower body to the upper body when the tag "tag_torso" is found in our tag array
	LinkModel(&m_Lower, &m_Upper, "tag_torso");

	// Link the upper body to the head when the tag "tag_head" is found in our tag array
	LinkModel(&m_Upper, &m_Head, "tag_head");

	// The character was loaded correctly so return true
	return true;


}


void CModelMD3::reLink()
{
	int i;

	// Initilialize our link pointers to NULL
	for (i = 0; i < m_Lower.numOfTags; i++)
		m_Lower.pLinks[i] = NULL;

	// Initilialize our link pointers to NULL
	for (i = 0; i < m_Upper.numOfTags; i++)
		m_Upper.pLinks[i] = NULL;

	// Initilialize our link pointers to NULL
	for (i = 0; i < m_Head.numOfTags; i++)
		m_Head.pLinks[i] = NULL;

	// Initilialize our link pointers to NULL
	for (i = 0; i < m_Weapon.numOfTags; i++)
		m_Weapon.pLinks[i] = NULL;

	// Link the lower body to the upper body when the tag "tag_torso" is found in our tag array
	LinkModel(&m_Lower, &m_Upper, "tag_torso");

	// Link the upper body to the head when the tag "tag_head" is found in our tag array
	LinkModel(&m_Upper, &m_Head, "tag_head");

	// Link the weapon to the model's hand that has the weapon tag
	LinkModel(&m_Upper, &m_Weapon, "tag_weapon");

	return;

}


bool CModelMD3::LoadWeapon(char * strPath, char * strModel)
{
	char strWeaponModel[255]  = {0};	// This stores the file name for the weapon model
	char strWeaponShader[255] = {0};	// This stores the file name for the weapon shader.
	CLoadMD3 loadMd3;					// This object allows us to load the.md3 and .shader file

	// Make sure the path and model were valid, otherwise return false
	if(!strPath || !strModel) return false;

	// Concatenate the path and model name together
	sprintf(strWeaponModel, "%s.md3", strModel);

	// Load the weapon mesh (*.md3) and make sure it loaded properly
	if(!loadMd3.ImportMD3(&m_Weapon,  strWeaponModel))
	{
		// Display the error message that we couldn't find the weapon MD3 file and return false
//		MessageBox(g_hWnd, "Unable to load the WEAPON model!", "Error", MB_OK);
std::cerr << "Unable to load the weapon model!" << std::endl;
		return false;
	}

	// Add the path, file name and .shader extension together to get the file name and path
	sprintf(strWeaponShader, "%s.shader", strModel);

	// Load our textures associated with the gun from the weapon shader file
	if(!loadMd3.LoadShader(&m_Weapon, strWeaponShader))
	{
		// Display the error message that we couldn't find the shader file and return false
	//	MessageBox(g_hWnd, "Unable to load the SHADER file!", "Error", MB_OK);
		std::cerr << "Unable to load the SHADER file!" << std::endl;
		return false;
	}

	// We should have the textures needed for each weapon part loaded from the weapon's
	// shader, so let's load them in the given path.
	LoadModelTextures(&m_Weapon, strPath);

	// Link the weapon to the model's hand that has the weapon tag
	LinkModel(&m_Upper, &m_Weapon, "tag_weapon");

	// The weapon loaded okay, so let's return true to reflect this
	return true;
}


void CModelMD3::LoadModelTextures(t3DModel *pModel, char * strPath)
{
	// Go through all the materials that are assigned to this model
	for(int i = 0; i < pModel->numOfMaterials; i++)
	{
		// Check to see if there is a file name to load in this material
		if(strlen(pModel->pMaterials[i].strFile) > 0)
		{
			// Create a boolean to tell us if we have a new texture to load
			bool bNewTexture = true;

			// Go through all the textures in our string list to see if it's already loaded
			for(unsigned int j = 0; j < strTextures.size(); j++)
			{
				// If the texture name is already in our list of texture, don't load it again.
				if(strcmp(pModel->pMaterials[i].strFile, strTextures[j].c_str()) == 0)
				{
					// We don't need to load this texture since it's already loaded
					bNewTexture = false;

					// Assign the texture index to our current material textureID.
					// This ID will them be used as an index into m_Textures[].
					pModel->pMaterials[i].texureId = j;
				}
			}

			// Make sure before going any further that this is a new texture to be loaded
			if(bNewTexture == false) continue;
			
			char strFullPath[255] = {0};

			// Add the file name and path together so we can load the texture
			sprintf(strFullPath, "%s/%s", strPath, pModel->pMaterials[i].strFile);
			//std::cerr << "Loading '" <<strFullPath << "'." << std::endl;

			// We pass in a reference to an index into our texture array member variable.
			// The size() function returns the current loaded texture count.  Initially
			// it will be 0 because we haven't added any texture names to our strTextures list.

			Image new_texture;

			glGenTextures(1, &m_Textures[strTextures.size()]);
			glBindTexture(GL_TEXTURE_2D, m_Textures[strTextures.size()]);

			//Load texture image
			if(!new_texture.Load(strFullPath))
			{

				std::cerr << "Couldn't load the texture due to an error." << std::endl;
				//return;
			}

			glTexImage2D(GL_TEXTURE_2D, 0, 3,
						new_texture.width, new_texture.height,
						0, new_texture.format, GL_UNSIGNED_BYTE, new_texture.data);

			//Set Parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Set the texture ID for this material by getting the current loaded texture count
			pModel->pMaterials[i].texureId = strTextures.size();

			// Now we increase the loaded texture count by adding the texture name to our
			// list of texture names.  Remember, this is used so we can check if a texture
			// is already loaded before we load 2 of the same textures.  Make sure you
			// understand what an STL vector list is.  We have a tutorial on it if you don't.
			strTextures.push_back(pModel->pMaterials[i].strFile);
		}
	}
}

bool CModelMD3::LoadAnimations(char * strConfigFile)
{
	// This function is given a path and name to an animation config file to load.
	// The implementation of this function is arbitrary, so if you have a better way
	// to parse the animation file, that is just as good.  Whatever works.
	// Basically, what is happening here, is that we are grabbing an animation line:
	//
	// "0	31	0	25		// BOTH_DEATH1"
	//
	// Then parsing it's values.  The first number is the starting frame, the next
	// is the frame count for that animation (endFrame would equal startFrame + frameCount),
	// the next is the looping frames (ignored), and finally the frames per second that
	// the animation should run at.  The end of this line is the name of the animation.
	// Once we get that data, we store the information in our tAnimationInfo object, then
	// after we finish parsing the file, the animations are assigned to each model.
	// Remember, that only the torso and the legs objects have animation.  It is important
	// to note also that the animation prefixed with BOTH_* are assigned to both the legs
	// and the torso animation list, hence the name "BOTH" :)

	// Create an animation object for every valid animation in the Quake3 Character
	tAnimationInfo animations[MAX_ANIMATIONS];

	// Open the config file
	std::ifstream fin(strConfigFile);

	// Here we make sure that the file was found and could be opened
	if( fin.fail() )
	{
		// Return an unsuccessful retrieval
		return false;
	}

	std::string strWord = "";				// This stores the current word we are reading in
	std::string strLine = "";				// This stores the current line we read in
	int currentAnim = 0;				// This stores the current animation count
	int torsoOffset = 0;				// The offset between the first torso and leg animation

	// Here we go through every word in the file until a numeric number if found.
	// This is how we know that we are on the animation lines, and past header info.
	// This of course isn't the most solid way, but it works fine.  It wouldn't hurt
	// to put in some more checks to make sure no numbers are in the header info.
	while( fin >> strWord)
	{
		// If the first character of the word is NOT a number, we haven't hit an animation line
		if(!isdigit( strWord[0] ))
		{
			// Store the rest of this line and go to the next one
			getline(fin, strLine);
			continue;
		}

		// If we get here, we must be on an animation line, so let's parse the data.
		// We should already have the starting frame stored in strWord, so let's extract it.

		// Get the number stored in the strWord string and create some variables for the rest
		int startFrame = atoi(strWord.c_str());
		int numOfFrames = 0, loopingFrames = 0, framesPerSecond = 0;
		
		// Read in the number of frames, the looping frames, then the frames per second
		// for this current animation we are on.
		fin >> numOfFrames >> loopingFrames >> framesPerSecond;

		// Initialize the current animation structure with the data just read in
		animations[currentAnim].startFrame		= startFrame;
		animations[currentAnim].endFrame		= startFrame + numOfFrames;
		animations[currentAnim].loopingFrames	= loopingFrames;
		animations[currentAnim].framesPerSecond = framesPerSecond;

		// Read past the "//" and read in the animation name (I.E. "BOTH_DEATH1").
		// This might not be how every config file is set up, so make sure.
		fin >> strLine >> strLine;

		// Copy the name of the animation to our animation structure
		strcpy(animations[currentAnim].strName, strLine.c_str());

		// If the animation is for both the legs and the torso, add it to their animation list
		if(IsInString(strLine, "BOTH"))
		{
			// Add the animation to each of the upper and lower mesh lists
			m_Upper.pAnimations.push_back(animations[currentAnim]);
			m_Lower.pAnimations.push_back(animations[currentAnim]);
		}
		// If the animation is for the torso, add it to the torso's list
		else if(IsInString(strLine, "TORSO"))
		{
			m_Upper.pAnimations.push_back(animations[currentAnim]);
		}
		// If the animation is for the legs, add it to the legs's list
		else if(IsInString(strLine, "LEGS"))
		{	
			// Because I found that some config files have the starting frame for the
			// torso and the legs a different number, we need to account for this by finding
			// the starting frame of the first legs animation, then subtracting the starting
			// frame of the first torso animation from it.  For some reason, some exporters
			// might keep counting up, instead of going back down to the next frame after the
			// end frame of the BOTH_DEAD3 anim.  This will make your program crash if so.
			
			// If the torso offset hasn't been set, set it
			if(!torsoOffset)
				torsoOffset = animations[LEGS_WALKCR].startFrame - animations[TORSO_GESTURE].startFrame;

			// Minus the offset from the legs animation start and end frame.
			animations[currentAnim].startFrame -= torsoOffset;
			animations[currentAnim].endFrame -= torsoOffset;

			// Add the animation to the list of leg animations
			m_Lower.pAnimations.push_back(animations[currentAnim]);
		}
	
		// Increase the current animation count
		currentAnim++;
	}	

	// Store the number if animations for each list by the STL vector size() function
	m_Lower.numOfAnimations = m_Lower.pAnimations.size();
	m_Upper.numOfAnimations = m_Upper.pAnimations.size();
	m_Head.numOfAnimations = m_Head.pAnimations.size();
	m_Weapon.numOfAnimations = m_Head.pAnimations.size();

	// Return a success
	return true;
}


void  CModelMD3::LinkModel(t3DModel *pModel, t3DModel *pLink, char * strTagName)
{
	// Make sure we have a valid model, link and tag name, otherwise quit this function
	if(!pModel || !pLink || !strTagName)
	{
		std::cerr << "Model link error!" << std::endl;
		return;
	}

	// Go through all of our tags and find which tag contains the strTagName, then link'em
	for(int i = 0; i < pModel->numOfTags; i++)
	{
		// If this current tag index has the tag name we are looking for
		if(strcmp(pModel->pTags[i].strName, strTagName) == 0)
		{
			// Link the model's link index to the link (or model/mesh) and return
			pModel->pLinks[i] = pLink;

			return;
		}
	}
}

void CModelMD3::UpdateModel(t3DModel *pModel)
{
	// Initialize a start and end frame, for models with no animation
	int startFrame = 0;
	int endFrame   = 1;

	// This function is used to keep track of the current and next frames of animation
	// for each model, depending on the current animation.  Some models down have animations,
	// so there won't be any change.

	// Here we grab the current animation that we are on from our model's animation list
	tAnimationInfo *pAnim = &(pModel->pAnimations[pModel->currentAnim]);

	// If there is any animations for this model
	if(pModel->numOfAnimations)
	{
		// Set the starting and end frame from for the current animation
		startFrame = pAnim->startFrame;
		endFrame   = pAnim->endFrame;
	}

	// This gives us the next frame we are going to.  We mod the current frame plus
	// 1 by the current animations end frame to make sure the next frame is valid.
	pModel->nextFrame = (pModel->currentFrame + 1) % endFrame;

	// If the next frame is zero, that means that we need to start the animation over.
	// To do this, we set nextFrame to the starting frame of this animation.
	if(pModel->nextFrame == 0) 
		pModel->nextFrame =  startFrame;

	// Next, we want to get the current time that we are interpolating by.  Remember,
	// if t = 0 then we are at the beginning of the animation, where if t = 1 we are at the end.
	// Anything from 0 to 1 can be thought of as a percentage from 0 to 100 percent complete.
	SetCurrentTime(pModel);
}


void CModelMD3::DrawModel()
{
	//set the current bounding store index to zero
	//This gets incremented in the draw model phase, where the BBs are calculated and stored
//	m_current_box = 0;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// Rotate the model to compensate for the z up orientation that the model was saved
	glRotatef(-90, 1, 0, 0);

	// Since we have animation now, when we draw the model the animation frames need
	// to be updated.  To do that, we pass in our lower and upper models to UpdateModel().
	// There is no need to pass in the head of weapon, since they don't have any animation.

	// Update the leg and torso animations
	UpdateModel(&m_Lower);
	UpdateModel(&m_Upper);

	// Draw the first link, which is the lower body.  This will then recursively go
	// through the models attached to this model and drawn them.
	DrawLink(&m_Lower);

	//save the number of bounding boxes calculated, so the node can use them
	//m_num_boxes = m_current_box - 1;

	glPopMatrix();
}

void CModelMD3::DrawLink(t3DModel *pModel)
{


	// Draw the current model passed in (Initially the legs)
	RenderModel(pModel);

	// Create some local variables to store all this crazy interpolation data
	CQuaternion qQuat, qNextQuat, qInterpolatedQuat;
	float *pMatrix, *pNextMatrix;
	float finalMatrix[16];

	// Now we need to go through all of this models tags and draw them.
	for(int i = 0; i < pModel->numOfTags; i++)
	{
		// Get the current link from the models array of links (Pointers to models)
		t3DModel *pLink = pModel->pLinks[i];

		// If this link has a valid address, let's draw it!
		if(pLink)
		{
			// To find the current translation position for this frame of animation, we times
			// the currentFrame by the number of tags, then add i.  This is similar to how
			// the vertex key frames are interpolated.
			VECTOR3D vPosition = pModel->pTags[pModel->currentFrame * pModel->numOfTags + i].vPosition;

			// Grab the next key frame translation position
			VECTOR3D vNextPosition = pModel->pTags[pModel->nextFrame * pModel->numOfTags + i].vPosition;
		
			// By using the equation: p(t) = p0 + t(p1 - p0), with a time t,
			// we create a new translation position that is closer to the next key frame.
			vPosition.x = vPosition.x + pModel->t * (vNextPosition.x - vPosition.x),
			vPosition.y	= vPosition.y + pModel->t * (vNextPosition.y - vPosition.y),
			vPosition.z	= vPosition.z + pModel->t * (vNextPosition.z - vPosition.z);

			// Now comes the more complex interpolation.  Just like the translation, we
			// want to store the current and next key frame rotation matrix, then interpolate
			// between the 2.

			// Get a pointer to the start of the 3x3 rotation matrix for the current frame
			pMatrix = &pModel->pTags[pModel->currentFrame * pModel->numOfTags + i].rotation[0][0];

			// Get a pointer to the start of the 3x3 rotation matrix for the next frame
			pNextMatrix = &pModel->pTags[pModel->nextFrame * pModel->numOfTags + i].rotation[0][0];

			// Now that we have 2 1D arrays that store the matrices, let's interpolate them

			// Convert the current and next key frame 3x3 matrix into a quaternion
			qQuat.CreateFromMatrix( pMatrix, 3);
			qNextQuat.CreateFromMatrix( pNextMatrix, 3 );

			// Using spherical linear interpolation, we find the interpolated quaternion
			qInterpolatedQuat = qQuat.Slerp(qQuat, qNextQuat, pModel->t);

			// Here we convert the interpolated quaternion into a 4x4 matrix
			qInterpolatedQuat.CreateMatrix( finalMatrix );
			
			// To cut out the need for 2 matrix calls, we can just slip the translation
			// into the same matrix that holds the rotation.  That is what index 12-14 holds.
			finalMatrix[12] = vPosition.x;
			finalMatrix[13] = vPosition.y;
			finalMatrix[14] = vPosition.z;

			// Start a new matrix scope
			glPushMatrix();

			// Finally, apply the rotation and translation matrix to the current matrix
			glMultMatrixf( finalMatrix );

			// Recursively draw the next model that is linked to the current one.
			// This could either be a body part or a gun that is attached to
			// the hand of the upper body model.
			DrawLink(pLink);

			// End the current matrix scope
			glPopMatrix();
		}
	}
}

//***************************************************************************************
//  SetCurrentTime
//
//  This is one of the functions that needed some fairly large changes.
//  The current time was being retrieved using a windows system call.
//  So I replaced the windows timer functions with Oliver Kreylos' Timer class.
//
//  I also fixed some of the ineffcient math by removing two or three floating point divisions.
//  The code also has a bug which allowed the interpolant pModel->t to be greater than 1.
//  I mediated the effects of this bug by adding the line pModel->t = t - 1.0f,
//  Which corrects the interpolant when the frame is advanced.
//
//  For this code to TRULY be correct, it should advance more than one frame if necessary.
//  At the present time, it advances no more than one frame per function call.
//  And depending on the system performance, this *might* be very bad.
//***************************************************************************************
void CModelMD3::SetCurrentTime(t3DModel *pModel)
{
	//return if animation is off
	if(!pModel->animation)
		return;

	 //only animate if the model has animations
	if(!pModel->pAnimations.size()) return;

	//get the current number of milliseconds
	float new_time = m_animation_timer.peekTime() * 1000.0f;

	//find the amount of time that has passed
	float elapsed_time = new_time - pModel->lastTime;

	//get the model's animation speed
	int animation_speed = pModel->pAnimations[pModel->currentAnim].framesPerSecond;

	//find the interpolant value
	float t = elapsed_time * animation_speed / 1000.0f;

	//if t > 1.0, then we must have passed the current frame.
	//so advance one frame and decrease t by one (so that it lies between 0 and 1).
	//The only time this will not work is when elapsed time >> 1000.0 / animation_speed, which means we have a crappy frame rate anyway.
	if(t > 1.0f)
	{
		//advance the current frame by one
                pModel->currentFrame = pModel->nextFrame;

                //reset the elapsed time
                pModel->lastTime = new_time;

		//set the model's interpolation value
		pModel->t = t - 1.0f;
	}
	else
	{
		//set the model's interpolation value
		pModel->t = t;
	}
}

void CModelMD3::RenderModel(t3DModel *pModel)
{
	float x, y, z;
	//float x2, y2, z2;
	float norm_x, norm_y, norm_z;

	// return if there are no objects to draw
	if(pModel->pObject.size() <= 0) return;

	// get the current modelview transformation.
	// We'll need it to find the eye coordinates
	// of the axis aligned bounding box for this model
//	GLfloat matrix[16];
//	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

	// initialize the bounding box to something really huge--we'll shrink it down to size during drawing
	// this is kind of a crappy shortcut, but we can fix it easily sometime later
	/*pModel->min_x = 1000000.0f;
	pModel->max_x = -1000000.0f;
	pModel->min_y = 1000000.0f;
	pModel->max_y = -1000000.0f;
	pModel->min_z = 1000000.0f;
	pModel->max_z = -1000000.0f;
*/
	// Go through all of the objects stored in this model
	int i = 0;
	for(i = 0; i < pModel->numOfObjects; i++)
	{
		// Get the current object that we are displaying
		t3DObject *pObject = pModel->pObject[i];			//tws 27.4 - 3

		// Find the current starting index for the current key frame we are on
		int currentIndex = pModel->currentFrame * pObject->numOfVerts;

		// Since we are interpolating, we also need the index for the next key frame
		int nextIndex = pModel->nextFrame * pObject->numOfVerts;

		//if lighting is supposed to be on, make sure it's enabled
		//otherwise we'll make sure it's disabled
		glColor3f(1.0, 1.0, 1.0);
		if(pModel->lighting == true)
		{
			glEnable(GL_LIGHTING);
		}
		else
		{
			glDisable(GL_LIGHTING);
		}

		//if textures are off, then draw wireframe triangles
		if(pModel->drawMode == false)
		{
			glDisable(GL_TEXTURE_2D);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			//bind the texture to use for this object
			if(pObject->bHasTexture)
			{
				// Turn on texture mapping
				glEnable(GL_TEXTURE_2D);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				// Grab the texture index from the materialID index into our material list
				int textureID = pModel->pMaterials[pObject->materialID].texureId;

				// Bind the texture index that we got from the material textureID
				glBindTexture(GL_TEXTURE_2D, m_Textures[textureID]);
			}
			else
			{
				// Turn off texture mapping
				glDisable(GL_TEXTURE_2D);
			}
		}

		// Start drawing our model triangles
		glBegin(GL_TRIANGLES);

			// Go through all of the faces (polygons) of the object and draw them
			for(int j = 0; j < pObject->numOfFaces; j++)
			{
				int whichVertex;

				// Go through each vertex of the triangle and draw it.
				for( whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					// Get the index for the current point in the face list
					int index = pObject->pFaces[j].vertIndex[whichVertex];

					// Make sure there is texture coordinates for this (%99.9 likelyhood)
					if(pObject->pTexVerts)
					{
						// Assign the texture coordinate to this vertex
						glTexCoord2f(pObject->pTexVerts[ index ].x,
									 pObject->pTexVerts[ index ].y);
					}


					// Like in the MD2 Animation tutorial, we use linear interpolation
					// between the current and next point to find the point in between,
					// depending on the model's "t" (0.0 to 1.0).

					// Store the current and next frame's vertex by adding the current
					// and next index to the initial index given from the face data.
					VECTOR3D vPoint1 = pObject->pVerts[ currentIndex + index ];
					VECTOR3D vPoint2 = pObject->pVerts[ nextIndex + index];

					//TWS -- get the normals. The normals are in spherical coordinates

					VECTOR3D nPoint1 = pObject->pNormals[ currentIndex + index ];
					VECTOR3D nPoint2 = pObject->pNormals[ nextIndex + index];

					//interpolate the vertices
					x = vPoint1.x + pModel->t * (vPoint2.x - vPoint1.x);
					y = vPoint1.y + pModel->t * (vPoint2.y - vPoint1.y);
					z = vPoint1.z + pModel->t * (vPoint2.z - vPoint1.z);

					//interpolate the normals
					norm_x = nPoint1.x + pModel->t * (nPoint2.x - nPoint1.x);
					norm_y = nPoint1.y + pModel->t * (nPoint2.y - nPoint1.y);
					norm_z = nPoint1.z + pModel->t * (nPoint2.z - nPoint1.z);

					// By using the equation: p(t) = p0 + t(p1 - p0), with a time t,
					// we create a new vertex that is closer to the next key frame.
					glNormal3f(norm_x, norm_y, norm_z);
					glVertex3f(x, y, z);

/*					//multiply the vertex by the current transformation
					//to get the world coordinates
					x2 = matrix[0]*x + matrix[4]*y + matrix[8]*z + matrix[12];
					y2 = matrix[1]*x + matrix[5]*y + matrix[9]*z +  matrix[13];
					z2 = matrix[2]*x + matrix[6]*y + matrix[10]*z + matrix[14];

					//find the max and min world coordinates for the bounding box
					if(x2 > pModel->max_x)
						pModel->max_x = x2;
					if(x2 < pModel->min_x)
						pModel->min_x = x2;

					if(y2 > pModel->max_y)
						pModel->max_y = y2;
					if(y2 < pModel->min_y)
						pModel->min_y = y2;

					if(z2 > pModel->max_z)
						pModel->max_z = z2;
					if(z2 < pModel->min_z)
						pModel->min_z = z2;

*/
				}
			}

		// Stop drawing polygons
		glEnd();
	}

	//store the bounding box
/*	if(m_current_box < MAX_BOUNDING_BOXES)
	{
		m_boxes[m_current_box].setBoundingBox(
			pModel->min_x, pModel->min_y, pModel->min_z,
			pModel->max_x, pModel->max_y, pModel->max_z	);

		m_current_box ++;
	}*/

	//draw the bounding box if necessary
	if(pModel->bounding_box)
	{
		//give us an identity matrix--the AABB coordinates are relative to the world coordinates
		glPushMatrix();
		glLoadIdentity();

		//Don't cull the AABB faces, we want to see all of them
		glDisable(GL_CULL_FACE);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);



		pModel->drawAABB();

		//put the model transformation back
		glPopMatrix();

		//re-enable cull faces
		glEnable(GL_CULL_FACE);
	}

	return;
}


void CModelMD3::SetTorsoAnimation(char * strAnimation)
{
	// Go through all of the animations in this model
	for(int i = 0; i < m_Upper.numOfAnimations; i++)
	{
		// If the animation name passed in is the same as the current animation's name
		if(strcmp(m_Upper.pAnimations[i].strName, strAnimation) == 0)
		{
			// Set the legs animation to the current animation we just found and return
			m_Upper.currentAnim = i;
			m_Upper.currentFrame = m_Upper.pAnimations[m_Upper.currentAnim].startFrame;
			return;
		}
	}
}


void CModelMD3::SetLegsAnimation(char * strAnimation)
{
	// Go through all of the animations in this model
	for(int i = 0; i < m_Lower.numOfAnimations; i++)
	{
		// If the animation name passed in is the same as the current animation's name
		if( strcmp(m_Lower.pAnimations[i].strName, strAnimation) == 0 )
		{
			// Set the legs animation to the current animation we just found and return
			m_Lower.currentAnim = i;
			m_Lower.currentFrame = m_Lower.pAnimations[m_Lower.currentAnim].startFrame;
			return;
		}
	}
}

void CModelMD3::setDrawMode(bool textures, bool lighting, bool bounding_boxes, bool animation)
{
	m_Lower.drawMode = textures;
	m_Upper.drawMode = textures;
	m_Head.drawMode = textures;
	m_Weapon.drawMode = textures;


	m_Lower.lighting = lighting;
	m_Upper.lighting = lighting;
	m_Head.lighting = lighting;
	m_Weapon.lighting = lighting;


	m_Lower.bounding_box = bounding_boxes;
	m_Upper.bounding_box = bounding_boxes;
	m_Head.bounding_box = bounding_boxes;
	m_Weapon.bounding_box = bounding_boxes;

	m_Lower.animation = animation;
	m_Upper.animation = animation;
	m_Head.animation = animation;
	m_Weapon.animation = animation;
}



void CModelMD3::select(void)
{
	//std::cerr << "selecting model" << std::endl;
	m_Lower.m_selected = true;
	m_Upper.m_selected = true;
	m_Head.m_selected = true;
	m_Weapon.m_selected = true;
}

void CModelMD3::unselect(void)
{
	m_Lower.m_selected = false;
	m_Upper.m_selected = false;
	m_Head.m_selected = false;
	m_Weapon.m_selected = false;
}


CLoadMD3::CLoadMD3()
{
	// Here we initialize our structures to 0
	memset(&m_Header, 0, sizeof(tMd3Header));

	// Set the pointers to null
	m_pSkins=NULL;
	m_pTexCoords=NULL;
	m_pTriangles=NULL;
	m_pBones=NULL;
}


bool CLoadMD3::ImportMD3(t3DModel *pModel, char *strFileName)
{
	std::cerr << "Loading MD3 data from '" << strFileName << "'..." << std::endl;

	char strMessage[255] = {0};

	// Open the MD3 file in binary
	m_FilePointer = fopen(strFileName, "rb");

	// Make sure we have a valid file pointer (we found the file)
	if(!m_FilePointer) 
	{
		// Display an error message and don't load anything if no file was found
		sprintf(strMessage, "Unable to find the file: %s!", strFileName);
		std::cerr << strMessage << std::endl;
	//	MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	// Read the header data and store it in our m_Header member variable
	fread(&m_Header, 1, sizeof(tMd3Header), m_FilePointer);

	// Get the 4 character ID
	char *ID = m_Header.fileID;

	// The ID MUST equal "IDP3" and the version MUST be 15, or else it isn't a valid
	// .MD3 file.  This is just the numbers ID Software chose.

	// Make sure the ID == IDP3 and the version is this crazy number '15' or else it's a bad egg
	if((ID[0] != 'I' || ID[1] != 'D' || ID[2] != 'P' || ID[3] != '3') || m_Header.version != 15)
	{
		// Display an error message for bad file format, then stop loading
		sprintf(strMessage, "Invalid file format (Version not 15): %s!", strFileName);
		std::cerr << strMessage << std::endl;
	//	MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}
	
	// Read in the model and animation data
	ReadMD3Data(pModel);

	// Clean up after everything
	CleanUp();

	// Return a success
	return true;
}

void CLoadMD3::ReadMD3Data(t3DModel *pModel)
{
	//std::cerr << "Reading MD3 data..." << std::endl;

	int i = 0;

	// Here we allocate memory for the bone information and read the bones in.
	m_pBones = new tMd3Bone [m_Header.numFrames];
	fread(m_pBones, sizeof(tMd3Bone), m_Header.numFrames, m_FilePointer);

	// Since we don't care about the bone positions, we just free it immediately.
	// It might be cool to display them so you could get a visual of them with the model.

	// Free the unused bones
	delete [] m_pBones;

	// Next, after the bones are read in, we need to read in the tags.  Below we allocate
	// memory for the tags and then read them in.  For every frame of animation there is
	// an array of tags.
	pModel->pTags = new tMd3Tag [m_Header.numFrames * m_Header.numTags];
	fread(pModel->pTags, sizeof(tMd3Tag), m_Header.numFrames * m_Header.numTags, m_FilePointer);

	// Assign the number of tags to our model
	pModel->numOfTags = m_Header.numTags;
	
	// Now we want to initialize our links.  Links are not read in from the .MD3 file, so
	// we need to create them all ourselves.  We use a double array so that we can have an
	// array of pointers.  We don't want to store any information, just pointers to t3DModels.
	pModel->pLinks = new t3DModel*[m_Header.numTags];

	// Initilialize our link pointers to NULL
	for (i = 0; i < m_Header.numTags; i++)
		pModel->pLinks[i] = NULL;

	// Get the current offset into the file
	long meshOffset = ftell(m_FilePointer);

	// Create a local meshHeader that stores the info about the mesh
	tMd3MeshInfo meshHeader;

	// Go through all of the sub-objects in this mesh
	for (i = 0; i < m_Header.numMeshes; i++)
	{
		// Seek to the start of this mesh and read in it's header
		fseek(m_FilePointer, meshOffset, SEEK_SET);
		fread(&meshHeader, sizeof(tMd3MeshInfo), 1, m_FilePointer);

		// Here we allocate all of our memory from the header's information
		m_pSkins     = new tMd3Skin [meshHeader.numSkins];
		m_pTexCoords = new tMd3TexCoord [meshHeader.numVertices];
		m_pTriangles = new tMd3Face [meshHeader.numTriangles];
		m_pVertices  = new tMd3Triangle [meshHeader.numVertices * meshHeader.numMeshFrames];

		// Read in the skin information
		fread(m_pSkins, sizeof(tMd3Skin), meshHeader.numSkins, m_FilePointer);
		
		// Seek to the start of the triangle/face data, then read it in
		fseek(m_FilePointer, meshOffset + meshHeader.triStart, SEEK_SET);
		fread(m_pTriangles, sizeof(tMd3Face), meshHeader.numTriangles, m_FilePointer);

		// Seek to the start of the UV coordinate data, then read it in
		fseek(m_FilePointer, meshOffset + meshHeader.uvStart, SEEK_SET);
		fread(m_pTexCoords, sizeof(tMd3TexCoord), meshHeader.numVertices, m_FilePointer);

		// Seek to the start of the vertex/face index information, then read it in.
		fseek(m_FilePointer, meshOffset + meshHeader.vertexStart, SEEK_SET);
		fread(m_pVertices, sizeof(tMd3Triangle), meshHeader.numMeshFrames * meshHeader.numVertices, m_FilePointer);

		// Now that we have the data loaded into the Quake3 structures, let's convert them to
		// our data types like t3DModel and t3DObject.  That way the rest of our model loading
		// code will be mostly the same as the other model loading tutorials.
		ConvertDataStructures(pModel, meshHeader);

		// Free all the memory for this mesh since we just converted it to our structures
		delete [] m_pSkins;    
		delete [] m_pTexCoords;
		delete [] m_pTriangles;
		delete [] m_pVertices;   

		// Increase the offset into the file
		meshOffset += meshHeader.meshSize;
	}
}



void CLoadMD3::ConvertDataStructures(t3DModel *pModel, tMd3MeshInfo meshHeader)
{
	//std::cerr << "Creating model data structure..." << std::endl;

	int i = 0;

	// Increase the number of objects (sub-objects) in our model since we are loading a new one
	pModel->numOfObjects++;
		
	// Create a empty object structure to store the object's info before we add it to our list
	t3DObject * currentMesh = new t3DObject();										//tws

	// Copy the name of the object to our object structure
	strcpy(currentMesh->strName, meshHeader.strName);

	// Assign the vertex, texture coord and face count to our new structure
	currentMesh->numOfVerts   = meshHeader.numVertices;
	currentMesh->numTexVertex = meshHeader.numVertices;
	currentMesh->numOfFaces   = meshHeader.numTriangles;

	// Allocate memory for the vertices, texture coordinates and face data.
	// Notice that we multiply the number of vertices to be allocated by the
	// number of frames in the mesh.  This is because each frame of animation has a 
	// totally new set of vertices.  This will be used in the next animation tutorial.
	currentMesh->pVerts    = new VECTOR3D [currentMesh->numOfVerts * meshHeader.numMeshFrames];
	currentMesh->pNormals  = new VECTOR3D [currentMesh->numOfVerts * meshHeader.numMeshFrames];		//TWS
	currentMesh->pTexVerts = new VECTOR2D [currentMesh->numOfVerts];
	currentMesh->pFaces    = new tFace [currentMesh->numOfFaces];


	float x, y, z, length;
	float phi, theta;

	// Go through all of the vertices and assign them over to our structure
	for (i=0; i < currentMesh->numOfVerts * meshHeader.numMeshFrames; i++)
	{
		//scale down the model
		currentMesh->pVerts[i].x =  m_pVertices[i].vertex[0] / 64.0f;
		currentMesh->pVerts[i].y =  m_pVertices[i].vertex[1] / 64.0f;
		currentMesh->pVerts[i].z =  m_pVertices[i].vertex[2] / 64.0f;

		//tws -- convert unsigned int into radians
		phi = 2.0 * M_PI * m_pVertices[i].normal[0] / 255.0f;
		theta = 2.0 * M_PI * m_pVertices[i].normal[1] / 255.0f;

		//TWS -- convert normals from spherical coordinates
		x = sin(phi)*cos(theta);
		y = sin(phi)*sin(theta);
		z = cos(phi);

		//tws - normalize the normals
		length = sqrt(x*x + y*y + z*z);

		length *= 100.0;

		x /= length;
		y /= length;
		z /= length;

		//tws - store
		currentMesh->pNormals[i].x = x;
		currentMesh->pNormals[i].y = y;
		currentMesh->pNormals[i].z = z;
	}

	// Go through all of the uv coords and assign them over to our structure
	for (i=0; i < currentMesh->numTexVertex; i++)
	{
		// Since I changed the image to bitmaps, we need to negate the V ( or y) value.
		// This is because I believe that TARGA (.tga) files, which were originally used
		// with this model, have the pixels flipped horizontally.  If you use other image
		// files and your texture mapping is crazy looking, try deleting this negative.
		currentMesh->pTexVerts[i].x =  m_pTexCoords[i].textureCoord[0];
		currentMesh->pTexVerts[i].y = -m_pTexCoords[i].textureCoord[1];
	}

	// Go through all of the face data and assign it over to OUR structure
	for(i=0; i < currentMesh->numOfFaces; i++)
	{
		// Assign the vertex indices to our face data
		currentMesh->pFaces[i].vertIndex[0] = m_pTriangles[i].vertexIndices[0];
		currentMesh->pFaces[i].vertIndex[1] = m_pTriangles[i].vertexIndices[1];
		currentMesh->pFaces[i].vertIndex[2] = m_pTriangles[i].vertexIndices[2];

		// Assign the texture coord indices to our face data (same as the vertex indices)
		currentMesh->pFaces[i].coordIndex[0] = m_pTriangles[i].vertexIndices[0];
		currentMesh->pFaces[i].coordIndex[1] = m_pTriangles[i].vertexIndices[1];
		currentMesh->pFaces[i].coordIndex[2] = m_pTriangles[i].vertexIndices[2];
	}

	// Here we add the current object to our list object list
	pModel->pObject.push_back(currentMesh);			//tws 27.4 - 3
}

bool CLoadMD3::LoadSkin(t3DModel *pModel, char * strSkin)
{
	//std::cerr << "Loading model skin..." << std::endl;

	// Make sure valid data was passed in
	if(!pModel || !strSkin) return false;

	// Open the skin file
	std::ifstream fin(strSkin);

	// Make sure the file was opened
	if(fin.fail())
	{
		// Display the error message and return false
	//	MessageBox(NULL, "Unable to load skin!", "Error", MB_OK);
	std::cerr << "Unable to load skin!" << std::endl;
		return false;
	}

	// These 2 variables are for reading in each line from the file, then storing
	// the index of where the bitmap name starts after the last '/' character.
	std::string strLine = "";
	int textureNameStart = 0;

	// Go through every line in the .skin file
	while(fin >> strLine)
	{
		// Loop through all of our objects to test if their name is in this line
		for(int i = 0; i < pModel->numOfObjects; i++)
		{
			// Check if the name of this object appears in this line from the skin file
			if( IsInString(strLine, pModel->pObject[i]->strName) )				//tws 27.4 - 3
			{
				// To abstract the texture name, we loop through the string, starting
				// at the end of it until we find a '/' character, then save that index + 1.
				for(int j = strLine.length() - 1; j > 0; j--)
				{
					// If this character is a '/', save the index + 1
					if(strLine[j] == '/')
					{
						// Save the index + 1 (the start of the texture name) and break
						textureNameStart = j + 1;
						break;
					}	
				}

				// Create a local material info structure
				tMaterialInfo texture;

				// Copy the name of the file into our texture file name variable.
				strcpy(texture.strFile, &strLine[textureNameStart]);
				
				// The tile or scale for the UV's is 1 to 1 
				texture.uTile = texture.uTile = 1;

				// Store the material ID for this object and set the texture boolean to true
				pModel->pObject[i]->materialID = pModel->numOfMaterials;			//tws 27.4 - 3
				pModel->pObject[i]->bHasTexture = true;						//tws 27.4 - 3

				// Here we increase the number of materials for the model
				pModel->numOfMaterials++;

				// Add the local material info structure to our model's material list
				pModel->pMaterials.push_back(texture);
			}
		}
	}

	// Close the file and return a success
	fin.close();
	return true;
}


bool CLoadMD3::LoadShader(t3DModel *pModel, char * strShader)
{
	//std::cerr << "Loading model shader..." << std::endl;

	// Make sure valid data was passed in
	if(!pModel || !strShader) return false;

	// Open the shader file
	std::ifstream fin(strShader);

	// Make sure the file was opened
	if(fin.fail())
	{
		// Display the error message and return false
	//	MessageBox(NULL, "Unable to load shader!", "Error", MB_OK);
		return false;
	}

	// These variables are used to read in a line at a time from the file, and also
	// to store the current line being read so that we can use that as an index for the 
	// textures, in relation to the index of the sub-object loaded in from the weapon model.
	std::string strLine = "";
	int currentIndex = 0;
	
	// Go through and read in every line of text from the file
	while(getline(fin, strLine))
	{
		// Create a local material info structure
		tMaterialInfo texture;

		// Copy the name of the file into our texture file name variable
		strcpy(texture.strFile, strLine.c_str());
				
		// The tile or scale for the UV's is 1 to 1 
		texture.uTile = texture.uTile = 1;

		// Store the material ID for this object and set the texture boolean to true
		pModel->pObject[currentIndex]->materialID = pModel->numOfMaterials;		//tws 27.4 - 3
		pModel->pObject[currentIndex]->bHasTexture = true;				//tws 27.4 - 3

		// Here we increase the number of materials for the model
		pModel->numOfMaterials++;

		// Add the local material info structure to our model's material list
		pModel->pMaterials.push_back(texture);

		// Here we increase the material index for the next texture (if any)
		currentIndex++;
	}

	// Close the file and return a success
	fin.close();
	return true;
}


void CLoadMD3::CleanUp()
{
	// Close the current file pointer
	fclose(m_FilePointer);
}

void CModelMD3::calculateBoundingBox()
{
	//set the current bounding store index to zero
	//This gets incremented in the draw model phase, where the BBs are calculated and stored
	m_current_box = 0;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// Rotate the model to compensate for the z up orientation that the model was saved
	glRotatef(-90, 1, 0, 0);

	// Since we have animation now, when we draw the model the animation frames need
	// to be updated.  To do that, we pass in our lower and upper models to UpdateModel().
	// There is no need to pass in the head of weapon, since they don't have any animation.

	// Update the leg and torso animations
	UpdateModel(&m_Lower);
	UpdateModel(&m_Upper);

	// Draw the first link, which is the lower body.  This will then recursively go
	// through the models attached to this model and drawn them.
	calculateLinkBoundingBox(&m_Lower);

	//save the number of bounding boxes calculated, so the node can use them
	m_num_boxes = m_current_box - 1;

	glPopMatrix();
}

void CModelMD3::calculateLinkBoundingBox(t3DModel * pModel)
{

	// Draw the current model passed in (Initially the legs)
	calculateModelBoundingBox(pModel);

	// Create some local variables to store all this crazy interpolation data
	CQuaternion qQuat, qNextQuat, qInterpolatedQuat;
	float *pMatrix, *pNextMatrix;
	float finalMatrix[16];

	// Now we need to go through all of this models tags and draw them.
	for(int i = 0; i < pModel->numOfTags; i++)
	{
		// Get the current link from the models array of links (Pointers to models)
		t3DModel *pLink = pModel->pLinks[i];

		// If this link has a valid address, let's draw it!
		if(pLink)
		{
			// To find the current translation position for this frame of animation, we times
			// the currentFrame by the number of tags, then add i.  This is similar to how
			// the vertex key frames are interpolated.
			VECTOR3D vPosition = pModel->pTags[pModel->currentFrame * pModel->numOfTags + i].vPosition;

			// Grab the next key frame translation position
			VECTOR3D vNextPosition = pModel->pTags[pModel->nextFrame * pModel->numOfTags + i].vPosition;
		
			// By using the equation: p(t) = p0 + t(p1 - p0), with a time t,
			// we create a new translation position that is closer to the next key frame.
			vPosition.x = vPosition.x + pModel->t * (vNextPosition.x - vPosition.x),
			vPosition.y	= vPosition.y + pModel->t * (vNextPosition.y - vPosition.y),
			vPosition.z	= vPosition.z + pModel->t * (vNextPosition.z - vPosition.z);

			// Now comes the more complex interpolation.  Just like the translation, we
			// want to store the current and next key frame rotation matrix, then interpolate
			// between the 2.

			// Get a pointer to the start of the 3x3 rotation matrix for the current frame
			pMatrix = &pModel->pTags[pModel->currentFrame * pModel->numOfTags + i].rotation[0][0];

			// Get a pointer to the start of the 3x3 rotation matrix for the next frame
			pNextMatrix = &pModel->pTags[pModel->nextFrame * pModel->numOfTags + i].rotation[0][0];

			// Now that we have 2 1D arrays that store the matrices, let's interpolate them

			// Convert the current and next key frame 3x3 matrix into a quaternion
			qQuat.CreateFromMatrix( pMatrix, 3);
			qNextQuat.CreateFromMatrix( pNextMatrix, 3 );

			// Using spherical linear interpolation, we find the interpolated quaternion
			qInterpolatedQuat = qQuat.Slerp(qQuat, qNextQuat, pModel->t);

			// Here we convert the interpolated quaternion into a 4x4 matrix
			qInterpolatedQuat.CreateMatrix( finalMatrix );
			
			// To cut out the need for 2 matrix calls, we can just slip the translation
			// into the same matrix that holds the rotation.  That is what index 12-14 holds.
			finalMatrix[12] = vPosition.x;
			finalMatrix[13] = vPosition.y;
			finalMatrix[14] = vPosition.z;

			// Start a new matrix scope
			glPushMatrix();

			// Finally, apply the rotation and translation matrix to the current matrix
			glMultMatrixf( finalMatrix );

			// Recursively draw the next model that is linked to the current one.
			// This could either be a body part or a gun that is attached to
			// the hand of the upper body model.
			calculateLinkBoundingBox(pLink);

			// End the current matrix scope
			glPopMatrix();
		}
	}

}

void CModelMD3::calculateModelBoundingBox(t3DModel * pModel)
{
	float x, y, z;
	float x2, y2, z2;
	float norm_x, norm_y, norm_z;

	// return if there are no objects to draw
	if(pModel->pObject.size() <= 0) return;

	// get the current modelview transformation.
	// We'll need it to find the eye coordinates
	// of the axis aligned bounding box for this model
	GLfloat matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

	// initialize the bounding box to something really huge--we'll shrink it down to size during drawing
	// this is kind of a crappy shortcut, but we can fix it easily sometime later
	pModel->min_x = 1000000.0f;
	pModel->max_x = -1000000.0f;
	pModel->min_y = 1000000.0f;
	pModel->max_y = -1000000.0f;
	pModel->min_z = 1000000.0f;
	pModel->max_z = -1000000.0f;

	// Go through all of the objects stored in this model
	int i = 0;
	for(i = 0; i < pModel->numOfObjects; i++)
	{
		// Get the current object that we are displaying
		t3DObject *pObject = pModel->pObject[i];			//tws 27.4 - 3

		// Find the current starting index for the current key frame we are on
		int currentIndex = pModel->currentFrame * pObject->numOfVerts;

		// Since we are interpolating, we also need the index for the next key frame
		int nextIndex = pModel->nextFrame * pObject->numOfVerts;

		// Go through all of the faces (polygons) of the object and draw them
		for(int j = 0; j < pObject->numOfFaces; j++)
		{
			int whichVertex;

			// Go through each vertex of the triangle and draw it.
			for( whichVertex = 0; whichVertex < 3; whichVertex++)
			{
				// Get the index for the current point in the face list
				int index = pObject->pFaces[j].vertIndex[whichVertex];

				// Like in the MD2 Animation tutorial, we use linear interpolation
				// between the current and next point to find the point in between,
				// depending on the model's "t" (0.0 to 1.0).

				// Store the current and next frame's vertex by adding the current
				// and next index to the initial index given from the face data.
				VECTOR3D vPoint1 = pObject->pVerts[ currentIndex + index ];
				VECTOR3D vPoint2 = pObject->pVerts[ nextIndex + index];

				//TWS -- get the normals. The normals are in spherical coordinates

				VECTOR3D nPoint1 = pObject->pNormals[ currentIndex + index ];
				VECTOR3D nPoint2 = pObject->pNormals[ nextIndex + index];

				//interpolate the vertices
				x = vPoint1.x + pModel->t * (vPoint2.x - vPoint1.x);
				y = vPoint1.y + pModel->t * (vPoint2.y - vPoint1.y);
				z = vPoint1.z + pModel->t * (vPoint2.z - vPoint1.z);

				//interpolate the normals
				norm_x = nPoint1.x + pModel->t * (nPoint2.x - nPoint1.x);
				norm_y = nPoint1.y + pModel->t * (nPoint2.y - nPoint1.y);
				norm_z = nPoint1.z + pModel->t * (nPoint2.z - nPoint1.z);

				//multiply the vertex by the current transformation
				//to get the world coordinates
				x2 = matrix[0]*x + matrix[4]*y + matrix[8]*z + matrix[12];
				y2 = matrix[1]*x + matrix[5]*y + matrix[9]*z +  matrix[13];
				z2 = matrix[2]*x + matrix[6]*y + matrix[10]*z + matrix[14];

				//find the max and min world coordinates for the bounding box
				if(x2 > pModel->max_x)
					pModel->max_x = x2;
				if(x2 < pModel->min_x)
					pModel->min_x = x2;

				if(y2 > pModel->max_y)
					pModel->max_y = y2;
				if(y2 < pModel->min_y)
					pModel->min_y = y2;

				if(z2 > pModel->max_z)
					pModel->max_z = z2;
				if(z2 < pModel->min_z)
					pModel->min_z = z2;


			}
		}
	}

	//store the bounding box
	if(m_current_box < MAX_BOUNDING_BOXES)
	{
		m_boxes[m_current_box].setBoundingBox(
			pModel->min_x, pModel->min_y, pModel->min_z,
			pModel->max_x, pModel->max_y, pModel->max_z	);

		m_current_box ++;
	}

	return;
}



