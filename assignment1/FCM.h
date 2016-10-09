#include <string>
#include <utility>
#include <vector>
#include <map>


using namespace std;

class FCM {

	public:	
		FCM(unsigned int order,	 string srcText);
		
		void loadTable(string fileName);
		void genText(int len);

		typedef map<string, map<char,float> > LUT;	// <context, array of values>
		void loadTextFile(string srcText);

	private:		
		void printLUT();

		const static int ALPHABET_SIZE = 27;
		LUT lut;
		typedef LUT::iterator it_lut;
		typedef map<char,float>::iterator it_map;	

		int alphabetArray[ALPHABET_SIZE];	// for zero-order case; includes whitespace ' '
		static const string TEXT_SEPARATOR;
		string firstWord;
		float calcEntropy(LUT l, map<string, float> counters, float total);

		map<char, char> dict;
		void initDict() {
			// 'a'
			dict.insert(make_pair('á', 'a'));
			dict.insert(make_pair('à', 'a'));
			dict.insert(make_pair('ã', 'a'));
			dict.insert(make_pair('â', 'a'));

			dict.insert(make_pair('ç', 'c'));

			dict.insert(make_pair('é', 'e'));
			dict.insert(make_pair('è', 'e'));
			dict.insert(make_pair('ẽ', 'e'));
			dict.insert(make_pair('ê', 'e'));

			dict.insert(make_pair('í', 'i'));
			dict.insert(make_pair('ì', 'i'));
			dict.insert(make_pair('î', 'i'));
			dict.insert(make_pair('ĩ', 'i'));

			dict.insert(make_pair('ó', 'o'));
			dict.insert(make_pair('ò', 'o'));
			dict.insert(make_pair('õ', 'o'));
			dict.insert(make_pair('ô', 'o'));

			dict.insert(make_pair('ú', 'u'));
			dict.insert(make_pair('ù', 'u'));
			dict.insert(make_pair('ũ', 'u'));
			dict.insert(make_pair('û', 'u'));

			dict.insert(make_pair('ñ', 'n'));
		}
		
};