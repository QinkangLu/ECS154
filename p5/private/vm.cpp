//
//  main.cpp
//  vm.cpp
//
//  Created by Qinkang Lu on 12/7/17.
//  Copyright © 2017 Qinkang Lu. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
class Page{
public:
    unsigned int page_num = 0;
    bool used = 0;
};

bool find(unsigned int address, Page* frames){
    unsigned int page_num = (address >> 10) << 10;
    for (int i = 0; i < 4; i++) {
        if (frames[i].page_num == page_num) {
            return true;
        }
    }
    return false;
}

void replace(Page* frames, int& index, unsigned address){
    unsigned int page_num = (address >> 10) << 10;
    while (frames[index].used) {
        frames[index].used = 0;
    }
    frames[index].page_num = page_num;
}

int main(int argc, const char * argv[]) {
    Page page_table[32];
    Page frames[4];
    
    string line;
    unsigned int address;
    int index = 0;
    
    ofstream file_out("vm-out.txt");
    ifstream file_in(argv[1]);
    if (!file_in) {
        cout << "File reading is wrong" << endl;
        return -1;
    }
    
    for (int i = 0; i < 32; i++) {
        file_in >> hex >> page_table[i].page_num;
    }
    
    while(getline(file_in, line)){
        istringstream read(line);
        read >> hex >> address;
        if (!find(address, frames)) {
            replace(frames, index, address);
        }
        
        for (int i = 0; i < 4; i++) {
            if(frames[i].page_num != 0){
                file_out << frames[i].page_num << endl;;
            }
        }
        
    }
    
    return 0;
}
