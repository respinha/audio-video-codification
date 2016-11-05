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

short BitStream::readBit () {

		if(readPosition >= writePosition) return -1;

		ifstream* stream = new ifstream(readFilename->c_str(), ifstream::binary);

		short bit = 0;
		if (stream->is_open()) {

				// get length of file
				stream->seekg(readPosition/8, stream->beg);

				char byteBuffer;

				// read data as a block
				stream->read (&byteBuffer,1);

				stream->close();

				// bitwise
				bit = (byteBuffer >> (7-readPosition%8)) & 0x1;
				//short bit = (bytesBuffer[readPosition%8] >> (7-(readPosition%8))) & 0x1;
				readPosition++;



		}
		return bit;
}

short* BitStream::readNBits(short nBits) {

	short div = nBits/8;
	short rem = nBits%8;
	short bytesToRead = !rem ? div : div+1;


	ifstream* stream = new ifstream(readFilename->c_str());
	short* bitBuffer = new short[nBits];

	if (stream->is_open()) {
		// get length of file:
		stream->seekg (readPosition/8, stream->beg);

		char* bytesBuffer;		// read data as a block

		short readUntilNow = 0;

		if(readPosition%8 != 0) {
			bytesToRead++;
			bytesBuffer = new char[bytesToRead];

			stream->read (bytesBuffer,bytesToRead);
			stream->close();

			short pos = readPosition%8;
			while(pos < 8) {
				short bit = (bytesBuffer[0] >> (7-pos++)) & 0x1;

				bitBuffer[readUntilNow] = bit;			

				if(++readUntilNow == nBits) {	
					// exit
					readPosition += nBits;
					return bitBuffer;
				}
			}

		cout << "POS: " << readPosition << "\n";
	
		} 
		else {
			bytesBuffer = new char[bytesToRead];

			stream->read (bytesBuffer,bytesToRead);
			stream->close();

		}


		// bytwise reading
		// if we already started reading bits, then skip to next byte
		for(short i = (readUntilNow != 0); i < bytesToRead; i++) {
			// bitwise reading

			for(short j = 0; j < 8; j++) {
				short bit = (bytesBuffer[i] >> (7-j)) & 0x1;

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
		cout << "POS: " << readPosition << "\n";
	}

	return bitBuffer;
}

void BitStream::writeBit(short* sequence) {

	ofstream* stream = new ofstream();

	unsigned char buffer = 0;
	short bufferPos = 0;

	if(remainingByteSlots) {
		short pos = 8 - remainingByteSlots;
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

void BitStream::writeNBits(short nBits, short* sequence, short finalWrite) {


	ofstream* stream = new ofstream();
	const char* filename = writeFilename->c_str();

	char buffer = 0;
	short bufferPos = 0;

	stream->open(filename, ios::out | ios::binary | ios::app);

	short j = 0;

	stream->seekp(writePosition/8, stream->beg);
	if(surplus) {

		short pos = 8 - remainingByteSlots;

		while(pos < 8) {		

			surplusByte = surplusByte | (sequence[j] << (7-pos));

			j++;
			pos++;

			remainingByteSlots--;
			if(j == nBits && pos < 7) {
				return;
			}
		}
	
		stream->write(&surplusByte,sizeof(surplusByte));		
		writePosition += 8;

		surplusByte = 0;
	}

	/*if(j > 0) stream->seekp((writePosition/8)+1, stream->beg);
	else stream->seekp((writePosition/8), stream->beg);*/

	for(short i = j; i < nBits; i++) {
		short bit = sequence[i];

		buffer = buffer | (bit << (7-bufferPos));
	

		if(finalWrite && i == nBits-1) {
					}

		if(++bufferPos == 8) {
			stream->write((char*) &buffer,sizeof(buffer));
			buffer = 0;
			bufferPos = 0;
			writePosition += 8;
		}
	}

	stream->close();
	if(finalWrite) {
		surplusByte = buffer;
		remainingByteSlots = (8-bufferPos);
		flush();
		return;
	}

	if(bufferPos != 0) {
		surplusByte = buffer;
		remainingByteSlots = (8 - bufferPos);
		surplus = 1;
	} else {
		surplusByte = 0;
		remainingByteSlots = 0;
		surplus = 0;
	}


}

void BitStream::flush() {
	cout << "FINAL: ";
	for(short k = 0; k < 8; k++)
		cout << ((surplusByte >> (7-k)) & 0x1);
	cout << "\n";

	ofstream* stream = new ofstream(writeFilename->c_str(), ios::out | ios::binary | ios::app);
	stream->seekp((writePosition/8)+1, stream->beg);

	stream->write(&surplusByte, sizeof(surplusByte));
	stream->close();

	writePosition+=remainingByteSlots;
	cout << "POS: " << remainingByteSlots << "\n";
	

}


