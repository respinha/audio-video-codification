#include <cstdlib> 
#include <string>
#include <algorithm>
#include <fstream>

#include "Predictor.h"

Predictor::Predictor(int m, string encodedFilename, int pos){
	g = new Golomb(m, encodedFilename, pos);
}

void Predictor::reverse_simple_predict(short* buffer, short* samples, int* end){
	cout << "entra reverse predictor TOP\n"; 

	// left  
	short residue = g->decode(end);
	if(end) return;

	samples[0] = residue + buffer[0];

	// right
	residue = g->decode(end);
	if(end) return;

	samples[1] = residue + buffer[1]; 

	cout << "Samples: ";
	cout << samples[0] << "; " << samples[1] << "\n";

	buffer[0] = samples[0];
	buffer[1] = samples[1];
}




void Predictor::simple_predict(short* sequence, short* sequence_buf){
	short remainderL;
    short remainderR; 	
		
	remainderL = sequence[0] - sequence_buf[0];
	remainderR = sequence[1] - sequence_buf[1];

	g->encode(remainderL,0);
	g->encode(remainderR,0); 
}		

int Predictor::getFilePosition() {
	return g->getFilePosition();
}
/*
void Predictor::predict(int* sequence, int length){
	//int length = sizeof(sequence)/sizeof(sequence[0]); 
	
	int remainder[length]; 
	
	
	remainder[0] = 0; 
	remainder[1] = 0; 

	for(int i=2; i<length; i++){
		remainder[i] = (2*sequence[i-1] - sequence[i-2]) - sequence[i]; 
		if( i==length)
			g->encode(remainder[i], 1);
		else
			g->encode(remainder[i],0);
	}
}*/




