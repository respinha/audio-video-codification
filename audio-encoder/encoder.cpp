#include <cstdlib> 
#include <string>

#include "Golomb.cpp"

int main(int argc, char** argv) {

	Golomb* g = new Golomb(atoi(argv[1]), "decoded", "encoded");

	g->encode(atoi(argv[2]));


	return 0;
}
