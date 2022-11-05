#include <stdio.h>

#include "opcodes.h"
#include "cpu.h"


int I_ADC(uint8_t byte, Processor *cpu) { 
    switch (byte)
    {
    case 0x69: printf("ADC\tIMM\n"); break;
    case 0x65: printf("ADC\tZP \n"); break;
    case 0x75: printf("ADC\tZP, X\n"); break;
    case 0x6D: printf("ADC\tABS\n"); break;
    case 0x7D: printf("ADC\tABS, X\n"); break;
    case 0x79: printf("ADC\tABS, Y\n"); break;
    case 0x61: printf("ADC\t(IND,X)\n"); break;
    case 0x71: printf("ADC\t(IND),Y\n"); break;
    default:
        break;
    }
    return 0;
};

int I_AND(uint8_t byte, Processor *cpu) { return 0; }
int I_ASL(uint8_t byte, Processor *cpu) { return 0; }
int I_BCC(uint8_t byte, Processor *cpu) { return 0; }
int I_BCS(uint8_t byte, Processor *cpu) { return 0; }
int I_BEQ(uint8_t byte, Processor *cpu) { return 0; }
int I_BIT(uint8_t byte, Processor *cpu) { return 0; }
int I_BMI(uint8_t byte, Processor *cpu) { return 0; }
int I_BNE(uint8_t byte, Processor *cpu) { return 0; }
int I_BPL(uint8_t byte, Processor *cpu) { return 0; }
int I_BRK(uint8_t byte, Processor *cpu) { return 0; }
int I_BVC(uint8_t byte, Processor *cpu) { return 0; }
int I_BVS(uint8_t byte, Processor *cpu) { return 0; }
int I_CLC(uint8_t byte, Processor *cpu) { return 0; }
int I_CLD(uint8_t byte, Processor *cpu) { return 0; }
int I_CLI(uint8_t byte, Processor *cpu) { return 0; }
int I_CLV(uint8_t byte, Processor *cpu) { return 0; }
int I_CMP(uint8_t byte, Processor *cpu) { return 0; }
int I_CPX(uint8_t byte, Processor *cpu) { return 0; }
int I_CPY(uint8_t byte, Processor *cpu) { return 0; }
int I_DEC(uint8_t byte, Processor *cpu) { return 0; }
int I_DEX(uint8_t byte, Processor *cpu) { return 0; }
int I_DEY(uint8_t byte, Processor *cpu) { return 0; }
int I_EOR(uint8_t byte, Processor *cpu) { return 0; }
int I_INC(uint8_t byte, Processor *cpu) { return 0; }
int I_INX(uint8_t byte, Processor *cpu) { return 0; }
int I_INY(uint8_t byte, Processor *cpu) { return 0; }
int I_JMP(uint8_t byte, Processor *cpu) { return 0; }
int I_JSR(uint8_t byte, Processor *cpu) { return 0; }
int I_LDA(uint8_t byte, Processor *cpu) { return 0; }
int I_LDX(uint8_t byte, Processor *cpu) { return 0; }
int I_LDY(uint8_t byte, Processor *cpu) { return 0; }
int I_LSR(uint8_t byte, Processor *cpu) { return 0; }
int I_NOP(uint8_t byte, Processor *cpu) { return 0; }
int I_ORA(uint8_t byte, Processor *cpu) { return 0; }
int I_PHA(uint8_t byte, Processor *cpu) { return 0; }
int I_PHP(uint8_t byte, Processor *cpu) { return 0; }
int I_PLA(uint8_t byte, Processor *cpu) { return 0; }
int I_PLP(uint8_t byte, Processor *cpu) { return 0; }
int I_ROL(uint8_t byte, Processor *cpu) { return 0; }
int I_ROR(uint8_t byte, Processor *cpu) { return 0; }
int I_RTI(uint8_t byte, Processor *cpu) { return 0; }
int I_RTS(uint8_t byte, Processor *cpu) { return 0; }
int I_SBC(uint8_t byte, Processor *cpu) { return 0; }
int I_SEC(uint8_t byte, Processor *cpu) { return 0; }
int I_SED(uint8_t byte, Processor *cpu) { return 0; }
int I_SEI(uint8_t byte, Processor *cpu) { return 0; }
int I_STA(uint8_t byte, Processor *cpu) { return 0; }
int I_STX(uint8_t byte, Processor *cpu) { return 0; }
int I_STY(uint8_t byte, Processor *cpu) { return 0; }
int I_TAX(uint8_t byte, Processor *cpu) { return 0; }
int I_TAY(uint8_t byte, Processor *cpu) { return 0; }
int I_TSX(uint8_t byte, Processor *cpu) { return 0; }
int I_TXA(uint8_t byte, Processor *cpu) { return 0; }
int I_TXS(uint8_t byte, Processor *cpu) { return 0; }
int I_TYA(uint8_t byte, Processor *cpu) { return 0; }

int U_DEF(uint8_t byte, Processor *cpu) { return -1; }

opcode opcode_matrix[] = {  I_BRK, I_ORA, U_DEF, U_DEF,     U_DEF, I_ORA, I_ASL, U_DEF,     I_PHP, I_ORA, I_ASL, U_DEF,     U_DEF, I_ORA, I_ASL, U_DEF,
                            I_BPL, I_ORA, U_DEF, U_DEF,     U_DEF, I_ORA, I_ASL, U_DEF,     I_CLC, I_ORA, U_DEF, U_DEF,     U_DEF, I_ORA, I_ASL, U_DEF,
                            I_JSR, I_AND, U_DEF, U_DEF,     I_BIT, I_AND, I_ROL, U_DEF,     I_PLP, I_AND, I_ROL, U_DEF,     I_BIT, I_AND, I_ROL, U_DEF,
                            I_BMI, I_AND, U_DEF, U_DEF,     U_DEF, I_AND, I_ROL, U_DEF,     I_SEC, I_AND, U_DEF, U_DEF,     U_DEF, I_AND, I_ROL, U_DEF,
                            
                            I_RTI, I_EOR, U_DEF, U_DEF,     U_DEF, I_EOR, I_LSR, U_DEF,     I_PHA, I_EOR, I_LSR, U_DEF,     I_JMP, I_EOR, I_LSR, U_DEF,
                            I_BVC, I_EOR, U_DEF, U_DEF,     U_DEF, I_EOR, I_LSR, U_DEF,     I_CLI, I_EOR, U_DEF, U_DEF,     U_DEF, I_EOR, I_LSR, U_DEF,
                            I_RTS, I_ADC, U_DEF, U_DEF,     U_DEF, I_ADC, I_ROR, U_DEF,     I_PLA, I_ADC, I_ROR, U_DEF,     I_JMP, I_ADC, I_ROR, U_DEF,
                            I_BVS, I_ADC, U_DEF, U_DEF,     U_DEF, I_ADC, I_ROR, U_DEF,     I_SEI, I_ADC, U_DEF, U_DEF,     U_DEF, I_ADC, I_ROR, U_DEF,
                            
                            U_DEF, I_STA, U_DEF, U_DEF,     I_STY, I_STA, I_STX, U_DEF,     I_DEY, U_DEF, I_TXA, U_DEF,     I_STY, I_STA, I_STX, U_DEF,
                            I_BCC, I_STA, U_DEF, U_DEF,     I_STY, I_STA, I_STX, U_DEF,     I_TYA, I_STA, I_TXS, U_DEF,     U_DEF, I_STA, U_DEF, U_DEF,
                            I_LDY, I_LDA, I_LDX, U_DEF,     I_LDY, I_LDA, I_LDX, U_DEF,     I_TAY, I_LDA, I_TAX, U_DEF,     I_LDY, I_LDA, I_LDX, U_DEF,
                            I_BCS, I_LDA, U_DEF, U_DEF,     I_LDY, I_LDA, I_LDX, U_DEF,     I_CLV, I_LDA, I_TSX, U_DEF,     I_LDY, I_LDA, I_LDX, U_DEF,
                            
                            I_CPY, I_CMP, U_DEF, U_DEF,     I_CPY, I_CMP, I_DEC, U_DEF,     I_INY, I_CMP, I_DEX, U_DEF,     I_CPY, I_CMP, I_DEC, U_DEF,
                            I_BNE, I_CMP, U_DEF, U_DEF,     U_DEF, I_CMP, I_DEC, U_DEF,     I_CLD, I_CMP, U_DEF, U_DEF,     U_DEF, I_CMP, I_DEC, U_DEF,
                            I_CPX, I_SBC, U_DEF, U_DEF,     I_CPX, I_SBC, I_INC, U_DEF,     I_INX, I_SBC, I_NOP, U_DEF,     I_CPX, I_SBC, I_INC, U_DEF,
                            I_BEQ, I_SBC, U_DEF, U_DEF,     U_DEF, I_SBC, I_INC, U_DEF,     I_SED, I_SBC, U_DEF, U_DEF,     U_DEF, I_SBC, I_INC, U_DEF  };

opcode decode(uint8_t byte) {
    return opcode_matrix[ byte ];
}
