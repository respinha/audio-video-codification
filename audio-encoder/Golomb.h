#include <cstdlib>
#include <string>

#include "BitStream.cpp"

using namespace std;

class Golomb {
	public:
		Golomb(int m, string encodedFilename, string decodedFilename);
		void encode(int n);		
		void decode(int n, int* sequence);

	private:
		const int M;
		const int B;
		BitStream* stream;
};
