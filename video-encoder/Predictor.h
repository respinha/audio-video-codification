#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Golomb.cpp"

class Predictor {
	public:
		Predictor(string filename, string encoded, int M, int pos=0);
		void predict_encode(int mode);
		void predict_decode();
	private:
		void calcEntropy();
		void predict_aux(int col, int row, uchar* x, uchar* p, uchar* prev, int mode);
		Golomb* g;
	
		string* file;

};

