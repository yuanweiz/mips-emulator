#include "MIPS.h"
int main()
{

    MIPS mips("imem.txt","dmem.txt","RFresult.txt","dmemresult.txt");
    mips.start();
    mips.dumpDataMem();
    return 0;
}
