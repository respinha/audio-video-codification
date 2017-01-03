#include <iostream>
#include <getopt.h>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
using namespace std;

void displayVideo(string);

int main(int argc, char **argv)
{
	if(argc < 2) {
		cerr << "Invalid argc\n";
		return -1;
	}

	string inputFileName = argv[1];
	displayVideo(inputFileName);

	return 0;
}

void displayVideo(string inputFileName) {
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
	cout << line << endl;

	istringstream(line) >> nCols >> nRows >> fps >> type;
	Mat frame = Mat(Size(nCols, nRows), CV_8UC3);
	
	int count = 0;
	while(true)
	{

		if(!myfile.read((char*)frame.data, frame.cols * frame.rows * frame.channels())) break;

		Mat bgr[3];
		split(frame, bgr);

		if (frame.empty()) break;         // check if at end

		imshow("Display frame", frame);

		if(waitKey((int)(1.0 / fps * 1000)) >= 0) break;

		count++;
		//if(count == 1) break;
	}

	if(myfile.is_open()) myfile.close();


}
