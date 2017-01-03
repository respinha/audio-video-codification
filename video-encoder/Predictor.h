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
	private:

		//void calcEntropy();
		BitStream* bs;
		void encodeIntraframe(Mat frame);

		void predict_aux(int col, int row, uint8_t* x, uint8_t* p, uint8_t* prev);

		void displayVideo(string filename);	

		GolombEncoder* ge;
		GolombDecoder* gd;

		ofstream* histogramFile;
		map<int, int> occurrences;
};
