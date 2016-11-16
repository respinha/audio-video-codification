#include <cstdlib>
#include <string>
#include <queue>

#include "BitStream.cpp"

using namespace std;

class Golomb {
	public:
		Golomb(int m, string encodedFilename, int pos);
		void encode(short n, short finalWrite);	
		short decode(int* end);

		int getFilePosition();
	private:
		const int M;
		const int B;
		BitStream* stream;

		int BinToDec(int* binary, int len);
		char* DecToBin(int number);
};
