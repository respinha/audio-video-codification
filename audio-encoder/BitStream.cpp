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

void BitStream::writeNBits(int nBits) {

	cout << "N Bits: " << nBits << "\n";
	cout << "File: " << *writeFilename << "\n";	

	ofstream* stream = new ofstream();
	stream->open(writeFilename->c_str(), ios::out | ios::binary | ios::app);
	stream->seekp(writePosition/8, stream->beg);

	char buffer = 0;
	int bufferPos = 0;

	int j = 0;
	if(writePosition > 0 && writePosition%8 !=0) {
		cout << "diff\n";			
		int lastByteWrittenPosition = writePosition%8;
		int pos = lastByteWrittenPosition;
		
		while(pos < 8) {
			buffer = buffer | (readBits.front() << (7-bufferPos));
			readBits.pop();
			j++;
			pos++;
		}

		stream->write((char*) &buffer,sizeof(buffer));
	
	} //else stream->seekp((writePosition/8)+1);// right on next byte

	for(int i = j; i < nBits; i++) {
		int bit = readBits.front();
		cout << bit;
		if(readBits.empty()) {
			cout << "Number of bits to write exceeded buffer's size" << "\n";
			break;
		}

		buffer = buffer | (bit << (7-bufferPos));		
		readBits.pop();

		if(++bufferPos == 8) {
			cout << buffer << "\n";
			stream->write((char*) &buffer,sizeof(buffer));
			buffer = 0;
			bufferPos = 0;
		}

	}

	if(bufferPos != 0) stream->write((char*) &buffer, sizeof(buffer));

	stream->close();
}
