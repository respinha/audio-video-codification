#include <cstdlib> 
#include <string>
#include <algorithm>
#include "Predictor.h"

#include <sndfile.h> 

Predictor::Predictor(int m, string encodedFilename, string decodedFilename){
	g = new Golomb(m, encodedFilename);
}

list<short> Predictor::reverse_simple_predict(int nFrames, short* sequence_buf){
	cout << "entra reverse predictor TOP\n"; 
	short sequenceL;
	short sequenceR;
	
	list<short> reverse; 

	list<short> decoded;// = g->decode();
        
	int i = 0; 
/*
	for(list<short>::iterator it = decoded.begin(); it !=decoded.end(); it++){
		cout <<"entra no for ainda todo TOP\n";	
		if(i%2==0){
			cout << "reverse predictor par\n";
			sequenceL = *it + sequence_buf[0];
			reverse.push_back(sequenceL);
		}else{
			cout << "reverse predictor impar\n"; 
			sequenceR = *it + sequence_buf[1]; 
			reverse.push_back(sequenceR);
		}


		i+=1; 
		cout << "reverse: "<<*it<<"\n"; 	
	
	} */

	for(int i = 0; i < 20; i++) {

		short sample = g->decode();
		cout << "SAMPLE: " << sample << "\n";
		if(i%2==0){
			sequenceL = sample + sequence_buf[0];
			reverse.push_back(sequenceL);
		}else{
			sequenceR = sample + sequence_buf[1]; 
			reverse.push_back(sequenceR);
		}
	}

	return reverse;	
}




void Predictor::simple_predict(short* sequence, short* sequence_buf, int length){
	short remainderL;
        short remainderR; 	



	
	remainderL = sequence[0] - sequence_buf[0];
	remainderR = sequence[1] - sequence_buf[1];

	g->encode(remainderL,0);
	g->encode(remainderR,0); 

}		

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
}


int main (int argc, char** argv){
	
	short samples_demo[]= {1,2,3,4,5,6,7,8,9,10};

	Predictor* predictor = new Predictor(8, "encoded","encoded"); 


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

	for (i = 0; i < soundInfoIn.frames ; i++)
	{
		if (sf_readf_short(soundFileIn, sample, nSamples) == 0){
			
			fprintf(stderr, "Error: Reached end of file\n");
			sf_close(soundFileIn);
			break;
		
		}else{


			if(i==0){
				short first[2]  = {0,0};
				short first_buffer[2] = {0,0};				

				predictor->simple_predict(first,first_buffer,2);
			}

			predictor->simple_predict(tmp_buffer,sample,2);
		
			int j; 
			for(j=0; j<2; j++){
				tmp_buffer[j] = sample[j]; 
			}

			//copy(begin(sample),end(sample),begin(tmp_buffer));				
		}	

	}

	sf_close(soundFileIn);
	
	/*list<short> top_lista = */predictor->reverse_simple_predict(soundInfoIn.frames, samples_demo);

	

//	Predictor* predictor = new Predictor(12, "encoded","encoded"); 

//	predictor->predict(samples,10); 
	
	cout << "top\n"; 	

	return 0; 
}


