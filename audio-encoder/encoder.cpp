#include <cstdlib> 
#include <string>

#include "Golomb.cpp"

int main(int argc, char** argv) {

	Golomb* g = new Golomb(atoi(argv[1]), "encoded1",0);


	g->encode(atoi(argv[2]), 0);
	g->encode(atoi(argv[3]), 0);
	g->encode(atoi(argv[4]), 1);
//	g->encode(atoi(argv[5]), 1);

	Golomb* h = new Golomb(atoi(argv[1]), "encoded1", g->getFilePosition());
	
	int e = 0;
	int* end = &e;
	while(*end == 0) {
		h->decode(end);	
	}

	return 0;
}
