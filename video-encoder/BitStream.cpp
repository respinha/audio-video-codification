#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <cstdlib>

#include "BitStream.h"

BitStream::BitStream(string fname, int read) {

	// read file	
	readPosition = 0;
	filename = new string(fname);
	surplusByte = 0;
	remainingByteSlots = 0;
	surplus = 0;

	if(read != 0) {
		inputstream = new ifstream();
		inputstream->open(filename->c_str(), ios::in | ios::binary);		

		char last = 'a';
		string num = "";
		int i = -1;

		while(true) {
			inputstream->seekg(i, ios::end);
			*inputstream >> last;
			if(last == ';') break;
			num = last + num;
			i--;
		}

		writePosition = atoi(num.c_str());
		cout << "Write position: " << writePosition << "\n";

		inputstream->seekg(0);
	} else {
		outputstream = new ofstream();
		outputstream->open(filename->c_str(), ios::out | ios::app | ios::binary);

		writePosition = 0;
	} 
}

int BitStream::readBit () {

	if(readPosition >= writePosition){
	//if(readingBytes == writtenBytes) {

		inputstream->close();
		return -1;
	}
	
	int bit = 0;

	// get length of file
	inputstream->seekg(readPosition/8, inputstream->beg);

	char byteBuffer;

	// read data as a block
	inputstream->read (&byteBuffer,1);

	bit = (byteBuffer >> (7-readPosition%8)) & 0x1;
	readPosition++;	

	return bit;
}

int BitStream::readNBits(int nBits) {

	int div = nBits/8;
	int rem = nBits%8;
	int bytesToRead = !rem ? div : div+1;

	char buffer = 0;
	int word = 0;

	// get length of file:
	inputstream->seekg (readPosition/8, inputstream->beg);

	int readUntilNow = 0;

	if(readPosition%8 != 0) {
		bytesToRead++;

		inputstream->read(&buffer, 1);

		int pos = readPosition%8;
		while(pos < 8) {
			int bit = (buffer >> (7-pos++)) & 0x1;

			readUntilNow++;
			word = word | (bit << (nBits-readUntilNow));

			if(readUntilNow == nBits) {	
				// exit
				readingBytes++;
				readPosition += nBits;

				return word;
			}
		}

	} 

	// bytwise reading
	// if we already started reading bits, then skip to next byte
	int i = (readUntilNow != 0);
	while(i < bytesToRead) {
		// bitwise reading
		inputstream->read(&buffer, 1);

		for(int j = 0; j < 8; j++) {
			int bit = (buffer >> (7-j)) & 0x1;

			readUntilNow++;
			word = word | (bit << (nBits-readUntilNow));

			if(readUntilNow == nBits) {	


				i = bytesToRead;
				break;
			}
		}

		i++;
	}

	readingBytes++;
	readPosition += nBits;	// update bit position on file

	return word;
}

void BitStream::writeBit(int bit) {


	if(remainingByteSlots) {
		int pos = 8 - remainingByteSlots;

		surplusByte = surplusByte | (bit << (7-pos));
		surplus = 1;
		remainingByteSlots--;

		if(!remainingByteSlots) {
			outputstream->write(&surplusByte, sizeof(surplusByte));

			writePosition += 8;

			surplusByte = 0;
			surplus = 0;
		}
	} else {
		surplusByte = surplusByte | (bit << 7);
		remainingByteSlots = 7;
		surplus = 1;
	}

}

void BitStream::writeNBits(int nBits, int bit_buff, int finalWrite) {


	// tmp char to be filled with bits
	char buffer = 0;
	// tmp char bitwise position
	int bufferPos = 0;
	int j = 0;

	// flag to tell if writing surplus byte should be aborted
	int abort = 0;
	
	if(surplus) {
			// position in surplus byte of file
		int pos = 8 - remainingByteSlots;

		while(pos < 8) {	
			int bit = (bit_buff >> (nBits-(j+1))) & 0x1;
			surplusByte = surplusByte | (bit << (7-pos));
		

			remainingByteSlots--;
			if(j == nBits-1 && pos < 7) {

				j = nBits;
				abort = 1;
				break;
			}
			
			j++;
			pos++;
		}

		if(!abort) {

			outputstream->write(&surplusByte,sizeof(surplusByte));					
			writePosition += 8;

			surplusByte = 0;
		}
	}

	for(int i = j; i < nBits; i++) {
		int bit = (bit_buff >> ((nBits-(i+1)))) & 0x1;

		buffer = buffer | (bit << (7-bufferPos));
			
		if(++bufferPos == 8) {
			outputstream->write(&buffer,sizeof(buffer));

			buffer = 0;
			bufferPos = 0;
			writePosition += 8;

		}
	}

	if(finalWrite) {

		if(!abort) {
			surplusByte = buffer;

			remainingByteSlots = bufferPos;
		} else remainingByteSlots = (8-remainingByteSlots);

		flush();

		return;
	}

	if(bufferPos != 0 || remainingByteSlots != 0) {
		if(!remainingByteSlots) 
			surplusByte = buffer;
		if(bufferPos != 0)
			remainingByteSlots = (8 - bufferPos);

		surplus = 1;
	} else {
		surplusByte = 0;
		remainingByteSlots = 0;
		surplus = 0;
	}

}

BitStream::~BitStream() {
	//stream->close();
	delete filename;
}

void BitStream::flush() {

	outputstream->write(&surplusByte, sizeof(surplusByte));

	writePosition+=remainingByteSlots;

	*outputstream << ";";
	*outputstream << writePosition;

	cout << "Write position: " << writePosition << "\n";

	outputstream->flush();
	outputstream->close();
}
/*
int BitStream::getFilePosition() {
	return writePosition;
}*/