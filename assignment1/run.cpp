#include "FCM.cpp"
#include <iostream>
#include <string>
#include <cstdlib>

int main(int argc, 	char** argv) {

	string text(argv[1]);

	FCM test(atoi(argv[2]), text, atoi(argv[3]));
	

	return 0;
}