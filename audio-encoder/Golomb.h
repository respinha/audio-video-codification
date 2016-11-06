#include <cstdlib>
#include <string>
#include <queue>

#include "BitStream.cpp"

using namespace std;

class Golomb {
	public:
		Golomb(int m, string encodedFilename);
		void encode(short n, short finalWrite);	
		short decode();

	private:
		const int M;
		const int B;
		BitStream* stream;

		int BinToDec(int* binary, int len);
		int* DecToBin(int number);
};
