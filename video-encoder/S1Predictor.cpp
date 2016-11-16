#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace cv;


S1Predictor::()


int main(int argc, char* argv){
		
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




}
