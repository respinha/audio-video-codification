#include <cstdlib> 
#include <string>
#include <fstream>
#include <utility>

#include "Predictor.cpp"

#include <sndfile.h> 

int main (int argc, char** argv){
	
	if (argc < 4){
		fprintf(stderr, "Usage: <M parameter> <encoded file> <input file> \n");
		return -1;
	}

	Predictor* predictor = new Predictor(atoi(argv[1]), argv[2]); 

	SNDFILE *soundFileIn; /* Pointer for input sound file */
	SF_INFO soundInfoIn; /* Input sound file Info */

	int i;
	sf_count_t nSamples = 1;



	/* When opening a file for read, the format field should be set to zero
	 * before calling sf_open(). All other fields of the structure are filled
	 * in by the library
	 */	
	soundInfoIn.format = 0;
	soundFileIn = sf_open (argv[3], SFM_READ, &soundInfoIn);


	if (soundFileIn == NULL){
		fprintf(stderr, "Could not open file for reading\n");
		sf_close(soundFileIn);
		return -1;
	}

	fprintf(stderr, "Format: %d\n", soundInfoIn.format);
	fprintf(stderr, "Samplerate: %d\n", soundInfoIn.samplerate);
	fprintf(stderr, "Channels: %d\n", soundInfoIn.channels);

	
	short* sample = new short[2];
	sample[0] = 0;
	sample[1] = 0;

	short bufferL[3] = {0,0,0}; 
	short bufferR[3] = {0,0,0};

	for (i = 0; i < soundInfoIn.frames ; i++)
	{
		if (sf_readf_short(soundFileIn, sample, nSamples) == 0){
			
			fprintf(stderr, "Error: Reached end of file\n");
			sf_close(soundFileIn);
			break;
		
		}else{
			//cout << "Samples " << sample[0] << "\n";
			//cout << "Samples2 " << sample[1] << "\n";

			predictor->order3_predict(bufferL, bufferR, sample, (i == (soundInfoIn.frames-1)));
		}	

	}

	ofstream* o = new ofstream("metadata", ios::out | ios::app);

	*o << soundInfoIn.samplerate << "\n";
	*o << soundInfoIn.channels << "\n";
	*o << soundInfoIn.format << "\n";

	sf_close(soundFileIn);

	cout << "The end: " << predictor->getFilePosition();
	*o << predictor->getFilePosition();

	o->flush();
	o->close();

	delete predictor;
	delete[] sample;

	return 0; 
}
