#include "MIPS.h"
#include <gtest/gtest.h>

MIPS getDummyMIPS (unsigned int maxInstructionCount=10){
    auto ret= MIPS("./testdata/imem_dummy.txt",
            "./testdata/dmem_dummy.txt", /*no data memory required */
            "/dev/null", /*I don't need to dump RF */
            "/dev/null" );
    writeDataToMem(ret.myInsMem.IMem,4*maxInstructionCount, 0xffffffff);
    return ret;
}

TEST(decode, pred ){
    //wrtEnable is the most important predication
    unsigned long ins[] = {
        0x00221821, //addu $3,$1,$2
        0x08000000, // j 0x0
        0x8c000000, // lw $0,0($0)
        0xac000000, // sw $0,0($0)
    };
    bool expectWrtEnable[] = {
        true,
        false,
        true,
        false,
    };
    for (size_t i=0;i< sizeof (expectWrtEnable)/sizeof(bool);++i){
        DecodeResult res(ins[i]);
        EXPECT_EQ( res.wrtEnable(),expectWrtEnable[i]) << "i=" << i;
    }
}
TEST(addu,onePlusTwo){
    DecodeResult res(bitset<32>(0x00221821));
    EXPECT_EQ(res.ALUOP(),ADDU )<< "expected ADDU";
    EXPECT_EQ(res.rs(),bitset<5>(1));
    EXPECT_EQ(res.rt(),bitset<5>(2));
    EXPECT_EQ(res.rd(),bitset<5>(3));
    EXPECT_EQ(res.wrtEnable(),true);
    EXPECT_EQ(res.isRtype(),true);
    EXPECT_EQ(res.isJtype(),false);

    //start running
    MIPS mips("./testdata/imem_addu.txt",
            "./testdata/dmem_dummy.txt", /*no data memory required */
            "/dev/null", /*I don't need to dump RF */
            "/dev/null" );
    mips.myRF.set(1,1);
    mips.myRF.set(2,2);
    mips.start();
    unsigned long val;
    EXPECT_EQ(val=mips.myRF.get(0), 0) << "actual value:" << val;
    EXPECT_EQ(val=mips.myRF.get(1), 1) << "actual value:" << val;
    EXPECT_EQ(val=mips.myRF.get(2), 2) << "actual value:" << val;
    EXPECT_EQ(val=mips.myRF.get(3), 3) << "actual value:" << val;
    for (int i=4;i<32;++i){
        EXPECT_EQ(mips.myRF.get(i),0) << "actual value:" << val;
    }
}
TEST(addu,addToSelf){
    auto mips = getDummyMIPS();
    //force load instruction and data
    bitset<32> ins (0x00210821);
    writeDataToMem(mips.myInsMem.IMem,0x0, ins);
    writeDataToMem(mips.myDataMem.DMem, 0x1234, 0x12345678 );
    DecodeResult res(ins);
    EXPECT_EQ(res.rs(),1);
    EXPECT_EQ(res.rt(),1);
    mips.myRF.set(1,5);
    mips.start();
    unsigned long val;
    EXPECT_EQ(val=mips.myRF.get(0), 0) ;
    EXPECT_EQ(val=mips.myRF.get(1), 10);
    for (int i=2;i<32;++i){
        EXPECT_EQ(mips.myRF.get(i),0) ;
    }
}

TEST(addu,overflow){
    auto ins=bitset<32>(0x00221821); //addu $3,$1,$2
    auto mips = getDummyMIPS();
    DecodeResult res(ins);
    writeDataToMem(mips.myInsMem.IMem,0x0, ins);
    EXPECT_EQ(res.ALUOP(),ADDU )<< "expected ADDU";
    EXPECT_EQ(res.rs(),bitset<5>(1));
    EXPECT_EQ(res.rt(),bitset<5>(2));
    EXPECT_EQ(res.rd(),bitset<5>(3));
    EXPECT_EQ(res.wrtEnable(),true);
    EXPECT_EQ(res.isRtype(),true);
    EXPECT_EQ(res.isJtype(),false);

    //start running
    mips.myRF.set(1,0xffffffff);
    mips.myRF.set(2,0xffffffff);
    mips.start();
    EXPECT_EQ(mips.myRF.get(3), 0xfffffffe) ;
}
TEST(subu,underflow){
    auto ins=bitset<32>(0x00221823); //subu $3,$1,$2
    auto mips = getDummyMIPS();
    DecodeResult res(ins);
    EXPECT_EQ(res.ALUOP(),SUBU )<< "expected SUBU";
    writeDataToMem(mips.myInsMem.IMem,0x0, ins);
    EXPECT_EQ(res.rs(),bitset<5>(1));
    EXPECT_EQ(res.rt(),bitset<5>(2));
    EXPECT_EQ(res.rd(),bitset<5>(3));
    EXPECT_EQ(res.wrtEnable(),true);
    EXPECT_EQ(res.isRtype(),true);
    EXPECT_EQ(res.isJtype(),false);

    //start running
    mips.myRF.set(1,1);
    mips.myRF.set(2,2);
    mips.start();
    EXPECT_EQ(mips.myRF.get(3), 0xffffffff) ;
}

TEST(addiu,addToSelf){
    //start running
    auto mips = getDummyMIPS();
    //force load instruction and data
    bitset<32> ins (0x24211234); //addiu $1, 0x1234($1)
    writeDataToMem(mips.myInsMem.IMem,0x0, ins);
    DecodeResult res(ins);
    EXPECT_EQ(res.imm(), 0x1234);
    EXPECT_EQ(res.extImm(), 0x1234);
    EXPECT_EQ(res.rs(),1);
    EXPECT_EQ(res.rt(),1);
    EXPECT_EQ(res.isItype(),true);
    mips.myRF.set(1,5);
    mips.start();
    unsigned long val;
    EXPECT_EQ(val=mips.myRF.get(1), 0x1239);
    for (int i=2;i<32;++i){
        EXPECT_EQ(mips.myRF.get(i),0) ;
    }
}

TEST(addiu,addToOtherReg){
    //start running
    auto mips=getDummyMIPS();
    //force load instruction and data
    bitset<32> ins (0x24221234); //addiu $1, 0x1234($2)
    writeDataToMem(mips.myInsMem.IMem,0x4, ins);
    DecodeResult res(ins);
    EXPECT_EQ(res.imm(), 0x1234);
    EXPECT_EQ(res.extImm(), 0x1234);
    EXPECT_EQ(res.rs(),1);
    EXPECT_EQ(res.rt(),2);
    EXPECT_EQ(res.isItype(),true);
    mips.myRF.set(1,5);
    mips.start();
    EXPECT_EQ(mips.myRF.get(2), 0x1239);
}
TEST(addiu,checkUnsigned){
    auto mips= getDummyMIPS();
    //force load instruction and data
    bitset<32> ins (0x2422ffff); //addiu $1, 0xffff($2)
    writeDataToMem(mips.myInsMem.IMem,0x0, ins);
    DecodeResult res(ins);
    EXPECT_EQ(res.imm(), 0xffff);
    EXPECT_EQ(res.extImm(), 0xffff); //should not be 0xffffffff
    EXPECT_EQ(res.rs(),1);
    EXPECT_EQ(res.rt(),2);
    EXPECT_EQ(res.isItype(),true);
    mips.myRF.set(1,5);
    mips.start();
    EXPECT_EQ(mips.myRF.get(2), 0x10004);
}

TEST(beq, equal){
    auto mips= getDummyMIPS();
    //force load instruction and data
    writeDataToMem(mips.myInsMem.IMem,0x0, 0x10220002); //beq $1,$2,0x2
    writeDataToMem(mips.myInsMem.IMem,0x4, 0x2422ffff); //addiu $1, 0xffff($2)
    writeDataToMem(mips.myInsMem.IMem,0x8, 0xffffffff); //halt, shouldn't reach here
    writeDataToMem(mips.myInsMem.IMem,0xc, 0x00221821); //but here, (addu $3,$1,$2)
    mips.myRF.set(1,5);
    mips.myRF.set(2,5); //shouldn't be modified
    mips.start();
    EXPECT_EQ(mips.myRF.get(1),5);
    EXPECT_EQ(mips.myRF.get(2),5);
    EXPECT_EQ(mips.myRF.get(3),10);
}

TEST(beq, notEqual){
    auto mips= getDummyMIPS();
    //force load instruction and data
    writeDataToMem(mips.myInsMem.IMem,0x0, 0x10220000); //beq $1,$2,0x0
    writeDataToMem(mips.myInsMem.IMem,0x4, 0x24220000); //addiu $1, 0x0($2)
    writeDataToMem(mips.myInsMem.IMem,0x8, 0x00221821); //addu $3,$1,$2
    mips.myRF.set(1,5);
    mips.myRF.set(2,10); //shouldn't be modified
    mips.start();
    EXPECT_EQ(mips.myRF.get(1),5);
    EXPECT_EQ(mips.myRF.get(2),5);
    EXPECT_EQ(mips.myRF.get(3),10);
}

TEST(j,forward){
    auto mips= getDummyMIPS();
    //force load instruction and data
    writeDataToMem(mips.myInsMem.IMem,0x0, 0x08000001); //j 0x0
    writeDataToMem(mips.myInsMem.IMem,0x4, 0x24220000); //addiu $1, 0x0($2)
    writeDataToMem(mips.myInsMem.IMem,0x8, 0x00221821); //addu $3,$1,$2
    mips.myRF.set(1,5);
    mips.myRF.set(2,10); //shouldn't be modified
    mips.start();
    EXPECT_EQ(mips.myRF.get(1),5);
    EXPECT_EQ(mips.myRF.get(2),5);
    EXPECT_EQ(mips.myRF.get(3),10);
}

TEST(lw,toSelf){
    auto mips=getDummyMIPS();
    writeDataToMem(mips.myInsMem.IMem,0x0, 0x8c214321); //lw $1,0x4321($1)
    writeDataToMem(mips.myDataMem.DMem,0x4326, 0x12345678); 
    mips.myRF.set(1,5);
    mips.start();
    EXPECT_EQ(mips.myRF.get(1),0x12345678);
}

TEST(lw,toOtherReg){
    auto mips=getDummyMIPS();
    writeDataToMem(mips.myInsMem.IMem,0x0, 0x8c224321); //lw $1,0x4321($2)
    writeDataToMem(mips.myDataMem.DMem,0x4326, 0x12345678); 
    mips.myRF.set(1,5);
    mips.start();
    EXPECT_EQ(mips.myRF.get(2),0x12345678);
}

TEST(sw,sw){
    auto mips=getDummyMIPS();
    writeDataToMem(mips.myInsMem.IMem,0x0, 0xac224321); //sw $1,0x4321($2)
    mips.myRF.set(1,5);
    mips.myRF.set(2,0x12345678);
    mips.start();
    EXPECT_EQ(readDataFromMem(mips.myDataMem.DMem,0x4326),0x12345678);
}

int main (int argc,char* argv[]){
    ::testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
