#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "BitStream.cpp"

using namespace std;

int main(int argc, char** argv) {

/*	int nBits = 8;
	char bits[] = {0,1,1,0,1,1,1,1};

	ofstream* stream = new ofstream();
	stream->open(argv[1], ios::out | ios::binary);

	int nBytes = (!nBits%8) ? nBits/8 : (nBits/8)+1;

	stream->seekp(0, stream->beg);

	char buffer = 0;
	int bufferPos = 0;
	for(int i = 0; i < nBits; i++) {
		buffer = buffer | (bits[i] << (7-bufferPos));
		cout <<  bufferPos << "\n";
		if(++bufferPos == 8 || i == nBits-1) {
			cout << buffer << "\n";
			stream->write((char*) &buffer,sizeof(buffer));
			buffer = 0;
			bufferPos = 0;
		}

	}
	
	stream->close();*/

	BitStream* bs = new BitStream(argv[1], argv[2]);
/*	bs->readNBits(atoi(argv[3]));
	bs->writeNBits(atoi(argv[3]));

	bs->readNBits(atoi(argv[3]));*/

	// 'b' 'o': 0110 0010 0110 1111
	int arr [16] = { 0, 1, 1, 0, 0, 0, 1, 0, 0,1,1,0,1,1,1,1};
	bs->writeNBits(13, arr);	

	int arr1[3] = { 1,1,1 };
	bs->writeNBits(3, arr1);
	
	return 0;
}
