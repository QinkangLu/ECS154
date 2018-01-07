//
//  Ram.hpp
//  Cache
//
//  Created by Qinkang Lu on 11/24/17.
//  Copyright © 2017 Qinkang Lu. All rights reserved.
//

#ifndef Ram_hpp
#define Ram_hpp

#include <cmath>
#include <stdio.h>

class Ram{
private:
    int* ram;
    int RAM_SIZE;
    int BLOCK_SIZE;
public:
    Ram();
    Ram(int block_size);
    int* getBlockFromRam(unsigned int address);
    void updateRamWithBlock(unsigned int address, int* block);
};
Ram::Ram(){
    RAM_SIZE = 65536;
    BLOCK_SIZE = 8;
    ram = new int[RAM_SIZE];
    for (int i = 0; i < RAM_SIZE; i++) {
        ram[i] = 0;
    }
}

Ram::Ram(int block_size){
    RAM_SIZE = 65536;
    BLOCK_SIZE = block_size;
    ram = new int[RAM_SIZE];
    for (int i = 0; i < RAM_SIZE; i++) {
        ram[i] = 0;
    }
}

int* Ram::getBlockFromRam(unsigned int address){
    int shift = log2(BLOCK_SIZE);
    unsigned int tag_index = address >> shift; //we shift the square root of block size
    tag_index <<= shift;
    int* block = new int[BLOCK_SIZE];
    for(int i = 0; i < BLOCK_SIZE; i++){
        block[i] = ram[tag_index+i];
    }
    return block;
}

void Ram::updateRamWithBlock(unsigned int address, int* block){
    int shift = log2(BLOCK_SIZE);
    unsigned int tag_index = address >> shift;
    tag_index <<= shift;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        ram[tag_index+i] = block[i];
    }
    return;
}



#endif /* Ram_hpp */


