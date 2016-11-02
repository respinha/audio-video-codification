#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "BitStream.cpp"

using namespace std;

int main(int argc, char** argv) {

	BitStream* bs = new BitStream(argv[1], argv[1]);
	//int nBits = atoi(argv[2]);

	int arr [16] = { 0, 1, 1, 0, 0, 0, 1, 0, 0,1,1,0,1,1,1,1};
	bs->writeNBits(16, arr, 1);

	while(1) {
		cout << "N bits: ";
		char c;
		cin >>  c;
		if(c == 'b') {
			int * bitos = bs->readNBits(8);
			for(int i = 0; i < 8; i++) 
				cout << "\n" << bitos[i] << "\n";
		}
		else 
			break;
	} 

	return 0;
}

