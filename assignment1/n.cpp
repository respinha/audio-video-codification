#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* wcstombs, wchar_t(C) */
using namespace std;

int main(void) {
	
	const wchar_t str[] = L"o balão do joão";
  char buffer[32];
  int ret;

  printf ("wchar_t string: %ls \n",str);

  ret = wcstombs ( buffer, str, sizeof(buffer) );
  if (ret==32) buffer[31]='\0';
  if (ret) printf ("multibyte string: %s \n",buffer);

  return 0;
}
