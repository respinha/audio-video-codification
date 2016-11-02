#include <cstdlib>
#include <string>
#include <cmath>
#include <bitset>
#include <sstream>
#include <queue>

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

queue<int> Golomb::decode() {

	int* sequence = stream->readNBits();

	int nUnary = 0;
	int isUnary = 1;
	int q = 0, b = 0;
	string* remainder = new string();

	queue<int> nQueue;

	int eof = 0;
	while(!eof) {
		
		int bit = stream->readBit();

		if(isUnary) {
			if(bit == 0) {
				isUnary = 0;
				q = nUnary;
				nUnary = 0;

			}
			else nUnary++;
		}
		
		if(!isUnary) {
			ostringstream ss;
			ss << bit;
			remainder->append(ss.str());
			
			b++;
				
			if(b == B) {	
				int r = BinToDec(*remainder);
				int n = r+(q*M);

				nQueue.push_back(n);
				b = 0;
				
				isUnary = true;
				q = 0;
				
						
				
			}
		}
		
	}
	
}

int BinToDec(string binary) {
	int decimalNumber = 0;

	int len = binary.length();	
	for(int i = 0; i < len; i++) {
		if(binary[i] == '1') {
			decimalNumber = decimalNumber + pow(2, len-i-1);
		}
	}
	
	return decimalNumber;
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
