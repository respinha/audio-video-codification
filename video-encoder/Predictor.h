#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Golomb.h"

class Predictor {
	public:
		Predictor(string filename, string encoded, int M, int pos=0);
		void predict_encode();
		void predict_decode();
	private:
		void calcEntropy();
		Golomb* g;
	
		string* file;

};

