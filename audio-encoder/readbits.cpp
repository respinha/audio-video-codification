#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "BitStream.cpp"

using namespace std;

int main(int argc, char** argv) {

//	BitStream* bs = new BitStream(argv[1], argv[1]);
	//int nBits = atoi(argv[2]);
	BitStream* bs = new BitStream("encoded", "encoded");
	while(1) {
		cout << "N bits: ";
		char c;
		cin >>  c;
		if(c == 'b') cout << bs->readBit() << "\n";
		else break;	
	}
//	int arr [16] = { 0, 1, 1, 0, 0, 0, 1, 0, 0,1,1,0,1,1,1,1};


	return 0;
}

