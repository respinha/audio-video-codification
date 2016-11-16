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
	g = new Golomb(filename, encoded, M, pos);
}

Predictor::predict_encode() {

	
	Mat img = imread(file, 1);
	
	if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }	

	/* should we worry about this???
	if (I.isContinuous())
    {
        nCols *= nRows;
        nRows = 1;
    }*/

	// file metadata
	// 1-> intra-frame prediction mode
	g->(1, 0);
	g->(img.rows, 0);
	g->(img.cols, 0);

	for(int r = 0; r < img.rows; r++) {

		for(int col = 0; col < img.cols; col++) {

			// JPEG-LS mode
			int a, b, c;
			int border = 0;
			if(col == 0) {
				a = 0;
				border = 1;
			} else  a = img[r][col-1];

			if(r == 0)  {
				b = 0;
				border = 1;
			} else b = img[r-1][col];

			c = border ? 0 : img[r-1][col-1];

			int x;
			if(c >= max(a, b)) {
				x = min(a,b);
			} else (c <= min(a,b)) {
				x = max(a,b);
			} else {
				x = a + b - c;
			}

			g->encode(x, (r == img.rows-1 && col == img.cols-1));
		}
	}
}

/*int main(int argc, char** argv){
		
	Mat I,J; 
	
	if( argc == 4 && !strcmp(argv[3],"G") )
		I = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	else
	I = imread(argv[1], CV_LOAD_IMAGE_COLOR);

	if (!I.data)
	{
		cout << "The image" << argv[1] << " could not be loaded." << endl;
		return -1;
	}

}*/
