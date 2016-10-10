#include "FCM.h"
#include <cmath>
#include <iostream>
#include <ctype.h>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <sstream>
#include <ctime>       

const string FCM::FILENAME = "saved_LUT";

FCM::FCM(unsigned int order, string srcText, unsigned int len) {

	LUT lut;
	initDict();

	if(order > srcText.size()) {
		cout << "No can do\n";
		return;
	}

	ifstream stream(FILENAME.c_str());

	cout << stream.good() << "\n";
	float total = 0;
	map<string, float> counters;

	if(stream.good()) {
		lut = loadTable(FILENAME);
		for(it_lut it = lut.begin(); it != lut.end(); it++) 
		{
			counters.insert(make_pair(it->first, 0));
			for(it_map it2 = it->second.begin(); it2 != it->second.end(); it2++)
			{
				counters[it->first] += it2->second;

			}

			total += counters[it->first];
		}
	}
	
	string approximation;			
	unsigned int n = 0;

	
	// iterate over source text
	for(int i = 0; i < srcText.size(); i++) 
	{
		// first operation: save the previous character
		if(i >= 1  && order != 0) approximation += srcText[i-1];


		if(dict.find(srcText[i]) != dict.end()) 
			srcText[i] = dict[srcText[i]];	
		
		if(i >= order)  
		{
			total++;

			bool found = false;
			if(order == 0) {

				if(lut.size() == 0) {

					map<char, float> values;
					values.insert(make_pair((char)srcText[i], 1));
					firstWord = "key";
					counters.insert(make_pair(firstWord, 1));
					lut.insert(make_pair(firstWord, values));	
				
				}
				else {
					if(lut.begin()->second.find(srcText[i]) != lut.begin()->second.end()) {
						lut.begin()->second[srcText[i]]++;
					} else {
							lut.begin()->second.insert(make_pair(srcText[i], 1));
					}

					counters[firstWord]++;
				}

				

			}
			else {
				if(i == order) firstWord = approximation;

				
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
	} 

	cout << "Entropy: " << calcEntropy(lut, counters, total, order) << "\n";

	saveTable(lut);

	// accumulated probabilities for text generation
	for(it_lut it = lut.begin(); it != lut.end(); it++) {
		float contextTotal = counters[it->first];

		float accumulated = 0;
		for(it_map it2 = it->second.begin(); it2 != it->second.end(); it2++) {

			accumulated += ((it2->second)/contextTotal);
			it2->second = accumulated;
		}

	}

	genText(lut, len, order);

}		

void FCM::printTable(LUT lut) {
	for(it_lut it = lut.begin(); it != lut.end(); it++) {
		for(it_map it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			cout << it->first << ": " << it2->first << ": " << it2->second << "\n";
		}
	}

}

void FCM::genText(LUT lut, int len, int order) {


	string text((order != 0) ? firstWord : "");
	string approximation(text);
	int i = (order != 0) ? approximation.size()-1 : 0;

	srand(time(NULL));
	while(i < len) {

		float r = ((float) rand() / (RAND_MAX));
		
		map<char,float> contextMap = (order != 0) ? lut[approximation] : lut.begin()->second;
		
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
	
	cout << text << "\n";
}

float FCM::calcEntropy(LUT l, map<string, float> counters, float total, int order) {

    float totalEntropy = 0;

    if(order == 0) {
    	for(it_map it = l.begin()->second.begin(); it != l.begin()->second.end(); it++) {
    		float prob = it->second/total;
    		totalEntropy += -prob*log2(prob);
    	}

    	return totalEntropy;
    }

	for(it_lut it = l.begin(); it != l.end(); it++) {
		// context

		float sumLine = counters[it->first];
		float localEntropy = 0;	

		for(it_map it2 = it->second.begin(); it2 != it->second.end(); it2++) {

			// symbols in each context

			float prob = it2->second/sumLine;
			localEntropy += -(prob * log2(prob));
		}

		totalEntropy += (localEntropy * (sumLine/total));
	}


	return totalEntropy;
}



FCM::LUT FCM::loadTable(string fileName) {
   
    ifstream infile;
    infile.open(fileName.c_str());
    string line;
    string delimiter = "=>";
    string delimiter2 = ";";
    string delimiter3 = ",";
    string delimiter4 = ":";   
 
    LUT l;
 
    if(infile.is_open()){
        while(infile.good()){
           
            getline(infile,line);
 
            size_t pos = 0;
            string context,occurr,prob_pair,caracter,prob_value;  
            while((pos = line.find(delimiter)) != string::npos){
               
                context = line.substr(0,pos);
                   
 
                map<char,float> mapa;
                l.insert(make_pair(context, mapa));
               
                line.erase(0,pos + delimiter.length());
                while((pos = line.find(delimiter2)) != string::npos){
                    occurr = line.substr(0,pos);
 
                    line.erase(pos,line.length());
                     
                    do{
 
                        
                        pos = line.find(delimiter3);
                        if(pos == -1){
                            if((pos = line.find(delimiter4)) != string::npos){
                                caracter = line.substr(0,pos);
                                prob_value = line.substr(pos+1,line.length()+1);
                               
                        
                                l[context].insert(make_pair(caracter.at(0),strtof(prob_value.c_str(),NULL)));
                            }
                            break;
                        }else{
                            prob_pair = line.substr(0,pos);
                            line.erase(0,pos + delimiter3.length());           
                        
                            if((pos = prob_pair.find(delimiter4)) != string::npos){
                                caracter = prob_pair.substr(0,pos);
                                prob_value = prob_pair.substr(pos+1,line.length()+1);
                                       
                        
 
                                l[context].insert(make_pair(caracter.at(0),strtof(prob_value.c_str(),NULL)));
                           
                            }
                       
                        }
                       
                    }while(1);
                }
            }
 
        }
        infile.close();
    }
   
    return l;
 
}

void FCM::saveTable(LUT lut) {
 
 
    ofstream myfile("saved_LUT", ios::out);
 
    string output;
 
    for(it_lut it = lut.begin(); it != lut.end(); it++) {
        char n = '\n';
        myfile << it->first;       
        myfile << "=>";
 
        int j = 0; 
        for(it_map it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            j++;
            if(j == it->second.size()) {
                myfile << it2->first << ":" << it2->second;
            }
            else{
                myfile << it2->first << ":" << it2->second << ","; 
            }
           
        }
        myfile << ";";
        myfile << "\n";
 
 
    }
 
    myfile << "$eot$";
    myfile.close();
 
}
 