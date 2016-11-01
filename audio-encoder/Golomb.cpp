#include <stdlib>
#include <string>
#include <cmath>
#include <bitset>

#include "Golomb.h"

Golomb::Golomb(int m, string encodedFilename, string decodedFilename) {
	M = m;
	B = math.log2(m);
	stream = new BitStream(encodedFilename, decodedFilename);
}


void Golomb::encode(int n, string filename) {

	int q = n/m;
	int r = n -(q*m);
	
	int i = 0;
	int* code = new int[q+1+B];

	// unary
	while(i < q-1) 
		code[i++] = 1;

	code[q] = 0;

	// get remainder in binary value
	string binaryRem = bitset<B>(r).to_string();
	for(i = 0; i < B; i++) 
		code[q+i] = atoi(binaryRem[i]);

	stream->writeNBits(q+1+B, code);	
}

void Golomb::decode(int n, string filename) {
}

