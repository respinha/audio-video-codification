#include <string>
#include <iostream>
#include <fstream>
#include <stringstream>

BitStream::BitStream(string filename) {
	// read file	
	readPosition = 0;
	writePosition = 0;
	*filename = new string(filename);
}

char BitStream::readBit () {

	ifstream* stream = new ifstream(filename->c_str(), ifstream::binary);
	if (stream.is_open()) {
		// get length of file:
		stream.seekg(readPosition, stream.beg);

		char* bytesBuffer = new char[1];

		// read data as a block
		stream->read (bytesBuffer,1);
		stream->close();

		// cout << "Buff: " << bytesBuffer << "\n";

		int readUntilNow = 0;
		// bytewise
		for(int j = 0; j < 8; j++) {
			// bitwise	
			int bit = (bytesBuffer[0] >> (7-j)) & 0x1;

			bitBuffer[readUntilNow] = bit;
			cout << bitBuffer[readUntilNow];

			if(++readUntilNow == nBits) {	
				cout << "\nDone!\n";	
				// exit
				break;

			}
		}

		delete[] bytesBuffer;
		readPosition += 1;	// update bit position on file
		//delete[] bitBuffer;

	}
	return (word >> 7) & 0x1;
}

char BitStream::readNBits(int nBits) {

	int div = nBits/8;
	int rem = nBits%8;
	int bytesToRead = !rem ? div : div+1;

	ifstream* stream = new ifstream(filename->c_str());
	int* bitBuffer = new int[nBits];

	if (stream->is_open()) {
		// get length of file:
		stream->seekg (readPosition, stream->beg);

		char* bytesBuffer = new char[bytesToRead];
		// read data as a block
		stream->read (bytesBuffer,bytesToRead);
		stream->close();

		// cout << "Buff: " << bytesBuffer << "\n";

		int readUntilNow = 0;
		for(int i = 0; i < bytesToRead; i++) {
			// bytewise
			for(int j = 0; j < 8; j++) {
				// bitwise	
				int bit = (bytesBuffer[i] >> (7-j)) & 0x1;

				bitBuffer[readUntilNow] = bit;
				cout << bitBuffer[readUntilNow];

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
		//delete[] bitBuffer;

	}
}
/*
   char[] BitStream::readNBits (char word, int n) {
   char[] bits = new char[n];
   for(int i = 0; i < n; i++) {
// 
std::stringstream ss;
ss<< hex << (i+1); 
word[i] = (word >> (7-i)) & (ss >> i);
}
return bits;
}*/
