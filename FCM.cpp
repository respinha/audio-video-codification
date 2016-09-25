// todo
#include "FCM.h"
#include <cmath>
#include <iostream>
#include <ctype.h>

FCM::FCM(unsigned int order, string srcText) {

	// build LUT
	string approximation;			
	unsigned int n = 0;

	// iterate over source text
	for(int i = 0; i < srcText.size(); i++) 
	{
		// first operation: save the previous character

		if(i >= 1) 
			approximation += srcText[i-1];

		
		if(order == 0) 
		{	
			// zero-order
			// only alphabetical unidimensional array is needed

			if(!isspace(srcText[i])) 
			{
				int alphabetPosition = srcText[i] - 'a' + 1;
				alphabetArray[alphabetPosition]++;				
			} else // whitespace
				alphabetArray[0]++;

		} else if(i >= order)  
		{

			Key key = Key(approximation,srcText[i]);
			
			typedef LUT::iterator it_lut;
			bool found = false;
			
			for(it_lut it = lut.begin(); it != lut.end(); it++) 
			{
				// cout << "approximation: " << approximation << "\n";
				// cout << "first: " << it->first.first << "\n";
				// cout << "second: " << it->first.second << "\n";
				if(it->first.first.compare(approximation) == 0 && it->first.second == (char) srcText[i])
				{

					found = true;
	
					break;
				}
				cout << "Found: " << found << "\n";			 
			}

			if(!found) 	{// key combination doesn't exist
				pair<Key,int> keyPair = make_pair(key,1);
				lut.insert(keyPair);
			}
			else 
				lut[key]++;	

			// move on
			approximation.erase(0,1);		
		}

	}

	if(order > 0) {
		typedef LUT::iterator it_type;

		for(it_type it = lut.begin(); it != lut.end(); it++) {
			Key key = it->first;
			cout << "Key first: " << key.first << "\n"; 
			cout << "Keysecond: " << key.second << "\n";
			cout << "Value: " << it->second << "\n";
		}
	}
}		
