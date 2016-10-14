#include "FCM.cpp"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

string loadTextFromFile(string filename); 

int main(int argc, 	char** argv) {

	char c;

	string text;

	if(argc == 1) cout << "No input!\n";
	for(int i = 1; i < argc; i+=2)  {
		string arg = argv[i];
		if(arg.compare("-t") == 0) {
			text = argv[i+1];
			cout << text;
			
		} else if(arg.compare("-f") == 0) {
			loadTextFromFile(argv[i+1]);
		}
		else if(arg.compare("-l") == 0) {
		}
		else if(arg.compare("-o") == 0) {
		} else {
			cout << "Unrecognized command\n";
		}


	}
	
	// TODO: Makefile and finish report
	//FCM test(text, atoi(argv[2]), atoi(argv[3]));
		
	return 0;
}

string loadTextFromFile(string filename) {

	
	/ut << content[content.size()-1];*/
	ifstream ifs(filename.c_str());
	if(ifs.is_open()) {
		string line;
		while(getline(ifs, line)) {
			cout << line << "\n";
		}
		ifs.close();
	}
	
}
