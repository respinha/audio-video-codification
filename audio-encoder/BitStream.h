#include <string>
#include <queue>

using namespace std;

class BitStream {

	public:
		BitStream(string readname, string writename);
		int readBit();	
		void readNBits(int nBits);
		void writeBit();
		void writeNBits(int nBits);
	private:
		string* readFilename;
		string* writeFilename;
		int readPosition;
		int writePosition;
		int fileWritePosition; 
		queue<int> readBits;

		char byteBuffer;
};	
