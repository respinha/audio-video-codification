#include "BitStream.h"
#include <iostream>

/* 
 *@class BitStream to read and write bits on a file.
 *@author Antonio J. R. Neves (an@ua.pt)
 */
 
BitStream::BitStream(std::string fileName, std::string m)
{
	if(m == "r")
	{
		mode = 0;
		fp.open(fileName.c_str(), std::fstream::in |  std::fstream::binary);
	}
	else
	{
		mode = 1;
		fp.open(fileName.c_str(), std::fstream::out | std::fstream::binary);
	}

	if(!fp)
		std::cerr << "ERROR " << std::endl;

	pos = 0;
	buffer = 0;
	count=0;
}


BitStream::BitStream()
{
	pos = 0;
	buffer = 0;
	count=0;
}

BitStream::~BitStream()
{
	
}

void BitStream::close()
{
	if(mode == 1)
	{
		fp.write(reinterpret_cast<char *>(&buffer), sizeof(char));
	}
	fp.close();
}

void BitStream::writeBit(int value)
{
	if(pos == 0)
		pos = 1;

	if(pos == 256)
	{
		fp.write(reinterpret_cast<char *>(&buffer), sizeof(char));
		pos = 1;
		buffer = 0;
		count++;
	}

	buffer = buffer | (value * pos);
	pos = pos << 1;
}

int BitStream::readBit()
{
	if(pos == 256 || pos == 0)
	{
		fp.read(reinterpret_cast<char *>(&buffer), sizeof(char));
		if(!fp)
		{
			return -1;
		}	
		pos = 1;
	}
	int tmp = (buffer & pos) != 0 ? 1 : 0;
	pos = pos << 1;
	return tmp;
}

void BitStream::writeNBits(int value, int nb)
{ 
	while(nb--)
		writeBit((value >> nb) & 1);
}

int BitStream::readNBits(int nb)
{
	int bits = 0;
	while(nb--)
	{
		int bit = readBit();
		if(bit == -1)
			return -1;

		bits = (bits << 1) | bit;
	}

	return bits;
}
