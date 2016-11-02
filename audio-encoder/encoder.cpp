#include <cstdlib> 
#include <string>

#include "Golomb.cpp"

int main(int argc, char** argv) {

	Golomb* g = new Golomb(atoi(argv[1]), "encoded", "encoded");

	g->encode(atoi(argv[2]), 1);
/*	g->encode(atoi(argv[2]), 0);
	g->encode(atoi(argv[2]), 0);
	g->encode(atoi(argv[2]), 1);*/

	return 0;
}
