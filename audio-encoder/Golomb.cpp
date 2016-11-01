#include <cstdlib>
#include <string>
#include <cmath>
#include <bitset>
#include <sstream>

#include "Golomb.h"

string DecToBin(int);

Golomb::Golomb(int m, string encodedFilename, string decodedFilename) : M(m), B(log2(m)){
	stream = new BitStream(encodedFilename, decodedFilename);
}


void Golomb::encode(int n) {

	int q = n/M;
	int r = n -(q*M);

	cout << "N = " << n << "\n";
	cout << "M = " << M << "\n";
	cout << "Q = " << q << "\n";
	cout << "R = " << r << "\n";
	cout << "B = " << B << "\n";

	int i = 0;
	int* code = new int[q+1+B];

	// unary
	while(i < q) {
		code[i++] = 1;
	}
	code[q] = 0;


	// get remainder in binary value
	string binaryRem = DecToBin(r);
	for(i = 0; i < B; i++) {
		char* value;
		*value  = binaryRem[i];
		code[q+1+i] =  atoi(value);
	}


	for(i = 0; i < (q+1+B); i++) 
		cout << i << ": " << code[i] << "\n";

	stream->writeNBits(q+1+B, code, 1);

}

// simple helper to convert from decimal to binary
// original src: http://stackoverflow.com/questions/2548282/decimal-to-binary-and-vice-versa
string DecToBin(int number)
{
	string sum = "";
	int dec = number,rem,i=1;
	do
	{
		rem=dec%2;
		ostringstream ss;
		ss << i*rem;
	        sum.append(ss.str());
		dec=dec/2;
	        i=i*10;
	    }while(dec>0);

	return sum;
}
