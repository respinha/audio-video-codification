#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Golomb.cpp"

using namespace std;
using namespace cv;

class Predictor {
	public:
		Predictor(string filename, string encoded, int M, int decodeFlag=0);
		void predict_encode(int mode);
		void predict_decode();
	private:
		void calcEntropy();
		void encode_frame(Mat frame, int mode, int isLastFrame);
		void predict_aux(int col, int row, uint8_t* x, uint8_t* p, uint8_t* prev, int mode);
		Golomb* g;
	
		string* file;

};


















