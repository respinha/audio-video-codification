#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

using namespace std;
using namespace cv;



int mergeBlock(Mat image, int blockHeight, int blockWidth, vector<Mat> blocks) {

	int W = 0, H = 0; 

	uint8_t *blockRow, *p;

	for( int b=0 ; b<blocks.size(); b++){

		//cout << blocks[b].rows << " " << blocks[b].cols << "\n";
	}
	for( int b=0 ; b<blocks.size(); b++){

		cout << "B: " << b << "\n";

		if( W >= image.cols){
			//cout << "H: " << H << "\n";
			H += blocks[b].rows;
			W=0;
		}

		Mat block = blocks[b]; 

		block.copyTo(image(cv::Rect(W,H,block.cols, block.rows))); 
		
		W+=blocks[b].cols;

		
	}

}

int blockSplit(Mat image, int blockHeight, int blockWidth,vector<Mat>* smallImages) {

	// get the image data
 	//int height = image.rows;
	//int width = image.cols;

	Size reducedSize ( blockHeight , blockWidth );

	//namedWindow("smallImages ", CV_WINDOW_AUTOSIZE );

	for  ( int y =  0 ; y < image.rows ; y += reducedSize.height )
	{
		for  ( int x =  0 ; x < image.cols ; x += reducedSize.width )
   	 	{	
			int smallW;
			int smallH;

			if( (image.cols - x) < reducedSize.width )
				smallW = image.cols - x; 
			else 
				smallW = reducedSize.width;

			if( (image.rows - y) < reducedSize.height)
				smallH = image.rows - y; 
			else
				smallH = reducedSize.height; 

    		Rect rect =   Rect ( x , y , smallW , smallH );
    		smallImages->push_back (Mat (image , rect));
			//imshow( "smallImages", cv::Mat ( image, rect ));
   			//waitKey(0);
    	}
	}

	return 0;	
}

int main(int argc, char** argv) {

	Mat img = imread(argv[1],CV_8UC1);
	Mat rec_img = Mat::zeros(Size(img.cols, img.rows), CV_8UC1);

	cout << img.rows << " " << img.cols << "\n"; 

	vector<Mat> blocks; 

	blockSplit(img, 10, 10 , &blocks);

	mergeBlock(rec_img,10,10,blocks); 

	cout << rec_img.rows << " " << rec_img.cols << "\n"; 

	imshow("kek",rec_img); 

	waitKey(0);


	
	return 0;
}