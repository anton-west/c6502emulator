#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wreturn-type"

#include <stdio.h>

#include "opcodes.h"
#include "cpu.h"

uint16_t get_address(address_mode mode, Processor *cpu, Ir *ir) {
    switch (mode)
    {
    case IMM:
        uint16_t address = cpu->pc + 1;
        cpu->pc += 2;
        return address;
    
    /*  CANT DO THIS ONE
    case ACC:
        cpu->pc += 1;   //TODO: check if this addition should be here or is already handled during instruction fetch
    */

    case REL:
        {
            uint16_t address = cpu->pc;
            uint8_t offset = read(cpu, cpu->pc + 1);
            address += offset + 2;
            return (uint16_t) address;
        }

    case ZPG:
        {
            uint8_t address = read(cpu, cpu->pc + 1);
            cpu->pc += 2;
            return (uint16_t) (address & 0x00FF);
        }

    case ZPX:
        {
            uint16_t address = read(cpu, cpu->pc + 1);
            address = (address + cpu->x_reg) & 0x00FF;
            cpu->pc += 2;
            return address;
        }

    case ZPY:
        {
            uint16_t address = read(cpu, cpu->pc + 1);
            address = (address + cpu->y_reg) & 0x00FF;
            cpu->pc += 2;
            return address;
        }

    case ABS:
        {
            uint16_t addr_low = read(cpu, cpu->pc + 1);
            uint16_t addr_high = read(cpu, cpu->pc + 2);
            uint16_t address = addr_low | (addr_high << 8);
            cpu->pc += 3;
            return address;
        }

    case ABX:
        {
            uint16_t addr_low = read(cpu, cpu->pc + 1);
            uint16_t addr_high = read(cpu, cpu->pc + 2);
            uint16_t temp = addr_low | (addr_high << 8);
            uint16_t address = temp + cpu->x_reg;
            if (address & 0xFF00 != temp &0xFF00) {
                ir->cycles += 1;
            }
            cpu->pc += 3;
            return address;
        }

    case ABY:
        {
            uint16_t addr_low = read(cpu, cpu->pc + 1);
            uint16_t addr_high = read(cpu, cpu->pc + 2);
            uint16_t temp = addr_low | (addr_high << 8);
            uint16_t address = temp + cpu->y_reg;
            if (address & 0xFF00 != temp &0xFF00) {
                ir->cycles += 1;
            }
            cpu->pc += 3;
            return address;
        }
    
    case IND:
        {
            uint16_t abs_addr_low = read(cpu, cpu->pc + 1);
            uint16_t abs_addr_high = read(cpu, cpu->pc + 2);
            uint16_t abs_addr = abs_addr_low + (abs_addr_high << 8);

            uint16_t address, addr_low, addr_high;
            if ((abs_addr & 0x00FF) == 0x00FF) {
                addr_low = read(cpu, abs_addr);
                addr_high = read(cpu, abs_addr & 0xFF00);
                address = addr_low + (addr_high << 8);
                //TODO: add 1 to cycles
                ir->cycles += 1;
            } else {
                addr_low = read(cpu, abs_addr);
                addr_high = read(cpu, abs_addr + 1);
                address = addr_low + (addr_high << 8);
            }
            cpu->pc += 3;
            return address;
        }

    case IDX:
        {
            uint16_t address = read(cpu, cpu->pc + 1);
            address += (cpu->x_reg & 0x00FF);
            uint16_t addr_low = read(cpu, address);
            uint16_t addr_high = read(cpu, (address + 1) & 0x00FF);
            address = addr_low + (addr_high << 8);
            cpu->pc += 2;
            return address;
        }
    
    case IDY:
        {
            uint16_t address = read(cpu, cpu->pc + 1);
            uint16_t addr_low = read(cpu, address);
            uint16_t addr_high = read(cpu, (address + 1) & 0x00FF);
            uint16_t temp = addr_low + (addr_high << 8);
            uint16_t address = temp + cpu->y_reg;
            if (address & 0xFF00 != temp &0xFF00) {
                ir->cycles += 1;
            }
            cpu->pc += 2;
            return address;
        }

    default:
        break;
    }
}


int I_ADC(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_AND(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_ASL(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_BCC(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_BCS(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_BEQ(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_BIT(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_BMI(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_BNE(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_BPL(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_BRK(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_BVC(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_BVS(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_CLC(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_CLD(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_CLI(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_CLV(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_CMP(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_CPX(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_CPY(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_DEC(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_DEX(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_DEY(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_EOR(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_INC(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_INX(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_INY(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_JMP(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_JSR(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_LDA(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_LDX(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_LDY(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_LSR(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_NOP(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_ORA(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_PHA(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_PHP(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_PLA(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_PLP(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_ROL(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_ROR(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_RTI(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_RTS(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_SBC(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_SEC(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_SED(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_SEI(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_STA(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_STX(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_STY(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_TAX(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_TAY(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_TSX(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_TXA(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_TXS(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_TYA(uint8_t byte, Processor *cpu, Ir *ir) { ir; }

int U_DEF(uint8_t byte, Processor *cpu, Ir *ir) { ir; }

opcode opcode_matrix[] = {          I_BRK, I_ORA, U_DEF, U_DEF,     U_DEF, I_ORA, I_ASL, U_DEF,     I_PHP, I_ORA, I_ASL, U_DEF,     U_DEF, I_ORA, I_ASL, U_DEF,
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

//struct Instruction_info opcode_matrix[] = { (II){I_BRK, IMP, 1, 7}, (II){I_ORA, XIN, 2, 6}, (II){U_DEF, UDF, 0, 0}, (II){U_DEF, UDF, 0, 0}, (II){U_DEF, UDF, 0, 0}};

opcode decode_instruction(uint8_t byte) {
    return opcode_matrix[ byte ];
}
