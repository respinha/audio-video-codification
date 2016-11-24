#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

int main(void) {

	ifstream* stream = new ifstream("ficheiro", ios::in);

	if(stream->is_open()) {

		char buf;
		stream->seekg(-2, ios::end);
		stream->read(&buf, sizeof(buf));
		cout << "letra: " << buf << "\n";

		stream->close();
	}
	return 0;
}
