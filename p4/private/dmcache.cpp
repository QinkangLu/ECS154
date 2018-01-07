//
//  main.cpp
//  Cache
//
//  Created by Qinkang Lu on 11/24/17.
//  Copyright © 2017 Qinkang Lu. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include "Ram.hpp"
#include "Cache.hpp"
using namespace std;

void writeFile(int* result, string data_out, ofstream &outFile);
void readFile(ifstream &inFile, string out_file, Cache* sacache, Ram* memory);

int main(int argc, const char * argv[]) {
    string out_file = "dm-out.txt";
    Cache* sacache = new Cache(256, 1, 8);
    Ram* memory = new Ram(8);
    
    ifstream inFile(argv[1]);
    if (inFile.fail()){
        cout << "could not open test file" << endl;
        return 0;
    }
    
    readFile(inFile, out_file, sacache, memory);
    inFile.close();
    return 0;
}




void readFile(ifstream &inFile, string out_file, Cache* sacache, Ram* memory){
    int* result = NULL;
    int data;
    unsigned int address;
    string line, mode, data_out;
    
    stringstream ss;
    
    ofstream outFile(out_file.c_str());
    
    while(getline(inFile, line)){
        istringstream read(line);
        
        //reading address
        read >> hex >> address;
        //reading read/write mode
        read >> mode;
        // reading data
        read >> data_out;
        //cout << data_out << endl;
        data = (int)strtoul(data_out.c_str(), NULL, 16);
        //cout << address << " " << mode << " " << data << endl;
        if (mode == "FF"){ //write to cache
            sacache->cacheWrite(address, data, memory);
        } else { //read from cache
            result = sacache->cacheRead(address, data, memory);
            writeFile(result, data_out, outFile);
        }
    }
    outFile.close();
}

/* writeFile
 * Parameters: data, isHit, dirty, filename
 * Function: write to the output file if we
 * have a read from cache. We also need to
 * write the dirty and hit bit to the output
 */
void writeFile(int* result, string data_out, ofstream &outFile){
    //int data = result[0];
    int hit = result[1];
    int dirty = result[2];
    stringstream read;
    
    
    if (outFile.fail()) {
        cout << "cout not open output file" << endl;
        return;
    }
    //cout << hex << data << " " << hit << " " << dirty << std::endl;
    outFile << hex << data_out << " " << hit << " " << dirty << std::endl;
}

