//**************************************************************************
//
//  t3dmodel.h
//
//  The data structures in this file were taken from the gametutorials md3 animation tutorial.
//  However, there have been a few changes.
//
//**************************************************************************
#ifndef T3DMODEL_H
#define T3DMODEL_H

#include <vector>
#include "VECTOR2D.h"
#include "VECTOR3D.h"

// This is our face structure.  This is used for indexing into the vertex
// and texture coordinate arrays.  From this information we know which vertices
// from our vertex array go to which face, along with the correct texture coordinates.
struct tFace
{
	int vertIndex[3];			// indicies for the verts that make up this triangle
	int coordIndex[3];			// indicies for the tex coords to texture this face
};

// This holds the information for a model's material.
struct tMaterialInfo
{
	char  strName[255];			// The texture name
	char  strFile[255];			// The texture file name (If this is set it's a texture map)
	unsigned char  color[3];		// The color of the object (R, G, B)
	int   texureId;				// the texture ID
	float uTile;				// u tiling of texture
	float vTile;				// v tiling of texture
	float uOffset;			    // u offset of texture
	float vOffset;				// v offset of texture
} ;

// This holds our information for each animation of the Quake model.
struct tAnimationInfo
{
	char strName[255];			// This stores the name of the animation (I.E. "TORSO_STAND")
	int startFrame;				// This stores the first frame number for this animation
	int endFrame;				// This stores the last frame number for this animation
	int loopingFrames;			// This stores the looping frames for this animation (not used)
	int framesPerSecond;		// This stores the frames per second that this animation runs
};

//this structure is where the actual geometry is stored
struct t3DObject
{
	int  numOfVerts;			// The number of verts in the model
	int  numOfFaces;			// The number of faces in the model
	int  numTexVertex;			// The number of texture coordinates
	int  materialID;			// The texture ID to use, which is the index into our texture array
	bool bHasTexture;			// This is TRUE if there is a texture map for this object
	char strName[255];			// The name of the object
	VECTOR3D  *pVerts;			// The object's vertices
	VECTOR3D  *pNormals;			// The object's normals
	VECTOR2D  *pTexVerts;			// The texture's UV coordinates
	tFace *pFaces;				// The faces information of the object
};

// This holds the header information that is read in at the beginning of the file
struct tMd3Header
{
	char	fileID[4];				// This stores the file ID - Must be "IDP3"
	int	version;				// This stores the file version - Must be 15
	char	strFile[68];				// This stores the name of the file
	int	numFrames;				// This stores the number of animation frames
	int	numTags;				// This stores the tag count
	int	numMeshes;				// This stores the number of sub-objects in the mesh
	int	numMaxSkins;				// This stores the number of skins for the mesh
	int	headerSize;				// This stores the mesh header size
	int	tagStart;				// This stores the offset into the file for tags
	int	tagEnd;					// This stores the end offset into the file for tags
	int	fileSize;				// This stores the file size
};

// This structure is used to read in the mesh data for the .md3 models
struct tMd3MeshInfo
{
	char	meshID[4];				// This stores the mesh ID (We don't care)
	char	strName[68];				// This stores the mesh name (We do care)
	int	numMeshFrames;				// This stores the mesh aniamtion frame count
	int	numSkins;				// This stores the mesh skin count
	int     numVertices;				// This stores the mesh vertex count
	int	numTriangles;				// This stores the mesh face count
	int	triStart;				// This stores the starting offset for the triangles
	int	headerSize;				// This stores the header size for the mesh
	int     uvStart;				// This stores the starting offset for the UV coordinates
	int	vertexStart;				// This stores the starting offset for the vertex indices
	int	meshSize;				// This stores the total mesh size
};

// This is our tag structure for the .MD3 file format.  These are used link other
// models to and the rotate and transate the child models of that model.
struct tMd3Tag
{
	char		strName[64];			// This stores the name of the tag (I.E. "tag_torso")
	VECTOR3D	vPosition;			// This stores the translation that should be performed
	float		rotation[3][3];			// This stores the 3x3 rotation matrix for this frame
};

// This stores the bone information (useless as far as I can see...)
struct tMd3Bone
{
	float	mins[3];				// This is the min (x, y, z) value for the bone
	float	maxs[3];				// This is the max (x, y, z) value for the bone
	float	position[3];				// This supposedly stores the bone position???
	float	scale;					// This stores the scale of the bone
	char	creator[16];				// The modeler used to create the model (I.E. "3DS Max")
};


// This stores the normals and vertex indices 
struct tMd3Triangle
{
   signed short	 vertex[3];				// The vertex for this face (scale down by 64.0f)
   unsigned char normal[2];				// This stores some crazy normal values (not sure...)
};


// This stores the indices into the vertex and texture coordinate arrays
struct tMd3Face
{
   int vertexIndices[3];				
};


// This stores UV coordinates
struct tMd3TexCoord
{
   float textureCoord[2];
};


// This stores a skin name (We don't use this, just the name of the model to get the texture)
struct tMd3Skin 
{
	char strName[68];
};


//this structure holds an instance of a head, torso, legs, or weapon
class t3DModel
{
	public:

	int numOfObjects;					// The number of objects in the model
	int numOfMaterials;					// The number of materials for the model
	std::vector<tMaterialInfo> pMaterials;			// The list of material information (Textures and colors)
	std::vector<t3DObject *> pObject;			// The object list for our model

	int numOfAnimations;					// The number of animations in this model
	int currentAnim;					// The current index into pAnimations list
	int currentFrame;					// The current frame of the current animation
	int nextFrame;						// The next frame of animation to interpolate too
	float t;						// The ratio of 0.0f to 1.0f between each key frame
	float lastTime;						// This stores the last time that was stored

	bool drawMode;						//TWS
	bool lighting;						//TWS
	bool bounding_box;					//TWS
	bool animation;						//tws
	bool m_selected;					//tws
	long int old_time;					//TWS -- test!

	//drawAABB -- draws the bounding box for this model
	void drawAABB(void);

	//tws -- for temporarily storing the BB
	float min_x, max_x;
	float min_y, max_y;
	float min_z, max_z;

	std::vector<tAnimationInfo> pAnimations; // The list of animations

	int numOfTags;					// This stores the number of tags in the model
	t3DModel		**pLinks;		// This stores a list of pointers that are linked to this model
	struct tMd3Tag		*pTags;			// This stores all the tags for the model animations


};

#endif

