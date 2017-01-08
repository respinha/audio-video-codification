#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cmath> 

#include "Predictor.h"


Predictor::Predictor(string encodedFile, int M, int decodeFlag) {

	string mode = decodeFlag ? "r" : "w";

	/*BitStream**/ bs = new BitStream(encodedFile, mode);
	if(mode == "w") 
		ge = new GolombEncoder(bs, M);
	else 
		gd = new GolombDecoder(bs, M);

	histogramFile = new ofstream();
	histogramFile->open("hist.txt", ios::out | ios::app);
}


void Predictor::hybridEncode(string filename, int blockHeight, int blockWidth){

	ifstream* stream = new ifstream();
	stream->open(filename);

	if (!stream->is_open())
	{
		cerr << "Error opening file\n";
		return;
	}

	// reading metadata
	string line;
	int nCols, nRows, type, fps;
	getline (*stream,line);

	istringstream(line) >> nCols >> nRows >> fps >> type;
	/*cout << "nCols "<<nCols<<"\n";
	cout << "nRows "<<nRows<<"\n";
	cout << "fps "<<fps<<"\n";*/

	Mat frame = Mat(Size(nCols, nRows), CV_8UC3);
	
	int nFrames = 0;

	while(true) {
		nFrames++;
		//cout << nFrames << "\n";

		if(!stream->read((char*) frame.data, frame.cols * frame.rows * frame.channels())) break; 
		if (frame.empty()) break;         // check if at end
	}

	stream->clear();
	stream->seekg(0, ios::beg);

	// writing metadata
	ge->encode(nFrames);
	ge->encode(nRows);
	ge->encode(nCols);
	ge->encode(fps);

	// initializing buffers    
    vector <Mat> prevBlocks[3]; 
    vector <Mat> currBlocks[3];
    int i = 0;

	Mat bgr[3];	// frame channels

	getline (*stream,line);
    while(true){

		//cout << i << "\n";
		/*if(i > 30){
			break; 
		}*/

		fprintf(stderr, "Frame: %d\n",i);

		if(!stream->read((char*) frame.data, frame.cols * frame.rows * frame.channels())) break; 
		if (frame.empty()) break;         // check if at end


		if(i==0){
			//cout << "encoding first frame..\n";
			float err = 0.0f;
			encodeIntraframe(frame, bgr,1,&err); 

			// splitting the first frame by blocks
			// to compare with next frame
			for(int j=0; j<3;j++){
				blockSplit(bgr[j],blockHeight,blockWidth,&prevBlocks[j]);
			}
					
		}
		else{

			float avg_error_inter_a[3]= {};
			float avg_error_inter=0.0f; 
			float avg_error_intra=0.0f;

			split(frame, bgr);

			//cout << "testing intraframe\n";
			encodeIntraframe(frame,bgr,0,&avg_error_intra);
			
			for(int j=0; j<3;j++){

				blockSplit(bgr[j],blockHeight,blockWidth,&currBlocks[j]);
				//cout << "testing interframe\n";
				encodeInterframe(&prevBlocks[j],currBlocks[j], 0,&avg_error_inter_a[j]); //evaluate interframe average residue 
				

				//cout.precision(20);
				//cout << "inter error: "<<fixed<< avg_error_inter<<"\n"; 
				//cout << "intra error: "<<fixed<< avg_error_intra<<"\n"; 
				
			}

			float sum = 0.0f; 
			for(int k=0; k<3;k++){
				sum+=avg_error_inter_a[k];
			}
			avg_error_inter = sum / 3.0f;

			//cout.precision(20);
				//cout << "inter error: "<<fixed<< avg_error_inter<<"\n"; 
				//cout << "intra error: "<<fixed<< avg_error_intra<<"\n"; 

			if(abs(avg_error_inter) < abs(avg_error_intra)){
				cout << "intra\n";
				//WriteBit(1)
				encodeIntraframe(frame,bgr,1,&avg_error_intra);
				
				for(int j=0; j<3;j++){
					blockSplit(bgr[j],blockHeight,blockWidth,&prevBlocks[j]);
				}

			}
			else{
				cout << "inter\n";
				//WriteBit(0)
				for(int j=0; j<3;j++){
					
					encodeInterframe(&prevBlocks[j],currBlocks[j], 1,&avg_error_inter);
					
					for(unsigned int v = 0; v < prevBlocks[j].size(); v++) {
                   		prevBlocks[j][v] = currBlocks[j][v].clone();
            		}

					currBlocks[j].clear();
				}
			}

		}
		i++;
	}

	bs->close();
	stream->close();



}

float Predictor::calcEntropy(int total) {

	float totalEntropy = 0;
	for(map<int,int>::iterator it = occurrences.begin(); it != occurrences.end(); it++) {

		float prob = (float) it->second/total;
		totalEntropy += -prob*log2(prob);
	}

	occurrences.clear();

	return totalEntropy;
}

void Predictor::temporalPredict(string filename, int blockHeight, int blockWidth){

	ifstream* stream = new ifstream();
	stream->open(filename);

	if (!stream->is_open())
	{
		cerr << "Error opening file\n";
		return;
	}

	// reading metadata
	string line;
	int nCols, nRows, type, fps;
	getline (*stream,line);

	istringstream(line) >> nCols >> nRows >> fps >> type;
	/*cout << "nCols "<<nCols<<"\n";
	cout << "nRows "<<nRows<<"\n";
	cout << "fps "<<fps<<"\n";*/

	Mat frame = Mat(Size(nCols, nRows), CV_8UC3);
	
	int nFrames = 0;

	while(true) {
		nFrames++;
		//cout << nFrames << "\n";

		if(!stream->read((char*) frame.data, frame.cols * frame.rows * frame.channels())) break; 
		if (frame.empty()) break;         // check if at end
	}

	stream->clear();
	stream->seekg(0, ios::beg);

	// writing metadata
	ge->encode(nFrames);
	ge->encode(nRows);
	ge->encode(nCols);
	ge->encode(fps);


	// initializing buffers    
    vector <Mat> prevBlocks[3]; 
    vector <Mat> currBlocks[3];
    int i = 0;

	Mat bgr[3];	// frame channels

	getline (*stream,line);
    while(true){

		//cout << i << "\n";
		/*if(i > 30){
			break; 
		}*/

		fprintf(stderr, "Frame: %d\n",i);

		if(!stream->read((char*) frame.data, frame.cols * frame.rows * frame.channels())) break; 
		if (frame.empty()) break;         // check if at end


		if(i==0){
			//cout << "encoding first frame..\n";

			encodeIntraframe(frame, bgr,1,0); 

			// splitting the first frame by blocks
			// to compare with next frame
			for(int j=0; j<3;j++){
				blockSplit(bgr[j],blockHeight,blockWidth,&prevBlocks[j]);
			}
					
		}
	
		else{
			//cout << "encoding inter frame "<<i<<"\n";
			//cout << "size NOT FIRST frame: "<<frames.size() << "\n"; 

			split(frame, bgr);
			
			for(int j=0; j<3;j++){

				blockSplit(bgr[j],blockHeight,blockWidth,&currBlocks[j]);
				encodeInterframe(&prevBlocks[j],currBlocks[j], 1,0);

				for(unsigned int v = 0; v < prevBlocks[j].size(); v++) {
                       prevBlocks[j][v] = currBlocks[j][v].clone();
                 }

				currBlocks[j].clear();
			}
		}

		i++;

	}
	
	bs->close();
	stream->close();	
}


//int Predictor::encodeInterframe(Mat frame, std::vector<Mat>* prevBlocks,std::vector<Mat> currBlocks){
int Predictor::encodeInterframe(vector<Mat> *prevBlocks, vector<Mat> currBlocks, int toEncode,float* avg_error){

	/*if ( !frame.data )
    {
		printf("No image data \n");
		return -1;
    }  */

	//cout << prevBlocks->size() << " " << currBlocks.size() << "\n";

	int16_t total_error = 0;
	int count = 0; 
	

	int8_t *current, *previous;
	for(vector<Mat>::size_type idx = 0; idx != currBlocks.size(); idx++){

		for(int r = 0; r < currBlocks[idx].rows; r++) {

			current = currBlocks[idx].ptr<int8_t>(r);
			previous = (*prevBlocks)[idx].ptr<int8_t>(r);
			//cout << "R: "<< r << "\n"; 

			for(int c = 0; c < currBlocks[idx].cols; c++) {

				// diff between two block values
				int16_t residue = (int16_t) current[c] - previous[c];

				total_error += abs(residue); 
				//cout << "residue :"<< residue <<"\n"; 
				count++; 

				/*if(nFrame == 1)
					cout << (int) previous[c] << "\n";*/
				//cout << (int) residue << "\n";
				if(toEncode){
					if(residue < 0) {
						residue = -2*(residue)-1;	
					}
					else {
						residue = 2*residue;
					}

					
					ge->encode((int) residue);
				}
			}
		}
	}
	
	*avg_error = (float)total_error / (float)count; 
	
	//cout << "total error: "<<total_error<<"\n"; 
	//cout << "count: "<<count<<"\n";
	//cout << "avg error: "<<*avg_error<<"\n";
	*prevBlocks = currBlocks;

	/*for ( int k = 0 ; k<currBlocks.size() ; k++){
		prevBlocks[k] = currBlocks[k].clone(); 
	}*/

	return 0;
}

int Predictor::mergeBlock(Mat image, vector<Mat> blocks) {

	int W = 0, H = 0; 

	for( unsigned int b=0 ; b<blocks.size(); b++){


		if( W >= image.cols){
			//cout << "H: " << H << "\n";
			H += blocks[b].rows;
			W=0;
		}

		Mat block = blocks[b]; 

		block.copyTo(image(cv::Rect(W,H,block.cols, block.rows))); 
		
		W+=blocks[b].cols;

	}

	return 0;
}

// block division
int Predictor::blockSplit(Mat image, int blockHeight, int blockWidth,vector<Mat>* smallImages) {

	// get the image data

	Size reducedSize ( blockHeight , blockWidth );

	//namedWindow("smallImages ", CV_WINDOW_AUTOSIZE );
	//
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

			Mat pic = Mat (image , rect).clone();
			pic.convertTo(pic, CV_8UC1);

    		smallImages->push_back (pic);
    		//smallImages->push_back (Mat (image , rect));

			//imshow( "smallImages", cv::Mat ( image, rect ));
   			//waitKey(0);
    	}
	}

	return 0;	
}


void Predictor::spatialPredict(string filename) {

	ifstream* stream = new ifstream();
	stream->open(filename);

	if (!stream->is_open())
	{
		cerr << "Error opening file\n";
		return;
	}

	string line;
	int nCols, nRows, type, fps;

	getline (*stream,line);

	istringstream(line) >> nCols >> nRows >> fps >> type;

	Mat frame = Mat(Size(nCols, nRows), CV_8UC3);

	int nFrames = 0;
	while(true) {
		if(!stream->read((char*) frame.data, frame.cols * frame.rows * frame.channels())) break; 
		nFrames++;
		//if(nFrames == 2) break;
	}

	stream->clear();
	stream->seekg(0, ios::beg);

	ge->encode(nFrames);
	ge->encode(nRows);
	ge->encode(nCols);
	ge->encode(fps);

	nFrames = 0;

	Mat bgr[3];
	while(true) {
		if(!stream->read((char*) frame.data, frame.cols * frame.rows * frame.channels())) break; 
		encodeIntraframe(frame, bgr,1,0);
		nFrames++;
		//if(nFrames == 2) break;
		if(nFrames == 1) break;
	}

	bs->close();


	// building dataset for histogram
	for(map<int,int>::iterator it = occurrences.begin(); it != occurrences.end(); it++) {

		*histogramFile << it->first << " " << it->second << "\n";
	}

	histogramFile->close();
}

// returns entropy
void Predictor::encodeIntraframe(Mat frame, Mat bgr[], int toEncode, float* avg_error) {
	
	//if(toEncode) cout << "Intra" << "\n";
	uint8_t* p, *prev;
	int16_t total_error = 0; 
	int count = 0; 

	//Mat bgr[3];
	split(frame, bgr);

	for(int m = 0; m < 3; m++) {


		for(int row = 0; row < bgr[m].rows; row++) {

			if(row > 0) 
				prev = p;
	
			p = bgr[m].ptr<uint8_t>(row);
			
			for(int col = 0; col < bgr[m].cols; col++) {

				// JPEG-LS mode
				uint8_t x;
				spatialPredictAux(col, row, &x, p, prev);

				int16_t residue = (int16_t) (p[col] - x);

				total_error += residue; 
				count++; 

				if(occurrences.find((int) residue) != occurrences.end()) 
					occurrences[(int) residue]++;
				else
					occurrences.insert(make_pair((int) residue, 1));

				if(toEncode){
					if(residue < 0) {
		
						residue = -2*(residue)-1;	
					}
					else {
						residue = 2*residue;
					}



					ge->encode((int) residue);
				}
			}
		}
	}

	//cout <<"out!!\n"; 

	*avg_error = (float)total_error / (float)count; 
}

void Predictor::temporalDecode(int blockHeight, int blockWidth) {
	
	int nFrames = gd->decode();
	int rows = gd->decode();	// height
	int cols = gd->decode();	// width
	int fps = gd->decode();

	ofstream* outputStream = new ofstream("decoded_video.rgb", 
											ios::binary | ios::out);

	*outputStream << cols << " " << rows << " " << fps << " rgb" << endl;

	Mat frame = Mat::zeros(Size(cols, rows), CV_8UC3);
	vector<Mat> bgr(3);
	for(unsigned int i = 0; i < bgr.size(); i++) 
		bgr[i] = Mat::zeros(Size(rows, cols), CV_8UC1);


	vector <Mat> prevBlocks[3]; 
    vector <Mat> currBlocks[3];

	int16_t residue;

	int8_t *previous,*current; 

	for(int f = 0; f < nFrames; f++) {

		/*if ( f > 30)
			break;*/ 
		fprintf(stderr, "f: %d\n", f);

		//intraframe decoding 
		//
		if(f == 0) { 

			for(int m = 0; m < 3; m++) {

				uint8_t* p, *prev;
				for(int row = 0; row < rows; row++) {

					if(row > 0) 
						prev = p;
			
					p = bgr[m].ptr<uint8_t>(row);

					for(int col = 0; col < cols; col++) {

						uint8_t x;
						spatialPredictAux(col, row, &x, p, prev);
						
						residue = (int16_t) gd->decode();
						
						if(residue %2 == 0) { 	// even
							residue = residue/2;
						}
						else {					// odd	
							residue = -(residue+1)/2;
						}

						
						p[col] = (uint8_t) (residue + x);
					}
				}


				blockSplit(bgr[m],blockHeight,blockWidth,&prevBlocks[m]);
			}

			
	
		}
		else{

			for(int m = 0; m < 3; m++) {
		
				for(unsigned int i = 0; i < prevBlocks[m].size(); i++) {

					Mat prevBlock = prevBlocks[m][i];
			
					int rows = prevBlock.rows;
					int cols = prevBlock.cols;

					Mat block = Mat::zeros(Size(cols, rows), CV_8UC1);

					for( int h=0; h< rows; h++){

						previous = prevBlock.ptr<int8_t>(h);
						current = block.ptr<int8_t>(h);
						
						for(int w=0; w < cols; w++){

							residue = (int16_t) gd->decode();	
							
							if(residue %2 == 0) { 	// even
								residue = residue/2;
							}
							else {					// odd	
								residue = -(residue+1)/2;
							}

							current[w] = previous[w] + residue;
						}
					}

					currBlocks[m].push_back(block);
				}

				for(unsigned int v = 0; v < currBlocks[m].size(); v++)
					prevBlocks[m][v] = currBlocks[m][v].clone();

				
				mergeBlock(bgr[m], currBlocks[m]);
				currBlocks[m].clear();
			}
	
		}

		merge(bgr, frame);
		outputStream->write((char*) frame.data, frame.cols * frame.rows * frame.channels());
	}


	bs->close();
	outputStream->close();
	//cout << "displaying video... ";
    //displayVideo("decoded_video.rgb");

}

void Predictor::spatialDecode() {

	// decoding metadata
	int nFrames = gd->decode();
	int rows = gd->decode();	// height
	int cols = gd->decode();	// width
	int fps = gd->decode();

	ofstream* outputStream = new ofstream("decoded_video1.rgb", 
											ios::binary | ios::out);

	Mat frame = Mat(Size(rows, cols), CV_8UC3);
	vector<Mat> bgr(3);
	for(unsigned int i = 0; i < bgr.size(); i++) 
		bgr[i] = Mat(Size(rows, cols), CV_8UC1);

	// writing metadata to decoded rgb file
	*outputStream << frame.cols << " " << frame.rows << " " << fps << " rgb" << endl;

	int16_t residue;

	// decoding frames residue-wise
	for(int f = 0; f < nFrames; f++) {

		fprintf(stderr, "Frame: %d\n", f);

		for(int m = 0; m < 3; m++) {

			uint8_t* p, *prev;
			for(int row = 0; row < rows; row++) {

				if(row > 0) 
					prev = p;
		
				p = bgr[m].ptr<uint8_t>(row);

				for(int col = 0; col < cols; col++) {

					// JPEG-LS prediction mode
					uint8_t x;
					spatialPredictAux(col, row, &x, p, prev);
					
					residue = (int16_t) gd->decode();
				
					// demodulating value
					if(residue %2 == 0) { 	// even
						residue = residue/2;
					}
					else {					// odd	
						residue = -(residue+1)/2;
					}

					p[col] = (uint8_t) (residue + x);
					//cout << "Residue: " << (int) residue << "\n";
				}
			}
		}

		// merging all channels into a single frame	
		merge(bgr, frame);
		outputStream->write((char*) frame.data, frame.cols * frame.rows * frame.channels());
	}


	bs->close();
	outputStream->close();
    displayVideo("decoded_video.rgb");
}

void Predictor::spatialPredictAux(int col, int row, uint8_t* x, uint8_t* p, uint8_t* prev) {

	// a = previous column pixel; b = previous row pixel in same column
	// c = previous column pixel in previous row
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

	// JPEG-LS prediction mode
	if(c >= max(a, b)) {
		*x = min(a,b);
	} else if (c <= min(a,b)) {
		*x = max(a,b);
	} else {
		*x = a + b - c;
	}
	
}

// video player
void Predictor::displayVideo(string inputFileName) {
	ifstream myfile;
	ofstream os;

	myfile.open(inputFileName);
	if (!myfile.is_open())
	{
		cerr << "Error opening file\n";
		return;
	}

	os.open("out2");

	string line;
	int nCols, nRows, type, fps;
	getline (myfile,line);
	cout << line << endl;

	istringstream(line) >> nCols >> nRows >> fps >> type;
	Mat frame = Mat(Size(nCols, nRows), CV_8UC3);
	
	while(true)
	{

		if(!myfile.read((char*)frame.data, frame.cols * frame.rows * frame.channels())) break;

		Mat bgr[3];
		split(frame, bgr);

		
		if (frame.empty()) break;         // check if at end

		imshow("Display frame", frame);

		if(waitKey((int)(1.0 / fps * 1000)) >= 0) break;

	}

	if(myfile.is_open()) myfile.close();
	os.close();
}
