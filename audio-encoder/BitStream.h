#include <string>

 using namespace std;

class BitStream {

	public:
		BitStream(string filename);
		void readBit();	// returns char
		void readNBits(int nBits);
		void writeBit(char bit, string filename);
		void writeNBits(char[] bits, string filename);
	private:
		static string* filename;
		static int readPosition;
		static int writePosition;
		static char byteBuffer;
}	
	
