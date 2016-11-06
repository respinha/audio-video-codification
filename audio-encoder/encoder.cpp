#include <cstdlib> 
#include <string>

#include "Golomb.cpp"

int main(int argc, char** argv) {

	Golomb* g = new Golomb(atoi(argv[1]), "encoded");

	g->encode(atoi(argv[2]), 0);
	g->encode(atoi(argv[3]), 0);
	g->encode(atoi(argv[4]), 0);
	g->encode(atoi(argv[5]), 0);
	g->encode(atoi(argv[6]), 0);
	g->encode(atoi(argv[7]), 1);
//	g->encode(atoi(argv[5]), 1);

	while(1) {
		char b;
		cin >> b;
		if(b == 'b') 
			g->decode();	
		else break;
	}
	return 0;
}
