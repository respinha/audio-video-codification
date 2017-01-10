#include <iostream>
#include "Predictor.h"

int main(int argc, char** argv) {

	if(argc < 5) {
		fprintf(stderr, "Usage: <video file> <dst encoded file> <M parameter> <block dim>\n");
		return -1;
	}

	Predictor* p = new Predictor(argv[2], atoi(argv[3]), 0);
	p->hybridEncode(argv[1],atoi(argv[4]), atoi(argv[4]));
		
	return 0;
}
