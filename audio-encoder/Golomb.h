#include <cstdlib>
#include <string>
#include <queue>

#include "BitStream.cpp"

using namespace std;

class Golomb {
	public:
		Golomb(int m, string encodedFilename, string decodedFilename);
		void encode(int n, int finalWrite);	
		list<int> decode();

	private:
		const int M;
		const int B;
		BitStream* stream;

		int BinToDec(int* binary, int len);
		int* DecToBin(int number);
};
