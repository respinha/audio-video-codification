#include <cstdlib> 
#include <string>

#include "Golomb.cpp"
#include "Predictor.h"




int* Predictor::simple_predict(int* sequence){
	int* remainder; 

	Golomb* g = new Golomb(atoi(argv[1]), "encoded", "encoded");


	for(int i=1; i<sequence.length(); i++){
		remainder[i] = sequence[i-1] - sequence[i]; 
		g->encode(remainder[i], 1);

		
	}

}

int* Predictor::predict(int* sequence){
	int* remainder; 
	
	Golomb* g = new Golomb(atoi(argv[1]), "encoded", "encoded");


	for(int i=2; i<sequence.length(); i++){
		remainder[i] = (2*sequence[i-1] - sequence[i-2]) - sequence[i]; 
		g->encode(remainder[i], 1);

		
	}



}
