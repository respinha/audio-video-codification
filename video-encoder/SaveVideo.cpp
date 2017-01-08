#include <iostream>
#include <getopt.h>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void print_usage()
{
	printf("Usage: SaveVideo [-v] [-h] [-c #camId] [-f fileName] [-d #dropFrames]\n");
}

int main(int argc, char **argv)
{
	int option;
	bool end = false;
	VideoCapture cap;
	int camId = 0;
	string outputFileName = "out.rgb";
	bool verbose = false;
	int dropCount = 1;
	int frameCount = 0;

	while ((option = getopt(argc, argv,"vhc:f:d:")) != -1) {
		switch (option)
		{
		case 'f':
			outputFileName = optarg;
			break;
		case 'c':
			camId = atoi(optarg);
			break;
		case 'd':
			dropCount = atoi(optarg);
			break;
		case 'h':
			print_usage();
			exit(EXIT_FAILURE);
			break;
		case 'v':
			verbose = true;
			break;
		default:
			print_usage();
			exit(EXIT_FAILURE);
		}
	}

	cap.open(camId);
	if(!cap.isOpened()) return -1;

	Mat frame;
	cap >> frame;

	cout << dropCount;
	cout << cap.get(CV_CAP_PROP_FPS);
	cout << (int)(cap.get(CV_CAP_PROP_FPS) / dropCount);

	ofstream myfile (outputFileName);
	myfile << frame.cols << " " << frame.rows << " " << (int)(cap.get(CV_CAP_PROP_FPS) / dropCount) << " rgb" << endl;
	if(verbose) cout << frame.cols << " " << frame.rows << " " << (int)(cap.get(CV_CAP_PROP_FPS) / dropCount) << " rgb" << endl;

	namedWindow("rgb",CV_WINDOW_AUTOSIZE);
	while(!end)
	{
		cap >> frame;
		frameCount++;

		if(frameCount % dropCount == 0)
		{
			myfile.write((char*)frame.data, frame.cols * frame.rows * frame.channels());
			frameCount = 0;
		}

		imshow("rgb", frame);

		int key = waitKey(30);
		switch((char)key)
		{
		case 'q':
			end = true;
			break;
		}
	}
	myfile.close();
	return 0;
}
