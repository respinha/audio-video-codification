#include <cstdlib> 
#include <string>

#include "Golomb.cpp"

int main(int argc, char** argv) {

	Golomb* g = new Golomb(atoi(argv[1]), "encoded", "encoded");

	g->encode(atoi(argv[2]), 0);
	g->encode(atoi(argv[3]), 0);
	g->encode(atoi(argv[4]), 1);
//	g->encode(atoi(argv[5]), 1);

	g->decode();
	return 0;
}
