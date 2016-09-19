#include <cstring.h>

class FCM {

	public:
		FCM(int order, string srcText);
		void updateContext(int order);
		void updateSrcText(string srcText);
		void printResult();
		void printTable();

	private:
		int lut[][];	
		static int N_ALPHABET;
		buildLUT();

}