#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include "Predictor.h"

int counter;
Predictor::Predictor(string encodedFile, int M, int decodeFlag) {

	counter = 0;
	string mode = decodeFlag ? "r" : "w";

	/*BitStream**/ bs = new BitStream(encodedFile, mode);
	if(mode == "w") 
		ge = new GolombEncoder(bs, M);
	else 
		gd = new GolombDecoder(bs, M);

	histogramFile = new ofstream();
	histogramFile->open("hist.txt", ios::out | ios::app);
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
		//if(nFrames == 1) break;
	}


	stream->clear();
	stream->seekg(0, ios::beg);

	ge->encode(nFrames);
	//ge->encode(1);
	ge->encode(nRows);
	ge->encode(nCols);
	ge->encode(fps);
	counter += 4;

	nFrames = 0;
	while(true) {
		if(!stream->read((char*) frame.data, frame.cols * frame.rows * frame.channels())) break; 
		encodeIntraframe(frame);
		nFrames++;

		cout << "Frame: " << nFrames << "\n";
		//if(nFrames == 1) break;
	}

	bs->close();


	// building dataset for histogram
	for(map<int,int>::iterator it = occurrences.begin(); it != occurrences.end(); it++) {

		*histogramFile << it->first << " " << it->second << "\n";
	}

	histogramFile->close();
}

void Predictor::encodeIntraframe(Mat frame) {

	if ( !frame.data )
    {
    	printf("No image data \n");
        return;
    }
	Mat bgr[3];

	split(frame, bgr);

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

				if(occurrences.find((int) residue) != occurrences.end()) 
					occurrences[(int) residue]++;
				else
					occurrences.insert(make_pair((int) residue, 1));


				counter++;

			
				if(residue < 0) {
	
					residue = -2*(residue)-1;	
				}
				else {
					residue = 2*residue;
				}

				//if(counter <= 3128734) {
					ge->encode((int) residue);
					//cout << "Residue: " << residue << "\n";
				//}
			}
		}
	}
}

void Predictor::spatialDecode() {

	int nFrames = gd->decode();
	int rows = gd->decode();	// height
	int cols = gd->decode();	// width
	int fps = gd->decode();

	counter += 4;
	ofstream* outputStream = new ofstream("decoded_video.rgb", 
											ios::binary | ios::out);

	Mat frame = Mat(Size(rows, cols), CV_8UC3);
	vector<Mat> bgr(3);
	for(unsigned int i = 0; i < bgr.size(); i++) 
		bgr[i] = Mat(Size(rows, cols), CV_8UC1);

	*outputStream << frame.cols << " " << frame.rows << " " << fps << " rgb" << endl;

	int16_t residue;
	for(int f = 0; f < nFrames; f++) {

		cout << "Frame: " << f << "\n";
		uint8_t* p, *prev;
	
		for(int m = 0; m < 3; m++) {
			for(int row = 0; row < rows; row++) {

				if(row > 0) 
					prev = p;
		
				p = bgr[m].ptr<uint8_t>(row);

				for(int col = 0; col < cols; col++) {

					counter++;
					uint8_t x;
					predict_aux(col, row, &x, p, prev);
					
					residue = (int16_t) gd->decode();

					//cout << "Residue: " << (int) residue << "\n";
					
					if(residue %2 == 0) { 	// even
						residue = residue/2;
					}
					else {					// odd	
						residue = -(residue+1)/2;
					}

					p[col] = (uint8_t) (residue + x);
				}
			}
		}
		
		merge(bgr, frame);

		outputStream->write((char*) frame.data, frame.cols * frame.rows * frame.channels());
	}

	/*namedWindow( "Decoded image", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Decoded image", output );                   // Show our image inside it.

    waitKey(0);                                          // Wait for a keystroke in the window*/
	bs->close();
	outputStream->close();
    //displayVideo("decoded_video.rgb");
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

void Predictor::displayVideo(string inputFileName) {
	ifstream myfile;

	myfile.open(inputFileName);
	if (!myfile.is_open())
	{
		cerr << "Error opening file\n";
		return;
	}

	string line;
	int nCols, nRows, type, fps;
	getline (myfile,line);

	istringstream(line) >> nCols >> nRows >> fps >> type;
	Mat frame = Mat(Size(nCols, nRows), CV_8UC3);
	

	while(true)
	{

		if(!myfile.read((char*)frame.data, frame.cols * frame.rows * frame.channels())) break;

		if (frame.empty()) break;         // check if at end

		imshow("Display frame", frame);

		if(waitKey((int)(1.0 / fps * 1000)) >= 0) break;
	}
	
	if(myfile.is_open()) myfile.close();

}
