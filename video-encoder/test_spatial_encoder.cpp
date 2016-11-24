#include <iostream>
#include "Predictor.cpp"

int main(int argc, char** argv) {

	// to implement

	Predictor* p = new Predictor(argv[1], argv[2], atoi(argv[3]), 0);
	p->predict_encode(1);

	
	return 0;
}
