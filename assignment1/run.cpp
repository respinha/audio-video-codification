#include "FCM.cpp"
#include <iostream>
#include <string>

int main(int argc, 	char** argv) {

	string text(argv[1]);

	FCM test(2, text);

	return 0;
}