#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <map>
#include "Golomb.h"

using namespace std;
using namespace cv;

class Predictor {
	public:
		Predictor(string encoded, int M, int decodeFlag=0);
		void spatialPredict(string filename);
		void spatialDecode();
		void predict_block_encode(string filename, int blockHeight, int blockWidth);

	private:
		void encodeIntraframe(Mat frame);
		int encodeInterFrame(Mat frame, int isLastFrame, int height, int width, std::vector<Mat>* smallImages,std::vector<Mat> blocks,int m);
		void displayVideo(string filename);	
		int aux_block_encode(Mat image, int isLastFrame, int blockHeight, int blockWidth,std::vector<Mat>* smallImages);
		//void calcEntropy();
		void predict_aux(int col, int row, uint8_t* x, uint8_t* p, uint8_t* prev);
		GolombEncoder* ge;
		GolombDecoder* gd;

		ofstream* histogramFile;
		map<int, int> occurrences;
		BitStream* bs;
		string* file;

};


















