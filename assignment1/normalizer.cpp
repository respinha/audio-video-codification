#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char** argv) {
	ifstream myReadFile;
	ofstream myWriteFile;

	 myReadFile.open("ensaio.txt");
	 myWriteFile.open("ensaio_.txt");
	 string line;
	 if (myReadFile.is_open()) {
	 while (getline(myReadFile, line)) {


	    
	    cout << line;
	    myWriteFile << line;
	    

	 }
	}
	myReadFile.close();
	myWriteFile.close();

}