#include <cstdlib> 
#include <string>

#include "Golomb.cpp"

int main(int argc, char** argv) {

	Golomb* g = new Golomb(atoi(argv[1]), "encoded1",0);


	g->encode(23, 0);
	g->encode(42, 0);
	g->encode(-56, 0);
	g->encode(500, 0);
	g->encode(-88, 0);
	g->encode(-14, 1);

	Golomb* h = new Golomb(atoi(argv[1]), "encoded1", g->getFilePosition());

	int e = 0;
	int* end = &e;
	while(*end == 0) {
		h->decode(end);	
	}

	return 0;
}
