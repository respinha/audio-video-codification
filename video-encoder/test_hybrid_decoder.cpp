#include <iostream>
#include "Predictor.h"

int main(int argc, char** argv) {

	if(argc < 3) {
		fprintf(stderr, "Usage: <rgb file> <dst encoded file> <M>\n");
		return -1; 
	}
	Predictor* p = new Predictor(argv[1], atoi(argv[2]), 1);
	p->hybridDecode(50,50);
		
	return 0;
}
