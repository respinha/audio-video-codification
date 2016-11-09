#include <cstdlib>
#include <string>
#include <cmath>
#include <bitset>
#include <sstream>
#include <queue>
#include <list>

#include "Golomb.h"


Golomb::Golomb(int m, string encodedFilename, int pos) : M(m), B(log2(m)){
	stream = new BitStream(encodedFilename,pos);
}


void Golomb::encode(short n, short finalWrite) {

		
	int transformedN = n >= 0 ? 2*n : (2*abs(n))-1;

	int q = transformedN/M;
	int r = transformedN -(q*M);

	cout << "N = " << n << "\n";
	cout << "Transformed N: " << transformedN << "\n";
	cout << "M = " << M << "\n";
	cout << "Q = " << q << "\n";
	cout << "R = " << r << "\n";
	cout << "B = " << B << "\n";
	cout << "###########################\n";

	int i = 0;

	// unary
	while(i < q) {
		stream->writeBit(1);
		i++;
	}

	//cout << 0;
	stream->writeBit(0);

	stream->writeNBits(B, r, finalWrite);

	/*cout << "BIN: ";
	for(int k = 0; k < B; k++) cout << code[q+1+k];
	cout << "\n";
	*/

/*	if(finalWrite) {
		cout << "Read: ";
		for(int i = 0; i < 22; i++) {			cout << stream->readBit();
			cout << stream->readBit();
			if((i+1)%8 ==0) cout << " | ";
		}
		cout << "\n";
	}*/

}

short Golomb::decode(int* end) {

	int nUnary = 0;
	int isUnary = 1;
	int q = 0;

	int bit = 0;
	int* binarySequence;
	
	short sample;
	
	while(1) {
		
		if(isUnary) {
			bit = stream->readBit();

			//cout << bit;
			if(bit == -1) {
				*end = 1;
				cout << "The end as we know it\n";
				//delete binarySequence;
			
				return -1;
			}
			
			if(bit) q++;			
			else isUnary = bit;		
		} else {
			binarySequence = stream->readNBits(B);

			//cout << "decode: ";
			int r = Golomb::BinToDec(binarySequence, B);
			//cout << "r = " << r;
			int n = r + (q*M);
			int original;
			// checking if even or odd to apply transformation

			if(n%2 == 0)
				original = n/2;
			else
				original = ((n+1)/2) * -1;
			
			cout << "decoded: " << n << "; Original: " << original << "\n";
			cout << "\n";
			cout << original << "\n";
			stringstream ss;
			ss << original;
			
			ss >> sample;

			//cout << "; q = " << q << "; n = " << sample << "\n";
			/*nList.push_back(sample);	
			q = 0;
			isUnary = 1;*/


			//delete binarySequence;
			break;
		}
		
	}

//	cout << "Here: " << *end << "\n";
	return sample;
	
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
char*Golomb::DecToBin(int number)
{
	char* sum = new char[B];
	int dec = number,rem,i=1;

	int value = number;
	for(int i = B-1; i >= 0; i--, value>>=1) {
		char val = (value & 1) + '0';
		sum[i] = val;
	}

		return sum;
}

int Golomb::getFilePosition() {
	return stream->getFilePosition();
}
