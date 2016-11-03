#include <cstdlib>
#include <string>
#include <cmath>
#include <bitset>
#include <sstream>
#include <queue>
#include <list>

#include "Golomb.h"


Golomb::Golomb(int m, string encodedFilename, string decodedFilename) : M(m), B(log2(m)){
	stream = new BitStream(encodedFilename, decodedFilename);
}


void Golomb::encode(int n, int finalWrite) {

	int q = n/M;
	int r = n -(q*M);

	cout << "N = " << n << "\n";
	cout << "M = " << M << "\n";
	cout << "Q = " << q << "\n";
	cout << "R = " << r << "\n";
	cout << "B = " << B << "\n";
	cout << "###########################\n";
	int i = 0;
	int* code = new int[q+1+B];

	// unary
	while(i < q) {
		code[i++] = 1;
	}

	code[q] = 0;


	// get remainder in binary value
	string binaryRem = Golomb::DecToBin(r);
	for(i = 0; i < B; i++) {
		char* value;
		*value  = binaryRem[i];
		code[q+1+i] =  atoi(value);
	}


	stream->writeNBits(q+1+B, code, finalWrite);

}

list<int> Golomb::decode() {

	int nUnary = 0;
	int isUnary = 1;
	int q = 0;

	list<int> nList;
	
	int bit = 0;
	int* binarySequence;
	
	while(1) {
		
		if(isUnary) {
			bit = stream->readBit();

			if(bit == -1) break;
			
			if(bit) q++;			
			else isUnary = bit;		
		} else {
			binarySequence = stream->readNBits(B);

			cout << "decode: ";
			int r = Golomb::BinToDec(binarySequence, B);
			cout << "r = " << r;
			int n = r + (q*M);
			
			cout << "; q = " << q << "; n = " << n << "\n";

			nList.push_back(n);	
			q = 0;
			isUnary = 1;
		}
		
	}

	for(list<int>::iterator it = nList.begin(); it !=nList.end(); it++) 
		cout << "N: " << *it << "\n";
	return nQueue;
	
}

int Golomb::BinToDec(int* binary, int len) {
	int decimalNumber = 0;

	for(int i = 0; i < len; i++) {
		if(binary[i] == 1) {			
			decimalNumber = decimalNumber + pow(2, len-i-1);
		}
	}

	return decimalNumber;
}


// simple helper to convert from decimal to binary
string Golomb::DecToBin(int number)
{
	string sum = "";
	int dec = number,rem,i=1;

	int value = number;
	for(int i = B-1; i >= 0; i--, value>>=1) {
		char val = (value & 1) + '0';
		sum = val + sum;
	}

	return sum;
}
