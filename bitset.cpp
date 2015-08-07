//////////////////////////////////////////////////////////////////////////////////////////
//	Bitset.cpp
//	functions for class for set of bits to represent many true/falses
//	You may use this code however you wish, but if you do, please credit me and
//	provide a link to my website in a readme file or similar
//	Downloaded from: www.paulsprojects.net
//	Created:	8th August 2002
//////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "bitset.h"
#include <cstring>

using namespace std;

bool Bitset::Init(int numberOfBits)
{
	//Delete any memory allocated to bits
	if(bits)
		delete [] bits;
	bits=NULL;

	//Calculate size
	numBytes=(numberOfBits>>3)+1;

	//Create memory
	bits=new unsigned char[numBytes];
	if(!bits)
	{
		cerr << "Unable to allocate space for a Bitset of " << numberOfBits << " bits." << endl;
		return false;
	}

	ClearAll();

	return true;
}

void Bitset::ClearAll()
{
	memset(bits, 0, numBytes);
}

void Bitset::SetAll()
{
	memset(bits, 0xFF, numBytes);
}

void Bitset::Clear(int bitNumber)
{
	bits[bitNumber>>3] &= ~(1<<(bitNumber & 7));
}

void Bitset::Set(int bitNumber)
{
	bits[bitNumber>>3] |= 1<<(bitNumber&7);
}

unsigned char Bitset::IsSet(int bitNumber)
{
	return bits[bitNumber>>3] & 1<<(bitNumber&7);
}


