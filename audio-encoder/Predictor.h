#include <cstdlib>
#include <string>
#include <list>
#include "Golomb.cpp"

using namespace std;

class Predictor {
	public:
		Predictor(int m, string encodedFilename, string decodedFilename);
		void simple_predict(short* sequence, short* sequence_buf, int length);		
		void predict(int* sequence, int length);
		list<short> reverse_simple_predict(short* sequence_buf); 
	private:
		Golomb* g;
};
