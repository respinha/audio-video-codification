#include <string>
#include <map>
#include <utility>

using namespace std;

class FCM {

	public:
		FCM(unsigned int order,	 string srcText);
		/*void updateContext(int order);
		void updateSrcText(string srcText);
		void printResult();
		void printTable();*/

	private:
		typedef pair<string, char> Key;
		typedef map<Key, int> LUT;
		LUT lut;
		int alphabetArray[27];	// for zero-order case; includes whitespace ' '

};