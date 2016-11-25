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


Predictor::Predictor(string filename, string encoded, int M, int decodeFlag) {

	file = new string(filename);
	g = new Golomb(M, encoded, decodeFlag);
}

void Predictor::predict_encode(int mode) {

	Mat img = imread(file->c_str(), 1);	
	if ( !img.data )
    {
    	printf("No image data \n");
        return;
    }	

	Mat bgr[3];

	split(img, bgr);

	// file metadata
	// 1-> intra-frame prediction mode
	g->encode(1, 0);
	g->encode(bgr[0].rows, 0);
	g->encode(bgr[0].cols, 0);

	uchar* p, *prev;
	for(int m = 0; m < 3; m++) {

		cout << "#### Channel: " << m << "\n";

		for(int row = 0; row < bgr[m].rows; row++) {

			if(row > 0) 
				prev = p;
	
			p = bgr[m].ptr<uchar>(row);
			
			for(int col = 0; col < bgr[m].cols; col++) {

				// JPEG-LS mode
				uchar x;
				predict_aux(col, row, &x, p, prev, mode);

				uchar residue = p[col] - x;

				cout << "Residue: " << (int) residue << "\n";
				//cout << "Residue: " << (int) residue << "; value " << (int) p[col] << "\n";
				cout << "Value: " << (int) p[col] << "\n";
				g->encode((int) residue, (row == bgr[m].rows-1 && col == bgr[m].cols-1 && m == 2));
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

	int rows = g->decode(pend);
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
	}

	Mat img = Mat::zeros(Size(rows, cols), CV_8UC3);

	// init channel matrices
	vector<Mat> bgr(3);
	for(unsigned int i = 0; i < bgr.size(); i++) bgr[i] = Mat::zeros(rows, cols, CV_8UC1);

	uchar* p, *prev;

	for(int m = 0; m < 3; m++) {
		cout << "#### Channel: " << m << "\n";
		for(int row = 0; row < rows; row++) {

			if(row > 0) 
				prev = p;
	
			p = bgr[m].ptr<uchar>(row);


			for(int col = 0; col < cols; col++) {

				if(predMode == 1) {
					uchar x;
					predict_aux(col, row, &x, p, prev, predMode);

					uchar residue = (uchar) g->decode(pend);
					//cout << "Residue: " << (int) residue << "\n";

					/*if(*pend && !(col == cols-1 && rows == rows-1)) {
						fprintf(stderr, "Error occurred decoding. Reached end of file.\n");
						return;
					}*/

					p[col] = residue + x;
					//cout << "Residue: " << (int) residue << "; value " << (int) p[col] << "\n";
					cout << "Value: " << (int) p[col] << "\n";
				}
			}
		}
	}

	merge(bgr, img);

	namedWindow( "Decoded image", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Decoded image", img );                   // Show our image inside it.

    waitKey(0);                                          // Wait for a keystroke in the window
}

void Predictor::predict_aux(int col, int row, uchar* x, uchar* p, uchar* prev, int mode) {

	if(mode != 1) {
		fprintf(stderr, "Not implemented yet.\n");
		return;
	}
	
	uchar a,b,c;
	switch(mode) {
		case 1: 
		{
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
			break;
		}
	}

}
