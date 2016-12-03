#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include "Predictor.h"

//using namespace std;
//using namespace cv;


Predictor::Predictor(string encoded_filename, int M, int decodeFlag) {

	//file = new string(filename);
	g = new Golomb(M, encoded_filename, decodeFlag);
}

void Predictor::predict_block_encode(string filename, int mode){

	/*string* file = new string(filename);

	VideoCapture cap(*file);

	// video parameters
	g->encode(cap.get(CV_CAP_PROP_FOURCC), 0);
	g->encode(cap.get(CV_CAP_PROP_FPS), 0);
	g->encode(cap.get(CV_CAP_PROP_FRAME_WIDTH), 0);
	g->encode(cap.get(CV_CAP_PROP_FRAME_HEIGHT), 0);

	if(!cap.isOpened()) {
		fprintf(stderr, "Error reading video file!\n");
		return;
	}

	Mat frame;

	double numFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);

	for(int i = 0; i < numFrames; i++) {
		cap >> frame;
		
		if(frame.empty()) {
			fprintf(stderr, "Unexpectedly reached end of video. No more frames to read.\n");
			break;
		}

		int isLastFrame = (i == numFrames-1);

		if((encodeFrame(frame, mode, isLastFrame)) != 0) {
			return;
		}
	*/
	cv::Mat image;
//	capture >> img;
	image = imread(filename,CV_LOAD_IMAGE_COLOR);
	int N = 10; 

	// get the image data
 	int height = image.rows;
	int width = image.cols;

	 printf("Processing a %dx%d image\n",height,width);
	
	cv :: Size smallSize ( 50 , 50 );

	std :: vector < Mat > smallImages ;
	namedWindow("smallImages ", CV_WINDOW_AUTOSIZE );

	for  ( int y =  0 ; y < image.rows ; y += smallSize.height )
	{
    		for  ( int x =  0 ; x < image.cols ; x += smallSize.width )
   	 	{	
			int smallW;
			int smallH;

			if( (image.cols - x) < smallSize.width )
				smallW = image.cols - x; 
			else 
				smallW = smallSize.width;

			if( (image.rows - y) < smallSize.height)
				smallH = image.rows - y; 
			else
				smallH = smallSize.height; 

        		cv::Rect rect =   cv::Rect ( x , y , smallW , smallH );
        		smallImages.push_back ( cv::Mat ( image , rect ));
    			imshow( "smallImages", cv::Mat ( image, rect ));
       			waitKey(0);
    		}
	}//	}	

}



void Predictor::predict_encode(string filename, int mode) {

	string* file = new string(filename);

	VideoCapture cap(*file);

	// video parameters
	g->encode(cap.get(CV_CAP_PROP_FOURCC), 0);
	g->encode(cap.get(CV_CAP_PROP_FPS), 0);
	g->encode(cap.get(CV_CAP_PROP_FRAME_WIDTH), 0);
	g->encode(cap.get(CV_CAP_PROP_FRAME_HEIGHT), 0);

	if(!cap.isOpened()) {
		fprintf(stderr, "Error reading video file!\n");
		return;
	}

	Mat frame;

	double numFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);

	for(int i = 0; i < numFrames; i++) {
		cap >> frame;
		
		if(frame.empty()) {
			fprintf(stderr, "Unexpectedly reached end of video. No more frames to read.\n");
			break;
		}

		int isLastFrame = (i == numFrames-1);

		if((encodeFrame(frame, mode, isLastFrame)) != 0) {
			return;
		}

	}	
}

int Predictor::encodeFrame(Mat img, int mode, int isLastFrame) {
	//Mat img = imread(file->c_str(), 1);	
	if ( !img.data )
    {
    	printf("No image data \n");
        return -1;
    }	

	Mat bgr[3];

	split(img, bgr);

	// file metadata
	// 1-> intra-frame prediction mode
	g->encode(mode, 0);
	g->encode(bgr[0].rows, 0);
	g->encode(bgr[0].cols, 0);


	uchar* p, *prev;

	for(int m = 0; m < 3; m++) {

		//cout << "#### Channel: " << m << "\n";

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
				g->encode((int) residue, (row == bgr[m].rows-1 && col == bgr[m].cols-1 && m == 2 && isLastFrame));
			}
		}
	}

	return 0;
}

void Predictor::predict_decode() {

	int end = 0;
	int* pend = &end;

	int videoParams[4];

	for(int i = 0; i< 4; i++) {
		videoParams[i] = g->decode(pend);
		if(*pend) {
			cout << videoParams[i]<< "\n";
			fprintf(stderr, "An error occurred video parameters!\n");
			return;
		}
	}

	VideoWriter outputVideo("output.mp4", 
							videoParams[0], 
							videoParams[1],
							Size(videoParams[2], videoParams[3]));

	if(!outputVideo.isOpened()) {
		fprintf(stderr, "Could not open output video for writing!\n");
		return;
	}
	// cap.get(CV_CAP_PROP_FOURCC
	// cap.get(CV_CAP_PROP_FPS
	// input_cap.get(CV_CAP_PROP_FRAME_WIDTH
	// cap.get(CV_CAP_PROP_FRAME_HEIGHT

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


	while(!(*pend)) {

		Mat img = Mat::zeros(Size(rows, cols), CV_8UC3);

		// init channel matrices
		vector<Mat> bgr(3);
		for(unsigned int i = 0; i < bgr.size(); i++) 
			bgr[i] = Mat::zeros(rows, cols, CV_8UC1);

		uchar* p, *prev;

		for(int m = 0; m < 3; m++) {
			//cout << "#### Channel: " << m << "\n";
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
						cout << "Residue: " << (int) residue << "\n";
						//cout << "Residue: " << (int) residue << "; value " << (int) p[col] << "\n";
						cout << "Value: " << (int) p[col] << "\n";
					}
				}
			}
		}

		merge(bgr, img);

		// writing frame to output
		outputVideo.write(img);
	}	


	/*namedWindow( "Decoded image", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Decoded image", img );                   // Show our image inside it.

    waitKey(0);                                          // Wait for a keystroke in the window
    */
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
