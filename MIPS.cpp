#include "MIPS.h"
//non-member auxilary funtions

bitset<32> readDataFromMem( const vector<bitset<8>>& mem, bitset<32> dataAddress){
    auto addr=dataAddress.to_ulong();
           unsigned long data = 
               mem[addr+3].to_ulong()
               |mem[addr+2].to_ulong() << 8
               |mem[addr+1].to_ulong() << 16
               |mem[addr+0].to_ulong() << 24;
           return bitset<32>(data);
}

void writeDataToMem(vector<bitset<8>>& mem, bitset<32> dataAddress,bitset<32> wrtData){
    auto addr = dataAddress.to_ulong();
    auto data = wrtData.to_ulong();
    mem[addr+3]= data ;
    mem[addr+2]=data >>8;
    mem[addr+1]=data >>16;
    mem[addr+0]=data >>24;
}
