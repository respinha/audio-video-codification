#include <string>

 using namespace std;

class BitStream {

	public:
		BitStream(string filename);
		char readBit();	// returns char
		char[] readNBits(char word, int nPositions);
		void writeBit(char bit, string filename);
		void writeNBits(char[] bits, string filename);
	private:
		static string* filename;
		static int readPosition;
		static int writePosition;
		static char byteBuffer;
}	
	
