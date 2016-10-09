#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
#include <assert.h>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

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
    mem[addr+3]= data & 0x000000ff;
    mem[addr+2]=data &0x0000ff00;
    mem[addr+1]=data &0x00ff0000;
    mem[addr+0]=data &0xff000000;
}
class RF
{
    public:
        bitset<32> ReadData1, ReadData2; 
     	RF()
    	{ 
          Registers.resize(32);  
          Registers[0] = bitset<32> (0);  
        }
	
        void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
        {   
            // implement the funciton by you.                
            ReadData1 = Registers[RdReg1.to_ulong()];
            ReadData2 = Registers[RdReg2.to_ulong()];
            if (WrtEnable.test(0)){
                //$0 should never be modified
                auto wrtRegIdx = WrtReg.to_ulong();
                if (wrtRegIdx==0) return;
                Registers[wrtRegIdx]=WrtData;
            }
         }
		 
	void OutputRF()
             {
               ofstream rfout;
                  rfout.open("RFresult.txt",std::ios_base::app);
                  if (rfout.is_open())
                  {
                    rfout<<"A state of RF:"<<endl;
                  for (int j = 0; j<32; j++)
                      {        
                        rfout << Registers[j]<<endl;
                      }
                     
                  }
                  else cout<<"Unable to open file";
                  rfout.close();
               
               }     
	private:
            vector<bitset<32> >Registers;
	
};

class ALU
{
      public:
             bitset<32> ALUresult;
             bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
             {   
                 // implement the ALU operations by you. 
                 auto opcode = ALUOP.to_ulong();
                 auto op1 = oprand1.to_ulong(), op2 = oprand2.to_ulong();
                 switch (opcode){
                     case ADDU:
                         ALUresult = op1+op2;
                         break;
                     case SUBU:
                         ALUresult = op1-op2;
                         break;
                     case NOR:
                         ALUresult = ~(oprand1 | oprand2);
                         break;
                     case AND:
                         ALUresult = oprand1 & oprand2;
                         break;
                     case OR:
                         ALUresult = oprand1 | oprand2;
                         break;
                 }
                 return ALUresult;
             }            
};

class INSMem
{
      public:
          bitset<32> Instruction;
          INSMem()
          {       IMem.resize(MemSize); 
                  ifstream imem;
                  string line;
                  int i=0;
                  imem.open("imem.txt");
                  if (imem.is_open())
                  {
                  while (getline(imem,line))
                     {      
                        IMem[i] = bitset<8>(line);
                        i++;
                     }
                     
                  }
                  else cout<<"Unable to open file";
                  imem.close();
                     
                  }
                  
          bitset<32> ReadMemory (bitset<32> ReadAddress) 
              {    
               // implement by you. (Read the byte at the ReadAddress and the following three byte).
               return Instruction = readDataFromMem(IMem,ReadAddress);
              }     
      
      private:
           vector<bitset<8> > IMem;
      
};
      
class DataMem    
{
      public:
          bitset<32> readdata;  
          DataMem()
          {
             DMem.resize(MemSize); 
             ifstream dmem;
                  string line;
                  int i=0;
                  dmem.open("dmem.txt");
                  if (dmem.is_open())
                  {
                  while (getline(dmem,line))
                       {      
                        DMem[i] = bitset<8>(line);
                        i++;
                       }
                  }
                  else cout<<"Unable to open file";
                  dmem.close();
          
          }  
          bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
          {    
               // implement by you.
               bool read = readmem.test(0);
               bool wrt = writemem.test(0);
               //generally read and wrt shouldn't be enabled at same time
               assert(read ^ wrt);
               if (read){
                   readdata = readDataFromMem(DMem, Address );
               }
               if (wrt){
                   writeDataToMem(DMem,Address,WriteData);
               }
               return readdata;     
          }   
                     
          void OutputDataMem()
          {
               ofstream dmemout;
                  dmemout.open("dmemresult.txt");
                  if (dmemout.is_open())
                  {
                  for (int j = 0; j< 1000; j++)
                       {     
                        dmemout << DMem[j]<<endl;
                       }
                     
                  }
                  else cout<<"Unable to open file";
                  dmemout.close();
               
               }             
      
      private:
           vector<bitset<8> > DMem;
      
};  
   
class DecodeResult{
    public:
        //void parseInstruction(bitset<32>ins){
        //    ins_=ins.to_ulong();
        //}
        explicit DecodeResult(bitset<32>ins)
            :ins_(ins.to_ulong()){}
        bool isRtype()const {
            return 0==(ins_ & 0xFC000000);
        }
        bool isJtype() const{
            return !isRtype() && !isItype();
        }
        bool isItype() const{
            return 0!=(ins_ & 0xFC000000);
        }
        bool isHalt() const{
            return (~ins_)==0x0;
        }
        
        //these three are actually not that correct,
        //but works for this lab
        bool isBranch()const{
            return bitset<6>(0x04)==opcode();
        }
        bool isStore() const{
            return bitset<6>(0x2B) == opcode();
        }
        bool isLoad() const {
            return bitset<6>(0x23) == opcode();
        }

        bitset<3> ALUOP()const{
            if (isRtype() ){
                return indice<0,3>();
            }
            else {
                return indice<26,29>();
            }
        }
        bitset<6> opcode() const {
            return indice<26,32> ();
        }
        bitset<5> rs() const {
            return indice<21,26>();
        }
        bitset<5> rt() const {
            return indice<16,21>();
        }
        bitset<5> rd() const {
            return indice<11,16>();
        }
        bitset<5> shamt() const{
            return indice<6,11>();
        }
        bitset<6> funct () const{
            return indice<0,6>();
        }
        bitset<16> imm()const{
            return indice<0,16>();
        }
        bitset<32> extImm()const{
            //in this Lab we only need zero-extended immediates
            //so things get simplified
            return bitset<32>( imm().to_ulong());

        }
        bitset<26> addr() const{
            return indice<0,26>();
        }

        //NOTICE: wrtEnable means "write to register file",
        // to judge if the instruction writes to memory
        // plese use isStore();
        bitset<1> wrtEnable()const{
            return indice<29,30>();
        }
    private:
        template <unsigned int s, unsigned int e >
            bitset<e-s> indice ()const {
                return bitset<e-s> (ins_ >> s );
            }
        unsigned long ins_;
};
int main()
{
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;

    bitset<32> PC;
    while (1)
	{
        // Fetch
        auto ins = myInsMem.ReadMemory(PC);
        
        
		// If current insturciton is "11111111111111111111111111111111", then break;
		// decode(Read RF)
        DecodeResult res(ins); 
        if ( res.isHalt())
            break;
        bitset<1> isStore(res.isStore()),
            isLoad(res.isLoad());
		
		// Execute
        // semantics: ReadData1<- Reg[reg1], ReadData2<-Reg[reg2]
        myRF.ReadWrite(res.rs(),res.rt(),res.rd(),ins,res.wrtEnable());

        //ALU operands:
        // semantics:ALUResult<- op ( operand1, operand2 , ALUOP)
        // operand1 is always ReadData1,
        // in respect of operand2:
        // for R-type, operand2 is ReadData2, for Load/Store, it's extended-imm
        // for branch instruction, the ALU still works, but the result
        // will be discarded 
        if (res.isRtype()){
            myALU.ALUOperation(res.ALUOP(),myRF.ReadData1,myRF.ReadData2);
        }
        else if (res.isItype()){
            myALU.ALUOperation(res.ALUOP(),myRF.ReadData1, res.extImm());
        }
        bool isEq = (myRF.ReadData1 == myRF.ReadData2);
		
		// Read/Write Mem
        // semantics: if Load, then cache<- Memory[ALUResult]
        // if Store, then Memory[ALUResult] <- reg
        myDataMem.MemoryAccess(myALU.ALUresult,myRF.ReadData1,isLoad,isStore);
        
		// Write back to RF
        // semantics: if Load, then reg<- DataMem::cache;
        myRF.ReadWrite(res.rs(),res.rt(),res.rd(),myDataMem.readdata,res.wrtEnable());

        //NextPC
        if (res.isBranch() && isEq ){
            PC = res.extImm();
        }
        else if (res.isJtype()){
            auto pc=PC.to_ulong();
            pc = res.addr().to_ulong() <<2 | (pc&0xFC000000);
            PC=pc;
        }
        else {
            PC=bitset<32>(PC.to_ulong()+4);
        }
		
        myRF.OutputRF(); // dump RF;    
    }
    myDataMem.OutputDataMem(); // dump data mem
  
    return 0;
        
}
