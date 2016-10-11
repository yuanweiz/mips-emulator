#include "MIPS.h"
#include <gtest/gtest.h>
TEST(addu,oneplustwo){
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
            "./testdata/dmem_dummy.txt", /*no data memory needed*/
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
TEST(addu,addtoself){
    SUCCEED();
    //start running
    MIPS mips("./testdata/imem_addu.txt",
            "./testdata/dmem_dummy.txt", /*no data memory needed*/
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

TEST(addiu,addtoself){
    MIPS mips("./testdata/imem_dummy.txt",
            "./testdata/dmem_dummy.txt", 
            "/dev/null", /*I don't need to dump RF */
            "/dev/null" );
    bitset<32> ins(0x24210000); //addiu $1, (0)$1
    DecodeResult res(ins); 
    EXPECT_EQ(res.ALUOP(),ADDU )<< "expected ADDU";
    EXPECT_EQ(res.rs(),bitset<5>(1));
    EXPECT_EQ(res.rt(),bitset<5>(1));
    EXPECT_EQ(res.wrtEnable(),true);
    EXPECT_EQ(res.isRtype(),true);
    EXPECT_EQ(res.isJtype(),false);

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

int main (int argc,char* argv[]){
    ::testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
