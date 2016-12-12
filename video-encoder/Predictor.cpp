#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include "Predictor.h"


Predictor::Predictor(string encoded_filename, int M, int decodeFlag) {

	//file = new string(filename);
	g = new Golomb(M, encoded_filename, decodeFlag);
}
int Predictor::encodeInterFrame(Mat frame, int isLastFrame, int height, int width, std::vector<Mat>* smallImages,std::vector<Mat> blocks,int m){
	 //Mat img = imread(file->c_str(), 1);   
       if ( !frame.data )
    {
       printf("No image data \n");
        return -1;
    }  

       //Mat bgr[3];

       //split(frame, bgr);

       // file metadata
       // 1-> intra-frame prediction mode
       /*g->encode(1, 0);
       g->encode(bgr[0].rows, 0);
       g->encode(bgr[0].cols, 0);*/

       //std::vector<Mat> blocks; 

       int8_t* p, *prev;

       //for(int m = 0; m < 3; m++) {

               //cout << "#### Channel: " << m << "\n";

               //aux_block_encode(bgr[m],0,height,width,&blocks);

               //cout << "smallImages size: "<<smallImages->size()<<"\n";
               //cout << "blocks size: "<<blocks.size()<<"\n";

               for( std::vector<Mat>::size_type idx = 0; idx != smallImages->size(); idx++){
                       //cout << "idx "<<idx<<"\n";

                       Mat diff = Mat::zeros(blocks[idx].rows, blocks[idx].cols, CV_8UC1);
                       // differences
                       for(int r = 0; r < blocks[idx].rows; r++) {

                               int8_t* ptr1 = blocks[idx].ptr<int8_t>(r);
                               int8_t* ptr2 = (*smallImages)[idx].ptr<int8_t>(r);
                               int8_t* ptrDiff = diff.ptr<int8_t>(r);
 
                               for(int c = 0; c < blocks[idx].cols; c++) {

                                       ptrDiff[c] = ptr1[c] - ptr2[c];
                               }
                       }


                       for(int row = 0; row < diff.rows; row++) {

                               /*if(row > 0) 
                                       prev = p;*/
               
                               p = diff.ptr<int8_t>(row);
                               
                               
                               for(int col = 0; col < diff.cols; col++) {
                                       
                                       //redict_aux(col, row, &x, p, prev, mode);

                                       int16_t residue = (int16_t) p[col];

                                       //cout << "IResidue: " << (int) residue << "\n";
                                       /*cout << "IResidue: " << (int) residue << "; value " << (int) p[col] << "\n";
                                       cout << "IValue: " << (int) p[col] << "\n";
                                       */

                                       int end = (row == diff.rows-1 && col == diff.cols-1 && m == 2 && isLastFrame && idx == smallImages->size()-1);

                                       g->encode((int) residue, end);

                               }
                       }
               }
       //}

       return 0;


}

void Predictor::predict_block_encode(string filename, int blockHeight, int blockWidth){

	//Mat frame;
	//VideoCapture cap(filename);

	ifstream myfile;

	myfile.open(filename);
	if (!myfile.is_open())
	{
		cerr << "Error opening file\n";
		return;
	}

	string line;
	int nCols, nRows, type, fps;
	getline (myfile,line);
	cout << line << endl;

	istringstream(line) >> nCols >> nRows >> fps >> type;
	Mat frame = Mat(Size(nCols, nRows), CV_8UC3);
	

	

 
       // double numFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);
       
       std :: vector <Mat> buffer_frames[3]; 

       std :: vector <Mat> blocks[3];

       int i = 0;
 
       while(true){
       		if(!myfile.read((char*)frame.data, frame.cols * frame.rows * frame.channels())) break;

			if (frame.empty()) break;         // check if at end


               
           std :: vector <Mat> frames[3];      

                //cap >> frame;
                
                
//@@ -49,30 +57,156 @@ void Predictor::predict_block_encode(string filename, int blockHeight, int block
 
                int isLastFrame = 0 ; 

           if(waitKey((int)(1.0 / fps * 1000)) >= 0){ 
           		isLastFrame = 1; 
           }

 

               if(i==0){
                       //cout << "encoding first frame..\n";
                       if((encodeIntraFrame(frame, 1, isLastFrame)) != 0 ) {
                              return;
                       }

                       Mat bgr[3];

                       split(frame, bgr);
                       for(int j=0; j<3;j++){
                               aux_block_encode(bgr[j],0,blockHeight,blockWidth,&buffer_frames[j]);
                       }
                             
                }

       
               else{
                       //cout << "encoding frame "<<i<<"\n";
               //cout << "size NOT FIRST frame: "<<frames.size() << "\n"; 
                       Mat bgr[3];

                       split(frame, bgr);

                       g->encode(1, 0);
                       g->encode(bgr[0].rows, 0);
                       g->encode(bgr[0].cols, 0);
                       
                       for(int j=0; j<3;j++){

                               aux_block_encode(bgr[j],0,blockHeight,blockWidth,&blocks[j]);



                               if((encodeInterFrame(frame,isLastFrame,blockHeight,blockWidth,&buffer_frames[j],blocks[j],j)) != 0) {
                                       return;
                               }
                       }

                       
               }

               i++;

               if (isLastFrame == 1){
               	break; 
               }
           }

               //buffer_frames = frames; 
               


       
       }

       //cv::Mat image;
       //image = imread(filename,CV_LOAD_IMAGE_COLOR);

       //encodeInterFrame(image, 1, blockHeight, blockWidth);






int Predictor::aux_block_encode(Mat image, int isLastFrame, int blockHeight, int blockWidth,std::vector<Mat>* smallImages) {

	// get the image data
 	int height = image.rows;
	int width = image.cols;

	//printf("Processing a %dx%d image\n",height,width);
	
	cv :: Size smallSize ( blockHeight , blockWidth );

	//std :: vector < Mat > smallImages ;
	//namedWindow("smallImages ", CV_WINDOW_AUTOSIZE );

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
        		smallImages->push_back ( cv::Mat ( image , rect ));
    			//imshow( "smallImages", cv::Mat ( image, rect ));
       			//waitKey(0);
    		}
	}

	return 0;	
}

void Predictor::predict_encode(string file, int mode) {

	VideoCapture cap(file);

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
		encodeIntraFrame(frame, mode, i==nFrames-1);
	}
	//Mat img = imread(file->c_str(), 1);
}

int Predictor::encodeIntraFrame(Mat frame, int mode, int isLastFrame) {

	if ( !frame.data )
    {
    	printf("No image data \n");
        return -1;
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
				predict_aux(col, row, &x, p, prev);

				int16_t residue = (int16_t) (p[col] - x);

				cout << "Residue: " << (int) residue << "\n";
				cout << "Value: " << (int) p[col] << "\n";
				g->encode((int) residue, (row == bgr[m].rows-1 && col == bgr[m].cols-1 && m == 2 && isLastFrame));
			}
		}
	}

	return 0;
}

/*void Predictor::predict_block_encode(string filename, int blockHeight, int blockWidth){


	   Mat frame;
 
        double numFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);
       
       std :: vector <Mat> buffer_frames[3]; 

       std :: vector <Mat> blocks[3];
 
        for(int i = 0; i < numFrames; i++) {

               
           std :: vector <Mat> frames[3];      

                cap >> frame;
                
                if(frame.empty()) {

 
                int isLastFrame = (i == numFrames-1);
 
               if(i==0){
                       //cout << "encoding first frame..\n";
                       if((encodeIntraFrame(frame, 1, isLastFrame)) != 0 ) {
                              return;
                       }

                       Mat bgr[3];

                       split(frame, bgr);
                       for(int j=0; j<3;j++){
                               aux_block_encode(bgr[j],0,blockHeight,blockWidth,&buffer_frames[j]);
                       }
                }else{
                       //cout << "encoding frame "<<i<<"\n";
                       //cout << "size NOT FIRST frame: "<<frames.size() << "\n"; 
                       Mat bgr[3];

                       split(frame, bgr);

                       g->encode(1, 0);
                       g->encode(bgr[0].rows, 0);
                       g->encode(bgr[0].cols, 0);
                       
                       for(int j=0; j<3;j++){

                               aux_block_encode(bgr[j],0,blockHeight,blockWidth,&blocks[j]);



                               if((encodeInterFrame(frame,isLastFrame,blockHeight,blockWidth,&buffer_frames[j],blocks[j],j)) != 0) {
                                       return;
                               }
                       }
                       
               }

               //buffer_frames = frames; 

                             


}*/

/*int Predictor::encodeInterFrame(Mat image, int isLastFrame, int blockHeight, int blockWidth) {

	// get the image data
 	int height = image.rows;
	int width = image.cols;

	 printf("Processing a %dx%d image\n",height,width);
	
	cv :: Size smallSize ( blockHeight , blockWidth );

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
	}

	return 0;	
}*/

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

	// decoding frame count
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
						predict_aux(col, row, &x, p, prev);

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
