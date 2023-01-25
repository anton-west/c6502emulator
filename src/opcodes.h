#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>

#include "cpu.h"

typedef enum address_mode {UDF, ACC, ABS, ABX, ABY, IMM, IMP, IND, IDX, IDY, REL, ZPG, ZPX, ZPY} address_mode;

typedef struct InstructionResult {
    char *opcode_mnemonic;
    address_mode addr_mode;
    unsigned int bytes;
    unsigned int cycles;
} Ir;

typedef int (*opcode)(uint8_t byte, Processor *cpu, Ir *ir);

//get address of operand
void fetch_target_value(address_mode mode, Processor *cpu, Ir *ir);

//opcode declarations
int I_ADC(uint8_t byte, Processor *cpu, Ir *ir);
int I_AND(uint8_t byte, Processor *cpu, Ir *ir);
int I_ASL(uint8_t byte, Processor *cpu, Ir *ir);
int I_BCC(uint8_t byte, Processor *cpu, Ir *ir);
int I_BCS(uint8_t byte, Processor *cpu, Ir *ir);
int I_BEQ(uint8_t byte, Processor *cpu, Ir *ir);
int I_BIT(uint8_t byte, Processor *cpu, Ir *ir);
int I_BMI(uint8_t byte, Processor *cpu, Ir *ir);
int I_BNE(uint8_t byte, Processor *cpu, Ir *ir);
int I_BPL(uint8_t byte, Processor *cpu, Ir *ir);
int I_BRK(uint8_t byte, Processor *cpu, Ir *ir);
int I_BVC(uint8_t byte, Processor *cpu, Ir *ir);
int I_BVS(uint8_t byte, Processor *cpu, Ir *ir);
int I_CLC(uint8_t byte, Processor *cpu, Ir *ir);
int I_CLD(uint8_t byte, Processor *cpu, Ir *ir);
int I_CLI(uint8_t byte, Processor *cpu, Ir *ir);
int I_CLV(uint8_t byte, Processor *cpu, Ir *ir);
int I_CMP(uint8_t byte, Processor *cpu, Ir *ir);
int I_CPX(uint8_t byte, Processor *cpu, Ir *ir);
int I_CPY(uint8_t byte, Processor *cpu, Ir *ir);
int I_DEC(uint8_t byte, Processor *cpu, Ir *ir);
int I_DEX(uint8_t byte, Processor *cpu, Ir *ir);
int I_DEY(uint8_t byte, Processor *cpu, Ir *ir);
int I_EOR(uint8_t byte, Processor *cpu, Ir *ir);
int I_INC(uint8_t byte, Processor *cpu, Ir *ir);
int I_INX(uint8_t byte, Processor *cpu, Ir *ir);
int I_INY(uint8_t byte, Processor *cpu, Ir *ir);
int I_JMP(uint8_t byte, Processor *cpu, Ir *ir);
int I_JSR(uint8_t byte, Processor *cpu, Ir *ir);
int I_LDA(uint8_t byte, Processor *cpu, Ir *ir);
int I_LDX(uint8_t byte, Processor *cpu, Ir *ir);
int I_LDY(uint8_t byte, Processor *cpu, Ir *ir);
int I_LSR(uint8_t byte, Processor *cpu, Ir *ir);
int I_NOP(uint8_t byte, Processor *cpu, Ir *ir);
int I_ORA(uint8_t byte, Processor *cpu, Ir *ir);
int I_PHA(uint8_t byte, Processor *cpu, Ir *ir);
int I_PHP(uint8_t byte, Processor *cpu, Ir *ir);
int I_PLA(uint8_t byte, Processor *cpu, Ir *ir);
int I_PLP(uint8_t byte, Processor *cpu, Ir *ir);
int I_ROL(uint8_t byte, Processor *cpu, Ir *ir);
int I_ROR(uint8_t byte, Processor *cpu, Ir *ir);
int I_RTI(uint8_t byte, Processor *cpu, Ir *ir);
int I_RTS(uint8_t byte, Processor *cpu, Ir *ir);
int I_SBC(uint8_t byte, Processor *cpu, Ir *ir);
int I_SEC(uint8_t byte, Processor *cpu, Ir *ir);
int I_SED(uint8_t byte, Processor *cpu, Ir *ir);
int I_SEI(uint8_t byte, Processor *cpu, Ir *ir);
int I_STA(uint8_t byte, Processor *cpu, Ir *ir);
int I_STX(uint8_t byte, Processor *cpu, Ir *ir);
int I_STY(uint8_t byte, Processor *cpu, Ir *ir);
int I_TAX(uint8_t byte, Processor *cpu, Ir *ir);
int I_TAY(uint8_t byte, Processor *cpu, Ir *ir);
int I_TSX(uint8_t byte, Processor *cpu, Ir *ir);
int I_TXA(uint8_t byte, Processor *cpu, Ir *ir);
int I_TXS(uint8_t byte, Processor *cpu, Ir *ir);
int I_TYA(uint8_t byte, Processor *cpu, Ir *ir);

int U_DEF(uint8_t byte, Processor *cpu, Ir *ir);    //UNDEFINED OPCODE, PLACEHOLDER FOR ALL ILLEGAL OPCODES

extern opcode opcode_matrix[];

opcode decode_instruction(uint8_t byte);

#endif
