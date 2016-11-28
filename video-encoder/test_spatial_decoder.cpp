#include <iostream>
#include "Predictor.cpp"

int main(int argc, char** argv) {

	if(argc < 2) {
		fprintf(stderr, "Usage: <encoded file name> <M parameter>\n");
		return -1;
	}
	// to implement
	Predictor* p = new Predictor(argv[1], atoi(argv[2]), 1);
	p->predict_decode();
	
	return 0;
}
