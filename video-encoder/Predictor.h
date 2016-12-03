#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Golomb.cpp"

using namespace std;
using namespace cv;

class Predictor {
	public:
		Predictor(string encoded_filename, int M, int decodeFlag=0);
		void predict_encode(string filename, int mode);
		void predict_decode();
		void predict_block_encode(string filename, int mode);
	private:
		int encodeFrame(Mat frame, int mode, int isLastFrame);
		void calcEntropy();
		void predict_aux(int col, int row, uchar* x, uchar* p, uchar* prev, int mode);
		Golomb* g;
	
		string* file;

};


















