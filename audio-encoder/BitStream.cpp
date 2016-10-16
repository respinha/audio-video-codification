#include <string>
#include <iostream>
#include <fstream>
#include <stringstream>

BitStream::BitStream(string filename) {
// read file
}

byte BitStream::readBit (byte word) (
	return (word >> 7) & 0x1;
)

byte[] BitStream::readNBits (byte word, int n) (
	byte[] bits = new byte[n];
	for(int i = 0; i < n; i++) {
		// 
		std::stringstream ss;
		ss<< hex << (i+1); 
		byte[i] = (word >> (7-i)) & (ss >> i);
	}
	return bits;
}
