// todo
#include "FCM.h"
#include <cmath>
#include <iostream>
#include <ctype.h>

class FCM
{
	public:
		// build LUT
		FCM(unsigned int order, string srcText) {

			string approximation;			
			int n = 0;

			// iterate over source text
			for(int i = 0; i < srcText.size()+1; i++) {
				// first operation: save the previous character
				if(i >= 0) 
					approximation[n] = srcText[i-1];
				if(order == 0) {	
					// zero-order
					// only alphabetical unidimensional array is needed

					if(!isspace(srcText[i])) 
					{
						int alphabetPosition = srcText[i] - 'a' + 1;
						alphabetArray[alphabetPosition]++;				
					} else // whitespace
						alphabetArray[0]++;

				} else if(i >= order)  {

					Key key = Key(approximation,srcText[i]);
					if(lut.find(key) == lut.end()) 	// key combination doesn't exist
						lut.insert(key, 1);
					else 
						lut[key]++;	

				}
			}
		}		

		~FCM();

	private:

};		