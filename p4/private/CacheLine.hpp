//
//  CacheLine.hpp
//  Cache
//
//  Created by Qinkang Lu on 11/24/17.
//  Copyright © 2017 Qinkang Lu. All rights reserved.
//

#ifndef CacheLine_hpp
#define CacheLine_hpp

#include <stdio.h>

class CacheLine{
private:
    int tag;
    int dirty;
    int* block;
    int LINE_SIZE;
public:
    CacheLine();
    CacheLine(int line_size);
    int getTag() const;
    int getDirty() const;
    int* getBlock() const;
    int getData(int offset) const;
    void setTag(int t);
    void setDirty(int d);
    void setBlock(int* cache_block);
    void setData(int data, int offset);
};
CacheLine::CacheLine(){
    tag = 0;
    dirty = 0;
    LINE_SIZE = 8;
    block = new int[LINE_SIZE];
    for (int i = 0; i < LINE_SIZE; i++) {
        block[i] = 0;
    }
}

CacheLine::CacheLine(int line_size){
    tag = 0;
    dirty = 0;
    LINE_SIZE = line_size;
    block = new int[LINE_SIZE];
    for (int i = 0; i < LINE_SIZE; i++) {
        block[i] = 0;
    }
}

int CacheLine::getTag() const{
    return tag;
}

int CacheLine::getDirty() const{
    return dirty;
}

int* CacheLine::getBlock() const{
    return block;
}

int CacheLine::getData(int offset) const{
    return block[offset];
}

void CacheLine::setTag(int t){
    tag = t;
}

void CacheLine::setDirty(int d){
    dirty = d;
}

void CacheLine::setBlock(int* cache_block){
    for (int i = 0; i < LINE_SIZE; i++) {
        block[i] = cache_block[i];
    }
}

void CacheLine::setData(int data, int offset){
    block[offset] = data;
}

#endif /* CacheLine_hpp */


