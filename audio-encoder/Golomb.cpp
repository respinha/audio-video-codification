#include <cstdlib>
#include <string>
#include <cmath>
#include <bitset>
#include <sstream>
#include <queue>
#include <list>

#include "Golomb.h"


Golomb::Golomb(short m, string encodedFilename, string decodedFilename) : M(m), B(log2(m)){
	stream = new BitStream(encodedFilename, decodedFilename);
}


void Golomb::encode(short n, short finalWrite) {

	short q = n/M;
	short r = n -(q*M);

	cout << "N = " << n << "\n";
	cout << "M = " << M << "\n";
	cout << "Q = " << q << "\n";
	cout << "R = " << r << "\n";
	cout << "B = " << B << "\n";
	cout << "###########################\n";
	short i = 0;
	short* code = new short[q+1+B];

	// unary
	while(i < q) {
		code[i++] = 1;
	}

	code[q] = 0;


	// get remainder in binary value
	short* binaryRem = Golomb::DecToBin(r);
	for(i = 0; i < B; i++) {
		/*char* value;
		*value  = binaryRem[i];*/
		
		//code[q+1+i] =  atoi(value);
		code[q+1+i] = binaryRem[i];
	}

	stream->writeNBits(q+1+B, code, finalWrite);

/*	if(finalWrite) {
		cout << "Read: ";
		for(short i = 0; i < 22; i++) {
			cout << stream->readBit();
			if((i+1)%8 ==0) cout << " | ";
		}
		cout << "\n";
	}*/
}

list<short> Golomb::decode() {

	short nUnary = 0;
	short isUnary = 1;
	short q = 0;

	list<short> nList;
	
	short bit = 0;
	short* binarySequence;
	
	while(1) {
		
		if(isUnary) {
			bit = stream->readBit();

			if(bit == -1) break;
			
			if(bit) q++;			
			else isUnary = bit;		
		} else {
			binarySequence = stream->readNBits(B);

			cout << "decode: ";
			short r = Golomb::BinToDec(binarySequence, B);
			cout << "r = " << r;
			short n = r + (q*M);
			
			cout << "; q = " << q << "; n = " << n << "\n";

			nList.push_back(n);	
			q = 0;
			isUnary = 1;
		}
		
	}

	for(list<short>::iterator it = nList.begin(); it !=nList.end(); it++) 
		cout << "N: " << *it << "\n";
	return nList;
	
}

short Golomb::BinToDec(short* binary, short len) {
	short decimalNumber = 0;

	for(short i = 0; i < len; i++) {
		if(binary[i] == 1) {			
			decimalNumber = decimalNumber + pow(2, len-i-1);
		}
	}

	return decimalNumber;
}


// simple helper to convert from decimal to binary
short*Golomb::DecToBin(short number)
{
	short* sum = new short[B];
	short dec = number,rem,i=1;

	short value = number;
	for(short i = B-1; i >= 0; i--, value>>=1) {
		short val = (value & 1) + '0';
		sum[i] = val;
	}

	return sum;
}
