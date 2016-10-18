#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv) {

	if(argc < 3) {
		cout << "Wrong number of arguments!";
		return -1;
	}
	
	int nBits = atoi(argv[1]);
	int div = nBits/8;
	int rem = nBits%8;
	int bytesToRead = !rem ? div : div+1;

	
	ifstream* stream = new ifstream(argv[2]);
	int* bitBuffer = new int[nBits];
	if (stream->is_open()) {
		// get length of file:
		//stream->seekg (0, stream->end);
		stream->seekg (0, stream->beg);

		char* bytesBuffer = new char[bytesToRead];
		// read data as a block
		stream->read (bytesBuffer,bytesToRead);
		stream->close();

		cout << "Buff: " << bytesBuffer << "\n";
		// print content:
		//int bit = (buffer[0] >> 5) & 0x1;
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
		delete[] bitBuffer;
		
	}
	
	return 0;
}

