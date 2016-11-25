#include <iostream>
#include "Predictor.cpp"

int main(int argc, char** argv) {

	if(argc < 4) {
		fprintf(stderr, "Usage: <video file> <dst encoded file> <M parameter>\n");
		return -1;
	}

	Predictor* p = new Predictor(argv[2], atoi(argv[3]), 0);
	p->predict_encode(argv[1], 1);

	
	return 0;
}
