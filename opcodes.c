#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wreturn-type"

#include <stdio.h>

#include "opcodes.h"
#include "cpu.h"


int I_ADC(uint8_t byte, Processor *cpu, Ir *ir) { 
    switch (byte)
    {
    case 0x69:
        printf("ADC\tIMM\n");
        //set instruction result struct
        ir->addr_mode = IMM; ir->bytes = 2; ir->cycles = 2;
        
        uint16_t operand_byte = fetch(cpu);
        uint16_t temp_res = (uint16_t)cpu->acc + (uint16_t)operand_byte + (uint16_t)getFlag('C', cpu);

        setFlag('C', temp_res > 255, cpu);
        setFlag('Z', (temp_res & 0x00FF) == 0, cpu);
        setFlag('N', temp_res & 0x0080, cpu);
        int overflow_flag = ((cpu->acc^temp_res) & !(cpu->acc^operand_byte)) & 0x0080;
        setFlag('V', overflow_flag, cpu);

        cpu->acc = temp_res & 0x00FF;
        break;
        
    case 0x65: printf("ADC\tZEP \n"); break;
    case 0x75: printf("ADC\tZEX, X\n"); break;
    case 0x6D: printf("ADC\tABS\n"); break;
    case 0x7D: printf("ADC\tABS, X\n"); break;
    case 0x79: printf("ADC\tABS, Y\n"); break;
    case 0x61: printf("ADC\t(IND,X)\n"); break;
    case 0x71: printf("ADC\t(IND),Y\n"); break;
    default:
        return -1;
    }
    return 0;
}

int I_AND(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_ASL(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_BCC(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_BCS(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_BEQ(uint8_t byte, Processor *cpu, Ir *ir) { ir; }
int I_BIT(uint8_t byte, Processor *cpu, Ir *ir) {
    switch (byte)
    {
    case 0x24:
        {
            printf("BIT\tZEP\n");
            //set instruction result struct
            ir->addr_mode = ZEP; ir->bytes = 2; ir->cycles = 3;
            uint8_t zptr_byte = fetch(cpu);
            uint8_t operand_byte = read(cpu, (uint16_t) zptr_byte);
            
            uint8_t temp_res = cpu->acc & operand_byte;
            cpu->acc = temp_res;
            
            setFlag('N', operand_byte & 0x80, cpu);
            setFlag('V', operand_byte & 0x40, cpu);
            setFlag('Z', temp_res == 0, cpu);
        }
        break;
    
    case 0x2C:
        {
            printf("BIT\tABS\n");
            //set instruction result struct
            ir->addr_mode = ABS; ir->bytes = 3; ir->cycles = 4;
            uint16_t low_byte = fetch(cpu);
            uint16_t high_byte = fetch(cpu);
            uint8_t operand_byte = read(cpu, (high_byte)<<8 | low_byte);
            
            uint8_t temp_res = cpu->acc & operand_byte;
            cpu->acc = temp_res;
            
            setFlag('N', operand_byte & 0x80, cpu);
            setFlag('V', operand_byte & 0x40, cpu);
            setFlag('Z', temp_res == 0, cpu);
        }
        break;
    default:
        return -1;
    }
    return 0;
}
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
int I_LDX(uint8_t byte, Processor *cpu, Ir *ir) {
    switch (byte)
    {
    case 0xA2:
        {
        ir->opcode_mnemonic="LDX";ir->addr_mode=IMM;ir->bytes=2;ir->cycles=2;
        uint8_t fetched_byte = fetch(cpu);
        cpu->x_reg = fetched_byte;
        setFlag('N', fetched_byte & 0x80, cpu);
        setFlag('Z', fetched_byte == 0, cpu);
        }
        break;
    case 0xA6:
        {
        ir->opcode_mnemonic="LDX";ir->addr_mode=ZEP;ir->bytes=2;ir->cycles=3;
        uint16_t zp_addr = fetch(cpu);
        uint8_t fetched_byte = read(cpu, zp_addr & 0x00FF);
        cpu->x_reg = fetched_byte;
        setFlag('N', fetched_byte & 0x80, cpu);
        setFlag('Z', fetched_byte == 0, cpu);
        }
        break;
    case 0xB6:
        {
        ir->opcode_mnemonic="LDX";ir->addr_mode=ZEY;ir->bytes=2;ir->cycles=4;
        uint16_t zp_addr = fetch(cpu);
        uint8_t fetched_byte = read(cpu, (zp_addr + cpu->y_reg) & 0x00FF);
        cpu->x_reg = fetched_byte;
        setFlag('N', fetched_byte & 0x80, cpu);
        setFlag('Z', fetched_byte == 0, cpu);
        }
        break;
    case 0xAE:
        {
        ir->opcode_mnemonic="LDX";ir->addr_mode=ABS;ir->bytes=3;ir->cycles=4;
        uint16_t l_byte = fetch(cpu);
        uint16_t h_byte = fetch(cpu);
        uint8_t fetched_byte = read(cpu, (h_byte << 8) | l_byte);
        cpu->x_reg = fetched_byte;
        setFlag('N', fetched_byte & 0x80, cpu);
        setFlag('Z', fetched_byte == 0, cpu);
        }
        break;
    case 0xBE:
        {
        ir->opcode_mnemonic="LDX";ir->addr_mode=ABY;ir->bytes=3;ir->cycles=4;
        uint16_t l_byte = fetch(cpu);
        uint16_t h_byte = fetch(cpu);
        uint16_t addr = ((h_byte << 8) | l_byte) + (uint16_t)cpu->y_reg;
        if ((addr & 0xFF00) != ((h_byte << 8) & 0xFF00)) {ir->cycles+=1;}
        uint8_t fetched_byte = read(cpu, addr);
        cpu->x_reg = fetched_byte;
        setFlag('N', fetched_byte & 0x80, cpu);
        setFlag('Z', fetched_byte == 0, cpu);
        }
        break;
    default:
        return -1;
    }
    return 0;
}
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
