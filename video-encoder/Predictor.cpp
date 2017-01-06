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

		if(i>1){
			break; 
		}

		if(!stream->read((char*) frame.data, frame.cols * frame.rows * frame.channels())) break; 
		if (frame.empty()) break;         // check if at end


		if(i==0){
			//cout << "encoding first frame..\n";

			encodeIntraframe(frame, bgr); 

			// todo: return splitted frame on intraframe
			//split(frame, bgr);

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

				if((encodeInterFrame(frame,&prevBlocks[j],currBlocks[j])) != 0) {
					bs->close();
					stream->close();	
					return;
				}
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

int Predictor::encodeInterFrame(Mat frame, std::vector<Mat>* prevBlocks,std::vector<Mat> currBlocks){

	if ( !frame.data )
    {
		printf("No image data \n");
		return -1;
    }  

	//cout << prevBlocks->size() << " " << currBlocks.size() << "\n";

	int8_t *current, *previous;
	for(vector<Mat>::size_type idx = 0; idx != currBlocks.size(); idx++){

		for(int r = 0; r < currBlocks[idx].rows; r++) {

			current = currBlocks[idx].ptr<int8_t>(r);
			previous = (*prevBlocks)[idx].ptr<int8_t>(r);

			for(int c = 0; c < currBlocks[idx].cols; c++) {

				//ptrDiff[c] = current[c] - previous[c];
				//
				// diff between two block values
				int16_t residue = (int16_t) current[c] - previous[c];

				//cout << "Current: " << (int) current[c] << "\n";	
				//cout << "Previous: " << (int) previous[c] << "\n";	
				//
				if (c < 10 && r == 0 && idx == 0)
					cout << (int)residue << "\n"; 

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
	
	*prevBlocks = currBlocks;

	/*for ( int k = 0 ; k<currBlocks.size() ; k++){
		prevBlocks[k] = currBlocks[k].clone(); 
	}*/

	return 0;


}

int Predictor::mergeBlock(Mat image, int blockHeight, int blockWidth, vector<Mat> blocks) {

	int W = 0, H = 0; 

	uint8_t* prev, *blockRow, *p;

	for( int b=0 ; b<blocks.size(); b++){

		for(int row =0 ; row < blocks[b].rows ; row++){

			cout << "H--"<<H<<"\n";
			p = image.ptr<uint8_t>(row + H);
			blockRow = blocks[b].ptr<uint8_t>(row);


			for(int col =0 ; col < blocks[b].cols; col++){
				//image[row + H][col + W]  = blocks[b][row][col]
				p[col + W]  = blockRow[col];
				
				//cout << "r "<<row<<" c "<<col<<"\n"; 
			
				W+=blocks[b].cols;
				if( W >= image.cols){

					H += blocks[b].rows;
					W=0;
				}
			}
		}
	}

}
// block division
int Predictor::blockSplit(Mat image, int blockHeight, int blockWidth,vector<Mat>* smallImages) {

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
		encodeIntraframe(frame, bgr);
		nFrames++;
		//if(nFrames == 2) break;
		//cout << "Frame: " << nFrames << "\n";
		if(nFrames == 1) break;
	}

	bs->close();


	// building dataset for histogram
	for(map<int,int>::iterator it = occurrences.begin(); it != occurrences.end(); it++) {

		*histogramFile << it->first << " " << it->second << "\n";
	}

	histogramFile->close();
}

void Predictor::encodeIntraframe(Mat frame, Mat bgr[]) {


/*	if ( !frame.data )
    {
    	printf("No image data \n");
        return;
    }*/
	
	uint8_t* p, *prev;

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

				
				/*if(m == 2)*/
				 //cout <<  "Value: " << (int) p[col] << "\n";

				if(occurrences.find((int) residue) != occurrences.end()) 
					occurrences[(int) residue]++;
				else
					occurrences.insert(make_pair((int) residue, 1));

				//if (col < 10 && row == 0 && m == 0)
					//cout << "Intra: " << (int) residue << "\n";

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

void Predictor::temporalDecode(int blockHeight, int blockWidth) {
	
	int nFrames = gd->decode();
	int rows = gd->decode();	// height
	int cols = gd->decode();	// width
	int fps = gd->decode();

	ofstream* outputStream = new ofstream("decoded_video.rgb", 
											ios::binary | ios::out);

	Mat frame = Mat(Size(rows, cols), CV_8UC3);
	vector<Mat> bgr(3);
	for(unsigned int i = 0; i < bgr.size(); i++) 
		bgr[i] = Mat(Size(rows, cols), CV_8UC1);

	//cout << frame.cols << " " << frame.rows << " " << fps << " rgb" << endl;

	vector <Mat> prevBlocks[3]; 
    vector <Mat> currBlocks[3];

	int16_t residue;

	int8_t *previous,*current; 

	for(int f = 0; f < nFrames; f++) {

		if ( f > 1)
			break; 

		cout << "Frame: " << f << "\n";

		if(f == 0){ //intraframe decoding 
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

						if (col < 10 && row == 0 && m == 0)
						//	cout << (int)residue << "\n";

						p[col] = (uint8_t) (residue + x);
						//cout << "Residue: " << (int) residue << "\n";
						//cout <<  "Value: " << (int) p[col] << "\n";
					}
				}

				blockSplit(bgr[m],blockHeight,blockWidth,&prevBlocks[m]);
			}
			
			
			//blockMerge(image, blockHeight, blockWidth, &prevBlocks);
		}
		else{



			for(int m = 0; m < 3; m++) {
				cout << "m: "<<m<<"\n";

				for(int i = 0; i < prevBlocks[m].size(); i++) {
					Mat block = Mat::zeros(Size(blockHeight, blockWidth), CV_8UC1);
					
					for( int h=0; h<blockHeight ; h++){
						
						//current = currBlocks[idx].ptr<int8_t>(h);
						previous = (*prevBlocks)[m].ptr<int8_t>(h);
						
						current = block.ptr<int8_t>(h);
						for(int w=0; w<blockWidth ; w++){
							
							residue = (int16_t) gd->decode();	
							

							if(residue %2 == 0) { 	// even
								residue = residue/2;
							}
							else {					// odd	
								residue = -(residue+1)/2;
							}

							//if (w < 10 && h == 0 && m == 0 && i==0)
								//cout << residue << "\n";
							//cout << "w: "<<w<<" h: "<<h<<" i: "<<i<<"\n";

							current[w] = previous[w] + residue;
						}


					}

					currBlocks[m].push_back(block);
				}

				mergeBlock(frame, blockHeight, blockWidth, currBlocks[m]);

			}
			
			for(int k = 0; k< 3; k++){
				prevBlocks[k] = currBlocks[k]; 
			}	
		}

		merge(bgr, frame);
		outputStream->write((char*) frame.data, frame.cols * frame.rows * frame.channels());		
	}


	bs->close();
	outputStream->close();
	cout << "displaying video... ";
    displayVideo("decoded_video.rgb");

}





void Predictor::spatialDecode() {

	int nFrames = gd->decode();
	int rows = gd->decode();	// height
	int cols = gd->decode();	// width
	int fps = gd->decode();

	ofstream* outputStream = new ofstream("decoded_video.rgb", 
											ios::binary | ios::out);

	Mat frame = Mat(Size(rows, cols), CV_8UC3);
	vector<Mat> bgr(3);
	for(unsigned int i = 0; i < bgr.size(); i++) 
		bgr[i] = Mat(Size(rows, cols), CV_8UC1);

	//*outputStream << frame.cols << " " << frame.rows << " " << fps << " rgb" << endl;

	int16_t residue;
	for(int f = 0; f < nFrames; f++) {

		cout << "Frame: " << f << "\n";
	
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
					//cout << "Residue: " << (int) residue << "\n";
					//cout <<  "Value: " << (int) p[col] << "\n";
				}
			}
		}
		
		merge(bgr, frame);
		outputStream->write((char*) frame.data, frame.cols * frame.rows * frame.channels());
	}


	bs->close();
	outputStream->close();
    displayVideo("decoded_video.rgb");
}

void Predictor::spatialPredictAux(int col, int row, uint8_t* x, uint8_t* p, uint8_t* prev) {

	
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
