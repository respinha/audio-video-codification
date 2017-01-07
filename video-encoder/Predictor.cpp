#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

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
		fprintf(stderr, "Frame: %d\n",i);

		if(!stream->read((char*) frame.data, frame.cols * frame.rows * frame.channels())) break; 
		if (frame.empty()) break;         // check if at end


		if(i==0){
			//cout << "encoding first frame..\n";

			split(frame, bgr);

			// encoding channel by channel
			// first frame is always intraframe
			for(int j=0; j< 3;j++) {

				encodeIntraframe(bgr[j], 1);
				splitFrame(bgr[j], blockHeight, blockWidth, &prevBlocks[j]);
			}
	
		}
	
		else{

			split(frame, bgr);

			// channel-wise	
			for(int j=0; j<3;j++){

				// evaluating entropy
				// of intraframe encoding
				encodeIntraframe(bgr[j], 0);

				float intraEntropy = calcEntropy(frame.cols * frame.rows);
				if(intraEntropy < 0) {

					bs->close();
					stream->close();	
					return;
				}

				// evaluating entropy
				// of interframe encoding
				splitFrame(bgr[j], blockHeight, blockWidth, &currBlocks[j]);	
				encodeInterframe(prevBlocks[j], currBlocks[j], 0);

				float interEntropy = calcEntropy(frame.rows * frame.cols);
				if(interEntropy < 0) {

					bs->close();
					stream->close();	
					return;
				}

				// signaling with 1 or 0 
				// if frame is intra or inter 
				// encoding
				// option: use directly bs->writeBit(mode)??
				int mode = interEntropy > intraEntropy;
				//ge->encode(mode);
				bs->writeBit(mode);
		
				/*if(mode)
					encodeInterframe(prevBlocks[j], currBlocks[j], 1);
				else*/
				encodeIntraframe(bgr[j], 1);

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
void Predictor::encodeInterframe(vector<Mat> prevBlocks, vector<Mat> currBlocks, int toEncode){

	if(toEncode) cout << "Inter" << "\n";

	int8_t *current, *previous;
	for(vector<Mat>::size_type idx = 0; idx != currBlocks.size(); idx++){

		//for(int r = 0; r < currBlock.rows; r++) {
		for(int r = 0; r < currBlocks[idx].rows; r++) {
		
			// row of current block
			current = currBlocks[idx].ptr<int8_t>(r);
			// row of previous block
			previous = prevBlocks[idx].ptr<int8_t>(r);

			for(int c = 0; c < currBlocks[idx].cols; c++) {

				// diff between two pixels
				int16_t residue = (int16_t) current[c] - previous[c];

				// counting occurrences of each residue 
				// in order to calculate entropy
				if(occurrences.find((int) residue) != occurrences.end()) 
					occurrences[(int) residue]++;
				else
					occurrences.insert(make_pair((int) residue, 1));

				if(toEncode) {
				
					// modulating residue value
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
}

// block division
void Predictor::splitFrame(Mat image, int blockHeight, int blockWidth,vector<Mat>* smallImages) {

	Size reducedSize ( blockHeight , blockWidth );

	for ( int y =  0 ; y < image.rows ; y += reducedSize.height )
	{
		for ( int x =  0 ; x < image.cols ; x += reducedSize.width )
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
    		}
	}
}

void Predictor::spatialPredict(string filename) {

	// loading rgb file
	ifstream* stream = new ifstream();
	stream->open(filename);

	if (!stream->is_open())
	{
		cerr << "Error opening file\n";
		return;
	}

	// loading file metadata
	string line;
	int nCols, nRows, type, fps;

	getline (*stream,line);

	istringstream(line) >> nCols >> nRows >> fps >> type;

	Mat frame = Mat(Size(nCols, nRows), CV_8UC3);

	int nFrames = 0;

	// counting number of frames
	while(true) {
		if(!stream->read((char*) frame.data, frame.cols * frame.rows * frame.channels())) break; 
		nFrames++;
		//if(nFrames == 2) break;
	}

	stream->clear();
	stream->seekg(0, ios::beg);

	// encoding to metadata
	ge->encode(nFrames);
	ge->encode(nRows);
	ge->encode(nCols);
	ge->encode(fps);

	nFrames = 0;

	Mat bgr[3]; // channels

	getline(*stream,line);

	
	while(true) {
		if(!stream->read((char*) frame.data, frame.cols * frame.rows * frame.channels())) break; 

		split(frame, bgr);
		
		for(int i = 0; i < 3; i++)
			encodeIntraframe(bgr[i], 1);

		nFrames++;
		//if(nFrames == 2) break;
		cout << "Frame: " << nFrames << "\n";
		//if(nFrames == 1) break;
	}

	bs->close();


	// building dataset for histogram
	/*for(map<int,int>::iterator it = occurrences.begin(); it != occurrences.end(); it++) {

		*histogramFile << it->first << " " << it->second << "\n";
	}

	histogramFile->close();*/
}

// returns entropy
void Predictor::encodeIntraframe(Mat frame, int toEncode) {
	
	if(toEncode) cout << "Intra" << "\n";
	uint8_t* p, *prev;

	//for(int m = 0; m < 3; m++) {
	//
		for(int row = 0; row < frame.rows; row++) {

			if(row > 0) 
				prev = p;
	
			p = frame.ptr<uint8_t>(row);
			
			for(int col = 0; col < frame.cols; col++) {

				// JPEG-LS prediction mode
				uint8_t x;
				spatialPredictAux(col, row, &x, p, prev);

				int16_t residue = (int16_t) (p[col] - x);

				 //cout <<  "Value: " << (int) p[col] << "\n";

				if(occurrences.find((int) residue) != occurrences.end()) 
					occurrences[(int) residue]++;
				else
					occurrences.insert(make_pair((int) residue, 1));

				//cout << "Intra: " << (int) residue << "\n";
				if(toEncode) {
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
