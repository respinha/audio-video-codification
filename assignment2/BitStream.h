#include <string>

 using namespace std;

class BitStream {

	public:
		BitStream(string filename);
		typedef unsigned char byte;
		byte readBit(byte word, int position);	// returns byte
		byte[] readNBits(byte word, int nPositions);
		void writeBit(byte bit, string filename);
		void writeNBits(byte[] bits, string filename);
	private:
		byte[] content;
		byte[] readFile(string filename);
}	
	
