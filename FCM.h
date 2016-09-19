#include <cstring.h>

class FCM {

	public:
		FCM(int nLetters, string srcText);
		void updateContext(int nLetters);
		void updateSrcText(string srcText);
		void printResult();
		void printTable();

	private:
		int lut[][];	
		buildLUT();
}