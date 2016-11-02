#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <cstdlib>

#include "BitStream.h"

BitStream::BitStream(string readname, string writename) {

	// read file	
	readPosition = 0;
	writePosition = 0;
	readFilename = new string(readname);	
	writeFilename = new string(writename);
	surplusByte = 0;
	remainingByteSlots = 0;
}

int BitStream::readBit () {

	if(readPosition == writePosition) return -1;

	ifstream* stream = new ifstream(readFilename->c_str(), ifstream::binary);
	
	int bit = 0;
	if (stream->is_open()) {
	
		cout << "POS: " << readPosition << "\n";
		// get length of file
		stream->seekg(readPosition/8, stream->beg);

		char* bytesBuffer = new char[1];

		// read data as a block
		stream->read (bytesBuffer,1);
		cout << "Byte: " << bytesBuffer[0] << "& " << 7-readPosition%8 << "\n";

		stream->close();
	
		// bitwise
		int bit = (bytesBuffer[0] >> (7-readPosition%8)) & 0x1;
		//int bit = (bytesBuffer[readPosition%8] >> (7-(readPosition%8))) & 0x1;
		readPosition++;

		delete[] bytesBuffer;


	}
	return bit;
}

int* BitStream::readNBits(int nBits) {

	int div = nBits/8;
	int rem = nBits%8;
	int bytesToRead = !rem ? div : div+1;


	ifstream* stream = new ifstream(readFilename->c_str());
	int* bitBuffer = new int[nBits];

	if (stream->is_open()) {
		// get length of file:
		stream->seekg (readPosition/8, stream->beg);

		char* bytesBuffer;		// read data as a block
	
		int readUntilNow = 0;
		
		if(readPosition%8 != 0) {
			bytesToRead++;
			bytesBuffer = new char[bytesToRead];

			stream->read (bytesBuffer,bytesToRead);
			stream->close();

			int pos = readPosition%8;
			while(pos < 8) {
	
				int bit = (bytesBuffer[0] >> (7-pos++)) & 0x1;

				bitBuffer[readUntilNow] = bit;			

				if(++readUntilNow == nBits) {	
					// exit
					readPosition += nBits;
					return bitBuffer;
				}
			}	
		} else {
			bytesBuffer = new char[bytesToRead];

			stream->read (bytesBuffer,bytesToRead);
			stream->close();

		}
		

		// bytwise reading
		// if we already started reading bits, then skip to next byte
		for(int i = (readUntilNow != 0); i < bytesToRead; i++) {
			// bitwise reading

			for(int j = 0; j < 8; j++) {
				int bit = (bytesBuffer[i] >> (7-j)) & 0x1;

				bitBuffer[readUntilNow] = bit;

				if(++readUntilNow == nBits) {	

					// exit
					i = bytesToRead;
					break;

				}
			}
		}

		delete[] bytesBuffer;
		readPosition += nBits;	// update bit position on file
	}

	return bitBuffer;
}

void BitStream::writeBit(int* sequence) {

	ofstream* stream = new ofstream();

	unsigned char buffer = 0;
	int bufferPos = 0;

	if(remainingByteSlots) {
		int pos = 8 - remainingByteSlots;
		surplusByte = surplusByte | (sequence[0] << (7-pos));
		remainingByteSlots--;

		if(!remainingByteSlots) {
			stream->open(writeFilename->c_str(), ios::out | ios::binary | ios::app);
			stream->seekp(writePosition/8, stream->beg);

			stream->write(&surplusByte, sizeof(surplusByte));

			stream->close();

			surplusByte = 0;
		}
	} else {
		surplusByte = surplusByte | (sequence[0] << 7);
		remainingByteSlots = 7;
	}

}

void BitStream::writeNBits(int nBits, int* sequence, int finalWrite) {

	for(int s = 0; s < nBits; s++) cout << "Sequence: " << sequence[s] << "\n";	

	ofstream* stream = new ofstream();
	const char* filename = writeFilename->c_str();

	char buffer = 0;
	int bufferPos = 0;

	stream->open(filename, ios::out | ios::binary | ios::app);

	int j = 0;
	

	if(surplusByte != 0) {
		int pos = 8 - remainingByteSlots;

		stream->seekp(writePosition/8, stream->beg);
		while(pos < 8) {			
			surplusByte = surplusByte | (sequence[j] << (7-pos));

			j++;
			pos++;
			writePosition++;
		}

		stream->write(&surplusByte,sizeof(surplusByte));
	}

	if(j > 0) stream->seekp(writePosition/8, stream->beg);

	for(int i = j; i < nBits; i++) {
		int bit = sequence[i];

	
		buffer = buffer | (bit << (7-bufferPos));

		if(++bufferPos == 8) {
			stream->write((char*) &buffer,sizeof(buffer));
			buffer = 0;
			bufferPos = 0;
		}
		writePosition++;
	}

	if(bufferPos != 0) {
		cout << "Buffer Pos: " << bufferPos << "\n";
		surplusByte = buffer;
		remainingByteSlots = (8 - bufferPos)+1;
	} else {
		surplusByte = 0;
		remainingByteSlots = 0;
	}

	stream->close();

	if(finalWrite) writeRemainingBits();
}

void BitStream::writeRemainingBits() {

	if(remainingByteSlots == 0) return;

	surplusByte = surplusByte << remainingByteSlots;

	ofstream* stream = new ofstream(writeFilename->c_str(), ios::out | ios::binary | ios::app);
	stream->seekp(writePosition/8, stream->beg);

	stream->write(&surplusByte, sizeof(surplusByte));

	stream->close();
}


