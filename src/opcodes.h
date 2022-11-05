#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>

#include "cpu.h"

typedef int (*opcode)(uint8_t byte, Processor *cpu);

typedef enum address_mode {ACC, ABS, ABX, ABY, IMM, IMP, IND, XIN, INY, REL, ZEP, ZEX, ZEY, UDF} address_mode;

typedef struct Instruction_info {
    opcode fnc_ptr;
    address_mode addr_mode;
    unsigned int bytes;
    unsigned int max_cycles;
} II;

//opcode declarations
int I_ADC(uint8_t byte, Processor *cpu);
int I_AND(uint8_t byte, Processor *cpu);
int I_ASL(uint8_t byte, Processor *cpu);
int I_BCC(uint8_t byte, Processor *cpu);
int I_BCS(uint8_t byte, Processor *cpu);
int I_BEQ(uint8_t byte, Processor *cpu);
int I_BIT(uint8_t byte, Processor *cpu);
int I_BMI(uint8_t byte, Processor *cpu);
int I_BNE(uint8_t byte, Processor *cpu);
int I_BPL(uint8_t byte, Processor *cpu);
int I_BRK(uint8_t byte, Processor *cpu);
int I_BVC(uint8_t byte, Processor *cpu);
int I_BVS(uint8_t byte, Processor *cpu);
int I_CLC(uint8_t byte, Processor *cpu);
int I_CLD(uint8_t byte, Processor *cpu);
int I_CLI(uint8_t byte, Processor *cpu);
int I_CLV(uint8_t byte, Processor *cpu);
int I_CMP(uint8_t byte, Processor *cpu);
int I_CPX(uint8_t byte, Processor *cpu);
int I_CPY(uint8_t byte, Processor *cpu);
int I_DEC(uint8_t byte, Processor *cpu);
int I_DEX(uint8_t byte, Processor *cpu);
int I_DEY(uint8_t byte, Processor *cpu);
int I_EOR(uint8_t byte, Processor *cpu);
int I_INC(uint8_t byte, Processor *cpu);
int I_INX(uint8_t byte, Processor *cpu);
int I_INY(uint8_t byte, Processor *cpu);
int I_JMP(uint8_t byte, Processor *cpu);
int I_JSR(uint8_t byte, Processor *cpu);
int I_LDA(uint8_t byte, Processor *cpu);
int I_LDX(uint8_t byte, Processor *cpu);
int I_LDY(uint8_t byte, Processor *cpu);
int I_LSR(uint8_t byte, Processor *cpu);
int I_NOP(uint8_t byte, Processor *cpu);
int I_ORA(uint8_t byte, Processor *cpu);
int I_PHA(uint8_t byte, Processor *cpu);
int I_PHP(uint8_t byte, Processor *cpu);
int I_PLA(uint8_t byte, Processor *cpu);
int I_PLP(uint8_t byte, Processor *cpu);
int I_ROL(uint8_t byte, Processor *cpu);
int I_ROR(uint8_t byte, Processor *cpu);
int I_RTI(uint8_t byte, Processor *cpu);
int I_RTS(uint8_t byte, Processor *cpu);
int I_SBC(uint8_t byte, Processor *cpu);
int I_SEC(uint8_t byte, Processor *cpu);
int I_SED(uint8_t byte, Processor *cpu);
int I_SEI(uint8_t byte, Processor *cpu);
int I_STA(uint8_t byte, Processor *cpu);
int I_STX(uint8_t byte, Processor *cpu);
int I_STY(uint8_t byte, Processor *cpu);
int I_TAX(uint8_t byte, Processor *cpu);
int I_TAY(uint8_t byte, Processor *cpu);
int I_TSX(uint8_t byte, Processor *cpu);
int I_TXA(uint8_t byte, Processor *cpu);
int I_TXS(uint8_t byte, Processor *cpu);
int I_TYA(uint8_t byte, Processor *cpu);

int U_DEF(uint8_t byte, Processor *cpu);    //UNDEFINED OPCODE, PLACEHOLDER FOR ALL ILLEGAL OPCODES

opcode opcode_matrix[];

opcode decode_instruction(uint8_t byte);

#endif
