#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop;  
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;  
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable;
    bool        nop;  
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;    
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        wrt_enable;    
    bool        nop;    
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;     
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;     
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public: 
        bitset<32> Reg_data;
     	RF()
    	{ 
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
        }
	
        bitset<32> readRF(bitset<5> Reg_addr)
        {   
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }
		 
		void outputRF()
		{
			ofstream rfout;
			rfout.open("RFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF:\t"<<endl;
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

class INSMem
{
	public:
        bitset<32> Instruction;
        INSMem()
        {       
			IMem.resize(MemSize); 
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
                  
		bitset<32> readInstr(bitset<32> ReadAddress) 
		{    
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;     
		}     
      
    private:
        vector<bitset<8> > IMem;     
};
      
class DataMem    
{
    public:
        bitset<32> ReadData;  
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
		
        bitset<32> readDataMem(bitset<32> Address)
        {	
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            return ReadData;               
		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)            
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));  
        }   
                     
        void outputDataMem()
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

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl; 
        
        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;        
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl; 
        
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl; 
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;
        
        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;        

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl; 
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;              
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;         
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;        

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;        
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;        
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl; 
    }
    else cout<<"Unable to open file";
    printstate.close();
}
 
 bitset<32> ALU(bitset<32> Rs,bitset<32> Rt,bitset<32> imm,bool sign, bool I)
 {
    bitset<32> c1,c2,d,result;
    c1 = bitset<32>(Rs.to_ulong() + Rt.to_ulong());
    c2 = bitset<32>(Rs.to_ulong() + imm.to_ulong());
    d = bitset<32>(Rs.to_ulong() - Rt.to_ulong());
    result = (sign)?((I)?c2:c1):d;
    return result;
 }

bitset<32> Signext_imm(bitset<16> imm)
{
  string IMM = (imm.to_string().at(0)=='0')?("0000000000000000"+imm.to_string()):("1111111111111111"+imm.to_string());
  return bitset<32>(IMM);
}

bitset<32> Signext_imm00(bitset<16> imm)
{
  string IMM = (imm.to_string().at(0)=='0')?("00000000000000"+imm.to_string()+"00"):("11111111111111"+imm.to_string()+"00");
  return bitset<32>(IMM);
}

int main()
{
    
    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
    stateStruct state, newState;
    bitset<6> opcode_ID;
    bitset<6> opcode_IF;
    bitset<3> alu_R;
    bitset<32> OP1, OP2 ,OP3;
    bitset<32> beqAddr = 0;
    bool STALL = 0;
    bool HALT = 0;
    bool beq = 0;
    bool neql = 0;
    bool is_beq_Rs_neql_Rt = 0;
    int cycle = 0; 
    //initial values
    state.WB.nop = 1;
    state.MEM.nop = 1;
    state.EX.nop = 1;
    state.ID.nop = 1;
    state.IF.nop = 0; 
    newState.EX.is_I_type = 0;
    newState.EX.rd_mem = 0;
    newState.EX.wrt_mem = 0;
    newState.EX.alu_op = 1;
    newState.EX.wrt_enable = 0;
    newState.MEM.rd_mem = 0;
    newState.MEM.wrt_mem = 0;
    newState.MEM.wrt_enable = 0;
    newState.WB.wrt_enable = 0;
             
    while (1) {

        /* --------------------- WB stage --------------------- */
        if(state.WB.nop)
            goto MEMstage;
        else{
            if(state.WB.wrt_enable)                                     //add,sub,lw=1
                myRF.writeRF(state.WB.Wrt_reg_addr,state.WB.Wrt_data);  //R ALU>>Rd || I MEM>>Rt
        } 

        /* --------------------- MEM stage --------------------- */ 
        MEMstage:
        if(state.MEM.nop)
            goto EXstage;
        else{
            newState.WB.Rs = state.MEM.Rs;
            newState.WB.Rt = state.MEM.Rt;
            newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
            newState.WB.wrt_enable = state.MEM.wrt_enable;

            if(state.MEM.wrt_mem){   //sw, write to MEM

                //Mem-Mem forwarding for Load-Store, Add-Store dependency
                if(state.MEM.Rt==state.WB.Wrt_reg_addr)                
                state.MEM.Store_data = state.WB.Wrt_data;

                myDataMem.writeDataMem(state.MEM.ALUresult,state.MEM.Store_data);
                newState.WB.Wrt_data = state.MEM.Store_data;
                
            }
            else if (state.MEM.wrt_enable)  //lw, add, sub
                newState.WB.Wrt_data = (state.MEM.rd_mem)?myDataMem.readDataMem(state.MEM.ALUresult):state.MEM.ALUresult; //lw MEM>>WB || R ALU>>WB
        }

        /* --------------------- EX stage --------------------- */ 
        EXstage:
        if(state.EX.nop)
            goto IDstage;
        else{
            newState.MEM.Rs = state.EX.Rs;
            newState.MEM.Rt = state.EX.Rt;
            newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
            newState.MEM.rd_mem = state.EX.rd_mem;
            newState.MEM.wrt_mem = state.EX.wrt_mem;
            newState.MEM.wrt_enable = state.EX.wrt_enable;
            newState.MEM.Store_data = state.EX.Read_data2;

            //Define 3 oprands for the ALU operation
            OP1 = state.EX.Read_data1;
            OP2 = state.EX.Read_data2;
            OP3 = Signext_imm(state.EX.Imm);

            //MEM-EX forwarding
            if(state.WB.wrt_enable){          
                if(state.EX.Rs == state.WB.Wrt_reg_addr)         
                    OP1 = state.WB.Wrt_data;
                if(state.EX.Rt == state.WB.Wrt_reg_addr){
                    OP2 = state.WB.Wrt_data;
                    newState.MEM.Store_data = state.WB.Wrt_data; //Used for rt of sw has dependency of its grandparent.
                }

            }

            //EX-EX forwarding
            if(state.MEM.wrt_enable && state.MEM.nop==0){       //Ex-Ex forwarding only happens when last instruction is add or lw AND STALL does not happen
                if(state.EX.Rs == state.MEM.Wrt_reg_addr)       
                    OP1 = state.MEM.ALUresult;
                if(state.EX.Rt == state.MEM.Wrt_reg_addr)
                    OP2 = state.MEM.ALUresult;
            } 
            //First, check for if Mem-Ex forwarding is needed
            //Then, check for if Ex-Ex forwarding is needed (end for using this value if there is EX-EX dependency)
            //If no dependency, op = readdata

            //Alu calculation
            newState.MEM.ALUresult = ALU(OP1,OP2,OP3,state.EX.alu_op,state.EX.is_I_type);
            //AlU operation defined above
            //Perform OP1 +/- OP2 for R type
            //Perform OP1 + signextended IMM field for I type
            }


        /* --------------------- ID stage --------------------- */ 

        IDstage:
        if(state.ID.nop)
            goto IFstage;
        else{
                opcode_ID = bitset<6> (state.ID.Instr.to_string().substr(0,6));
                alu_R =  bitset<3> (state.ID.Instr.to_string().substr(29,3));

                newState.EX.Rs = bitset<5>(state.ID.Instr.to_string().substr(6,5));
                newState.EX.Rt = bitset<5>(state.ID.Instr.to_string().substr(11,5));
                newState.EX.Imm = bitset<16>(state.ID.Instr.to_string().substr(16,16));
                newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
                newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);
                
                newState.EX.alu_op = (opcode_ID.to_ulong()==35 || opcode_ID.to_ulong()==43 || alu_R.to_ulong()==1)?1:0; //set 1 for lw, addu(I),addu(R);0 for subu
                newState.EX.is_I_type = (opcode_ID.to_ulong()!=0)?1:0;
                newState.EX.rd_mem = (opcode_ID.to_ulong()==35)?1:0;   //if LOAD, read mem
                newState.EX.wrt_mem = (opcode_ID.to_ulong()==43)?1:0;  //if STORE, wtite mem
                newState.EX.wrt_enable = (opcode_ID.to_ulong()==43 || opcode_ID.to_ulong()==4)?0:1;  //addu,subu,lw -> write RF
                newState.EX.Wrt_reg_addr = (newState.EX.is_I_type)?newState.EX.Rt:bitset<5>(state.ID.Instr.to_string().substr(16,5));
                
                //branch
                beq = (opcode_ID.to_ulong()==4)?1:0;  //1:branch; 0:not branch.
                neql = (newState.EX.Read_data1.to_ulong()!=newState.EX.Read_data2.to_ulong())?1:0;  //1:R[rs]!=R[rt]; 0: R[rs]==R[rt] 
                is_beq_Rs_neql_Rt = (beq && neql)?1:0;  //1: branch(R[rs]!=R[rt]); 0:otherwise           
                beqAddr = (is_beq_Rs_neql_Rt)?Signext_imm00(newState.EX.Imm):0;  //Calculate branch address.
                
                //stall
                if(state.EX.rd_mem && newState.EX.is_I_type && (newState.EX.Rs==state.EX.Wrt_reg_addr))  //Load-Load and Load-Store dependency
                    STALL = 1;
                if(state.EX.rd_mem && newState.EX.is_I_type==0 && (newState.EX.Rs==state.EX.Wrt_reg_addr || newState.EX.Rt==state.EX.Wrt_reg_addr)) //Load-Add dependency
                    STALL = 1;
                    //Implement STALL: If last instruction is lw and New instruction fetches from the same address of last writing before it is done.
            }


        /* --------------------- IF stage --------------------- */
        IFstage:
                    
        HALT = (opcode_IF.to_ulong()==63)?1:0; // if all 1's, then set HALT=1

        //nops propagation
        newState.WB.nop = state.MEM.nop; 
        newState.MEM.nop = state.EX.nop; 
        newState.EX.nop = (STALL)?1:state.ID.nop;            //Setting nop for EX stage if STALL or branch happens.
        newState.ID.nop = (HALT)?1:((is_beq_Rs_neql_Rt)?1:state.IF.nop);   //If halt, set nop. If branch and $Rs!=$Rt, set nop.       
        newState.IF.nop = (HALT)?1:0;    //If halt, set nop.

        if(STALL!=1 && is_beq_Rs_neql_Rt!=1)  // fetch instruction if not STALL AND not branch($Rs!=$Rt)
            newState.ID.Instr = myInsMem.readInstr(state.IF.PC);  

        if(STALL || HALT) 
            goto next;
        else{   
                newState.IF.PC = (is_beq_Rs_neql_Rt!=1)?bitset<32>(state.IF.PC.to_ulong() + 4):bitset<32>(state.IF.PC.to_ulong() + beqAddr.to_ulong()); //PC=PC+4 if not branch($Rs!=$Rt); PC=PC+4+BranchAddress if branch($Rs!=$Rt).
                opcode_IF = bitset<6>(myInsMem.readInstr(newState.IF.PC).to_string().substr(0,6));  //opcode of new instruction 
            }

        next: 
        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            break;
        
        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
       
        state = newState; /*The end of the cycle and updates the current state with the values calculated in this cycle */ 
        cout<<cycle<<","; //cycle counter
        cycle++;
     	STALL = 0;  //reset STALL's status
        beq = 0;   //reset branch's status
        neql = 0;  
        is_beq_Rs_neql_Rt = 0;
    }
    
    myRF.outputRF(); // dump RF;	
	myDataMem.outputDataMem(); // dump data mem 
	
	return 0;
}