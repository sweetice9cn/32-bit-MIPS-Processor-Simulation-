# 32-bit-MIPS-Processor-Simulation-
<h3>Pipeline Structure</h3>
The MIPS pipeline has the following 5 stages:<br/>
1. Fetch (IF): fetches an instruction from instruction memory. Updates PC.<br/>
2. Decode (ID/RF): reads from the register RF and generates control signals required in subsequent stages. In addition, branches are resolved in this stage by checking for the branch condition and computing the effective address.<br/>
3. Execute (EX): performs an ALU operation.<br/>
4. Memory (MEM): loads or stores a 32-bit word from data memory.<br/>
5. Writeback (WB): writes back data to the RF.<br/>

<p>Each pipeline stages takes inputs from flip-flops. The input flip-flops for each pipeline stage are described in the tables below.</p><br/>
<img src="1.png"><br/>
<img src="2.png"><br/>
<img src="3.png"><br/>
<img src="4.png"><br/>
<h3>Dealing with Hazards</h3>
1. RAW Hazards: RAW hazards are dealt with using either only forwarding (if possible) or, if not, using stalling + forwarding.<br/>
2. Control Flow Hazards: Assume that branch conditions are resolved in the ID/RF stage of the pipeline. The processor deals with beq instructions as follows:<br/>
a. Branches are always assumed to be NOT TAKEN. That is, when a beq is fetched in the IF stage, the PC is speculatively updated as PC+4.<br/>
b. Branch conditions are resolved in the ID/RF stage.<br/>
c. Two operations are performed in the ID/RF stage: (i) Read_data1 and Read_data2 are compared to determine the branch outcome; (ii) the effective branch address is computed.<br/>
d. If the branch is NOT TAKEN, execution proceeds normally. However, if the branch is TAKEN, the speculatively fetched instruction from PC+4 is quashed in its ID/RF stage using the nop bit and the next instruction is fetched from the effective branch address. Execution now proceeds normally.<br/>

<h3>The nop bit</h3>
The nop bit for any stage indicates whether it is performing a valid operation in the current clock cycle. The nop bit for the IF stage is initialized to 0 and for all other stages is initialized to 1. (This is because in the first clock cycle, only the IF stage performs a valid operation.)
In the absence of hazards, the value of the nop bit for a stage in the current clock cycle is equal to the nop bit of the prior stage in the previous clock cycle.
However, the nop bit is also used to implement stall that result from a RAW hazard or to quash speculatively fetched instructions if the branch condition evaluates to TAKEN.<br/>

<h3>The HALT Instruction</h3>
The halt instruction is a “custom” instruction to stop the simulation. When a HALT instruction is fetched in IF stage at cycle N, the nop bit of the IF stage in the next clock cycle (cycle N+1) is set to 1 and subsequently stays at 1. The nop bit of the ID/RF stage is set to 1 in cycle N+1 and subsequently stays at 1. The nop bit of the EX stage is set to 1 in cycle N+2 and subsequently stays at 1. The nop bit of the MEM stage is set to 1 in cycle N+3 and subsequently stays at 1. The nop bit of the WB stage is set to 1 in cycle N+4 and subsequently stays at 1.
At the end of each clock cycle the simulator checks to see if the nop bit of each stage is 1. If so, the simulation halts.