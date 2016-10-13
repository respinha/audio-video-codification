#include <string>
#include <utility>
#include <vector>
#include <map>


using namespace std;

class FCM {

	public:	
		FCM(unsigned int order,	 string srcText, unsigned int len);
		typedef map<string, map<char,float> > LUT;	// <context, array of values>
		void genText(LUT lut, int len, int order);

		
	private:		
		void loadTextFile(string srcText);
		LUT loadTable(string fileName);
		void saveTable(LUT l);
		void printTable(LUT l);
		float calcEntropy(LUT l, map<string, float> counters, float total, int order);

		map<string, float> counters;
		float total;

		typedef LUT::iterator it_lut;
		typedef map<char,float>::iterator it_map;	

		string firstWord;
		static const string FILENAME;

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
