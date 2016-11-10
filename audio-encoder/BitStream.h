#include <string>
#include <fstream>

using namespace std;

class BitStream {

	public:
		BitStream(string fname, int pos);
		~BitStream();
		int readBit();	
		int readNBits(int nBits);

		void writeBit(int bit);
		void writeNBits(int nBits, int bit_buff, int finalWrite);
		int getFilePosition();
	private:
		string* filename;
		int readPosition;
		int writePosition;

		int surplus;
		char surplusByte;
		unsigned int remainingByteSlots;

		void flush();

		fstream* stream;
		unsigned int bufferCount;
		unsigned int threshold;
 		char* readBuffer;

		void putByte(char c);
		void checkReadBuffer();
};	
