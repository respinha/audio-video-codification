#include <stdlib>
#include <string>

#include "BitStream.cpp"

using namespace std;

class Golomb {
	public:
		Golomb(int m);
		void encode(int n, string filename);		
		void decode(int n, string filename);

	private:
		int M;
		int B;
		BitStream* stream;
};
