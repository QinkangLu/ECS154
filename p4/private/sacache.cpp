//this is the correct result: 29/11/2017
//  main.cpp
//  saca
//
//  Created by Qinkang Lu on 11/28/17.
//  Copyright © 2017 Qinkang Lu. All rights reserved.
//
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

class CacheSet{
    //need 16 sets
    //each set only need 8 dirty and 8 LRU bits and 8 tags
    int num_lines;
    int tags[8];
    int dirty_bits[8];
    int LRU[8];
    
public:
    CacheSet(){
        num_lines = 8;
        for (int i = 0; i < 8; i++) {
            tags[i] = -1;
            dirty_bits[i] = 0;
            LRU[i] = i;
        }
    }
    
    int* setRead(int tag){
        int* result = new int[2];
        int i = 0;
        for (i = 0; i < num_lines; i++) {
            if (tags[i] == tag) { // read hit
                result[0] = 1;
                result[1] = dirty_bits[i];
                updateLRU(i);
                return result;
            }
        }
        if (i == num_lines) {  //read miss
            //cout << "miss" << endl;
            //1. we have to evict the least recently used line
            int index = lookForLRU();
            //2. get the dirty bit at this state
            result[0] = 0; //miss
            result[1] = dirty_bits[index];
            //3. we evict a line by store it back to ram
            //4. and then set the dirty bit back to zero
            dirty_bits[index] = 0;
            //5. look for the line we want to read
            //6. which means set the tag
            tags[index] = tag;
            updateLRU(index);
        }
        return result;
    }
    
    void setWrite(int tag){
        int i = 0;
        for (i = 0; i < num_lines; i++) {
            if (tags[i] == tag) {  //write hit
                //1. write data
                //2. set dirty
                dirty_bits[i] = 1;
                updateLRU(i);
                return;
            }
        }
        if (i == num_lines) { // write miss
            //1. look for the least recently used line
            int index = lookForLRU();
            //2. store this block back to ram if dirty
            //3. now look for the block from ram
            //4. to represent we brought the block back, we set the tag
            tags[index] = tag;
            //5. set the dirty bit since we just modified the block
            dirty_bits[index] = 1;
            updateLRU(index);
        }
        return;
    }
    
    void updateLRU(int index){
        for (int i = 0; i < num_lines; i++) {
            if (LRU[i] > LRU[index]) {
                LRU[i]--;
            }
        }
        LRU[index] = num_lines-1;
    }
    
    int lookForLRU(){
        for (int i = 0; i < num_lines; i++) {
            if (LRU[i] == 0) {
                return i;
            }
        }
        cout << "something is wrong" << endl;
        return -1;
    }
};

class Cache{
private:
    CacheSet cache[16];
public:
    Cache(){
    }
    
    int* cacheRead(unsigned int address){
        int tag = address >> 6;
        unsigned int index = (address & 0x003C) >> 2;
        return cache[index].setRead(tag);
    }
    
    void cacheWrite(unsigned int address, string data){
        int tag = address >> 6;
        unsigned int index = (address & 0x003C) >> 2;
        if (index == 2) {
            //cout << "cache write: " << data << " " << index  << " at " << hex << address << endl;
        }
        
        cache[index].setWrite(tag);
    }
    
};


void writeFile(int* result, string data_out, ofstream &outFile);
void readFile(ifstream &inFile, string out_file, Cache* sacache);

int main(int argc, const char * argv[]) {
    string out_file = "sa-out.txt";
    Cache* sacache = new Cache();
    
    ifstream inFile(argv[1]);
    if (inFile.fail()){
        cout << "could not open test file" << endl;
        return 0;
    }
    
    readFile(inFile, out_file, sacache);
    inFile.close();
    return 0;
}


void readFile(ifstream &inFile, string out_file, Cache* sacache){
    int* result = NULL;
    int data;
    unsigned int address;
    string line, mode, data_out;
    
    stringstream ss;
    
    ofstream outFile(out_file.c_str());
    
    while(getline(inFile, line)){
        istringstream read(line);
        
        read >> hex >> address;
        read >> mode;
        read >> data_out;
        //cout << data_out << endl;
        data = (int)strtoul(data_out.c_str(), NULL, 16);
        if (mode == "FF"){
            sacache->cacheWrite(address, data_out);
        } else {
            result = sacache->cacheRead(address);
            writeFile(result, data_out, outFile);
        }
    }
    outFile.close();
}


void writeFile(int* result, string data_out, ofstream &outFile){
    //int data = result[0];
    int hit = result[0];
    int dirty = result[1];
    stringstream read;
    
    
    if (outFile.fail()) {
        cout << "cout not open output file" << endl;
        return;
    }
    //cout << hex << data_out << " " << hit << " " << dirty << std::endl;
    outFile << hex << data_out << " " << hit << " " << dirty << std::endl;
}


