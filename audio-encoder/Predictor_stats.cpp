#include <cstdlib> 
#include <string>
#include <algorithm>

// entropy or histogram dependencies
#include <map>
#include <cmath>

#include "Golomb.cpp"
#include "Predictor.h"

#include <sndfile.h> 

Predictor::Predictor(int m, string encodedFilename){
	
}
float Predictor::calcEntropy(map<short,float> o) {

	float total = 0, sum = 0;

	for(map<short,float>::iterator it = o.begin(); it != o.end(); it++)
		total++;

	for(map<short,float>::iterator it = o.begin(); it != o.end(); it++) {
		float prob = it->second/total;
		sum += prob * log2(prob);
	}

	return sum;

}

void Predictor::simple_predict(short* sequence, short* sequence_buf, int length){
//	int length = sizeof(sequence)/sizeof(*sequence); 
	
	

	short remainderL;
	short remainderR; 	

	Golomb* g = new Golomb(8, "encoded");

	cout << "DEBUG: L = " << sequence[0] << " - " <<  sequence_buf[0] << "\n"; 

	cout << "DEBUG: R= " << sequence[1] << " - " <<  sequence_buf[1] << "\n"; 

	
	remainderL = sequence[0] - sequence_buf[0];
	remainderR = sequence[1] - sequence_buf[1];
	int rL = remainderL;

	g->encode(remainderL,0);
	g->encode(remainderR,0); 

}		

void Predictor::predict(int* sequence, int length){
	//int length = sizeof(sequence)/sizeof(sequence[0]); 
	
	int remainder[length]; 
	
	Golomb* g = new Golomb(8, "encoded");
	
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
	
//	int samples[]= {1,2,3,4,5,6,7,8,9,10};


	Predictor* predictor = new Predictor(12, "encoded"); 

	SNDFILE *soundFileIn; /* Pointer for input sound file */
	//SNDFILE *soundFileOut; /* Pointer for output sound file */
	SF_INFO soundInfoIn; /* Input sound file Info */
	//SF_INFO soundInfoOut; /* Output sound file Info */

	int i;
	short sample[2] = {0,0};
	sf_count_t nSamples = 1;

	if (argc < 2){
		fprintf(stderr, "Usage: <input file> \n");
		return -1;
	}


	/* When opening a file for read, the format field should be set to zero
	 * before calling sf_open(). All other fields of the structure are filled
	 * in by the library
	 */	
	soundInfoIn.format = 0;
	soundFileIn = sf_open (argv[1], SFM_READ, &soundInfoIn);
	
	if (soundFileIn == NULL){
		fprintf(stderr, "Could not open file for reading: \"%s\"\n",
		  argv[argc-2]);
		sf_close(soundFileIn);
		return -1;
	}

	fprintf(stderr, "Frames (samples): %d\n", (int) soundInfoIn.frames);
	fprintf(stderr, "Samplerate: %d\n", soundInfoIn.samplerate);
	fprintf(stderr, "Channels: %d\n", soundInfoIn.channels);

	/* When opening a file for write, it is necessary to fill in the
	 * structure members samplerate, channels and format.
	 */
	/*soundInfoOut.samplerate = soundInfoIn.samplerate;
	soundInfoOut.channels = soundInfoIn.channels;
	soundInfoOut.format = soundInfoIn.format;*/

//	soundFileOut = sf_open (argv[2], SFM_WRITE, &soundInfoOut);

	/*if (soundFileOut == NULL){
		fprintf(stderr, "Could not open file for writing: \"%s\"\n", argv[2]);
		sf_close(soundFileIn);
		return -1;
	}*/
	
	short tmp_buffer[2] = {0,0}; 

	
	cout << "top 1\n"; 
	
	short matrix[soundInfoIn.frames+1][2];

	ofstream* left = new ofstream("hist_left", ios::out | ios::app);
	ofstream* right = new ofstream("hist_right", ios::out | ios::app);

	matrix[0][0] = 0;
	matrix[0][1] = 0;

	*left << 1;
	*left << ",";
	*left << 0;
	*left << "\n";

	*right << 1;
	*right << ",";
	*right << 0;
	*right << "\n";

	map<short, float> occurrenciesL;	
	map<short, float> occurrenciesR;	
	occurrenciesL.insert(make_pair(0, 1));
	occurrenciesR.insert(make_pair(0, 1));

	
	for (i = 0; i < soundInfoIn.frames ; i++)
	{
		
		cout << "cheguei memo top\n";
		if (sf_readf_short(soundFileIn, sample, nSamples) == 0){
			
			fprintf(stderr, "Error: Reached end of file\n");
			sf_close(soundFileIn);
			break;
		
		}else{
			

			/*if(i==0){
				short* first  = new short[2];
				short *first_buffer = new short[2];

				predictor->simple_predict(first,first_buffer,2);				
			}*/

			matrix[i+1][0] = sample[0];
			matrix[i+1][1] = sample[1];

			short valL = sample[0];
			short valR = sample[1];	

			
		
			if(i == soundInfoIn.frames-1) {
				cout << "ShortL: " << valL << "\n";
				cout << "ShortR: " << valR << "\n";
			}

			*left << i+2;
			*left << ",";
			*left << valL;
			*left << "\n";

			*right << i+2;
			*right << ",";
			*right << valR;
			*right << "\n";

			if(occurrenciesL.find(valL) != occurrenciesL.end())
				occurrenciesL[valL]++;
			else
				occurrenciesL.insert(make_pair(valL, 1));

			if(occurrenciesR.find(valR) != occurrenciesR.end())
				occurrenciesR[valR]++;
			else
				occurrenciesR.insert(make_pair(valR, 1));

			/*			
			cout << "Values to insert :\n";	
			for(int j = 0; j < 2; j++) cout << tmp_buffer[j] << "\n";
			for(int j = 0; j < 2; j++) cout << sample[j] << "\n";
					
			predictor->simple_predict(tmp_buffer,sample,2);
		
			int j; 
			for(j=0; j<2; j++){
				tmp_buffer[j] = sample[j]; 
			}*/

		}	

	}

	
	sf_close(soundFileIn);
	/*for(int i = 0; i < soundInfoIn.frames+1; i++) {
		cout << "Channel 1: " << matrix[i][0] << "; Channel 2: " << matrix[i][1] << "\n";
	}*/

	left->flush();
	left->close();
	right->flush();
	right->close();


	cout << "Left entropy: " << predictor->calcEntropy(occurrenciesL) << "\n";	
	cout << "Right entropy: " << predictor->calcEntropy(occurrenciesR) << "\n";	
	cout << "top\n"; 	
 
	return 0; 
}


