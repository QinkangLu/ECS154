//
//  CacheSet.hpp
//  Cache
//
//  Created by Qinkang Lu on 11/25/17.
//  Copyright © 2017 Qinkang Lu. All rights reserved.
//

#ifndef CacheSet_hpp
#define CacheSet_hpp

#include <cmath>
#include <stdio.h>
#include "Ram.hpp"
#include "CacheLine.hpp"

class CacheSet{
private:
    int* LRU;
    int num_lines;
    CacheLine** set;
public:
    CacheSet(int num_ways, int block_size);
    int lookForLRU();
    void updateLRU(int index);
    void updateRamIfDirty(int index, unsigned int address, Ram* memory);
    int* setRead(unsigned int address, int tag, int offset, int data, Ram* memory);
    void setWrite(unsigned int address, int tag, int offset, int data, Ram* memory);
};
CacheSet::CacheSet(int num_ways, int block_size){
    num_lines = num_ways;
    LRU = new int[num_lines];
    for (int i = 0; i < num_lines; i++) {
        LRU[i] = i;
    }
    set = new CacheLine*[num_lines];
    for (int i = 0; i < num_lines; i++) {
        set[i] = new CacheLine(block_size);
    }
}

void CacheSet::setWrite(unsigned int address, int tag, int offset, int data, Ram* memory){
    int i;
    for (i = 0; i < num_lines; i++) {
        if (set[i]->getTag() == tag) {      //we have a write hit
            set[i]->setData(data, offset);  //1. write data to cache
            set[i]->setDirty(1);            //2. set dirty to 1
            updateLRU(i);                   //3. since we just accessed a line, we update LRU
            return;
        }
    }
    if (i == num_lines) {  // it means we have write miss
        
        int index = lookForLRU();               //1. look for the LRU line and try to replace it
        updateRamIfDirty(index, address, memory);//2. check if the dirty is set. If so, updateRam
        int* block = memory->getBlockFromRam(address);
        set[index]->setBlock(block);        //3. if not, we get block from ram
        set[index]->setData(data, offset);  //4. we write our data to the position
        set[index]->setDirty(1);            //5. update dirty bit to 1
        set[index]->setTag(tag);            //6. remember to set the tag as well
    }
}

int* CacheSet::setRead(unsigned int address, int tag, int offset, int data, Ram* memory){
    int* result = new int[3];
    int i;
    
    for (i = 0; i < num_lines; i++) {
        if (set[i]->getTag() == tag) {  //we have a read hit
            set[i]->setData(data, offset);
            updateLRU(i);
            
            result[0] = data;
            result[1] = 1;
            result[2] = set[i]->getDirty();
            return result;
        }
    }
    if (i == num_lines) {               //we have a read miss
        int index = lookForLRU();          //1. look for LRU
        updateRamIfDirty(index, address, memory);//2. update ram
        int* block = memory->getBlockFromRam(address);
        set[index]->setBlock(block);        //3. get block from ram
        int dirty = set[index]->getDirty(); //4. get the dirty before we clean
        set[index]->setTag(tag);            //5. don't forget to set the tag bit
        set[index]->setDirty(0);
        
        result[0] = data;
        result[1] = 0;
        result[2] = dirty;
    }
    return result;
}

void CacheSet::updateLRU(int index){
    for (int i = 0; i < num_lines; i++) {
        if (LRU[i] > LRU[index]) {
            LRU[i]--;
        }
    }
    LRU[index] = num_lines-1;
}

int CacheSet::lookForLRU(){
    int result = 0;
    int min  = LRU[result];
    for (int i = 1; i < num_lines; i++) {
        if (LRU[i] < min) {
	    result = i;
            min = LRU[result];
        }
    }
    return result;
}

void CacheSet::updateRamIfDirty(int index, unsigned int address, Ram* memory) {
    if (set[index]->getDirty() == 1) {
        int* block = set[index]->getBlock();
        memory->updateRamWithBlock(address, block);
    }
}
#endif /* CacheSet_hpp */


