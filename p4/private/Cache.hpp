//
//  Cache.hpp
//  Cache
//
//  Created by Qinkang Lu on 11/24/17.
//  Copyright © 2017 Qinkang Lu. All rights reserved.
//

#ifndef Cache_hpp
#define Cache_hpp

#include <cmath>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include "CacheSet.hpp"
#include "Ram.hpp"

class Cache{
private:
    int CACHE_SIZE;
    unsigned int TAG_BITS;
    unsigned int INDEX_BITS;
    unsigned int OFFSET_BITS;
    int NUM_SETS;
    CacheSet** cache;
public:
    Cache(int cache_size, int num_ways, int block_size);
    int* cacheRead(unsigned int address, int data, Ram* memory);
    void cacheWrite(unsigned int address, int data, Ram* memory);
    void parseAddress(unsigned int address, unsigned int &tag, unsigned int &index, unsigned int &offset);
};

Cache::Cache(int cache_size, int num_ways, int block_size){
    NUM_SETS = cache_size / block_size / num_ways;
    INDEX_BITS = log2(NUM_SETS);
    OFFSET_BITS = log2(block_size);
    TAG_BITS = 16 - OFFSET_BITS - INDEX_BITS;
    
    cache = new CacheSet*[NUM_SETS];
    for (int i = 0; i < NUM_SETS; i++) {
        cache[i] = new CacheSet(num_ways, block_size);
    }
}

/*
 * cacheRead
 * parameters: address, memory
 * return val: an array of 3 elements: data, hit, dirty
 */
int* Cache::cacheRead(unsigned int address, int data, Ram* memory){
    unsigned int tag = 0;
    unsigned int index = 0;
    unsigned int offset = 0;
    parseAddress(address, tag, index, offset);
    return cache[index]->setRead(address, tag, offset, data, memory);
}


/* cacheWrite
 * parameters: address, data, memory
 * return val: NULL (trying to signify that we
 *             aren't going to output anything
 */
void Cache::cacheWrite(unsigned int address, int data, Ram* memory){
    unsigned int tag = 0;
    unsigned int index = 0;
    unsigned int offset = 0;
    parseAddress(address, tag, index, offset);
    cache[index]->setWrite(address, tag, offset, data, memory);
}


void Cache::parseAddress(unsigned int address, unsigned int &tag, unsigned int &index, unsigned int &offset) {
    tag = address >> (INDEX_BITS+OFFSET_BITS);
    index = address << (16+TAG_BITS);
    index >>= (32-INDEX_BITS);
    offset = address << (32-OFFSET_BITS);
    offset >>= (32-OFFSET_BITS);
}

#endif /* Cache_hpp */


