#include <cstdlib>
#include <string>
#include <utility>
#include "Predictor.cpp"
#include <sstream>

#include <sndfile.h> 

using namespace std;

int main(int argc, char** argv) {

	if (argc < 4){
		fprintf(stderr, "Usage: <M parameter> <encoded file> <output wav file>\n");
		return -1;
	}

	SNDFILE *soundFileOut; /* Pointer for input sound file */
	SF_INFO soundInfoOut; /* Output sound file Info */

	sf_count_t nSamples = 1;

	
	// needing samplerate, channels, format
	cout << "Reading metadata...\n";

	int fileWritePosition;
	ifstream* stream = new ifstream("metadata", ios::in);

	if(stream->is_open()) {
		string strRate, strChannels, strFormat, filePos;

		getline(*stream, strRate);
		getline(*stream, strChannels);
		getline(*stream, strFormat);
		getline(*stream, filePos);

		int channels, rate, format;
/*		stringstream ss;

		ss << strRate;
		ss >> rate;
		
		ss << strChannels;
		ss >> channels;
		
		ss << strFormat;
		cout << "format: " << ss.str() << "\n";

		ss >> format;
		cout << "format: " << format << "\n";

		ss << filePos;
		ss >> fileWritePosition;
*/

		soundInfoOut.samplerate = atoi(strRate.c_str());
		soundInfoOut.channels = atoi(strChannels.c_str());
		soundInfoOut.format = atoi(strFormat.c_str());

		fileWritePosition = atoi(filePos.c_str());

		stream->close();
	} else {
		cout << "Error reading metadata!\n";
		return -1;
	}

	fprintf(stderr, "Format: %d\n", soundInfoOut.format);
	fprintf(stderr, "Samplerate: %d\n", soundInfoOut.samplerate);
	fprintf(stderr, "Channels: %d\n", soundInfoOut.channels);

	/* When opening a file for write, it is necessary to fill in the
	 * structure members samplerate, channels and format.
	*/

	// argv[3] = output wav file
	soundFileOut = sf_open (argv[3], SFM_WRITE, &soundInfoOut);

	if (soundFileOut == NULL){
		fprintf(stderr, "Could not open file for writing\n");
		sf_close(soundFileOut);
		return -1;
	}

	short outputSamples[2];


	// assuming zero as starting value
	// value is updated in each call
	short buffer[2] = {0,0};
/*
	buffer[0] = 0:
	buffer[1] = 0;
*/
	int end = 0;
	int* pend = &end;

	// argv[1] = parameter M; argv[2] = file with encoded content
	Predictor* predictor = new Predictor(atoi(argv[1]), argv[2], fileWritePosition);

	while(*pend == 0) {

		predictor->reverse_simple_predict(buffer, outputSamples, pend);

		if (sf_writef_short(soundFileOut, outputSamples, nSamples) != 1){
			fprintf(stderr, "Error writing frames to the output:\n");
			sf_close(soundFileOut);
			return -1;
		}
	}

				
		

	sf_close(soundFileOut);

	delete pend;
	delete predictor;
	delete stream;

		
	return 0;
}
