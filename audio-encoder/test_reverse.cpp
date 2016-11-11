#include <cstdlib>
#include <iostream>

using namespace std;

void do_stuff(short[],short[],int*);

int main(void) {

	
	short outputSamples[2] = {0,0};
	short buffer[2] = {4,5};

	int end = 0;
	int* pend = &end;

	for(int i = 0; i < 2; i++) outputSamples[i] = 0;

	do_stuff(buffer, outputSamples, pend);
	for(int i = 0; i < 2; i++) outputSamples[i] = 0;

	
	do_stuff(buffer, outputSamples, pend);
	for(int i = 0; i < 2; i++) cout << outputSamples[i] << "\n";
	do_stuff(buffer, outputSamples, pend);
	for(int i = 0; i < 2; i++) cout << outputSamples[i] << "\n";
	
	do_stuff(buffer, outputSamples, pend);
	for(int i = 0; i < 2; i++) cout << outputSamples[i] << "\n";
	
	do_stuff(buffer, outputSamples, pend);
	for(int i = 0; i < 2; i++) cout << outputSamples[i] << "\n";

	return 0;
}

void do_stuff(short buffer[], short samples[], int* end) {

	// left 
	

	samples[0] = samples[0] + buffer[0];

	samples[1] = samples[1] + buffer[1]; 

/*	cout << "Samples: ";
	cout << samples[0] << "; " << samples[1] << "\n";*/

	buffer[0] = samples[0];
	buffer[1] = samples[1];
}
