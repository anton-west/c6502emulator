#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wreturn-type"

#include <stdio.h>

#include "opcodes.h"
#include "cpu.h"

uint16_t get_target_address(address_mode mode, Processor *cpu, Ir *ir) {
    switch (mode)
    {
    case IMM:
        {
            uint16_t trg_addr = cpu->pc + 1;
            cpu->pc += 2;
            return trg_addr;
        }
    
    case ACC:
        {
            cpu->pc += 1;   //TODO: check if this addition should be here or is already handled during instruction fetch
            return 0;   //return 0 but instead operate on cpu accumulator register outside this func
        }

    case REL:
        {
            uint8_t offset = read(cpu, cpu->pc + 1);
            uint16_t trg_addr = cpu->pc + offset + 2;
            return (uint16_t) trg_addr;
        }

    case ZPG:
        {
            uint8_t trg_addr = read(cpu, cpu->pc + 1);
            cpu->pc += 2;
            return (uint16_t) (trg_addr & 0x00FF);
        }

    case ZPX:
        {
            uint16_t trg_addr = read(cpu, cpu->pc + 1);
            trg_addr = (trg_addr + cpu->x_reg) & 0x00FF;
            cpu->pc += 2;
            return trg_addr;
        }

    case ZPY:
        {
            uint16_t trg_addr = read(cpu, cpu->pc + 1);
            trg_addr = (trg_addr + cpu->y_reg) & 0x00FF;
            cpu->pc += 2;
            return trg_addr;
        }

    case ABS:
        {
            uint16_t addr_low = read(cpu, cpu->pc + 1);
            uint16_t addr_high = read(cpu, cpu->pc + 2);
            uint16_t trg_addr = addr_low | (addr_high << 8);
            cpu->pc += 3;
            return trg_addr;
        }

    case ABX:
        {
            uint16_t addr_low = read(cpu, cpu->pc + 1);
            uint16_t addr_high = read(cpu, cpu->pc + 2);
            uint16_t temp = addr_low | (addr_high << 8);
            uint16_t trg_addr = temp + cpu->x_reg;
            if ((trg_addr & 0xFF00) != (temp & 0xFF00)) {
                ir->cycles += 1;    //extra cycle
            }
            cpu->pc += 3;
            return trg_addr;
        }

    case ABY:
        {
            uint16_t addr_low = read(cpu, cpu->pc + 1);
            uint16_t addr_high = read(cpu, cpu->pc + 2);
            uint16_t temp = addr_low | (addr_high << 8);
            uint16_t trg_addr = temp + cpu->y_reg;
            if ((trg_addr & 0xFF00) != (temp & 0xFF00)) {
                ir->cycles += 1;    //extra cycle
            }
            cpu->pc += 3;
            return trg_addr;
        }
    
    case IND:
        {
            uint16_t abs_addr_low = read(cpu, cpu->pc + 1);
            uint16_t abs_addr_high = read(cpu, cpu->pc + 2);
            uint16_t abs_addr = abs_addr_low + (abs_addr_high << 8);

            uint16_t addr_low = read(cpu, abs_addr);
            uint16_t addr_high = read(cpu, abs_addr + 1);
            uint16_t trg_addr = addr_low + (addr_high << 8);
            cpu->pc += 3;
            return trg_addr;
        }

    case IDX:
        {
            uint16_t addr = (read(cpu, cpu->pc + 1) + cpu->x_reg) & 0x00FF;
            uint16_t addr_low = read(cpu, addr);
            uint16_t addr_high = read(cpu, (addr + 1) & 0x00FF);    //stay on zero page
            uint16_t trg_addr = addr_low + (addr_high << 8);
            cpu->pc += 2;
            return trg_addr;
        }
    
    case IDY:
        {
            uint16_t addr = read(cpu, cpu->pc + 1);
            uint16_t addr_low = read(cpu, addr);
            uint16_t addr_high = read(cpu, (addr + 1) & 0x00FF);
            uint16_t temp = addr_low + (addr_high << 8);
            uint16_t trg_addr = temp + cpu->y_reg;
            if ((trg_addr & 0xFF00) != (temp & 0xFF00)) {
                ir->cycles += 1;    //extra cycle
            }
            cpu->pc += 2;
            return trg_addr;
        }

    default:
        return 0;
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
int I_LDA(uint8_t byte, Processor *cpu, Ir *ir) {
    uint16_t trg_addr;
    ir->opcode_mnemonic="LDA";  
    switch (byte)
    {
    case 0xA9:
        ir->addr_mode=IMM; ir->bytes=2; ir->cycles=2;
        trg_addr = get_target_address(IMM, cpu, ir);
        break;
    case 0xA5:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=3;
        trg_addr = get_target_address(ZPG, cpu, ir);
        break;
    case 0xB5:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=4;
        trg_addr = get_target_address(ZPX, cpu, ir);
        break;
    case 0xAD:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=4;
        trg_addr = get_target_address(ABS, cpu, ir);
        break;
    case 0xBD:
        ir->addr_mode=ABX; ir->bytes=3; ir->cycles=4;
        trg_addr = get_target_address(ABX, cpu, ir);
        break;
    case 0xB9:
        ir->addr_mode=ABY; ir->bytes=3; ir->cycles=4;
        trg_addr = get_target_address(ABY, cpu, ir);
        break;
    case 0xA1:
        ir->addr_mode=IDX; ir->bytes=2; ir->cycles=6;
        trg_addr = get_target_address(IDX, cpu, ir);
        break;
    case 0xB1:
        ir->addr_mode=IDY; ir->bytes=2; ir->cycles=5;
        trg_addr = get_target_address(IDY, cpu, ir);
        break;
    default:
        break;
    }
    uint8_t trg_byte = read(cpu, trg_addr);
    cpu->acc = trg_byte;
    setFlag('N', trg_byte & 0x80, cpu);
    setFlag('Z', trg_byte == 0, cpu);
 }
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
