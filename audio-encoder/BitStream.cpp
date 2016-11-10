#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "BitStream.h"

BitStream::BitStream(string fname, int pos) {

	// read file	
	readPosition = 0;
	writePosition = pos;
	filename = new string(fname);
	surplusByte = 0;
	remainingByteSlots = 0;
	surplus = 0;

	threshold = 10000;
	bufferCount = 0;
	stream = new fstream(); 

	if(pos != 0) {
		stream->open(filename->c_str(), ios::out | ios::binary | ios::app);
	} else {
		stream->open(filename->c_str(), ios::in | ios::binary);
		readBuffer = new char[threshold];
	}
}

int BitStream::readBit () {

		// cout << "POS: " << readPosition << " " << writePosition << "\n";
		if(readPosition >= writePosition){
			//cout << "FINISH: " << readPosition << "; " << writePosition << "\n";
			delete[] readBuffer;
			return -1;
		}
		
		// ifstream* stream = new ifstream(filename->c_str(), ifstream::binary);
		
		int bit = 0;
		//if (stream->is_open()) {

				// get length of file

				char charBuffer;

				// load bytes from file
				if(bufferCount == 0 || bufferCount >= threshold) {

					if(bufferCount) delete[] readBuffer;

					stream->seekg(readPosition/8, stream->beg);
					stream->read(readBuffer,threshold);
				}

				charBuffer = readBuffer[bufferCount++];
				// stream->close();

				// bitwise
				bit = (charBuffer >> (7-readPosition%8)) & 0x1;
				readPosition++;	

		// }

		//cout << bit;	
		return bit;
}

int BitStream::readNBits(int nBits) {

	// cout << "POS: " << readPosition << "\n";

	int div = nBits/8;
	int rem = nBits%8;
	int bytesToRead = !rem ? div : div+1;

	// ifstream* stream = new ifstream(filename->c_str());
	// int* bitBuffer = new int[nBits];

	char buffer = 0;
	int word = 0;

	// if (stream->is_open()) {
		// stream->seekg (readPosition/8, stream->beg);

		int readUntilNow = 0;

		if(readPosition%8 != 0) {
			bytesToRead++;

			// stream->read(&buffer, 1);
			// stream->close();
			// load bytes from file
			checkReadBuffer();
			buffer = readBuffer[bufferCount++];

			int pos = readPosition%8;
			while(pos < 8) {
				int bit = (buffer >> (7-pos++)) & 0x1;

				readUntilNow++;
				word = word | (bit << (nBits-readUntilNow));
				//cout << "updating word: " << word << "\n";
				// bitBuffer[readUntilNow] = bit;			

				if(readUntilNow == nBits) {	
					// exit
					readPosition += nBits;

					// stream->close();
					// delete bytesBuffer;
					return word;
				}
			}

		} 

		// bytwise reading
		// if we already started reading bits, then skip to next byte
		int i = (readUntilNow != 0);
		while(i < bytesToRead) {
			// bitwise reading
			// stream->read(&buffer, 1);
			checkReadBuffer();
			buffer = readBuffer[bufferCount++];

			for(int j = 0; j < 8; j++) {
				int bit = (buffer >> (7-j)) & 0x1;
				// cout << bit;

				// bitBuffer[readUntilNow] = bit;
				readUntilNow++;
				word = word | (bit << (nBits-readUntilNow));

				if(readUntilNow == nBits) {	

					// exit
					i = bytesToRead;
					break;
				}
			}

			//cout << "\n";
			i++;
		}
		// delete[] bytesBuffer;

		readPosition += nBits;	// update bit position on file

		// stream->close();
//	}

	// cout << "Just read some bits. POS: " << readPosition << "\n";
	return word;
}

void BitStream::writeBit(int bit) {

	// cout << "SOLO: Remaining bits: " << remainingByteSlots << "\n";

	if(remainingByteSlots) {
		int pos = 8 - remainingByteSlots;

		surplusByte = surplusByte | (bit << (7-pos));
		surplus = 1;
		remainingByteSlots--;

		if(!remainingByteSlots) {

			// ofstream* stream = new ofstream();
			// stream->open(filename->c_str(), ios::out | ios::binary | ios::app);		
			/*for(int k = 0; k < 8; k++) 
				cout << ((surplusByte >> (7-k)) & 0x1);
			cout << "\n";*/

			// stream->seekp(writePosition/8, stream->beg);
			// stream->write(&surplusByte, sizeof(surplusByte));
			writePosition += 8;
			putByte(surplusByte);
			// stream->close();

			// cout << "solo update write pos " << writePosition << "\n";

			surplusByte = 0;
			surplus = 0;
		}
	} else {	
		surplusByte = surplusByte | (bit << 7);
		remainingByteSlots = 7;
	}
		/*for(int k = 0; k < 8; k++) 
				cout << ((surplusByte >> (7-k)) & 0x1);
			cout << "\n";*/
}

void BitStream::writeNBits(int nBits, int bit_buff, int finalWrite) {


	// ofstream* stream = new ofstream();

	// tmp char to be filled with bits
	char buffer = 0;
	// tmp char bitwise position
	int bufferPos = 0;
	int j = 0;

	// flag to tell if writing surplus byte should be aborted
	int abort = 0;

	// stream->open(filename->c_str(), ios::out | ios::binary | ios::app);
	// stream->seekp(writePosition/8, stream->beg);

	if(surplus) {
		// position in surplus byte of file
		int pos = 8 - remainingByteSlots;


		while(pos < 8) {	
			// cout << "remaining bits... " << remainingByteSlots << " and j = " << j << "\n";
			int bit = (bit_buff >> (nBits-(j+1))) & 0x1;
			surplusByte = surplusByte | (bit << (7-pos));

			remainingByteSlots--;
			if(j == nBits-1 && pos < 7) {
				/*cout << "Abortion\n";
				for(int k = 0; k < 8; k++) 
					cout << ((surplusByte >> (7-k)) & 0x1);
				cout << "\n";
				cout << "there arre still " << remainingByteSlots << " bits\n";*/
				j = nBits;
				abort = 1;
				break;
			}
			
			j++;
			pos++;
		}
		
		
		if(!abort) {

			/*for(int k = 0; k < 8; k++) 
				cout << ((surplusByte >> (7-k)) & 0x1);
			cout << "\n";*/

			// stream->write(&surplusByte,sizeof(surplusByte));		
			writePosition += 8;
			putByte(surplusByte);

			// cout << "Update write pos " << writePosition << "\n";
			surplusByte = 0;
		}
	}

	for(int i = j; i < nBits; i++) {
		int bit = (bit_buff >> ((nBits-(i+1)))) & 0x1;

		buffer = buffer | (bit << (7-bufferPos));
			
		if(++bufferPos == 8) {
			// stream->write(&buffer,sizeof(buffer));
			writePosition += 8;
			putByte(buffer);

			/*for(int k = 0; k < 8; k++) 
				cout << ((buffer >> (7-k)) & 0x1);
			cout << "\n";*/

			buffer = 0;
			bufferPos = 0;

			// cout << "update write pos: " << writePosition << "\n";
		}
	}

	// stream->close();
	if(finalWrite) {

		// cout << "Buffer pos... " << bufferPos << "\n";
		if(!abort) {
			surplusByte = buffer;

			remainingByteSlots = bufferPos;
		} else remainingByteSlots = (8-remainingByteSlots);

		// writing surplus byte 
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

void BitStream::flush() {

	// ofstream* stream = new ofstream(filename->c_str(), ios::out | ios::binary | ios::app);
	// stream->seekp(writePosition/8, stream->beg);

	/*cout << "flush\n";
		for(int k = 0; k < 8; k++) 
				cout << ((surplusByte >> (7-k)) & 0x1);
			cout << "\n";*/

	// stream->write(&surplusByte, sizeof(surplusByte));
	stream->put(surplusByte);
	stream->flush();

	writePosition+=remainingByteSlots;
	// cout << "update write pos " << writePosition << "\n";

	stream->close();
	// stream->close();
}

int BitStream::getFilePosition() {
	return writePosition;
}

BitStream::~BitStream() {
	delete filename;
}
void BitStream::putByte(char c) {

	stream->put(c);
	if(++bufferCount == threshold) {
			stream->flush();
			bufferCount = 0;
			stream->seekp(writePosition/8, stream->beg);
	}
}

void BitStream::checkReadBuffer() {
	if(bufferCount == 0 || bufferCount >= threshold) {

		if(bufferCount) delete[] readBuffer;
		
		if(stream->is_open()) {
			stream->seekg(readPosition/8, stream->beg);
			stream->read(readBuffer,threshold);	
		}
	}
}
