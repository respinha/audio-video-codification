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
		void predict_block_encode(string filename, int blockHeight, int blockWidth);

	private:
		int encodeIntraFrame(Mat frame, int mode, int isLastFrame);
		int encodeInterFrame(Mat frame, int isLastFrame, int height, int width, std::vector<Mat>* smallImages,std::vector<Mat> blocks,int m);

		int aux_block_encode(Mat image, int isLastFrame, int blockHeight, int blockWidth,std::vector<Mat>* smallImages);
		void calcEntropy();
		void predict_aux(int col, int row, uint8_t* x, uint8_t* p, uint8_t* prev);
		Golomb* g;
	
		string* file;

};


















