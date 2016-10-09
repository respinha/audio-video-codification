#include "FCM.cpp"
#include <iostream>
#include <string>
#include <cstdlib>

int main(int argc, 	char** argv) {

	string text(argv[1]);

	FCM test(atoi(argv[2]), text);
	test.genText(atoi(argv[3]), atoi(argv[2]));

	return 0;
}