#include <cstdlib>
#include <string>
#include <list>
#include "Golomb.cpp"

using namespace std;

class Predictor {
	public:
		Predictor(int m, string encodedFilename, int pos=0);
		void simple_predict(short* sequence, short* sequence_buf);		
		void predict(int* sequence);
		void reverse_simple_predict(short* sequence_buf, short* outputSamples, int* end);
		int getFilePosition(); 
	private:
		Golomb* g;
};
