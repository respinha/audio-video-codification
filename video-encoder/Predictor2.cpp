#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include "Predictor.h"

/*using namespace std;
using namespace cv;*/


Predictor::Predictor(string filename, string encoded, int M, int decodeFlag) {

	file = new string(filename);
	g = new Golomb(M, encoded, decodeFlag);
}

void Predictor::predict_encode(int mode) {

	VideoCapture cap(*file);

	g->encode(mode, 0);
	
	g->encode(cap.get(CV_CAP_PROP_FOURCC), 0);
	g->encode(cap.get(CV_CAP_PROP_FPS), 0);
	g->encode(cap.get(CV_CAP_PROP_FRAME_WIDTH), 0);
	g->encode(cap.get(CV_CAP_PROP_FRAME_HEIGHT), 0); 

	int nFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);
	g->encode(nFrames, 0);

	Mat frame;
	for(int i = 0; i < nFrames; i++) {
		cap.read(frame);
		encode_frame(frame, mode, i==nFrames-1);
	}
	//Mat img = imread(file->c_str(), 1);
}

void Predictor::encode_frame(Mat frame, int mode, int isLastFrame) {

	if ( !frame.data )
    {
    	printf("No image data \n");
        return;
    }
	Mat bgr[3];

	split(frame, bgr);

	// file metadata
	// 1-> intra-frame prediction mode
	/*g->encode(1, 0);
	g->encode(bgr[0].rows, 0);
	g->encode(bgr[0].cols, 0);*/

	uint8_t* p, *prev;
	for(int m = 0; m < 3; m++) {

		for(int row = 0; row < bgr[m].rows; row++) {

			if(row > 0) 
				prev = p;
	
			p = bgr[m].ptr<uint8_t>(row);
			
			for(int col = 0; col < bgr[m].cols; col++) {

				// JPEG-LS mode
				uint8_t x;
				predict_aux(col, row, &x, p, prev, mode);

				int16_t residue = (int16_t) (p[col] - x);

				cout << "Residue: " << (int) residue << "\n";
				cout << "Value: " << (int) p[col] << "\n";
				g->encode((int) residue, (row == bgr[m].rows-1 && col == bgr[m].cols-1 && m == 2 && isLastFrame));
			}
		}
	}
}
void Predictor::predict_decode() {

	int end = 0;
	int* pend = &end;
	int predMode = g->decode(pend);
	if(*pend) {
		cout << predMode << "\n";
		fprintf(stderr, "An error occurred decoding prediction mode!\n");
		return;
	}

	int videoParams[4];
	for(int i = 0; i < 4; i++) {
		videoParams[i] = g->decode(pend);
		if(*pend) {
			cout << predMode << "\n";
			fprintf(stderr, "An error occurred decoding video params!\n");
			return;
		}
	}


	/*g->encode(cap.get(CV_CAP_PROP_FOURCC), 0);
	g->encode(cap.get(CV_CAP_PROP_FPS), 0);
	g->encode(cap.get(CV_CAP_PROP_FRAME_WIDTH), 0);
	g->encode(cap.get(CV_CAP_PROP_FRAME_HEIGHT), 0); */

	/*int rows = g->decode(pend);
	if(*pend) {
		cout << rows << "\n";
		fprintf(stderr, "An error occurred decoding rows!\n");
		return;
	}

	int cols = g->decode(pend);
	if(*pend) {
		cout << cols << "\n";
		fprintf(stderr, "An error occurred decoding columns!\n");
		return;
	}*/

	int cols = videoParams[2];	// width
	int rows = videoParams[3];	// height

	int nFrames = g->decode(pend);
	if(*pend) {
		cout << nFrames << "\n";
		fprintf(stderr, "An error occurred decoding number of frames!\n");
		return;
	}

	VideoWriter output("output.mkv", 
						videoParams[0],
						videoParams[1],
						Size(videoParams[2], videoParams[3]));

	for(int f = 0; f < nFrames; f++) {
		Mat img = Mat::zeros(Size(rows, cols), CV_8UC3);

		// init channel matrices
		vector<Mat> bgr(3);
		for(unsigned int i = 0; i < bgr.size(); i++) bgr[i] = Mat::zeros(rows, cols, CV_8UC1);

		uint8_t* p, *prev;

		for(int m = 0; m < 3; m++) {
			for(int row = 0; row < rows; row++) {

				if(row > 0) 
					prev = p;
		
				p = bgr[m].ptr<uint8_t>(row);


				for(int col = 0; col < cols; col++) {

					if(predMode == 1) {
						uint8_t x;
						predict_aux(col, row, &x, p, prev, predMode);

						int16_t residue = (int16_t) g->decode(pend);
						//cout << "Residue: " << (int) residue << "\n";


						p[col] = (uint8_t) (residue + x);
						cout << "Residue: " << (int) residue << "\n";
						cout << "Value: " << (int) p[col] << "\n";

						if(*pend && !(col == cols-1 && rows == rows-1 && f == nFrames-1)) {
							fprintf(stderr, "Error occurred decoding. Reached end of file.\n");
							return;
						}
					}
				}
			}
		}

		merge(bgr, img);
		output << img;
	}
	/*namedWindow( "Decoded image", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Decoded image", output );                   // Show our image inside it.

    waitKey(0);                                          // Wait for a keystroke in the window*/

}

void Predictor::predict_aux(int col, int row, uint8_t* x, uint8_t* p, uint8_t* prev) {

	
	uint8_t a,b,c;
	
	int border = 0;

	if(col == 0) {
		a = 0;
		border = 1;
	} else  a = p[col-1];

	if(row == 0)  {
		b = 0;
		border = 1;
	} else b = prev[col];

	c = border ? 0 : prev[col-1];

	if(c >= max(a, b)) {
		*x = min(a,b);
	} else if (c <= min(a,b)) {
		*x = max(a,b);
	} else {
		*x = a + b - c;
	}
	
}
