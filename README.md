# 32-bit-MIPS-Processor-Simulation-
Pipeline Structure
The MIPS pipeline has the following 5 stages:
1. Fetch (IF): fetches an instruction from instruction memory. Updates PC.
2. Decode (ID/RF): reads from the register RF and generates control signals required in subsequent stages. In addition, branches are resolved in this stage by checking for the branch condition and computing the effective address.
3. Execute (EX): performs an ALU operation.
4. Memory (MEM): loads or stores a 32-bit word from data memory.
5. Writeback (WB): writes back data to the RF.
