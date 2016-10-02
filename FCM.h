#include <string>
#include <utility>
#include <vector>
#include <map>

using namespace std;

class FCM {

	public:	
		FCM(unsigned int order,	 string srcText);

		/*void updateContext(int order);
		void updateSrcText(string srcText);
		void printResult();
		void printTable();*/

	private:		
		typedef map<string, map<char,float> > LUT;	// <context, array of values>
		LUT lut;
		typedef LUT::iterator it_lut;
		typedef map<char,float>::iterator it_map;	

		int alphabetArray[27];	// for zero-order case; includes whitespace ' '

};