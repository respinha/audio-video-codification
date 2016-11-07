#include <iostream>
#include <fstream>

using namespace std;

int main(void) {

	ofstream* o = new ofstream("o meu teste", ios::app | ios::out);
	*o << "o";
	*o << "l";
	*o << "a";
	ifstream* r = new ifstream("o meu teste", ios::in);

	if(r->is_open()) {
		r->seekg(0, r->end);
		int pos = r->tellg();		
		r->seekg(pos-1, r->beg);
		char c;
		r->read(&c, 1);
	
		cout << c << "\n";
		r->close();
		o->close();
	}

	return 0;
}
