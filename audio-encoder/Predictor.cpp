#include <cstdlib> 
#include <string>

#include "Golomb.cpp"
#include "Predictor.h"


Predictor::Predictor(int m, string encodedFilename, string decodedFilename){
	
}


void Predictor::simple_predict(int* sequence, int length){
//	int length = sizeof(sequence)/sizeof(*sequence); 

	int remainder[length]; 

	Golomb* g = new Golomb(8, "encoded", "encoded");

	remainder[0] = 0; 

	for(int i=1; i<length; i++){
		cout <<i<<" top\n";  
		remainder[i] = sequence[i-1] - sequence[i]; 
		if( i== length)
			g->encode(remainder[i], 1);
		else
			g->encode(remainder[i],0); 
	}

}

void Predictor::predict(int* sequence, int length){
	//int length = sizeof(sequence)/sizeof(sequence[0]); 
	
	int remainder[length]; 
	
	Golomb* g = new Golomb(8, "encoded", "encoded");
	
	remainder[0] = 0; 
	remainder[1] = 0; 

	for(int i=2; i<length; i++){
		cout <<i<<" top\n"; 
		remainder[i] = (2*sequence[i-1] - sequence[i-2]) - sequence[i]; 
		if( i==length)
			g->encode(remainder[i], 1);
		else
			g->encode(remainder[i],0);
	}



}


int main (int argc, char** argv){
	
	int samples[]= {1,2,3,4,5,6,7,8,9,10};
	Predictor* predictor = new Predictor(12, "encoded","encoded"); 

	predictor->predict(samples,10); 
	
	cout << "top\n"; 	

	return 0; 
}


