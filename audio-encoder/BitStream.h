#include <string>
#include <queue>

using namespace std;

class BitStream {

	public:
		BitStream(string fname, int pos);
		int readBit();	
		int* readNBits(int nBits);
		void writeBit(int* sequence);
		void writeNBits(int nBits, int* sequence, int finalWrite);

		int getFilePosition();
	private:
		string* filename;
		int readPosition;
		int writePosition;

		int surplus;
		char surplusByte;
		unsigned int remainingByteSlots;

		void flush();
};	
