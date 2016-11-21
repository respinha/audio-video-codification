#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include "Predictor.h"

using namespace std;
using namespace cv;


Predictor::Predictor(string filename, string encoded, int M, int pos) {

	file = new string(filename);
	g = new Golomb(M, encoded, pos);
}

void Predictor::predict_encode() {

	
	Mat img = imread(file->c_str(), 1);
	
	if ( !img.data )
    {
        printf("No image data \n");
        return;
    }	

	Mat bgr[3];

	split(img, bgr);
	/* should we worry about this???
	if (I.isContinuous())
    {
        nCols *= nRows;
        nRows = 1;
    }*/

	// file metadata
	// 1-> intra-frame prediction mode
	g->encode(1, 0);
	g->encode(bgr[0].rows, 0);
	g->encode(bgr[0].cols, 0);

	uchar* p, *prev;
	for(int m = 0; m < 3; m++) {

		for(int row = 0; row < bgr[m].rows; row++) {

			if(row > 0) 
				prev = p;
	
			p = bgr[m].ptr<uchar>(row);
			
			for(int col = 0; col < bgr[m].cols; col++) {

				// JPEG-LS mode
				uchar a, b, c, x;
				predict_aux(col, row, &a, &b, &c, &x, p, prev);


				uchar residue = p[col] - x;
				g->encode((int) residue, (row == bgr[m].rows-1 && col == bgr[m].cols-1));
			}
		}
	}
}

void Predictor::predict_decode() {

	Mat img;

	int end = 0;
	int* pend = &end;

	int predMode = g->decode(pend);
	if(!*pend) fprintf(stderr, "An error occurred decoding!\n");

	int rows = g->decode(pend);
	if(!*pend) fprintf(stderr, "An error occurred decoding!\n");

	int cols = g->decode(pend);
	if(!*pend) fprintf(stderr, "An error occurred decoding!\n");

	// init channel matrices
	vector<Mat> bgr(3);
	for(unsigned int i = 0; i < bgr.size(); i++) bgr[i] = Mat::zeros(rows, cols, CV_8UC3);

	uchar* p, *prev;

	for(int m = 0; m < 3; m++) {
		for(int row = 0; row < rows; row++) {

			// LEFT HERE; UNFINISHED!!
			if(row > 0) 
				prev = p;
	
			p = bgr[m].ptr<uchar>(row);

			for(int col = 0; col < cols; col++) {

				if(predMode == 1) {
					uchar a, b, c, x;
					predict_aux(col, row, &a, &b, &c, &x, p, prev);
				
					uchar residue = (uchar) g->decode(pend);

					if(*pend && !(col == cols-1 && rows == rows-1)) {
						fprintf(stderr, "Error occurred decoding. Reached end of file.\n");

						p[col] = residue + x;
					} else fprintf(stderr, "Error occurred decoding. Predictor mode not implemented.\n");
				}
			}
		}
	}

	merge(bgr, img);
}

void Predictor::predict_aux(int col, int row, uchar* a, uchar* b, uchar* c, uchar* x, uchar* p, uchar* prev) {

	int border = 0;
	
	if(col == 0) {
		*a = 0;
		border = 1;
	} else  *a = p[col-1];

	if(row == 0)  {
		*b = 0;
		border = 1;
	} else *b = prev[col];

	*c = border ? 0 : prev[col-1];

	if(*c >= max(*a, *b)) {
		*x = min(*a,*b);
	} else if (*c <= min(*a,*b)) {
		*x = max(*a,*b);
	} else {
		*x = *a + *b - *c;
	}

}

int main(int argc, char** argv){
	
/*	Mat I,J; 
	
	if( argc == 4 && !strcmp(argv[3],"G") )
		I = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	else
	I = imread(argv[1], CV_LOAD_IMAGE_COLOR);

	if (!I.data)
	{
		cout << "The image" << argv[1] << " could not be loaded." << endl;
		return -1;
	}*/

	return 0;
}
