#include <iostream>
#include "Predictor.h"

int main(int argc, char** argv) {

	// to implement
	Predictor* p = new Predictor(argv[1], atoi(argv[2]), 1);
	p->temporalDecode(50,50);
	
	return 0;
}