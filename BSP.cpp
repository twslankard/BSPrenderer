//////////////////////////////////////////////////////////////////////////////////////////
//	BSP.cpp
//	Functions for bsp file
//	You may use this code however you wish, but if you do, please credit me and
//	provide a link to my website in a readme file or similar
//	Downloaded from: www.paulsprojects.net
//	Created:	8th August 2002
//////////////////////////////////////////////////////////////////////////////////////////	

#include <cstdio>
#include <cstring>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include "image.h"
//#include "jpeg.h"
#include "BSP.h"

using namespace std;

////////////////////BSP::Load///////////////
////////////////////////////////////////////
bool BSP::Load(char * filename, int curveTesselation)
{
	FILE * file;

	file=fopen(filename, "rb");
	if(!file)
	{
		//errorLog.OutputError("Unable to open %s", filename);
		return false;
	}

	//read in header
	fread(&header, sizeof(BSP_HEADER), 1, file);

	//check header data is correct
	if(	header.string[0]!='I' || header.string[1]!='B' ||
		header.string[2]!='S' || header.string[3]!='P' ||
		header.version  !=0x2E )
	{
		//errorLog.OutputError("%s is not a version 0x2E .bsp map file", filename);
		return false;
	}


	//Load in vertices
	if(!LoadVertices(file))
		return false;


	//Load in mesh indices
	//Calculate number of indices
	int numMeshIndices=header.directoryEntries[bspMeshIndices].length/sizeof(int);

	//Create space
	meshIndices=new int[numMeshIndices];
	if(!meshIndices)
	{
		//errorLog.OutputError("Unable to allocate memory for %d mesh indices", numMeshIndices);
		return false;
	}

	//read in the mesh indices
	fseek(file, header.directoryEntries[bspMeshIndices].offset, SEEK_SET);
	fread(meshIndices, header.directoryEntries[bspMeshIndices].length, 1, file);

	

	//Load in faces
	if(!LoadFaces(file, curveTesselation))
		return false;

	
	//Load textures
	if(!LoadTextures(file))
		return false;

		
	//Load Lightmaps
	if(!LoadLightmaps(file))
		return false;


	//Load BSP Data
	if(!LoadBSPData(file))
		return false;

	//tws -- load the shaders... *gulp*
	if(!LoadShaders(file))
	{
		cerr << "Couldn't load shaders! It are sad." << endl;
		return false;
	}

	//tws -- load the brushes
	if(!LoadBrushes(file))
	{
		cerr << "Couldn't load brushes! It are sad." << endl;
		return false;
	}

	//Load in entity string
	entityString=new char[header.directoryEntries[bspEntities].length];
	if(!entityString)
	{
		//errorLog.OutputError(	"Unable to allocate memory for %d length entity string",
		//						header.directoryEntries[bspEntities].length);
		return false;
	}

	//Go to entity string in file
	fseek(file, header.directoryEntries[bspEntities].offset, SEEK_SET);
	fread(entityString, 1, header.directoryEntries[bspEntities].length, file);

	//Output the entity string
	//errorLog.OutputSuccess("Entity String: %s", entityString);


	fclose(file);

	//errorLog.OutputSuccess("%s Loaded successfully", filename);

	return true;
}



///////////////////BSP::LoadVertices////////
////////////////////////////////////////////
bool BSP::LoadVertices(FILE * file)
{
	//calculate number of vertices
	numVertices=header.directoryEntries[bspVertices].length/sizeof(BSP_LOAD_VERTEX);

	//Create space for this many BSP_LOAD_VERTICES
	BSP_LOAD_VERTEX * loadVertices=new BSP_LOAD_VERTEX[numVertices];
	if(!loadVertices)
	{
		//errorLog.OutputError("Unable to allocate memory for %d BSP_LOAD_VERTEXes", numVertices);
		return false;
	}

	//go to vertices in file
	fseek(file, header.directoryEntries[bspVertices].offset, SEEK_SET);

	//read in the vertices
	fread(loadVertices, header.directoryEntries[bspVertices].length, 1, file);

	//Convert to BSP_VERTEXes
	vertices=new BSP_VERTEX[numVertices];
	if(!vertices)
	{
		//errorLog.OutputError("Unable to allocate memory for vertices");
		return false;
	}

	for(int i=0; i<numVertices; ++i)
	{
		//swap y and z and negate z
		vertices[i].position.x=loadVertices[i].position.x;
		vertices[i].position.y=loadVertices[i].position.z;
		vertices[i].position.z=-loadVertices[i].position.y;

		//scale down
		vertices[i].position/=64;

		//Transfer texture coordinates (Invert t)
		vertices[i].decalS=loadVertices[i].decalS;
		vertices[i].decalT=-loadVertices[i].decalT;

		//Transfer lightmap coordinates
		vertices[i].lightmapS=loadVertices[i].lightmapS;
		vertices[i].lightmapT=loadVertices[i].lightmapT;
	}

	if(loadVertices)
		delete [] loadVertices;
	loadVertices=NULL;

	return true;
}

///////////////////BSP::LoadFaces///////////
////////////////////////////////////////////
bool BSP::LoadFaces(FILE * file, int curveTesselation)
{
	//calculate number of load faces
	numTotalFaces=header.directoryEntries[bspFaces].length/sizeof(BSP_LOAD_FACE);

	//Create space for this many BSP_LOAD_FACES
	BSP_LOAD_FACE * loadFaces=new BSP_LOAD_FACE[numTotalFaces];
	if(!loadFaces)
	{
		//errorLog.OutputError("Unable to allocate memory for %d BSP_LOAD_FACEs", numTotalFaces);
		return false;
	}

	//go to faces in file
	fseek(file, header.directoryEntries[bspFaces].offset, SEEK_SET);

	//read in the faces
	fread(loadFaces, header.directoryEntries[bspFaces].length, 1, file);


	//Create space for face directory
	faceDirectory=new BSP_FACE_DIRECTORY_ENTRY[numTotalFaces];
	if(!faceDirectory)
	{
		//errorLog.OutputError(	"Unable to allocate space for face directory with %d entries",
		//						numTotalFaces);
		return false;
	}
	
	//Clear the face directory
	memset(faceDirectory, 0, numTotalFaces*sizeof(BSP_FACE_DIRECTORY_ENTRY));

	//Init the "faces drawn" Bitset
	facesToDraw.Init(numTotalFaces);


	//Calculate how many of each face type there is
	int i;
	for(i=0; i<numTotalFaces; ++i)
	{
		if(loadFaces[i].type==bspPolygonFace)
			++numPolygonFaces;
		if(loadFaces[i].type==bspPatch)
			++numPatches;
		if(loadFaces[i].type==bspMeshFace)
			++numMeshFaces;
	}



	//Create space for BSP_POLYGON_FACEs
	polygonFaces=new BSP_POLYGON_FACE[numPolygonFaces];
	if(!polygonFaces)
	{
		//errorLog.OutputError("Unable To Allocate memory for BSP_POLYGON_FACEs");
		return false;
	}

	int currentFace=0;
	//convert loadFaces to polygonFaces
	for(i=0; i<numTotalFaces; ++i)
	{
		if(loadFaces[i].type!=bspPolygonFace)		//skip this loadFace if it is not a polygon face
			continue;

		polygonFaces[currentFace].firstVertexIndex=loadFaces[i].firstVertexIndex;
		polygonFaces[currentFace].numVertices=loadFaces[i].numVertices;
		polygonFaces[currentFace].textureIndex=loadFaces[i].texture;
		polygonFaces[currentFace].lightmapIndex=loadFaces[i].lightmapIndex;

		//fill in this entry on the face directory
		faceDirectory[i].faceType=bspPolygonFace;
		faceDirectory[i].typeFaceNumber=currentFace;

		++currentFace;
	}

	//Create space for BSP_MESH_FACEs
	meshFaces=new BSP_MESH_FACE[numMeshFaces];
	if(!meshFaces)
	{
		//errorLog.OutputError("Unable To Allocate memory for BSP_MESH_FACEs");
		return false;
	}

	int currentMeshFace=0;
	//convert loadFaces to faces
	for(i=0; i<numTotalFaces; ++i)
	{
		if(loadFaces[i].type!=bspMeshFace)		//skip this loadFace if it is not a mesh face
			continue;

		meshFaces[currentMeshFace].firstVertexIndex=loadFaces[i].firstVertexIndex;
		meshFaces[currentMeshFace].numVertices=loadFaces[i].numVertices;
		meshFaces[currentMeshFace].textureIndex=loadFaces[i].texture;
		meshFaces[currentMeshFace].lightmapIndex=loadFaces[i].lightmapIndex;
		meshFaces[currentMeshFace].firstMeshIndex=loadFaces[i].firstMeshIndex;
		meshFaces[currentMeshFace].numMeshIndices=loadFaces[i].numMeshIndices;

		//fill in this entry on the face directory
		faceDirectory[i].faceType=bspMeshFace;
		faceDirectory[i].typeFaceNumber=currentMeshFace;

		++currentMeshFace;
	}

	//Create space for BSP_PATCHes
	patches=new BSP_PATCH[numPatches];
	if(!patches)
	{
		//errorLog.OutputError("Unable To Allocate memory for BSP_PATCHes");
		return false;
	}

	int currentPatch=0;
	//convert loadFaces to patches
	for(i=0; i<numTotalFaces; ++i)
	{
		if(loadFaces[i].type!=bspPatch)		//skip this loadFace if it is not a patch
			continue;

		patches[currentPatch].textureIndex=loadFaces[i].texture;
		patches[currentPatch].lightmapIndex=loadFaces[i].lightmapIndex;
		patches[currentPatch].width=loadFaces[i].patchSize[0];
		patches[currentPatch].height=loadFaces[i].patchSize[1];
		
		//fill in this entry on the face directory
		faceDirectory[i].faceType=bspPatch;
		faceDirectory[i].typeFaceNumber=currentPatch;

		//Create space to hold quadratic patches
		int numPatchesWide=(patches[currentPatch].width-1)/2;
		int numPatchesHigh=(patches[currentPatch].height-1)/2;

		patches[currentPatch].numQuadraticPatches=	numPatchesWide*numPatchesHigh;
		patches[currentPatch].quadraticPatches=new BSP_BIQUADRATIC_PATCH
													[patches[currentPatch].numQuadraticPatches];
		if(!patches[currentPatch].quadraticPatches)
		{
			//errorLog.OutputError(	"Unable to allocate memory for %d quadratic patches",
			//						patches[currentPatch].numQuadraticPatches);
			return false;
		}

		//fill in the quadratic patches
		for(int y=0; y<numPatchesHigh; ++y)
		{
			for(int x=0; x<numPatchesWide; ++x)
			{
				for(int row=0; row<3; ++row)
				{
					for(int point=0; point<3; ++point)
					{
						patches[currentPatch].quadraticPatches[y*numPatchesWide+x].
							controlPoints[row*3+point]=vertices[loadFaces[i].firstVertexIndex+
								(y*2*patches[currentPatch].width+x*2)+
									row*patches[currentPatch].width+point];
					}
				}

				//tesselate the patch
				patches[currentPatch].quadraticPatches[y*numPatchesWide+x].Tesselate(curveTesselation);
			}
		}


		++currentPatch;
	}

	if(loadFaces)
		delete [] loadFaces;
	loadFaces=NULL;

	return true;
}

///////////////////BSP::LoadTextures////////
////////////////////////////////////////////
bool BSP::LoadTextures(FILE * file)
{
	//Calculate number of textures
	numTextures=header.directoryEntries[bspTextures].length/sizeof(BSP_TEXTURE);

	//Create space for this many BSP_LOAD_TEXTUREs
	textures = new BSP_TEXTURE[numTextures];
	if(!textures)
	{
		//errorLog.OutputError("Unable to allocate space for %d BSP_LOAD_TEXTUREs", numTextures);
		return false;
	}

	//Load textures
	fseek(file, header.directoryEntries[bspTextures].offset, SEEK_SET);
	fread(textures, 1, header.directoryEntries[bspTextures].length, file);

	//Create storage space for that many texture identifiers
	decalTextures=new GLuint[numTextures];
	if(!decalTextures)
	{
		//errorLog.OutputError("Unable to create storage space for %d texture IDs", numTextures);
		return false;
	}
	
	//Create storage space for that many booleans to tell if texture has loaded
	isTextureLoaded=new bool[numTextures];
	if(!isTextureLoaded)
	{
		//errorLog.OutputError("Unable to create storage space for %d booleans", numTextures);
		return false;
	}
	

	//Generate the texture identifiers
	glGenTextures(numTextures, decalTextures);

	//Loop through and create textures
	Image textureImage;				//Image used to load textures

	for(int i=0; i<numTextures; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, decalTextures[i]);
		
		//add file extension to the name
		char tgaExtendedName[68];
		//char jpgExtendedName[68];
		strcpy(tgaExtendedName, textures[i].name);
		strcat(tgaExtendedName, ".tga");
		//strcpy(jpgExtendedName, loadTextures[i].name);
		//strcat(jpgExtendedName, ".jpg");

		//Load texture image
		bool isJpgTexture=false;				//have we loaded a jpg?
		if(!textureImage.Load(tgaExtendedName))	//try to load .tga, if not
		{
			//if(LoadJPG(&textureImage, jpgExtendedName))	//try to load jpg
			//{
			//	isJpgTexture=true;
			//	isTextureLoaded[i]=true;
			//}
			//else
				isTextureLoaded[i]=false;
		}
		else
			isTextureLoaded[i]=true;
		
		//if a jpg texture, need to set UNPACK_ALIGNMENT to 1
		if(isJpgTexture)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		//Create texture
		gluBuild2DMipmaps(	GL_TEXTURE_2D, GL_RGBA8, textureImage.width, textureImage.height,
							textureImage.format, GL_UNSIGNED_BYTE, textureImage.data);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		//Set Parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	//tws -- we want to keep these around !
	//if(loadTextures)
	//	delete [] loadTextures;
	//loadTextures=NULL;

	return true;
}

///////////////////BSP::LoadLightmaps///////
////////////////////////////////////////////
bool BSP::LoadLightmaps(FILE * file)
{
	//Calculate number of lightmaps
	numLightmaps=header.directoryEntries[bspLightmaps].length/sizeof(BSP_LOAD_LIGHTMAP);

	//Create space for this many BSP_LOAD_LIGHTMAPs
	BSP_LOAD_LIGHTMAP * loadLightmaps=new BSP_LOAD_LIGHTMAP[numLightmaps];
	if(!loadLightmaps)
	{
		//errorLog.OutputError("Unable to allocate space for %d BSP_LOAD_LIGHTMAPs", numLightmaps);
		return false;
	}

	//Load textures
	fseek(file, header.directoryEntries[bspLightmaps].offset, SEEK_SET);
	fread(loadLightmaps, 1, header.directoryEntries[bspLightmaps].length, file);

	//Create storage space for that many texture identifiers
	lightmapTextures=new GLuint[numLightmaps];
	if(!lightmapTextures)
	{
		//errorLog.OutputError("Unable to create storage space for %d texture IDs", numLightmaps);
		return false;
	}
	
	//Generate the texture identifiers
	glGenTextures(numLightmaps, lightmapTextures);

	//Change the gamma settings on the lightmaps (make them brighter)
	float gamma=2.5f;
	int i;
	for(i=0; i<numLightmaps; ++i)
	{
		for(int j=0; j<128*128; ++j)
		{
			float r, g, b;
			r=loadLightmaps[i].lightmapData[j*3+0];
			g=loadLightmaps[i].lightmapData[j*3+1];
			b=loadLightmaps[i].lightmapData[j*3+2];

			r*=gamma/255.0f;
			g*=gamma/255.0f;
			b*=gamma/255.0f;

			//find the value to scale back up
			float scale=1.0f;
			float temp;
			if(r > 1.0f && (temp = (1.0f/r)) < scale) scale=temp;
			if(g > 1.0f && (temp = (1.0f/g)) < scale) scale=temp;
			if(b > 1.0f && (temp = (1.0f/b)) < scale) scale=temp;

			// scale up color values
			scale*=255.0f;		
			r*=scale;
			g*=scale;
			b*=scale;

			//fill data back in
			loadLightmaps[i].lightmapData[j*3+0]=(GLubyte)r;
			loadLightmaps[i].lightmapData[j*3+1]=(GLubyte)g;
			loadLightmaps[i].lightmapData[j*3+2]=(GLubyte)b;
		}
	}

	for(i=0; i<numLightmaps; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, lightmapTextures[i]);
		
		//Create texture
		gluBuild2DMipmaps(	GL_TEXTURE_2D, GL_RGBA8, 128, 128,
							GL_RGB, GL_UNSIGNED_BYTE, loadLightmaps[i].lightmapData);

		//Set Parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	//Create white texture for if no lightmap specified
	GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glGenTextures(1, &whiteTexture);
	glBindTexture(GL_TEXTURE_2D, whiteTexture);
	//Create texture
	gluBuild2DMipmaps(	GL_TEXTURE_2D, GL_RGBA8, 1, 1,
						GL_RGB, GL_FLOAT, white);
	//Set Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if(loadLightmaps)
		delete [] loadLightmaps;
	loadLightmaps=NULL;

	return true;
}

///////////////////BSP::LoadBSPData/////////
////////////////////////////////////////////
bool BSP::LoadBSPData(FILE * file)
{
	//Load leaves
	//Calculate number of leaves
	numLeaves=header.directoryEntries[bspLeaves].length/sizeof(BSP_LOAD_LEAF);

	//Create space for this many BSP_LOAD_LEAFS
	BSP_LOAD_LEAF * loadLeaves=new BSP_LOAD_LEAF[numLeaves];
	if(!loadLeaves)
	{
		//errorLog.OutputError("Unable to allocate space for %d BSP_LOAD_LEAFs", numLeaves);
		return false;
	}

	//Create space for this many BSP_LEAFs
	leaves=new BSP_LEAF[numLeaves];
	if(!leaves)
	{
		//errorLog.OutputError("Unable to allocate space for %d BSP_LEAFs", numLeaves);
		return false;
	}

	//Load leaves
	fseek(file, header.directoryEntries[bspLeaves].offset, SEEK_SET);
	fread(loadLeaves, 1, header.directoryEntries[bspLeaves].length, file);

	int i;
	//Convert the load leaves to leaves
	for(i=0; i<numLeaves; ++i)
	{
		leaves[i].cluster=loadLeaves[i].cluster;
		leaves[i].firstLeafFace=loadLeaves[i].firstLeafFace;
		leaves[i].numFaces=loadLeaves[i].numFaces;
		leaves[i].firstLeafBrush = loadLeaves[i].firstLeafBrush;	//tws -- need to actually store the brushes
		leaves[i].numBrushes = loadLeaves[i].numBrushes;		//tws

		//Create the bounding box
		leaves[i].boundingBoxVertices[0].Set((float)loadLeaves[i].mins[0], (float)loadLeaves[i].mins[2],-(float)loadLeaves[i].mins[1]);
		leaves[i].boundingBoxVertices[1].Set((float)loadLeaves[i].mins[0], (float)loadLeaves[i].mins[2],-(float)loadLeaves[i].maxs[1]);
		leaves[i].boundingBoxVertices[2].Set((float)loadLeaves[i].mins[0], (float)loadLeaves[i].maxs[2],-(float)loadLeaves[i].mins[1]);
		leaves[i].boundingBoxVertices[3].Set((float)loadLeaves[i].mins[0], (float)loadLeaves[i].maxs[2],-(float)loadLeaves[i].maxs[1]);
		leaves[i].boundingBoxVertices[4].Set((float)loadLeaves[i].maxs[0], (float)loadLeaves[i].mins[2],-(float)loadLeaves[i].mins[1]);
		leaves[i].boundingBoxVertices[5].Set((float)loadLeaves[i].maxs[0], (float)loadLeaves[i].mins[2],-(float)loadLeaves[i].maxs[1]);
		leaves[i].boundingBoxVertices[6].Set((float)loadLeaves[i].maxs[0], (float)loadLeaves[i].maxs[2],-(float)loadLeaves[i].mins[1]);
		leaves[i].boundingBoxVertices[7].Set((float)loadLeaves[i].maxs[0], (float)loadLeaves[i].maxs[2],-(float)loadLeaves[i].maxs[1]);

		for(int j=0; j<8; ++j)
			leaves[i].boundingBoxVertices[j]/=64;
	}

	
	
	//Load leaf faces array
	int numLeafFaces=header.directoryEntries[bspLeafFaces].length/sizeof(int);

	//Create space for this many leaf faces
	leafFaces=new int[numLeafFaces];
	if(!leafFaces)
	{
		//errorLog.OutputError("Unable to allocate space for %d leaf faces", numLeafFaces);
		return false;
	}

	//Load leaf faces
	fseek(file, header.directoryEntries[bspLeafFaces].offset, SEEK_SET);
	fread(leafFaces, 1, header.directoryEntries[bspLeafFaces].length, file);


	
	//Load Planes
	numPlanes=header.directoryEntries[bspPlanes].length/sizeof(PLANE);

	//Create space for this many planes
	planes=new PLANE[numPlanes];
	if(!planes)
	{
		//errorLog.OutputError("Unable to allocate space for %d planes", numPlanes);
		return false;
	}

	fseek(file, header.directoryEntries[bspPlanes].offset, SEEK_SET);
	fread(planes, 1, header.directoryEntries[bspPlanes].length, file);

	//reverse the intercept on the planes and convert planes to OGL coordinates
	for(i=0; i<numPlanes; ++i)
	{
		//swap y and z and negate z
		float temp=planes[i].normal.y;
		planes[i].normal.y=planes[i].normal.z;
		planes[i].normal.z=-temp;

		planes[i].intercept=-planes[i].intercept;
		planes[i].intercept/=64;	//scale down
	}




	//Load nodes
	numNodes=header.directoryEntries[bspNodes].length/sizeof(BSP_NODE);

	//Create space for this many nodes
	nodes=new BSP_NODE[numNodes];
	if(!nodes)
	{
		//errorLog.OutputError("Unable to allocate space for %d nodes", numNodes);
		return false;
	}

	fseek(file, header.directoryEntries[bspNodes].offset, SEEK_SET);
	fread(nodes, 1, header.directoryEntries[bspNodes].length, file);

	//Load visibility data
	//load numClusters and bytesPerCluster
	fseek(file, header.directoryEntries[bspVisData].offset, SEEK_SET);
	fread(&visibilityData, 2, sizeof(int), file);

	//Calculate the size of the Bitset
	int BitsetSize=visibilityData.numClusters*visibilityData.bytesPerCluster;

	//Create space for Bitset
	visibilityData.bitset=new GLubyte[BitsetSize];
	if(!visibilityData.bitset)
	{
		//errorLog.OutputError(	"Unable to allocate memory for visibility Bitset of size %d bytes",
		//						BitsetSize);
		return false;
	}
	//read Bitset
	fread(visibilityData.bitset, 1, BitsetSize, file);

	if(loadLeaves)
		delete [] loadLeaves;
	loadLeaves=NULL;


	return true;
}


//BSP Functions
//calculate which leaf the camera lies in
int BSP::CalculateCameraLeaf(const VECTOR3D & cameraPosition)
{
	int currentNode=0;
	
	//loop until we find a negative index
	while(currentNode>=0)
	{
		//if the camera is in front of the plane for this node, assign i to be the front node
		if(planes[nodes[currentNode].planeIndex].ClassifyPoint(cameraPosition)==POINT_IN_FRONT_OF_PLANE)
			currentNode=nodes[currentNode].front;
		else
			currentNode=nodes[currentNode].back;
	}

	//return leaf index
	return ~currentNode;
}

//See if one cluster is visible from another
int BSP::isClusterVisible(int cameraCluster, int testCluster)
{
	int index=	cameraCluster*visibilityData.bytesPerCluster + testCluster/8;

	int returnValue=visibilityData.bitset[index] & (1<<(testCluster & 7));

	return returnValue;
}

//Calculate which faces to draw given a position & frustum
void BSP::CalculateVisibleFaces(const VECTOR3D & cameraPosition, Frustum frustum)
{
	//Clear the list of faces drawn
	facesToDraw.ClearAll();
	
	//calculate the camera leaf
	int cameraLeaf=CalculateCameraLeaf(cameraPosition);
	int cameraCluster=leaves[cameraLeaf].cluster;

	//loop through the leaves
	for(int i=0; i<numLeaves; ++i)
	{
		//if the leaf is not in the PVS, continue
		if(!isClusterVisible(cameraCluster, leaves[i].cluster))
			continue;

		//if this leaf does not lie in the frustum, continue
		if(!frustum.IsBoundingBoxInside(leaves[i].boundingBoxVertices))
			continue;

		//loop through faces in this leaf and mark them to be drawn
		for(int j=0; j<leaves[i].numFaces; ++j)
		{
			facesToDraw.Set(leafFaces[leaves[i].firstLeafFace+j]);
		}
	}

	
}

//DRAWING FUNCTIONS
//Draw all faces marked as visible
void BSP::Draw()
{
	glFrontFace(GL_CW);

	//enable vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//loop through faces
	for(int i=0; i<numTotalFaces; ++i)
	{
		//if this face is to be drawn, draw it
		if(facesToDraw.IsSet(i))
			DrawFace(i);
	}

	glFrontFace(GL_CCW);
}


//Draw a face
void BSP::DrawFace(int faceNumber)
{
	//look this face up in the face directory
	if(faceDirectory[faceNumber].faceType==0)
		return;
	
	if(faceDirectory[faceNumber].faceType==bspPolygonFace)
		DrawPolygonFace(faceDirectory[faceNumber].typeFaceNumber);

	if(faceDirectory[faceNumber].faceType==bspMeshFace)
		DrawMeshFace(faceDirectory[faceNumber].typeFaceNumber);

	if(faceDirectory[faceNumber].faceType==bspPatch)
		DrawPatch(faceDirectory[faceNumber].typeFaceNumber);
}


//Draw a polygon face
void BSP::DrawPolygonFace(int polygonFaceNumber)
{
	if(!isTextureLoaded[polygonFaces[polygonFaceNumber].textureIndex])
		return;
	glBindTexture( GL_TEXTURE_2D, decalTextures[polygonFaces[polygonFaceNumber].textureIndex]);
	glTexCoordPointer(2, GL_FLOAT, sizeof(BSP_VERTEX), &vertices[0].decalS);
	glVertexPointer(3, GL_FLOAT, sizeof(BSP_VERTEX), &vertices[0].position);
	glDrawArrays(GL_TRIANGLE_FAN, polygonFaces[polygonFaceNumber].firstVertexIndex, polygonFaces[polygonFaceNumber].numVertices);

	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);

	glTexCoordPointer(2, GL_FLOAT, sizeof(BSP_VERTEX), &vertices[0].lightmapS);
	glBindTexture(GL_TEXTURE_2D, lightmapTextures[polygonFaces[polygonFaceNumber].lightmapIndex]);
	glDrawArrays(GL_TRIANGLE_FAN, polygonFaces[polygonFaceNumber].firstVertexIndex, polygonFaces[polygonFaceNumber].numVertices);
	glDisable(GL_BLEND);
}

//Draw a mesh face
void BSP::DrawMeshFace(int meshFaceNumber)
{
	if(!isTextureLoaded[meshFaces[meshFaceNumber].textureIndex])
		return;

	glDisable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, decalTextures[meshFaces[meshFaceNumber].textureIndex]);
	glVertexPointer(3, GL_FLOAT, sizeof(BSP_VERTEX), &vertices[meshFaces[meshFaceNumber].firstVertexIndex].position);
	glTexCoordPointer(2, GL_FLOAT, sizeof(BSP_VERTEX), &vertices[meshFaces[meshFaceNumber].firstVertexIndex].decalS);
	glDrawElements(	GL_TRIANGLES, meshFaces[meshFaceNumber].numMeshIndices, GL_UNSIGNED_INT,
						&meshIndices[meshFaces[meshFaceNumber].firstMeshIndex]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);


	glTexCoordPointer(2, GL_FLOAT, sizeof(BSP_VERTEX), &vertices[0].lightmapS);

	//only bind a lightmap if one exists
	if(meshFaces[meshFaceNumber].lightmapIndex>=0)
		glBindTexture(GL_TEXTURE_2D, lightmapTextures[meshFaces[meshFaceNumber].lightmapIndex]);
	else
		glBindTexture(GL_TEXTURE_2D, whiteTexture);

	glDrawElements(	GL_TRIANGLES, meshFaces[meshFaceNumber].numMeshIndices, GL_UNSIGNED_INT,
						&meshIndices[meshFaces[meshFaceNumber].firstMeshIndex]);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

//Draw a patch
void BSP::DrawPatch(int patchNumber)
{
	glBindTexture(GL_TEXTURE_2D, decalTextures[patches[patchNumber].textureIndex]);

	for(int i=0; i<patches[patchNumber].numQuadraticPatches; ++i)
	{
		//patches[patchNumber].quadraticPatches[i].Draw();
		BSP_BIQUADRATIC_PATCH * patch = &patches[patchNumber].quadraticPatches[i];

		for(int row=0; row<patch->tesselation; ++row)
		{
			glVertexPointer(3, GL_FLOAT, sizeof(BSP_VERTEX), &patch->vertices[0].position);
			glTexCoordPointer(2, GL_FLOAT, sizeof(BSP_VERTEX), &patch->vertices[0].decalS);
			glDrawElements(	GL_TRIANGLE_STRIP, 2*(patch->tesselation+1), GL_UNSIGNED_INT,
							&patch->indices[row*2*(patch->tesselation+1)]);
		}
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);

	//only bind a lightmap if one exists
	if(patches[patchNumber].lightmapIndex>=0)
		glBindTexture(GL_TEXTURE_2D, lightmapTextures[patches[patchNumber].lightmapIndex]);
	else
		glBindTexture(GL_TEXTURE_2D, whiteTexture);

	for(int i=0; i<patches[patchNumber].numQuadraticPatches; ++i)
	{
		//patches[patchNumber].quadraticPatches[i].Draw();
		BSP_BIQUADRATIC_PATCH * patch = &patches[patchNumber].quadraticPatches[i];

		for(int row=0; row<patch->tesselation; ++row)
		{
			glVertexPointer(3, GL_FLOAT, sizeof(BSP_VERTEX), &patch->vertices[0].position);
			glTexCoordPointer(2, GL_FLOAT, sizeof(BSP_VERTEX), &patch->vertices[0].lightmapS);
			glDrawElements(	GL_TRIANGLE_STRIP, 2*(patch->tesselation+1), GL_UNSIGNED_INT,
							&patch->indices[row*2*(patch->tesselation+1)]);
		}
	}

	glDisable(GL_BLEND);
}


//Tesselate a biquadratic patch
bool BSP_BIQUADRATIC_PATCH::Tesselate(int newTesselation)
{
	tesselation=newTesselation;

	float px, py;
	BSP_VERTEX temp[3];
	vertices=new BSP_VERTEX[(tesselation+1)*(tesselation+1)];

	for(int v=0; v<=tesselation; ++v)
	{
		px=(float)v/tesselation;

		vertices[v]=controlPoints[0]*((1.0f-px)*(1.0f-px))+
					controlPoints[3]*((1.0f-px)*px*2)+
					controlPoints[6]*(px*px);
	}

	for(int u=1; u<=tesselation; ++u)
	{
		py=(float)u/tesselation;

		temp[0]=controlPoints[0]*((1.0f-py)*(1.0f-py))+
				controlPoints[1]*((1.0f-py)*py*2)+
				controlPoints[2]*(py*py);

		temp[1]=controlPoints[3]*((1.0f-py)*(1.0f-py))+
				controlPoints[4]*((1.0f-py)*py*2)+
				controlPoints[5]*(py*py);

		temp[2]=controlPoints[6]*((1.0f-py)*(1.0f-py))+
				controlPoints[7]*((1.0f-py)*py*2)+
				controlPoints[8]*(py*py);

		for(int v=0; v<=tesselation; ++v)
		{
			px=(float)v/tesselation;

			vertices[u*(tesselation+1)+v]=	temp[0]*((1.0f-px)*(1.0f-px))+
											temp[1]*((1.0f-px)*px*2)+
											temp[2]*(px*px);
		}
	}

	//Create indices
	indices=new GLuint[tesselation*(tesselation+1)*2];
	if(!indices)
	{
		//errorLog.OutputError("Unable to allocate memory for surface indices");
		return false;
	}
	int row;
	for(row=0; row<tesselation; ++row)
	{
		for(int point=0; point<=tesselation; ++point)
		{
			//calculate indices
			//reverse them to reverse winding
			indices[(row*(tesselation+1)+point)*2+1]=row*(tesselation+1)+point;
			indices[(row*(tesselation+1)+point)*2]=(row+1)*(tesselation+1)+point;
		}
	}

	return true;
}

//TWS****************************************************************************
//
//  LoadBrushes
//
//  Brushes are convex polyhedra used to describe solid geometry
//  I had to add this function to load the Brushes, because Paul's tutorial didn't do it for me
//
//TWS****************************************************************************
bool BSP::LoadBrushes(FILE * file)
{
	//find the number of leafbrushes
	numLeafbrushes = header.directoryEntries[bspLeafBrushes].length/sizeof(BSP_LEAFBRUSH);

	//find the number of brushes
	numBrushes = header.directoryEntries[bspBrushes].length/sizeof(BSP_BRUSH);

	//find the number of brushsides
	numBrushsides = header.directoryEntries[bspBrushSides].length/sizeof(BSP_BRUSHSIDE);

	//create space for the leafbrushes
	leafbrushes = new BSP_LEAFBRUSH [numLeafbrushes];

	//if the leafbrushes couldn't be allocated, then return
	if(!leafbrushes)
	{
		cerr << "Error: couldn't allocate the BSP leafbrushes." << endl;
		return false;
	}

	//create space for the brushes
	brushes = new BSP_BRUSH [numBrushes];

	//return if not allocated
	if(!brushes)
	{
		cerr << "Error: couldn't allocate the BSP brushes." << endl;
		return false;
	}

	//create space for the brushesides
	brushsides = new BSP_BRUSHSIDE [numBrushsides];

	//return if not allocated
	if(!brushsides)
	{
		cerr << "Error: couldn't allocate the BSP brushsides." << endl;
		return false;
	}

	//seek to the leafbrushes & read them
	fseek(file, header.directoryEntries[bspLeafBrushes].offset, SEEK_SET);
	fread(leafbrushes, header.directoryEntries[bspLeafBrushes].length, 1, file);

	//seek to the brushes & read them
	fseek(file, header.directoryEntries[bspBrushes].offset, SEEK_SET);
	fread(brushes, header.directoryEntries[bspBrushes].length, 1, file);

	//seek to the brushsides & read them
	fseek(file, header.directoryEntries[bspBrushSides].offset, SEEK_SET);
	fread(brushsides, header.directoryEntries[bspBrushSides].length, 1, file);



	//all done!
	return true;
}


//TWS****************************************************************************
//
//  LoadShaders
//
//  I'm adding the ability to load the shaders for collision detection purposes.
//  I may not actually need them, but I guess I'll find out.
//
//TWS****************************************************************************
bool BSP::LoadShaders(FILE * file)
{
	//find the number of shaders
	numShaders = header.directoryEntries[bspEffect].length/sizeof(BSP_SHADER);

	//allocate the array
	shaders = new BSP_SHADER [numShaders];

	//if not allocated, then return
	if(!shaders)
	{
		cerr << "Error: couldn't load level shaders." << endl;
		return false;
	}

	//seek to the shaders and read them
	fseek(file, header.directoryEntries[bspEffect].offset, SEEK_SET);
	fread(shaders, header.directoryEntries[bspEffect].length, 1, file);

 	return true;
}

//TWS****************************************************************************
//
//  getCollision
//
//  returns the collision location of the character with the world,
//  when moving from start to end
//
//TWS****************************************************************************
VECTOR3D BSP::getCollision(VECTOR3D start, VECTOR3D end)
{
	TraceSphere(start, end, 0.25);
	return outputEnd;
}


//TWS****************************************************************************
//
//  TraceRay
//
//  allows us to check a collision of a ray with the BSP
//
//TWS****************************************************************************
void BSP::TraceRay(VECTOR3D start, VECTOR3D end)
{
	traceType = TT_RAY;
	Trace(start, end);
}

//TWS****************************************************************************
//
//  TraceSphere
//
//  allows us to check a collision of a sphere with the BSP
//
//TWS****************************************************************************
void BSP::TraceSphere(VECTOR3D start, VECTOR3D end, float inputRadius)
{
	traceType = TT_SPHERE;
	traceRadius = inputRadius;
	Trace(start, end);
}


//TWS****************************************************************************
//
//  Trace
//
//  This function checks for a collision along a line segment
//  specified by the two input vectors. If a collision occurs, it is stored
//  in the member variable "outputEnd".
//
//TWS****************************************************************************
void BSP::Trace(VECTOR3D inputStart, VECTOR3D inputEnd)
{
	//set everything to true, initially
	outputStartsOut = true;
	outputAllSolid = false;

	//assume that we won't collide
	outputFraction = 1.0;

	//traverse the BSP tree, checking for collisions
	CheckNode(0, 0.0f, 1.0f, inputStart, inputEnd);

	//if the outputFraction is still 1.0f, then we never collided.
	if(outputFraction == 1.0f)
	{
		//...so just return the inputEnd.
		outputEnd = inputEnd;
	}
	else
	{
		//otherwise, there must have been a collision somewhere.
		//linearly interpolate to find the collision location.
		//outputEnd.x = inputStart.x + outputFraction * (inputEnd.x - inputStart.x);
		//outputEnd.y = inputStart.y + outputFraction * (inputEnd.y - inputStart.y);
		//outputEnd.z = inputStart.z + outputFraction * (inputEnd.z - inputStart.z);

		outputEnd = inputStart + (inputEnd - inputStart) * outputFraction;
	}
}

//TWS****************************************************************************
//
//  CheckNode
//
//  This function is called by the Trace functions to check for a collision with the node
//  specified by nodeIndex.
//
//TWS****************************************************************************
void BSP::CheckNode(int nodeIndex, float startFraction, float endFraction, VECTOR3D start, VECTOR3D end)
{
	float offset = 0.0f;
	int brushIndex;
	int leafbrushIndex;

	//if the node index is negative, then the node is a leaf node
	if(nodeIndex < 0)
	{
		//Get the leaf pointer. Since the index is negative, we need to add one and negate it (because the indices start at zero.)
		BSP_LEAF * leaf = &leaves[-(nodeIndex + 1)];

		//go through all of the leaf brushes and check for collisions
		for(brushIndex = 0; brushIndex < leaf->numBrushes; brushIndex++)
		{
			//first get the index into the leafbrush array
			leafbrushIndex = leaf->firstLeafBrush + brushIndex;

			//get the pointer of the brush specified by the leafbrush
			BSP_BRUSH * brush = &brushes[leafbrushes[leafbrushIndex].brush];

			//check to make sure the contents are solid based on the shader.
			//Note: may not need to do this, because I'm not implementing the shaders
			//UPDATE: actually we check the texture properties.
			//I'm not sure why some people refer to the shaders here.

			if(brush->n_brushsides > 0 && (textures[brush->texture].contents & 1))
			{
				//finally, we check the brush
				CheckBrush(brush, start, end);
			}
		}

		//all done!
		return;
	}

	//get the node and its split plane
	BSP_NODE * node = &nodes[nodeIndex];
	PLANE * plane = &planes[node->planeIndex];

	float startDistance = plane->GetDistance(start);//start.DotProduct(plane->normal) - plane->intercept;
	float endDistance = plane->GetDistance(end);// end.DotProduct(plane->normal) - plane->intercept;

	if(traceType == TT_SPHERE)
		offset = traceRadius;

	//if both points are in front of the split plane, check the front child
	if(startDistance >= offset && endDistance >= offset)
	{
		CheckNode(node->front, startFraction, endFraction, start, end);
	}
	else if(startDistance < -offset && endDistance < -offset) //if they're both behind, check the back child
	{
		CheckNode(node->back, startFraction, endFraction, start, end);
	}
	else	//the line spans the splitting plane, so we'll check both nodes and combine the result
	{
		int side;
		float fraction1, fraction2, middleFraction;
		VECTOR3D middle;

		//split the line segment at the plane intersection
		if(startDistance < endDistance)
		{
			side = node->back; //back side
			float inverseDistance = 1.0f / (startDistance - endDistance);
			fraction1 = (startDistance - offset + EPSILON) * inverseDistance;
			fraction2 = (startDistance + offset + EPSILON) * inverseDistance;
		}
		else if(endDistance < startDistance)
		{
			side = node->front; //front side
			float inverseDistance = 1.0f / (startDistance - endDistance);
			fraction1 = (startDistance + offset + EPSILON) * inverseDistance;
			fraction2 = (startDistance - offset - EPSILON) * inverseDistance;
		}
		else
		{
			side = node->front;
			fraction1 = 1.0f;
			fraction2 = 0.0f;
		}

		if(fraction1 < 0.0f)
			fraction1 = 0.0f;
		else if(fraction1 > 1.0f)
			fraction1 = 1.0f;

		if(fraction2 < 0.0f)
			fraction2 = 0.0f;
		else if(fraction2 > 1.0f)
			fraction2 = 1.0f;

		//find the mid point for the first side(?)
		middleFraction = startFraction + (endFraction - startFraction) * fraction1;
		middle = start + (end - start) * fraction1;

		//check the first side
		CheckNode(side, startFraction, middleFraction, start, middle);

		//find the mid point for the second side(?)
		middleFraction = startFraction + (endFraction - startFraction) * fraction2;
		middle = start + (end - start) * fraction2;

		//check the second side'
		if(side == node->back)
			side = node->front;
		else
			side = node->back;

		CheckNode(side, middleFraction, endFraction, middle, end);
	}
}

//TWS****************************************************************************
//
//  CheckBrush
//
//  This function is called by the Trace functions to check for a collision with the BSP_BRUSH brush.
//  To do this, it checks every side, gets the side's plane, and determines whether or not there is a collision
//  with the plane (and where it happened.)
//
//TWS****************************************************************************
void BSP::CheckBrush(BSP_BRUSH * brush, VECTOR3D inputStart, VECTOR3D inputEnd)
{

	float startFraction = -1.0;
	float endFraction = 1.0;
	bool startsOut = false;
	bool endsOut = false;
	int brushsideIndex;

	//go through each of the brushes' brushsides
	for(brushsideIndex = 0; brushsideIndex < brush->n_brushsides; brushsideIndex++)
	{
		//get a pointer to the brushside
		BSP_BRUSHSIDE * brushside = &brushsides[brush->first_brushside + brushsideIndex];

		//find the plane of the bushside
		PLANE * plane = &planes[brushside->plane];

		//find the distance from the start and end points to the plane
		float startDistance;
		float endDistance;

		if(traceType == TT_RAY)
		{

			startDistance = plane->GetDistance(inputStart);
			endDistance = plane->GetDistance(inputEnd);
		}
		else
		{
			startDistance = plane->GetDistance(inputStart) - traceRadius;
			endDistance = plane->GetDistance(inputEnd)  - traceRadius;
		}




		if(startDistance > 0)
			startsOut = true;
		if(endDistance > 0)
			endsOut = true;

		//if the start and end are both on the outside of the plane,
		//then they are outside of the brush alltogether--which means we can skip this brush.
		if(startDistance > 0 && endDistance > 0)
		{
			return;
		}

		//if they are both behind the plane, we should still check the remaining brushsides
		if(startDistance <= 0 && endDistance <= 0)
		{
			continue;
		}

		//if neither of the above tests are true, then the line intersects this brushside's plane.
		//So now we need to find the minimum distance from each of the endpoints that the collision(s) occur
		//to ensure we get the closest collision.

		//if the start is further from the plan than the end is, then the line is entering the brush
		if(startDistance > endDistance)
		{
			float fraction = (startDistance - EPSILON) / (startDistance - endDistance);
			if(fraction > startFraction)
				startFraction = fraction;
		}
		else	//if the end is closer, then the line is leaving the brush
		{
			float fraction = (startDistance + EPSILON) / (startDistance - endDistance);
			if(fraction < endFraction)
				endFraction = fraction;
		}
	}

	//if both the start and end are within the brush
	//then the line is completely contained by solid geometry(?)
	if(startsOut == false)
	{
		outputStartsOut = false;
		if(endsOut == false)
		{
			//cerr << "All solid" << endl;
			outputAllSolid = true;
		}
		return;
	}

	/*//keep track of the min output fraction (?)
	if(startFraction < endFraction)
	{
		if(startFraction > -1 && startFraction < outputFraction)
		{
			if(startFraction < 0)
				startFraction = 0;
			outputFraction = startFraction;


		}
	}*/

	if(startFraction < 0)
			startFraction = 0;
	outputFraction = startFraction;

}


