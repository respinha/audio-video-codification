#include <cstdlib>
#include <string>

#include "BitStream.cpp"

using namespace std;

class Predictor {
	public:
		Predictor(int m, string encodedFilename, string decodedFilename);
		int* simple_predict(int* sequence);		
		int* predict(int* sequence);

	private:
		const int M;
		const int B;
		BitStream* stream;
};
