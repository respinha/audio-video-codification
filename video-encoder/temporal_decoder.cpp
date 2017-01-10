#include <iostream>
#include "Predictor.h"

int main(int argc, char** argv) {

	if(argc < 4) {
		fprintf(stderr, "Usage: <encoded file> <M parameter> <block dim>\n");
		return -1;
	}	

	Predictor* p = new Predictor(argv[1], atoi(argv[2]), 1);
	p->temporalDecode(atoi(argv[3]),atoi(argv[3]));
	
	return 0;
}
