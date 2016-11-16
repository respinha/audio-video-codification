#include <cstdlib>
#include <string>
#include <queue>

#include "BitStream.cpp"

using namespace std;

class Golomb {
	public:
		Golomb(int m, string encodedFilename, int pos);
		void encode(int n, int finalWrite);	
		int decode(int* end);

		int getFilePosition();
	private:
		const int M;
		const int B;
		BitStream* stream;
};
