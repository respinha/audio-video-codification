#include <string>
#include <utility>
#include <vector>
#include <map>


using namespace std;

class FCM {

	public:	
		FCM(unsigned int order,	 string srcText);
		void saveTable();
		void loadTable(string fileName);
		typedef map<string, map<char,float> > LUT;	// <context, array of values>

		/*
		
		void loadTextFile(string srcText);
		void genText(LUT lut, int len);*/

	private:		
		
		LUT lut;
		typedef LUT::iterator it_lut;
		typedef map<char,float>::iterator it_map;	

		int alphabetArray[27];	// for zero-order case; includes whitespace ' '
		static const string TEXT_SEPARATOR;

};