#include <cstdlib>
#include <string>
#include <list>
#include "Golomb.cpp"

using namespace std;

class Predictor {
	public:
		Predictor(int m, string encodedFilename, int pos=0);
		void order1_predict(short* sequence, short* sequence_buf, int end);		
		void order2_predict(short* sequence, short* sequence_buf, short* sequence_buf2,short* residues, int end);
		void reverse_order1_predict(short* sequence_buf, short* outputSamples, int* end);

		void reverse_order2_predict(short* prev_buffer, short* buffer, short* samples, int* end);

		void order3_predict(short* left_buffer, short* right_buffer, short* samples, int end);
		void reverse_order3(short* left_buffer, short* right_buffer, short* samples, int* end);

		int getFilePosition(); 
	private:
		Golomb* g;
};
