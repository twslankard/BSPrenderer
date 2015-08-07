//****************************************************************************************
//
//  Image.h
//
//  TWS:I've had to make quite a few changes to image.h and image.cpp, due to a few OS conflicts.
//
//****************************************************************************************
//////////////////////////////////////////////////////////////////////////////////////////
//	image.h
//	functions for images to be turned to textures
//	You may use this code however you wish, but if you do, please credit me and
//	provide a link to my website in a readme file or similar
//	Downloaded from: www.paulsprojects.net
//	Created:	20th July 2002
//////////////////////////////////////////////////////////////////////////////////////////	

#ifndef IMAGE_H
#define IMAGE_H

#include <GL/gl.h>
#include <cstdio>

class Image
{
public:
	Image() : data(NULL), bpp(0), width(0), height(0)
	{}

	~Image()
	{
		if(data)
			delete [] data;
		data=NULL;
	}
	
	bool Load(char * filename);					//load a texture from a file

	bool LoadBMP(char * filename);				//Load BMP
	bool Load24BitBMP(char * filename);
	bool Load8BitBMP(char * filename);

	bool LoadPCX(char * filename);				//Load PCX

	bool LoadTGA(char * filename);				//Load TGA
	bool LoadUncompressed8BitTGA(char * filename);
	bool LoadUncompressedTrueColorTGA(char * filename);
	bool LoadCompressedTrueColorTGA(char * filename);
	
	//load an uncompressed greyscale TGA for alpha channel
	bool LoadAlphaTGA(char * filename);

	//Alter the image
	void FlipVertically();

	unsigned char * data;				//Image data
	unsigned int	bpp;				//Image color depth in bits per pixel
	unsigned int	width;				//Image width
	unsigned int	height;			//Image height

	GLuint	 format;				//Data format, eg GL_RGBA
};

//Universal bitmap identifier
const unsigned long BITMAP_ID=0x4D42;

#endif	//TEXTURE_IMAGE_H


