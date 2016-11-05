#include <string>
#include <queue>

using namespace std;

class BitStream {

	public:
		BitStream(string readname, string writename);
		short readBit();	
		short* readNBits(short nBits);
		void writeBit(short* sequence);
		void writeNBits(short nBits, short* sequence, short finalWrite);

	private:
		string* readFilename;
		string* writeFilename;
		short readPosition;
		short writePosition;

		short surplus;
		char surplusByte;
		unsigned short remainingByteSlots;

		void flush();
};	
