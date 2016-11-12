#include <cstdlib> 
#include <string>
#include <algorithm>
#include <fstream>

#include "Predictor.h"

Predictor::Predictor(int m, string encodedFilename, int pos){
	g = new Golomb(m, encodedFilename, pos);
}

void Predictor::order1_predict(short* sequence, short* sequence_buf, int end){
	short remainderL;
    short remainderR; 	


	remainderL = sequence[0] - sequence_buf[0];
	remainderR = sequence[1] - sequence_buf[1];

	//cout << "sampleL: " << sequence[0] << " - " << sequence_buf[0];
	// cout << "sampleR: " << sequence[1] << " - " << sequence_buf[1];
	cout << "value " << remainderL << "\n";
	cout << "value2 " << remainderR<< "\n";

	g->encode(remainderL,0);
	g->encode(remainderR,end); 

}		

int Predictor::getFilePosition() {
	return g->getFilePosition();
}

void Predictor::reverse_order1_predict(short* buffer, short* samples, int* end){

	// left
	short residue = g->decode(end);
	if(*end) return;

	samples[0] = residue + buffer[0];

	// right
	residue = g->decode(end);
	if(*end) return;


	samples[1] = residue + buffer[1];

	buffer[0] = samples[0];
	buffer[1] = samples[1];

}

void Predictor::order2_predict(short* sequence, short* left_buffer, short* right_buffer,short* residues, int end){
	short remainderL = (2*left_buffer[1] - left_buffer[0]) - sequence[0];
	short remainderR = (2*right_buffer[1] -right_buffer[0]) - sequence[1];

	residues[0] = remainderL;
	residues[1] = remainderR;

	g->encode(remainderL,0);
	g->encode(remainderR,end);

	left_buffer[0] = left_buffer[1];
	right_buffer[0] = right_buffer[1];

	left_buffer[1] = sequence[0];
	right_buffer[1] = sequence[1];

}
void Predictor::reverse_order2_predict(short* prev_buffer, short* buffer, short* samples, int* end) {

	// left
	short residue = g->decode(end);
	if(*end) return;
	// right

	samples[0] = residue - (2*prev_buffer[0]) + buffer[0];

	residue = g->decode(end);
	if(*end) return;

	samples[1] = residue - (2*prev_buffer[1]) + buffer[1];

	prev_buffer[0] = buffer[0];
	prev_buffer[1] = buffer[1];

	buffer[0] = samples[0];
	buffer[1] = samples[1];
}

void Predictor::order3_predict(short* left_buffer, short* right_buffer, short* samples, int end) {

	// 3(n−1) − 3(n−2) + (n−3)
	
	short remainderL = samples[0] - ((3* left_buffer[2]) - 3*(left_buffer[1]) + left_buffer[2]);
	short remainderR = samples[1] - ((3* right_buffer[2]) - 3*(right_buffer[1]) + right_buffer[2]);

	//cout << "sampleL: " << sequence[0] << " - " << sequence_buf[0];
	// cout << "sampleR: " << sequence[1] << " - " << sequence_buf[1];
	cout << "value " << remainderL << "\n";
	cout << "value2 " << remainderR<< "\n";

	g->encode(remainderL,0);
	g->encode(remainderR,end); 

}

void Predictor::reverse_order3(short* left_buffer, short* right_buffer, short* samples, int* end) {

	// left
	short residue = g->decode(end);
	if(*end) return;
	// right

	samples[0] = residue + ((3* left_buffer[2]) - 3*(left_buffer[1]) + left_buffer[2]);

	residue = g->decode(end);
	if(*end) return;

	samples[1] = residue + ((3* left_buffer[2]) - 3*(left_buffer[1]) + left_buffer[2]);
}
