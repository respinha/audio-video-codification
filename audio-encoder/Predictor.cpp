#include <cstdlib> 
#include <string>
#include <algorithm>
#include "Golomb.cpp"
#include "Predictor.h"

#include <sndfile.h> 

Predictor::Predictor(int m, string encodedFilename, string decodedFilename){
	
}


void Predictor::simple_predict(short* sequence, short* sequence_buf, int length){
//	int length = sizeof(sequence)/sizeof(*sequence); 
	
	

	short remainderL;
        short remainderR; 	

	Golomb* g = new Golomb(8, "encoded", "encoded");

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
	
//	int samples[]= {1,2,3,4,5,6,7,8,9,10};


	Predictor* predictor = new Predictor(12, "encoded","encoded"); 

	SNDFILE *soundFileIn; /* Pointer for input sound file */
	//SNDFILE *soundFileOut; /* Pointer for output sound file */
	SF_INFO soundInfoIn; /* Input sound file Info */
	//SF_INFO soundInfoOut; /* Output sound file Info */

	int i;
	short sample[2] = {0,0};
	sf_count_t nSamples = 1;

	if (argc < 1){
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
	for (i = 0; i < soundInfoIn.frames ; i++)
	{
		cout << "cheguei memo top\n";
		if (sf_readf_short(soundFileIn, sample, nSamples) == 0){
			
			fprintf(stderr, "Error: Reached end of file\n");
			sf_close(soundFileIn);
			break;
		
		}else{

			cout << "ShortL: " << sample[0] << "\n";
			cout << "ShortR: " << sample[1] << "\n";

			if(i==0){
				short* first  = new short[2];
				short *first_buffer = new short[2];
				
				predictor->simple_predict(first,first_buffer,2);
			}

			cout << "Values to insert :\n";	
			for(int j = 0; j < 2; j++) cout << tmp_buffer[j] << "\n";
			for(int j = 0; j < 2; j++) cout << sample[j] << "\n";
					
			predictor->simple_predict(tmp_buffer,sample,2);
		
			int j; 
			for(j=0; j<2; j++){
				tmp_buffer[j] = sample[j]; 
			}

			//copy(begin(sample),end(sample),begin(tmp_buffer));				
		}	

	}

	sf_close(soundFileIn);



//	Predictor* predictor = new Predictor(12, "encoded","encoded"); 

//	predictor->predict(samples,10); 
	
	cout << "top\n"; 	

	return 0; 
}


