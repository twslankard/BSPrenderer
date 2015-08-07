//****************************************************************************************
//
//  image.cpp
//
//  TWS:I've had to make quite a few changes to image.h and image.cpp, due to a few OS conflicts.
//
//****************************************************************************************
//////////////////////////////////////////////////////////////////////////////////////////
//	image.cpp
//	functions for image for a texture
//	You may use this code however you wish, but if you do, please credit me and
//	provide a link to my website in a readme file or similar
//	Downloaded from: www.paulsprojects.net
//	Created:	20th July 2002
//////////////////////////////////////////////////////////////////////////////////////////

#include <GL/gl.h>
#include "image.h"
#include <cstdio>	//tws
#include <cstdlib>	//tws
#include <cstring>
#include <iostream>

using namespace std;

//Load - load a texture from a file
bool Image::Load(char * filename)
{
	//Clear the data if already used
	if(data)
		delete [] data;
	data=NULL;
	bpp=0;
	width=0;
	height=0;
	format=0;

	int filenameLength=strlen(filename);

	if(	strncmp((filename+filenameLength-3), "BMP", 3)==0 ||
		strncmp((filename+filenameLength-3), "bmp", 3)==0)
		return LoadBMP(filename);
	
	if(	strncmp((filename+filenameLength-3), "PCX", 3)==0 ||
		strncmp((filename+filenameLength-3), "pcx", 3)==0)
		return LoadPCX(filename);
	
	if(	strncmp((filename+filenameLength-3), "TGA", 3)==0 ||
		strncmp((filename+filenameLength-3), "tga", 3)==0)
		return LoadTGA(filename);

	cerr << "%s does not end in \".tga\", \".bmp\" or \"pcx\"" << filename << endl;
	return false;
}



bool Image::LoadBMP(char * filename)
{
	FILE *file;
	unsigned long i;
	unsigned short int planes;

		// make sure the file is there.
	if ((file = fopen((const char *)filename, "rb"))==NULL)
	{
		printf("File Not Found : '%s'\n",filename);
		return false;
	}

	// seek through the bmp header, up to the width/height:
	fseek(file, 18, SEEK_CUR);

	// read the width
	if ((i = fread(&width, 4, 1, file)) != 1) {
		printf("Error reading width from %s.\n", filename);
		return false;
	}
	//printf("Width of %s: %lu\n", filename, image->sizeX);

	// read the height
	if ((i = fread(&height, 4, 1, file)) != 1) {
		printf("Error reading height from %s.\n", filename);
		return false;
	}
	//printf("Height of %s: %lu\n", filename, image->sizeY);

	// calculate the size (assuming 24 bits or 3 bytes per pixel).
	//size = imageWidth * imageHeight * 3;

	// read the planes
	if ((fread(&planes, 2, 1, file)) != 1) {
		printf("Error reading planes from %s.\n", filename);
		return false;
	}
	if (planes != 1) {
		printf("Planes from %s is not 1: %u\n", filename, planes);
		return false;
	}

	// read the bpp
	if ((i = fread(&bpp, 2, 1, file)) != 1) {
		printf("Error reading bpp from %s.\n", filename);
		return false;
	}
	if (bpp == 24)
		return Load24BitBMP(filename);
	if (bpp == 8)
		return Load8BitBMP(filename);
	else
	  	cerr << filename << " has an unknown bpp '"<< bpp << "'." << endl;

	// we're done.
	return false;

}


	

//Load24BitBMP - load a 24 bit bitmap file
bool Image::Load24BitBMP(char * filename)
{
	format = GL_RGB;

	FILE *file;
	unsigned long size;                 // size of the image in bytes.
	unsigned long i;                    // standard counter.
	unsigned short int planes;          // number of planes in image (must be 1)
	unsigned short int bpp;             // number of bits per pixel (must be 24)
	char temp;                          // temporary color storage for bgr-rgb conversion.

		// make sure the file is there.
	if ((file = fopen((const char *)filename, "rb"))==NULL)
	{
		printf("File Not Found : '%s'\n",filename);
		return false;
	}

	// seek through the bmp header, up to the width/height:
	fseek(file, 18, SEEK_CUR);

	// read the width
	if ((i = fread(&width, 4, 1, file)) != 1) {
		printf("Error reading width from %s.\n", filename);
		return false;
	}
	//printf("Width of %s: %lu\n", filename, image->sizeX);

	// read the height
	if ((i = fread(&height, 4, 1, file)) != 1) {
		printf("Error reading height from %s.\n", filename);
		return false;
	}
	//printf("Height of %s: %lu\n", filename, image->sizeY);

	// calculate the size (assuming 24 bits or 3 bytes per pixel).
	size = width * height * 3;

	// read the planes
	if ((fread(&planes, 2, 1, file)) != 1) {
		printf("Error reading planes from %s.\n", filename);
		return false;
	}
	if (planes != 1) {
		printf("Planes from %s is not 1: %u\n", filename, planes);
		return false;
	}

	// read the bpp
	if ((i = fread(&bpp, 2, 1, file)) != 1) {
		printf("Error reading bpp from %s.\n", filename);
		return false;
	}
	if (bpp != 24) {
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		return false;
	}

	// seek past the rest of the bitmap header.
	fseek(file, 24, SEEK_CUR);

	// read the data.
	data = (unsigned char *) malloc(size);
	if (data == NULL) {
		printf("Error allocating memory for color-corrected image data");
		return false;
	}

	if ((i = fread(data, size, 1, file)) != 1) {
		printf("Error reading image data from %s.\n", filename);
		return false;
	}

	for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
		temp = data[i];
		data[i] = data[i+2];
		data[i+2] = temp;
	}

	// we're done.
	return true;

}

//Load8BitBMP - load an 8 bit paletted bitmap file
bool Image::Load8BitBMP(char * filename)
{
	cerr << "Sorry, I don't yet support 8-bit bitmaps." << endl;
	return true;
}


//LoadPCX - load a .pcx texture - 256 color, paletted
bool Image::LoadPCX(char * filename)
{
	cerr << "Loading %s in LoadPCX()" << filename << endl;

	//set bpp and format
	bpp=24;
	format=GL_RGB;

	FILE * file;

	file=fopen(filename, "rb");
	if(!file)
	{
		cerr << "Unable to open %s" << endl;
		return false;
	}

	//retrieve header, first 4 bytes, first 2 should be 0x0A0C
	unsigned short header[2];
	fread(header, 4, 1, file);

	if(header[0]!=0x050A)
	{
		cerr << "%s is not a legal .PCX file" <<  filename << endl;
		fclose(file);
		return false;
	}

	//retrieve minimum x value
	int xMin=fgetc(file);		//loword
	xMin |= fgetc(file) << 8;	//hiword

	//retrieve minimum y value
	int yMin=fgetc(file);		//loword
	yMin |= fgetc(file) << 8;	//hiword

	//retrieve maximum x value
	int xMax=fgetc(file);		//loword
	xMax |= fgetc(file) << 8;	//hiword

	//retrieve maximum y value
	int yMax=fgetc(file);		//loword
	yMax |= fgetc(file) << 8;	//hiword

	//calculate width and height
	width = xMax-xMin+1;
	height= yMax-yMin+1;

	//allocate memory for pixel data (paletted)
	unsigned char * pixelData=new unsigned char[width*height];
	if(!pixelData)
	{
		cerr << "Unable to allocate %d bytes for the image data of %s" <<
								width*height <<  filename << endl;
		fclose(file);
		return false;
	}

	//set file pointer to beginning of image data
	fseek(file, 128, SEEK_SET);

	//decode and store the pixel data
	unsigned int index=0;

	while(index<(width*height))
	{
		int c = getc(file);

		if(c>0xBF)
		{
			int numRepeat = 0x3F & c;
			c=getc(file);

			for(int i=0; i<numRepeat; i++)
				pixelData[index++] = c;
		}
		else
			pixelData[index++] = c;

		fflush(stdout);
	}

	//allocate memory for the image palette
	unsigned char * paletteData = new unsigned char[768];

	//the palette is the last 769 bytes of the file
	fseek(file, -769, SEEK_END);

	//retrieve first character, should be equal to 12
	int c=getc(file);
	if(c!=12)
	{
		cerr << "%s is not a legal .PCX file - the palette data has an illegal header, %d"
								<< filename <<  c << endl;
		fclose(file);
		return false;
	}

	//read and store the palette
	fread(paletteData, 1, 768, file);
	
	//close the file
	fclose(file);

	//allocate memory for the "unpaletted" data
	data = new unsigned char[width*height*3];
	if(!data)
	{
		cerr << "Unable to allocate memory for the expanded data of %s" << filename << endl;
		return false;
	}

	//calculate the "unpaletted" data - "flipping" the texture top-bottom
	for(unsigned int j=0; j<height; j++)
	{
		for(unsigned int i=0; i<width; i++)
		{
			data[3*(j*width+i)]		= (unsigned char) paletteData[3*pixelData[(height-1-j)*width+i]];
			data[3*(j*width+i)+1]	= (unsigned char) paletteData[3*pixelData[(height-1-j)*width+i]+1];
			data[3*(j*width+i)+2]	= (unsigned char) paletteData[3*pixelData[(height-1-j)*width+i]+2];
		}
	}

	cerr << "Loaded %s correctly." << filename << endl;
	return true;
}





//Load a TGA texture
bool Image::LoadTGA(char * filename)
{
	unsigned char	UncompressedTGAHeader[12]={0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char	CompressedTGAHeader[12]={0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char	Uncompressed8BitTGAHeader[12]={0, 1, 1, 0, 0, 0, 1, 24, 0, 0, 0, 0};

	unsigned char	TGAcompare[12];						//Used to compare TGA header
	
	FILE * file = fopen(filename, "rb");		//Open the TGA file
	
	if(	file==NULL )							//Does the file exist?
	{
		cerr << "Couldn't find the TGA file '" << filename << "'" << endl;
		return false;
	}
	
	//read the header
	fread(TGAcompare, 1, sizeof(TGAcompare), file);
	fclose(file);

	if(memcmp(UncompressedTGAHeader, TGAcompare, sizeof(UncompressedTGAHeader))==0)
	{
		return LoadUncompressedTrueColorTGA(filename);
	}
	else if(memcmp(CompressedTGAHeader, TGAcompare, sizeof(CompressedTGAHeader))==0)
	{
		return LoadCompressedTrueColorTGA(filename);
	}
	else if(memcmp(Uncompressed8BitTGAHeader, TGAcompare, sizeof(Uncompressed8BitTGAHeader))==0)
	{
		return LoadUncompressed8BitTGA(filename);
	}
	else
	{
		cerr << filename << " is not a valid TGA file." << endl;
		return false;
	}
	
	return false;
}

//load an 8 bit uncompressed paletted TGA
bool Image::LoadUncompressed8BitTGA(char * filename)
{
	unsigned char		TGAHeader[12]={0, 1, 1, 0, 0, 0, 1, 24, 0, 0, 0, 0};
	unsigned char		TGAcompare[12];						//Used to compare TGA header
	unsigned char		header[6];							//First 6 useful bytes of the header

	//cerr << "Loading the uncompressed 8-bit tga file " << filename << "." << endl;

	FILE * file = fopen(filename, "rb");				//Open the TGA file
	
	if(file == NULL)								//Does the file exist?
	{
		cerr << "%s does not exist." << filename << endl;
		return false;
	}

	if(	fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)||	//Are there 12 bytes to read?
		memcmp(TGAHeader, TGAcompare, sizeof(TGAHeader))!=0	||					//Is the header correct?
		fread(header, 1, sizeof(header), file)!=sizeof(header))		//Read next 6 bytes
	{
		fclose(file);								//If anything else failed, close the file
		cerr << "Could not load the file correctly, general failure." << endl;
		return false;
	}
	
	//save data into class member variables
	width=	header[1]*256+header[0];						//determine the image width
	height=	header[3]*256+header[2];						//determine image height
	bpp=	header[4];

	if(	width<=0	||											//if width <=0
		height<=0	||											//or height<=0
		bpp!=8)													//bpp not 8
	{
		fclose(file);											//close the file
		cerr << "The image's height or width is less than zero, or the TGA is not 8 bpp." << endl;
		return false;
	}

	//set format
	format=GL_RGB;

	//make space for palette
	unsigned char * palette=new unsigned char[256*3];
	if(!palette)
	{
		cerr << "Unable to allocate memory for palette." << endl;
		return false;
	}
	
	//load the palette
	fread(palette, 256*3, 1, file);
	
	//allocate space for color indices
	unsigned char * indices=new unsigned char[width*height];
	if(!indices)
	{
		cerr << "Unable to allocate memory for indices."<< endl;
		return false;
	}
	
	//load indices
	fread(indices, 1, width*height, file);

	//close the file
	fclose(file);

	//allocate space for the image data
	data=new unsigned char[width*height*3];
	if(!data)
	{
		fclose(file);
		cerr << "Unable to allocate memory for the texture " << filename<< "." << endl;
		return false;
	}

	//calculate the color values
	for(unsigned int currentRow=0; currentRow<height; currentRow++)
	{
		for(unsigned int i=0; i<width; i++)
		{
			data[(currentRow*width+i)*3+0]=palette[indices[currentRow*width+i]*3+2];
			data[(currentRow*width+i)*3+1]=palette[indices[currentRow*width+i]*3+1];
			data[(currentRow*width+i)*3+2]=palette[indices[currentRow*width+i]*3+0];//BGR
		}
	}

//	cerr << "File loaded correctly." << endl;
	return true;
}





//load an uncompressed TGA texture (24 or 32 bpp)
bool Image::LoadUncompressedTrueColorTGA(char * filename)
{
	unsigned char	TGAheader[12]={0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};	//Uncompressed TGA header
	unsigned char	TGAcompare[12];						//Used to compare TGA header
	unsigned char	header[6];							//First 6 useful bytes of the header
	unsigned int	bytesPerPixel;						//bytes per pixel
	unsigned int	imageSize;							//Stores Image size when in RAM

	//cerr <<"Loading TGA texture " <<  filename << "." << endl;

	FILE * file = fopen(filename, "rb");				//Open the TGA file
	
	if(file == NULL)								//Does the file exist?
	{
		cerr << filename << " does not exist."  << endl;
		return false;
	}

	if(	fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)||	//Are there 12 bytes to read?
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader))!=0	||					//Is the header correct?
		fread(header, 1, sizeof(header), file)!=sizeof(header))		//Read next 6 bytes
	{
		fclose(file);								//If anything else failed, close the file
		cerr << "Could not load the file correctly, general failure."  << endl;
		return false;
	}
	
	//save data into class member variables
	width=	header[1]*256+header[0];						//determine the image width
	height=	header[3]*256+header[2];						//determine image height
	bpp=	header[4];

	if(	width<=0	||											//if width <=0
		height<=0	||											//or height<=0
		bpp!=24 && bpp!=32)										//bpp not 24 or 32
	{
		fclose(file);											//close the file
		cerr << "The texture's height or width is less than zero, or the TGA is not 24 or 32 bpp." << endl;
		return false;
	}

	//set format
	if(bpp == 24)
		format=GL_RGB;
	else
		format=GL_RGBA;

	bytesPerPixel=bpp/8;										//calc bytes per pixel
	imageSize=width*height*bytesPerPixel;						//calc memory required

	data=new unsigned char[imageSize];							//reserve the memory for the data

	if(	data==NULL)											//Does the storage memory exist?
	{
		cerr << "Unable to allocate memory for the image."  << endl;
		fclose(file);
		return false;
	}
	
	//read in the image data
	if(fread(data, 1, imageSize, file)!=imageSize)				//Does the image size match the required?
	{															//If not
		if(data)												//If data loaded
			delete [] data;										//free memory
		cerr << "Could not read the image data." << endl;
		fclose(file);											//close file
		return false;
	}

	fclose(file);

	//data is in BGR format
	//swap b and r
	for(int i=0; i<(int)imageSize; i+=bytesPerPixel)
	{	
		//repeated XOR to swap bytes 0 and 2
		data[i] ^= data[i+2] ^= data[i] ^= data[i+2];
	}
	
	//cerr << "Successfully loaded a texture from " << filename << "." << endl;
	return true;
}





//load a compressed TGA texture (24 or 32 bpp)
bool Image::LoadCompressedTrueColorTGA(char * filename)
{
	unsigned char	TGAheader[12]={0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0};	//Compressed TGA header
	unsigned char	TGAcompare[12];						//Used to compare TGA header
	unsigned char	header[6];							//First 6 useful bytes of the header
	unsigned int	bytesPerPixel;						//bytes per pixel
	unsigned int	imageSize;							//Stores Image size when in RAM
		
	//errorLog.OutputSuccess("Loading %s in LoadCompressedTGA()", filename);

	FILE * file = fopen(filename, "rb");				//Open the TGA file
	
	if(file == NULL)								//Does the file exist?
	{
		//errorLog.OutputError("%s does not exist.", filename);
		return false;
	}

	if(	fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)||	//Are there 12 bytes to read?
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader))!=0	||					//Is the header correct?
		fread(header, 1, sizeof(header), file)!=sizeof(header))		//Read next 6 bytes
	{
		fclose(file);								//If anything else failed, close the file
		//errorLog.OutputError("Could not load %s correctly, general failure.", filename);
		return false;
	}
	
	//save data into class member variables
	width=	header[1]*256+header[0];						//determine the image width
	height=	header[3]*256+header[2];						//determine image height
	bpp=	header[4];

	if(	width<=0	||											//if width <=0
		height<=0	||											//or height<=0
		bpp!=24 && bpp!=32)										//bpp not 24 or 32
	{
		fclose(file);											//close the file
		//errorLog.OutputError("%s's height or width is less than zero, or the TGA is not 24 or 32 bpp.", filename);
		return false;
	}

	//set format
	if(bpp == 24)
		format=GL_RGB;
	else
		format=GL_RGBA;

	bytesPerPixel=bpp/8;										//calc bytes per pixel
	imageSize=width*height*bytesPerPixel;						//calc memory required

	data=new unsigned char[imageSize];							//reserve the memory for the data
	if(!data)													//Does the storage memory exist?
	{
		//errorLog.OutputError("Unable to allocate memory for %s image", filename);
		fclose(file);
		return false;
	}
	
	//read in the image data
	int pixelCount	= height*width;
	int currentPixel= 0;
	int currentByte	= 0;
	unsigned char * colorBuffer=new unsigned char[bytesPerPixel];

	do
	{
		unsigned char chunkHeader=0;

		if(fread(&chunkHeader, sizeof(unsigned char), 1, file) == 0)
		{
			//errorLog.OutputError("Could not read RLE chunk header");
			if(file)
				fclose(file);
			if(data)
				delete [] data;
			return false;
		}

		if(chunkHeader<128)	//Read raw color values
		{
			chunkHeader++;

			for(short counter=0; counter<chunkHeader; counter++)
			{
				if(fread(colorBuffer, 1, bytesPerPixel, file) != bytesPerPixel)
				{
					//errorLog.OutputError("Unable to read %s image data", filename);
					
					if(file)
						fclose(file);

					if(colorBuffer)
						delete [] colorBuffer;

					if(data)
						delete [] data;

					return false;
				}

				//transfer pixel color to data (swapping r and b values)
				data[currentByte] = colorBuffer[2];
				data[currentByte+1] = colorBuffer[1];
				data[currentByte+2] = colorBuffer[0];

				if(bytesPerPixel==4)
					data[currentByte+3]=colorBuffer[3];

				currentByte+=bytesPerPixel;
				currentPixel++;

				if(currentPixel > pixelCount)
				{//
					//errorLog.OutputError("Too many pixels read");
					if(file)
						fclose(file);
					if(colorBuffer)
						delete [] colorBuffer;
					if(data)
						delete [] data;
					return false;
				}
			}
		}
		else	//chunkHeader>=128
		{
			chunkHeader-=127;

			if(fread(colorBuffer, 1, bytesPerPixel, file) != bytesPerPixel)
			{
			//	errorLog.OutputError("Unable to read %s image data", filename);
					
				if(file)
					fclose(file);
				if(colorBuffer)
					delete [] colorBuffer;
				if(data)
					delete [] data;
				return false;
			}

			for(short counter=0; counter<chunkHeader; counter++)
			{
				//transfer pixel color to data (swapping r and b values)
				data[currentByte] = colorBuffer[2];
				data[currentByte+1] = colorBuffer[1];
				data[currentByte+2] = colorBuffer[0];

				if(bytesPerPixel==4)
					data[currentByte+3]=colorBuffer[3];

				currentByte+=bytesPerPixel;
				currentPixel++;

				if(currentPixel > pixelCount)
				{
				//	errorLog.OutputError("Too many pixels read");
					if(file)
						fclose(file);
					if(colorBuffer)
						delete [] colorBuffer;
					if(data)
						delete [] data;
					return false;
				}
			}
		}
	}while(currentPixel<pixelCount);

	fclose(file);
//
//	errorLog.OutputSuccess("Loaded %s correctly.", filename);
	return true;
}













//load in an 8 bit greyscale TGA as an alpha channel
bool Image::LoadAlphaTGA(char * filename)
{
	unsigned char	TGAHeader[12]={0, 1, 1, 0, 0, 0, 1, 24, 0, 0, 0, 0};
	unsigned char	TGAcompare[12];						//Used to compare TGA header
	unsigned char	header[6];							//First 6 useful bytes of the header
		


	if(!(format==GL_RGB || format==GL_RGBA))
	{

		return false;
	}

	FILE * file = fopen(filename, "rb");				//Open the TGA file
	
	if(file == NULL)								//Does the file exist?
	{

		return false;
	}

	if(	fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)||	//Are there 12 bytes to read?
		memcmp(TGAHeader, TGAcompare, sizeof(TGAHeader))!=0	||					//Is the header correct?
		fread(header, 1, sizeof(header), file)!=sizeof(header))		//Read next 6 bytes
	{
		fclose(file);								//If anything else failed, close the file

		return false;
	}
	
	//save data into class member variables
	unsigned int alphaWidth=	header[1]*256+header[0];						//determine the image width
	unsigned int alphaHeight=	header[3]*256+header[2];						//determine image height
	int alphaBpp=				header[4];

	if(	alphaWidth<=0	||											//if width <=0
		alphaHeight<=0	||											//or height<=0
		alphaBpp!=8)												//bpp not 8
	{
		fclose(file);											//close the file

		return false;
	}

	//check it is the same size as the image
	if(alphaWidth!=width || alphaHeight!=height)
	{

		return false;
	}

	//make space for palette
	unsigned char * palette=new unsigned char[256*3];
	if(!palette)
	{

		return false;
	}
	
	//load the palette
	fread(palette, 256*3, 1, file);
	
	//we dont use the palette
	delete [] palette;
	palette=NULL;

	//allocate space for alpha values
	unsigned char * values=new unsigned char[width*height];
	if(!values)
	{

		return false;
	}
	
	//load indices
	fread(values, 1, alphaWidth*alphaHeight, file);

	//close the file
	fclose(file);

	//now put in the alpha data
	if(format==GL_RGBA)
	{
		for(unsigned int i=0; i<width*height; i++)
		{
			data[i*4+3]=values[i];
		}
	}
	else if(format==GL_RGB)
	{
		unsigned char * tempData=new unsigned char[width*height*4];
		if(!tempData)
		{

			return false;
		}

		for(unsigned int i=0; i<width*height; i++)
		{
			tempData[i*4+0]=data[i*3+0];
			tempData[i*4+1]=data[i*3+1];
			tempData[i*4+2]=data[i*3+2];
			tempData[i*4+3]=values[i];
		}

		//update member variables
		bpp=32;
		format=GL_RGBA;

		if(data)
			delete [] data;
		data=tempData;
	}


	return true;
}


void Image::FlipVertically()
{
	//dont flip zero or 1 height images
	if(height==0 || height==1)
		return;

	int rowsToSwap=0;
	//see how many rows to swap
	if(height%2==1)
		rowsToSwap=(height-1)/2;
	else
		rowsToSwap=height/2;

	//create space for a temporary row
	GLubyte * tempRow=new GLubyte[width*bpp/8];
	if(!tempRow)
	{
		//errorLog.OutputError("Unable to flip image, unable to create space for temporary row");
		return;
	}

	//loop through rows to swap
	for(int i=0; i<rowsToSwap; ++i)
	{
		//copy row i into temp
		memcpy(tempRow, &data[i*width*bpp/8], width*bpp/8);
		//copy row height-i-1 to row i
		memcpy(&data[i*width*bpp/8], &data[(height-i-1)*width*bpp/8], width*bpp/8);
		//copy temp into row height-i-1
		memcpy(&data[(height-i-1)*width*bpp/8], tempRow, width*bpp/8);
	}

	//free tempRow
	if(tempRow)
		delete [] tempRow;
	tempRow=NULL;
}
		
