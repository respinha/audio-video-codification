#include "FCM.cpp"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

string loadTextFromFile(string filename); 
void normalize(string* line);

int main(int argc, 	char** argv) {

	char c;

	string* text = new string();
	int len = 0, order = 0;
	
	if(argc == 1) cout << "No input!\n";
	for(int i = 1; i < argc; i+=2)  {
		string* arg = new string(argv[i]);
		if(arg->compare("-t") == 0) {
			text = new string(argv[i+1]);

		} else if(arg->compare("-f") == 0) {
			string* argument = new string(argv[i+1]);
			ifstream ifs(argument->c_str());
			if(ifs.is_open()) {
				string* line = new string();
				while(getline(ifs, *line)) {
					normalize(line);
					text->append(*line);
				}

				ifs.close();
			}
			


		}
		else if(arg->compare("-l") == 0) {
			len = atoi(argv[i+1]);
		}
		else if(arg->compare("-o") == 0) {
			 order = atoi(argv[i+1]);
		} else {
			cout << "Unrecognized command\n";
		}


	}

	// TODO: Makefile and finish report
	FCM test(*text, order, len);

	return 0;
}

void normalize(string* line) {

	string l = *line;
	for(int i = 0; i < l.size(); i++) {
		char c = l[i];
		int ascii = (int) c;
		
		if((ascii != 32 && ascii< 65) || (ascii > 90 && ascii < 97) || ascii > 122) {
			l.erase(i,1);
		}
	}
	
	*line = l;	
}
