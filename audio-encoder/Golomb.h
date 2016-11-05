#include <cstdlib>
#include <string>
#include <queue>

#include "BitStream.cpp"

using namespace std;

class Golomb {
	public:
		Golomb(short m, string encodedFilename, string decodedFilename);
		void encode(short n, short finalWrite);	
		list<short> decode();

	private:
		const short M;
		const short B;
		BitStream* stream;

		short BinToDec(short* binary, short len);
		short* DecToBin(short number);
};
