#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
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


	ifstream* stream = new ifstream(readFilename->c_str(), ifstream::binary);
	if (stream->is_open()) {
		// get length of file:
		stream->seekg(readPosition/8, stream->beg);

		char* bytesBuffer = new char[1];

		// read data as a block
		stream->read (bytesBuffer,1);
		stream->close();
		
		// bitwise	
		int bit = (bytesBuffer[0] >> (7-readPosition%8)) & 0x1;
		//int bit = (bytesBuffer[readPosition%8] >> (7-(readPosition%8))) & 0x1;
		readPosition++;

		delete[] bytesBuffer;

		readBits.push(bit);
		return bit;

	}
	return -1;
}

void BitStream::readNBits(int nBits) {

	int div = nBits/8;
	int rem = nBits%8;
	int bytesToRead = !rem ? div : div+1;


	ifstream* stream = new ifstream(readFilename->c_str());
	int* bitBuffer = new int[nBits];

	if (stream->is_open()) {
		// get length of file:
		stream->seekg (readPosition/8, stream->beg);

		char* bytesBuffer = new char[bytesToRead];
		// read data as a block
		stream->read (bytesBuffer,bytesToRead);
		stream->close();

		// cout << "Buff: " << bytesBuffer << "\n";

		int readUntilNow = 0;

		// bytewise
		for(int i = 0; i < bytesToRead; i++) {
			// bitwise 
			cout << "Byte: " << bytesBuffer[i] << "\n";
			for(int j = 0; j < 8; j++) {
				int bit = (bytesBuffer[i] >> (7-j)) & 0x1;

				bitBuffer[readUntilNow] = bit;
				cout << bitBuffer[readUntilNow];
				cout << "\n";

				if(++readUntilNow == nBits) {	

					cout << "\nDone!\n";	
					// exit
					i = bytesToRead;
					break;

				}
			}
		}

		delete[] bytesBuffer;
		readPosition += nBits;	// update bit position on file

		for(int i = 0; i < nBits; i++) 
			readBits.push(bitBuffer[i]);
			
		delete[] bitBuffer;
	}

	//return bitBuffer;
}

void BitStream::writeBit(int* sequence) {

	ofstream* stream = new ofstream();
	
	unsigned char buffer = 0;
	int bufferPos = 0;

	if(writePosition > 0 && writePosition%8 !=0) {

		stream->open(writeFilename->c_str(), ios::out | ios::binary | ios::app);
		ifstream* is = new ifstream(writeFilename->c_str());

		if(is->is_open()) {
			is->seekg(writePosition/8, stream->beg);

			int j = writePosition%8-1;
			is->read((char*) &buffer, 1);

			buffer = buffer | (sequence[0] << (7-j));
			writePosition++;

			stream->write((char*) &buffer,sizeof(buffer));
			is->close();
		}

		stream->close();
		return;			
	}

	stream->open(writeFilename->c_str(), ios::out | ios::binary | ios::app);
	stream->seekp(writePosition/8, stream->beg);

	int bit = sequence[0];
	buffer = buffer | (bit << (7-writePosition%8));
	stream->write((char*) &buffer,sizeof(buffer));

	writePosition++;
	stream->close();

}
void BitStream::writeNBits(int nBits, int* sequence) {

	ofstream* stream = new ofstream();
	const char* filename = writeFilename->c_str();
	
	char buffer = 0;
	int bufferPos = 0;

	stream->open(filename, ios::out | ios::binary | ios::app);
	stream->seekp(writePosition/8, stream->beg);

	int j = 0;
	if(surplusByte != 0) {
		cout << "remainder: " << remainingByteSlots << "\n";
		int pos = 8 - remainingByteSlots;

		while(pos < 8) {
			cout << "Current sequence pos is: " << j << " and current shifting pos is " << pos << "\n";
			surplusByte = surplusByte | (sequence[j] << (7-pos));
			
			cout << "Excess byte: " << surplusByte << "\n";

			j++;
			pos++;
		}
		
		stream->write(&surplusByte,sizeof(surplusByte));
	}

	for(int i = j; i < nBits; i++) {
		int bit = sequence[i];
		cout << bit;

		buffer = buffer | (bit << (7-bufferPos));

		if(++bufferPos == 8) {
			stream->write((char*) &buffer,sizeof(buffer));
			buffer = 0;
			bufferPos = 0;
		}
		writePosition++;
	}

	if(bufferPos != 0) {//stream->write((char*) &buffer, sizeof(buffer));
		surplusByte = buffer;
		writePosition -= bufferPos;
		remainingByteSlots = 8 - bufferPos;
	} else {
		surplusByte = 0;
		remainingByteSlots = 0;
	}

	stream->close();
}
