#include <cstdlib> 
#include <string>

#include "Golomb.cpp"

int main(int argc, char** argv) {

	Golomb* g = new Golomb(atoi(argv[1]), "encoded1",0);


/*	g->encode(23, 0);
	g->encode(42, 0);
	g->encode(-56, 0);
	g->encode(500, 0);
	g->encode(500, 0);
	g->encode(44, 0);
	g->encode(-17, 0);
	g->encode(-3, 0);
	g->encode(-9, 0);
	g->encode(-18, 0);
	g->encode(-7, 0);
	g->encode(15, 0);
	g->encode(-15, 0);
	g->encode(16, 0);
	g->encode(432, 0);
	g->encode(211, 0);
	g->encode(-13, 0);
	g->encode(-17, 0);
	g->encode(-14, 1);*/
	for(int i = 1; i <= 10; i++) {
		if(i%4 == 0)
			g->encode(i*-1, 0);
		else
			g->encode(i,0);
	}

	g->encode(1, 1);

	Golomb* h = new Golomb(atoi(argv[1]), "encoded1", g->getFilePosition());

	int e = 0;
	int* end = &e;
	while(*end == 0) {
		 h->decode(end);	
	}

	return 0;
}
