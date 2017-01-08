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
		void temporalDecode(int blockHeight, int blockWidth);
		void temporalPredict(string filename, int blockHeight, int blockWidth);

	private:
		void encodeIntraframe(Mat frame, Mat bgr[],int toEncode);
		int encodeInterframe(vector<Mat>* prevBlocks, vector<Mat> currBlocks, int toEncode);

		void displayVideo(string filename);	

		int blockSplit(Mat image, int blockHeight, int blockWidth, vector<Mat>* smallImages);
		int mergeBlock(Mat image, vector<Mat> blocks);

		float calcEntropy(int total);
		void spatialPredictAux(int col, int row, uint8_t* x, uint8_t* p, uint8_t* prev);

		GolombEncoder* ge;
		GolombDecoder* gd;

		ofstream* histogramFile;
		map<int, int> occurrences;
		BitStream* bs;
		string* file;

};


















