#include "FCM.h"
#include <cmath>
#include <iostream>
#include <ctype.h>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <sstream>
#include <ctime>       

const string FCM::TEXT_SEPARATOR = "\n////////_________\n";
FCM::FCM(unsigned int order, string srcText) {

	initDict();

	if(order > srcText.size()) {
		cout << "No can do\n";
		return;
	}


	// build LUT
	string approximation;			
	unsigned int n = 0;

	map<string, float> counters;

	// iterate over source text
	for(int i = 0; i < srcText.size(); i++) 
	{
		// first operation: save the previous character
		if(i >= 1) approximation += srcText[i-1];


		if(dict.find(srcText[i]) != dict.end()) 
			srcText[i] = dict[srcText[i]];	
		

		if(order == 0) 
		{	
			// zero-order -> no context                                                              		
			// only alphabetical unidimensional array is needed

			if(!isspace(srcText[i])) 
			{
				int alphabetPosition = srcText[i] - 'a' + 1;
				alphabetArray[alphabetPosition]++;				
			} else // whitespace
				alphabetArray[0]++;

		} else if(i >= order)  
		{

			if(i == order) firstWord = approximation;

			bool found = false;
			typedef LUT::iterator it_lut;

			for(it_lut it = lut.begin(); it != lut.end(); it++) 
			{
				if(it->first.compare(approximation) == 0)
				{
					if(it->second.find(srcText[i]) != it->second.end()) {
						it->second[srcText[i]]++;
					} else {
						it->second.insert(make_pair(srcText[i], 1));
					}

					found = true;
					break;
				} 

			}

			if(!found) {
				map<char, float> values;
				values.insert(make_pair((char)srcText[i], 1));
				lut.insert(make_pair(approximation, values));
			}	


			counters[approximation]++;	
			// move on
			approximation.erase(0,1);	
		}
	} 

	for(it_lut it = lut.begin(); it != lut.end(); it++) {
		float total = counters[it->first];

		float accumulated = 0;
		for(it_map it2 = it->second.begin(); it2 != it->second.end(); it2++) {

			accumulated += ((it2->second)/total);
			it2->second = accumulated;
		}

	}

	//printLUT();

}		

void FCM::printLUT() {
	for(it_lut it = lut.begin(); it != lut.end(); it++) {
		for(it_map it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			cout << it->first << ": " << it2->first << ": " << it2->second << "\n";
		}
	}

}

void FCM::loadTable(string fileName) {


}

void FCM::genText(int len) {


	string text(firstWord);
	string approximation(firstWord);
	int i = approximation.size()-1;

	srand(time(NULL));
	while(i < len) {

		map<char,float> contextMap = lut[approximation];

		
		float r = ((float) rand() / (RAND_MAX));
		//cout << r << "\n";
		for(it_map it = contextMap.begin(); it != contextMap.end(); it++) {

			if(r <= it->second) {				
				text.push_back(it->first);
				approximation.push_back(it->first);
				break;
			}
		}


		approximation.erase(0,1);
		i++;
	}	
	
	cout << "$$$$$$$\n";
	cout << text << "\n";
}

float FCM::calcEntropy(LUT l) {

	for(it_lut it = l.begin(); it != l.end(); it++) {
		for(it_map it2 = it->second.begin(); it2 != it->second.end(); it2++) {

		}
	}
}

