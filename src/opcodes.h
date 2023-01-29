#ifndef OPCODES_H
#define OPCODES_H

#include "cpu.h"

typedef void (*opcode) (Processor *cpu, InstrInfo *ir);

//set the internal helper values abs_addr and fetched_value of a cpu
//pass InstrInfo to record information of instruction execution
void fetch_target_value(Processor *cpu, InstrInfo *ir);

//convert byte to corresping opcode, returned InstrInfo will contain
//function, addressmode, instruction byte size, and cycle count of the corresponding instruction
InstrInfo decode_instruction(uint8_t byte);

//opcode declarations
void I_ADC(Processor *cpu, InstrInfo *ir);
void I_AND(Processor *cpu, InstrInfo *ir);
void I_ASL(Processor *cpu, InstrInfo *ir);
void I_BCC(Processor *cpu, InstrInfo *ir);
void I_BCS(Processor *cpu, InstrInfo *ir);
void I_BEQ(Processor *cpu, InstrInfo *ir);
void I_BIT(Processor *cpu, InstrInfo *ir);
void I_BMI(Processor *cpu, InstrInfo *ir);
void I_BNE(Processor *cpu, InstrInfo *ir);
void I_BPL(Processor *cpu, InstrInfo *ir);
void I_BRK(Processor *cpu, InstrInfo *ir);
void I_BVC(Processor *cpu, InstrInfo *ir);
void I_BVS(Processor *cpu, InstrInfo *ir);
void I_CLC(Processor *cpu, InstrInfo *ir);
void I_CLD(Processor *cpu, InstrInfo *ir);
void I_CLI(Processor *cpu, InstrInfo *ir);
void I_CLV(Processor *cpu, InstrInfo *ir);
void I_CMP(Processor *cpu, InstrInfo *ir);
void I_CPX(Processor *cpu, InstrInfo *ir);
void I_CPY(Processor *cpu, InstrInfo *ir);
void I_DEC(Processor *cpu, InstrInfo *ir);
void I_DEX(Processor *cpu, InstrInfo *ir);
void I_DEY(Processor *cpu, InstrInfo *ir);
void I_EOR(Processor *cpu, InstrInfo *ir);
void I_INC(Processor *cpu, InstrInfo *ir);
void I_INX(Processor *cpu, InstrInfo *ir);
void I_INY(Processor *cpu, InstrInfo *ir);
void I_JMP(Processor *cpu, InstrInfo *ir);
void I_JSR(Processor *cpu, InstrInfo *ir);
void I_LDA(Processor *cpu, InstrInfo *ir);
void I_LDX(Processor *cpu, InstrInfo *ir);
void I_LDY(Processor *cpu, InstrInfo *ir);
void I_LSR(Processor *cpu, InstrInfo *ir);
void I_NOP(Processor *cpu, InstrInfo *ir);
void I_ORA(Processor *cpu, InstrInfo *ir);
void I_PHA(Processor *cpu, InstrInfo *ir);
void I_PHP(Processor *cpu, InstrInfo *ir);
void I_PLA(Processor *cpu, InstrInfo *ir);
void I_PLP(Processor *cpu, InstrInfo *ir);
void I_ROL(Processor *cpu, InstrInfo *ir);
void I_ROR(Processor *cpu, InstrInfo *ir);
void I_RTI(Processor *cpu, InstrInfo *ir);
void I_RTS(Processor *cpu, InstrInfo *ir);
void I_SBC(Processor *cpu, InstrInfo *ir);
void I_SEC(Processor *cpu, InstrInfo *ir);
void I_SED(Processor *cpu, InstrInfo *ir);
void I_SEI(Processor *cpu, InstrInfo *ir);
void I_STA(Processor *cpu, InstrInfo *ir);
void I_STX(Processor *cpu, InstrInfo *ir);
void I_STY(Processor *cpu, InstrInfo *ir);
void I_TAX(Processor *cpu, InstrInfo *ir);
void I_TAY(Processor *cpu, InstrInfo *ir);
void I_TSX(Processor *cpu, InstrInfo *ir);
void I_TXA(Processor *cpu, InstrInfo *ir);
void I_TXS(Processor *cpu, InstrInfo *ir);
void I_TYA(Processor *cpu, InstrInfo *ir);

void U_DEF(Processor *cpu, InstrInfo *ir);    //UNDEFINED OPCODE, PLACEHOLDER FOR ALL ILLEGAL OPCODES

#endif
