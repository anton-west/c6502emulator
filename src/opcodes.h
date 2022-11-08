#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>

#include "cpu.h"

typedef int (*opcode)(uint8_t byte, Processor *cpu);

typedef enum address_mode {UDF, ACC, ABS, ABX, ABY, IMM, IMP, IND, XIN, INY, REL, ZEP, ZEX, ZEY} address_mode;

typedef struct InstructionResult {
    opcode fnc_ptr;
    char *opcode_mnemonic;
    address_mode addr_mode;
    unsigned int bytes;
    unsigned int cycles;
} Ir;

//opcode declarations
Ir* I_ADC(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_AND(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_ASL(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_BCC(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_BCS(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_BEQ(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_BIT(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_BMI(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_BNE(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_BPL(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_BRK(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_BVC(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_BVS(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_CLC(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_CLD(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_CLI(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_CLV(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_CMP(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_CPX(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_CPY(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_DEC(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_DEX(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_DEY(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_EOR(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_INC(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_INX(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_INY(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_JMP(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_JSR(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_LDA(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_LDX(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_LDY(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_LSR(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_NOP(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_ORA(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_PHA(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_PHP(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_PLA(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_PLP(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_ROL(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_ROR(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_RTI(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_RTS(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_SBC(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_SEC(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_SED(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_SEI(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_STA(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_STX(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_STY(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_TAX(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_TAY(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_TSX(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_TXA(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_TXS(uint8_t byte, Processor *cpu, Ir *ir);
Ir* I_TYA(uint8_t byte, Processor *cpu, Ir *ir);

Ir* U_DEF(uint8_t byte, Processor *cpu, Ir *ir);    //UNDEFINED OPCODE, PLACEHOLDER FOR ALL ILLEGAL OPCODES

opcode opcode_matrix[];

opcode decode_instruction(uint8_t byte);

#endif
