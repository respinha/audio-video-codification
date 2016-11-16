#include <sstream>
#include <string>
#include <iostream>

using namespace std;

int main(void) {

	short a = -14532;
	stringstream ss;
	
	ss << a;
	int b;
	ss >> b;

	cout << "a = " << a << "; b = " << b << "\n";	
	return 0;
}
