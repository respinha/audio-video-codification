#include <string>
#include <queue>

using namespace std;

class BitStream {

	public:
		BitStream(string readname, string writename);
		int readBit();	
		int* readNBits(int nBits);
		void writeBit(int* sequence);
		void writeNBits(int nBits, int* sequence, int finalWrite);

	private:
		string* readFilename;
		string* writeFilename;
		int readPosition;
		int writePosition;

		int surplus;
		char surplusByte;
		unsigned int remainingByteSlots;

		void writeRemainingBits();
};	
