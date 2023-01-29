//uncomment to supress messages
//#pragma GCC diagnostic ignored "-Wunused-parameter"
//#pragma GCC diagnostic ignored "-Wunused-value"
//#pragma GCC diagnostic ignored "-Wreturn-type"

#include <stdio.h>
#include <string.h>

#include "opcodes.h"
#include "cpu.h"

/* Sets cpu->abs_addr and cpu->fetched_value to correct values
*/

/*
void decode_instructions(Ir **ir_arr, size_t n_ir, uint8_t* mem) {
    for (int i = 0; i < n_ir; i++) {
        uint8_t instruction = *(mem + i);
        opcode ptr = decode_instruction(instruction);

        (ptr)(instruction, cpu, &ir);
    }
}
*/










void fetch_target_value(Processor *cpu, InstrInfo *ir) {
    switch (ir->addr_mode)
    {
    case IMP:
        {
            cpu->pc += 1;
            break;
        }
    case IMM:
        {
            cpu->abs_addr = cpu->pc + 1;
            cpu->fetched_value = cpu_read(cpu, cpu->abs_addr);
            cpu->pc += 2;
            break;
        }
    
    case ACC:
        {
            cpu->pc += 1;
            cpu->abs_addr = 0;              //instruction will act on accumulator, abs_addr will not be touched, set to 0 to signal this
            cpu->fetched_value = cpu->acc;
            break;
        }

    case REL:
        {
            uint16_t offset = cpu_read(cpu, cpu->pc + 1);
            if (offset & 0x80) {
		        offset |= 0xFF00;
            }
            uint16_t trg_addr = cpu->pc + offset + 2;
            if((trg_addr & 0xFF00) != (cpu->pc & 0xFF00)) {
                ir->n_cycles++;
            }
            cpu->abs_addr = trg_addr;
            cpu->fetched_value = cpu_read(cpu, cpu->abs_addr);
            break;
        }

    case ZPG:
        {
            uint8_t trg_addr = cpu_read(cpu, cpu->pc + 1);
            cpu->pc += 2;
            cpu->abs_addr = (uint16_t) (trg_addr & 0x00FF);
            cpu->fetched_value = cpu_read(cpu, cpu->abs_addr);
            break;
        }

    case ZPX:
        {
            uint16_t trg_addr = cpu_read(cpu, cpu->pc + 1);
            trg_addr = (trg_addr + cpu->x_reg) & 0x00FF;
            cpu->pc += 2;
            cpu->abs_addr = trg_addr;
            cpu->fetched_value = cpu_read(cpu, cpu->abs_addr);
            break;
        }

    case ZPY:
        {
            uint16_t trg_addr = cpu_read(cpu, cpu->pc + 1);
            trg_addr = (trg_addr + cpu->y_reg) & 0x00FF;
            cpu->pc += 2;
            cpu->abs_addr =  trg_addr;
            cpu->fetched_value = cpu_read(cpu, cpu->abs_addr);
            break;
        }

    case ABS:
        {
            uint16_t addr_low = cpu_read(cpu, cpu->pc + 1);
            uint16_t addr_high = cpu_read(cpu, cpu->pc + 2);
            uint16_t trg_addr = addr_low | (addr_high << 8);
            cpu->pc += 3;
            cpu->abs_addr = trg_addr;
            cpu->fetched_value = cpu_read(cpu, cpu->abs_addr);
            break;
        }

    case ABX:
        {
            uint16_t addr_low = cpu_read(cpu, cpu->pc + 1);
            uint16_t addr_high = cpu_read(cpu, cpu->pc + 2);
            uint16_t temp = addr_low | (addr_high << 8);
            uint16_t trg_addr = temp + cpu->x_reg;
            if ((trg_addr & 0xFF00) != (temp & 0xFF00)) {
                ir->n_cycles += 1;    //extra cycle
            }
            cpu->pc += 3;
            cpu->abs_addr =  trg_addr;
            cpu->fetched_value = cpu_read(cpu, cpu->abs_addr);
            break;
        }

    case ABY:
        {
            uint16_t addr_low = cpu_read(cpu, cpu->pc + 1);
            uint16_t addr_high = cpu_read(cpu, cpu->pc + 2);
            uint16_t temp = addr_low | (addr_high << 8);
            uint16_t trg_addr = temp + cpu->y_reg;
            if ((trg_addr & 0xFF00) != (temp & 0xFF00)) {
                ir->n_cycles += 1;    //extra cycle
            }
            cpu->pc += 3;
            cpu->abs_addr =  trg_addr;
            cpu->fetched_value = cpu_read(cpu, cpu->abs_addr);
            break;
        }
    
    case IND:
        {
            uint16_t abs_addr_low = cpu_read(cpu, cpu->pc + 1);
            uint16_t abs_addr_high = cpu_read(cpu, cpu->pc + 2);
            uint16_t abs_addr = abs_addr_low + (abs_addr_high << 8);

            uint16_t addr_low = cpu_read(cpu, abs_addr);
            uint16_t addr_high = cpu_read(cpu, abs_addr + 1);
            uint16_t trg_addr = addr_low + (addr_high << 8);
            cpu->pc += 3;
            cpu->abs_addr =  trg_addr;
            cpu->fetched_value = cpu_read(cpu, cpu->abs_addr);
            break;
        }

    case IDX:
        {
            uint16_t addr = (cpu_read(cpu, cpu->pc + 1) + cpu->x_reg) & 0x00FF;
            uint16_t addr_low = cpu_read(cpu, addr);
            uint16_t addr_high = cpu_read(cpu, (addr + 1) & 0x00FF);    //stay on zero page
            uint16_t trg_addr = addr_low + (addr_high << 8);
            cpu->pc += 2;
            cpu->abs_addr = trg_addr;
            cpu->fetched_value = cpu_read(cpu, cpu->abs_addr);
            break;
        }
    
    case IDY:
        {
            uint16_t addr = cpu_read(cpu, cpu->pc + 1);
            uint16_t addr_low = cpu_read(cpu, addr);
            uint16_t addr_high = cpu_read(cpu, (addr + 1) & 0x00FF);
            uint16_t temp = addr_low + (addr_high << 8);
            uint16_t trg_addr = temp + cpu->y_reg;
            if ((trg_addr & 0xFF00) != (temp & 0xFF00)) {
                ir->n_cycles += 1;    //extra cycle
            }
            cpu->pc += 2;
            cpu->abs_addr = trg_addr;
            cpu->fetched_value = cpu_read(cpu, cpu->abs_addr);
            break;
        }

    default:
        break;
    }
}


void I_ADC(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "ADC");
   
    fetch_target_value(cpu, ir);
    uint16_t result = (uint16_t) cpu->acc + (uint16_t) cpu->fetched_value + (uint16_t) getFlag('C', cpu);
    
    setFlag('C', result > 255, cpu);
    setFlag('Z', (result & 0x00FF) == 0, cpu);
    setFlag('N', result & 0x80, cpu);
    setFlag('V', ( ( (uint16_t)cpu->acc ^ (uint16_t)result ) & ~( (uint16_t)cpu->acc ^ (uint16_t)cpu->fetched_value ) ) & 0x0080, cpu);
    cpu->acc = (uint8_t) (result & 0x00FF);
}
void I_AND(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "AND");
    
    fetch_target_value(cpu, ir);
    uint8_t and_comparison = cpu->fetched_value & cpu->acc;
    cpu->acc = and_comparison;
    setFlag('N', and_comparison & 0x80, cpu);
    setFlag('Z', and_comparison == 0, cpu);
}
void I_ASL(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "ASL");  

    fetch_target_value(cpu, ir);
    uint8_t carry_flag_value = cpu->fetched_value & 0x80; //carry flag value is the bit that is getting shifted out
    uint8_t new_value = cpu->fetched_value << 1;if
    
    (ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   //TODO: fix the abs_addr writing somehow

    setFlag('N', new_value & 0x80, cpu);
    setFlag('Z', new_value == 0, cpu);
    setFlag('C', carry_flag_value, cpu);
}
void I_BCC(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BCC");
    
    fetch_target_value(cpu, ir);
    if (getFlag('C', cpu) == 0) {
        cpu->pc = cpu->abs_addr;
    }
}
void I_BCS(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BCS");
    fetch_target_value(cpu, ir);
    if (getFlag('C', cpu) == 1) {
        cpu->pc = cpu->abs_addr;
    }
}
void I_BEQ(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BEQ");
    fetch_target_value(cpu, ir);
    if (getFlag('Z', cpu) == 1) {
        cpu->pc = cpu->abs_addr;
    }
}
void I_BIT(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BIT");
    
    fetch_target_value(cpu, ir);

    uint8_t result = cpu->acc & cpu->fetched_value;
    setFlag('N', cpu->fetched_value & 0x80, cpu);
    setFlag('V', cpu->fetched_value & 0x40, cpu);
    setFlag('Z', result == 0, cpu);
}
void I_BMI(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BMI");
    fetch_target_value(cpu, ir);
    if (getFlag('N', cpu) == 1) {
        cpu->pc = cpu->abs_addr;
    }
}
void I_BNE(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BNE");
    fetch_target_value(cpu, ir);
    if (getFlag('Z', cpu) == 0) {
        cpu->pc = cpu->abs_addr;
    }
}
void I_BPL(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BPL");

    fetch_target_value(cpu, ir);
    if (getFlag('N', cpu) == 0) {
        cpu->pc = cpu->abs_addr;
    }
}
void I_BRK(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BRK");

    setFlag('I', 1, cpu);
    cpu_write(cpu, cpu->sp, cpu->pc+2);
    cpu->sp--;
    cpu_write(cpu, cpu->sp, cpu->status_reg);
    cpu->sp--;
}
void I_BVC(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BVC");
    fetch_target_value(cpu, ir);
    if (getFlag('V', cpu) == 0) {
        cpu->pc = cpu->abs_addr;
    }
}
void I_BVS(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BVC");
    fetch_target_value(cpu, ir);
    if (getFlag('V', cpu) == 01) {
        cpu->pc = cpu->abs_addr;
    }
}
void I_CLC(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "CLC");
    cpu->pc += 1;
    setFlag('C', 0, cpu);   //clear carry
}
void I_CLD(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "CLD");
    setFlag('D', 0, cpu);   //clear decimal
}
void I_CLI(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "CLI");
    setFlag('I', 0, cpu);   //clear interrupt disable
}
void I_CLV(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "CLV");
    setFlag('V', 0, cpu);   //clear overflow
}
void I_CMP(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "CMP");
    fetch_target_value(cpu, ir);
    uint8_t cmp_result = cpu->acc - cpu->fetched_value;

    setFlag('N', cmp_result & 0x80, cpu);
    setFlag('Z', (cpu->acc == cpu->fetched_value) ? 1 : 0, cpu);
    setFlag('C', (cpu->acc >= cpu->fetched_value) ? 1 : 0, cpu);
}
void I_CPX(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "CPX");
    fetch_target_value(cpu, ir);
    uint8_t cmp_result = cpu->x_reg - cpu->fetched_value;
    setFlag('N', cmp_result & 0x80, cpu);
    setFlag('Z', (cpu->x_reg == cpu->fetched_value) ? 1 : 0, cpu);
    setFlag('C', (cpu->x_reg >= cpu->fetched_value) ? 1 : 0, cpu);
}
void I_CPY(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "CPY");
    fetch_target_value(cpu, ir);
    uint8_t cmp_result = cpu->y_reg - cpu->fetched_value;
    setFlag('N', cmp_result & 0x80, cpu);
    setFlag('Z', (cpu->y_reg == cpu->fetched_value) ? 1 : 0, cpu);
    setFlag('C', (cpu->y_reg >= cpu->fetched_value) ? 1 : 0, cpu);
}
void I_DEC(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "DEC");

    fetch_target_value(cpu, ir);
    uint8_t new_value = cpu->fetched_value - 1;
    cpu_write(cpu, cpu->abs_addr, new_value);
    setFlag('N', new_value & 0x80, cpu);
    setFlag('Z', new_value == 0, cpu);
}
void I_DEX(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "DEX");
    cpu->x_reg--;
    setFlag('N', cpu->x_reg & 0x80, cpu);
    setFlag('Z', cpu->x_reg == 0, cpu);
}
void I_DEY(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "DEY");
    cpu->y_reg--;
    setFlag('N', cpu->y_reg & 0x80, cpu);
    setFlag('Z', cpu->y_reg == 0, cpu);
}
void I_EOR(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "EOR");
    fetch_target_value(cpu, ir);
    uint8_t and_comparison = cpu->fetched_value ^ cpu->acc;
    cpu->acc = and_comparison;
    setFlag('N', and_comparison & 0x80, cpu);
    setFlag('Z', and_comparison == 0, cpu);
}
void I_INC(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "INC");
    fetch_target_value(cpu, ir);
    uint8_t new_value = cpu->fetched_value + 1;
    cpu_write(cpu, cpu->abs_addr, new_value);
    setFlag('N', new_value & 0x80, cpu);
    setFlag('Z', new_value == 0, cpu);
}
void I_INX(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "INX");
    cpu->x_reg++;
    setFlag('N', cpu->x_reg & 0x80, cpu);
    setFlag('Z', cpu->x_reg == 0, cpu);
}
void I_INY(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "INY");
    cpu->y_reg++;
    setFlag('N', cpu->y_reg & 0x80, cpu);
    setFlag('Z', cpu->y_reg == 0, cpu);
}
void I_JMP(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "JMP");
    fetch_target_value(cpu, ir);
    cpu->pc=cpu->abs_addr;
}
void I_JSR(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "JSR");
    cpu_write(cpu, cpu->sp, (cpu->pc >> 8) & 0x00FF );
    cpu->sp--;
    cpu_write(cpu, cpu->sp, cpu->pc & 0x00FF );
    cpu->sp--;
    fetch_target_value(cpu, ir);
    cpu->pc=cpu->abs_addr;;
}
void I_LDA(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "LDA");  
    fetch_target_value(cpu, ir);
    cpu->acc = cpu->fetched_value;
    setFlag('N', cpu->fetched_value & 0x80, cpu);
    setFlag('Z', cpu->fetched_value == 0, cpu);
}
void I_LDX(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "LDX");  
    fetch_target_value(cpu, ir);
    cpu->x_reg = cpu->fetched_value;
    setFlag('N', cpu->fetched_value & 0x80, cpu);
    setFlag('Z', cpu->fetched_value == 0, cpu);
}
void I_LDY(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "LDY");  
    fetch_target_value(cpu, ir);
    cpu->y_reg = cpu->fetched_value;
    setFlag('N', cpu->fetched_value & 0x80, cpu);
    setFlag('Z', cpu->fetched_value == 0, cpu);
}
void I_LSR(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "LSR");  

    fetch_target_value(cpu, ir);
    uint8_t carry_flag_value = cpu->fetched_value & 0x01; //carry flag value is the bit that is getting shifted out
    uint8_t new_value = cpu->fetched_value >> 1;
    if (ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   

    setFlag('N', 0, cpu);
    setFlag('Z', new_value == 0, cpu);
    setFlag('C', carry_flag_value, cpu);
}
void I_NOP(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "NOP");
    cpu->pc++;
}
void I_ORA(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "ORA");
    fetch_target_value(cpu, ir);
    uint8_t or_comparison = cpu->fetched_value | cpu->acc;
    cpu->acc = or_comparison;
    setFlag('N', or_comparison & 0x80, cpu);
    setFlag('Z', or_comparison == 0, cpu);
}
void I_PHA(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "PHA");
    cpu_write(cpu, (uint16_t)cpu->sp, cpu->acc);
    cpu->sp--;
}
void I_PHP(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "PHP");
    cpu_write(cpu, (uint16_t)cpu->sp, cpu->status_reg | 0x30);
    cpu->sp--;
}
void I_PLA(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "PLA");
    cpu->acc = cpu_read(cpu, (uint16_t)cpu->sp);
    cpu->sp++;
    setFlag('N', cpu->acc & 0x80, cpu);
    setFlag('Z', cpu->acc == 0, cpu);
}
void I_PLP(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "PHP");
    cpu->status_reg = cpu_read(cpu, (uint16_t)cpu->sp) & 0xCF;
    cpu->sp++;
}
void I_ROL(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "ROL");  
    fetch_target_value(cpu, ir);
    uint8_t carry_flag_value = cpu->fetched_value & 0x80; //carry flag value is the bit that is getting shifted out
    uint8_t new_value = (cpu->fetched_value << 1) | getFlag('C', cpu); //shift left and insert old carry value
    if (ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   

    setFlag('N', new_value & 0x80, cpu);
    setFlag('Z', new_value == 0, cpu);
    setFlag('C', carry_flag_value, cpu);    //set shifted out bit as new carry
}  
void I_ROR(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "ROR");  
    fetch_target_value(cpu, ir);
    uint8_t carry_flag_value = cpu->fetched_value & 0x01; //carry flag value is the bit that is getting shifted out
    uint8_t new_value = (cpu->fetched_value >> 1) | ((cpu->fetched_value & 0x01 ) << 7); //shift right and insert bit zero on left
    if (ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   

    setFlag('N', new_value & 0x80, cpu);
    setFlag('Z', new_value == 0, cpu);
    setFlag('C', carry_flag_value, cpu);    //set shifted out bit as new carry}
}
void I_RTI(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "RTI");
    cpu->sp++;
    cpu->status_reg = cpu_read(cpu, cpu->sp) & 0xCB;    //interrupt flag ignored and b flags
    cpu->sp++;
    cpu->pc = cpu_read(cpu, cpu->sp);
}
void I_RTS(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "RTS");
    cpu->sp++;
    uint16_t low_byte = cpu_read(cpu, cpu->sp);
    uint16_t high_byte = cpu_read(cpu, cpu->sp);

    uint16_t trg_addr = low_byte | ((high_byte << 8)  & 0xFF00);
    cpu->pc=trg_addr;
    cpu->pc++;
}
void I_SBC(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "SBC");
    fetch_target_value(cpu, ir);

    uint16_t value_inv = ((uint16_t)cpu->fetched_value) ^ 0x00FF;

    uint16_t result = (uint16_t) cpu->acc + value_inv + (uint16_t) getFlag('C', cpu);
    setFlag('C', result > 255, cpu);
    setFlag('Z', (result & 0x00FF) == 0, cpu);
    setFlag('N', result & 0x80, cpu);
    setFlag('V', ( ( (uint16_t)cpu->acc ^ (uint16_t)result ) & ~( (uint16_t)cpu->acc ^ (uint16_t)cpu->fetched_value ) ) & 0x0080, cpu);
    cpu->acc = (uint8_t) (result & 0x00FF);
}
void I_SEC(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "SEC");
    cpu->pc += 1;
    setFlag('C', 1, cpu);   //set carry bit
}
void I_SED(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "SED");
    setFlag('D', 1, cpu);   //set decimal bit
}
void I_SEI(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "SEI");
    setFlag('I', 1, cpu);   //set interrupt disable bit
}
void I_STA(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "STA");  
    fetch_target_value(cpu, ir);
    cpu_write(cpu, cpu->abs_addr, cpu->acc);
}
void I_STX(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "STX");  
    fetch_target_value(cpu, ir);
    cpu_write(cpu, cpu->abs_addr, cpu->x_reg);
}
void I_STY(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "STY");  
    fetch_target_value(cpu, ir);
    cpu_write(cpu, cpu->abs_addr, cpu->y_reg);
}
void I_TAX(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "TAX");

    cpu->x_reg = cpu->acc;
    setFlag('N', cpu->x_reg & 0x80, cpu);
    setFlag('Z', cpu->x_reg == 0, cpu);
}
void I_TAY(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "TAY");

    cpu->y_reg = cpu->acc;
    setFlag('N', cpu->y_reg & 0x80, cpu);
    setFlag('Z', cpu->y_reg == 0, cpu);
}
void I_TSX(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "TSX");

    cpu->x_reg = cpu->sp;
    setFlag('N', cpu->x_reg & 0x80, cpu);
    setFlag('Z', cpu->x_reg == 0, cpu);
}
void I_TXA(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "TXA");

    cpu->acc = cpu->x_reg;
    setFlag('N', cpu->x_reg & 0x80, cpu);
    setFlag('Z', cpu->x_reg == 0, cpu);
}
void I_TXS(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "TXS");

    cpu->sp = cpu->x_reg;
}
void I_TYA(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "TYA");

    cpu->acc = cpu->y_reg;
    setFlag('N', cpu->y_reg & 0x80, cpu);
    setFlag('Z', cpu->y_reg == 0, cpu);
}

void U_DEF(Processor *cpu, InstrInfo *ir) {
    //do nothing
    strcpy(ir->opcode_mnemonic, "UDF");
    cpu->acc=cpu->acc;
}

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
                                    I_BEQ, I_SBC, U_DEF, U_DEF,     U_DEF, I_SBC, I_INC, U_DEF,     I_SED, I_SBC, U_DEF, U_DEF,     U_DEF, I_SBC, I_INC, U_DEF,  };                      

address_mode address_mode_matrix[] = {  IMP,IDX,UDF,UDF,    UDF,ZPG,ZPG,UDF,    IMP,IMM,ACC,UDF,    UDF,ABS,ABS,UDF,
                                        REL,IDY,UDF,UDF,    UDF,ZPX,ZPX,UDF,    IMP,ABY,UDF,UDF,    UDF,ABX,ABX,UDF,
                                        ABS,IDX,UDF,UDF,    ZPG,ZPG,ZPG,UDF,    IMP,IMM,ACC,UDF,    ABS,ABS,ABS,UDF,
                                        REL,IDY,UDF,UDF,    UDF,ZPX,ZPX,UDF,    IMP,ABY,UDF,UDF,    UDF,ABX,ABX,UDF,
                                        
                                        IMP,IDX,UDF,UDF,    UDF,ZPG,ZPG,UDF,    IMP,IMM,ACC,UDF,    ABS,ABS,ABS,UDF,
                                        REL,IDY,UDF,UDF,    UDF,ZPX,ZPX,UDF,    IMP,ABY,UDF,UDF,    UDF,ABX,ABX,UDF,
                                        IMP,IDX,UDF,UDF,    UDF,ZPG,ZPG,UDF,    IMP,IMM,ACC,UDF,    IND,ABS,ABS,UDF,
                                        REL,IDY,UDF,UDF,    UDF,ZPX,ZPX,UDF,    IMP,ABY,UDF,UDF,    UDF,ABX,ABX,UDF,
                                        
                                        UDF,IDX,UDF,UDF,    ZPG,ZPG,ZPG,UDF,    IMP,UDF,IMP,UDF,    ABS,ABS,ABS,UDF,
                                        REL,IDY,UDF,UDF,    ZPX,ZPX,ZPY,UDF,    IMP,ABY,IMP,UDF,    UDF,ABX,UDF,UDF,
                                        IMM,IDX,IMM,UDF,    ZPG,ZPG,ZPG,UDF,    IMP,IMM,IMP,UDF,    ABS,ABS,ABS,UDF,
                                        REL,IDY,UDF,UDF,    ZPX,ZPX,ZPY,UDF,    IMP,ABY,IMP,UDF,    ABX,ABX,ABY,UDF,
                                        
                                        IMM,IDX,UDF,UDF,    ZPG,ZPG,ZPG,UDF,    IMP,IMM,IMP,UDF,    ABS,ABS,ABS,UDF,
                                        REL,IDY,UDF,UDF,    UDF,ZPX,ZPX,UDF,    IMP,ABY,UDF,UDF,    UDF,ABX,ABX,UDF,
                                        IMM,IDX,UDF,UDF,    ZPG,ZPG,ZPG,UDF,    IMP,IMM,IMP,UDF,    ABS,ABS,ABS,UDF,
                                        REL,IDY,UDF,UDF,    UDF,ZPX,ZPX,UDF,    IMP,ABY,UDF,UDF,    UDF,ABX,ABX,UDF,    };

unsigned int byte_matrix[] = {  1,2,0,0,    0,2,2,0,    1,2,1,0,    0,3,3,0,
                                2,2,0,0,    0,2,2,0,    1,3,0,0,    0,3,3,0,
                                3,2,0,0,    2,2,2,0,    1,2,1,0,    3,3,3,0,
                                2,2,0,0,    0,2,2,0,    1,3,0,0,    0,3,3,0,
                                
                                1,2,0,0,    0,2,2,0,    1,2,1,0,    3,3,3,0,
                                2,2,0,0,    0,2,2,0,    1,3,0,0,    0,3,3,0,
                                1,2,0,0,    0,2,2,0,    1,2,1,0,    3,3,3,0,
                                2,2,0,0,    0,2,2,0,    1,3,0,0,    0,3,3,0,
                                
                                0,2,0,0,    2,2,2,0,    1,0,1,0,    3,3,3,0,
                                2,2,0,0,    2,2,2,0,    1,3,1,0,    0,3,0,0,
                                2,2,2,0,    2,2,2,0,    1,2,1,0,    3,3,3,0,
                                2,2,0,0,    2,2,2,0,    1,3,1,0,    3,3,3,0,
                                
                                2,2,0,0,    2,2,2,0,    1,2,1,0,    3,3,3,0,
                                2,2,0,0,    0,2,2,0,    1,3,0,0,    0,3,3,0,
                                2,2,0,0,    2,2,2,0,    1,2,1,0,    3,3,3,0,
                                2,2,0,0,    0,2,2,0,    1,3,0,0,    0,3,3,0,    };

unsigned int cycle_matrix[] = { 7,6,0,0,    0,3,5,0,    3,2,2,0,    0,4,6,0,
                                2,5,0,0,    0,4,6,0,    2,4,0,0,    0,4,7,0,
                                6,6,0,0,    3,3,5,0,    4,2,2,0,    4,4,6,0,
                                2,5,0,0,    0,4,6,0,    2,4,0,0,    0,4,7,0,
                                
                                6,6,0,0,    0,3,5,0,    3,2,2,0,    3,4,6,0,
                                2,5,0,0,    0,4,6,0,    2,4,0,0,    0,4,7,0,
                                6,6,0,0,    0,3,5,0,    4,2,2,0,    5,4,6,0,
                                2,5,0,0,    0,4,6,0,    2,4,0,0,    0,4,7,0,
                                
                                0,6,0,0,    3,3,3,0,    2,0,2,0,    4,4,4,0,
                                2,6,0,0,    4,4,4,0,    2,5,2,0,    0,5,0,0,
                                2,6,2,0,    3,3,3,0,    2,2,2,0,    4,4,4,0,
                                2,5,0,0,    4,4,4,0,    2,4,2,0,    4,4,4,0,
                                
                                2,6,0,0,    3,3,5,0,    2,2,2,0,    4,4,6,0,
                                2,5,0,0,    0,4,6,0,    2,4,0,0,    0,4,7,0,
                                2,6,0,0,    3,3,5,0,    2,2,2,0,    4,4,6,0,
                                2,5,0,0,    0,4,6,0,    2,4,0,0,    0,4,7,0,    };

char *opcode_str_matrix[] = {       "BRK","ORA","UDF","UDF",    "UDF","ORA","ASL","UDF",    "PHP","ORA","ASL","UDF",    "UDF","ORA","ASL","UDF",
                                    "BPL","ORA","UDF","UDF",    "UDF","ORA","ASL","UDF",    "CLC","ORA","UDF","UDF",    "UDF","ORA","ASL","UDF",
                                    "JSR","AND","UDF","UDF",    "BIT","AND","ROL","UDF",    "PLP","AND","ROL","UDF",    "BIT","AND","ROL","UDF",
                                    "BMI","AND","UDF","UDF",    "UDF","AND","ROL","UDF",    "SEC","AND","UDF","UDF",    "UDF","AND","ROL","UDF",
                                    
                                    "RTI","EOR","UDF","UDF",    "UDF","EOR","LSR","UDF",    "PHA","EOR","LSR","UDF",    "JMP","EOR","LSR","UDF",
                                    "BVC","EOR","UDF","UDF",    "UDF","EOR","LSR","UDF",    "CLI","EOR","UDF","UDF",    "UDF","EOR","LSR","UDF",
                                    "RTS","ADC","UDF","UDF",    "UDF","ADC","ROR","UDF",    "PLA","ADC","ROR","UDF",    "JMP","ADC","ROR","UDF",
                                    "BVS","ADC","UDF","UDF",    "UDF","ADC","ROR","UDF",    "SEI","ADC","UDF","UDF",    "UDF","ADC","ROR","UDF",
                                    
                                    "UDF","STA","UDF","UDF",    "STY","STA","STX","UDF",    "DEY","UDF","TXA","UDF",    "STY","STA","STX","UDF",
                                    "BCC","STA","UDF","UDF",    "STY","STA","STX","UDF",    "TYA","STA","TXS","UDF",    "UDF","STA","UDF","UDF",
                                    "LDY","LDA","LDX","UDF",    "LDY","LDA","LDX","UDF",    "TAY","LDA","TAX","UDF",    "LDY","LDA","LDX","UDF",
                                    "BCS","LDA","UDF","UDF",    "LDY","LDA","LDX","UDF",    "CLV","LDA","TSX","UDF",    "LDY","LDA","LDX","UDF",
                                    
                                    "CPY","CMP","UDF","UDF",    "CPY","CMP","DEC","UDF",    "INY","CMP","DEX","UDF",    "CPY","CMP","DEC","UDF",
                                    "BNE","CMP","UDF","UDF",    "UDF","CMP","DEC","UDF",    "CLD","CMP","UDF","UDF",    "UDF","CMP","DEC","UDF",
                                    "CPX","SBC","UDF","UDF",    "CPX","SBC","INC","UDF",    "INX","SBC","NOP","UDF",    "CPX","SBC","INC","UDF",
                                    "BEQ","SBC","UDF","UDF",    "UDF","SBC","INC","UDF",    "SED","SBC","UDF","UDF",    "UDF","SBC","INC","UDF",    };

//TODO: design proper decoding
InstrInfo decode_instruction(uint8_t byte) {

    opcode ptr = opcode_matrix[byte];
    address_mode addr_mode = address_mode_matrix[byte];
    unsigned int n_bytes = byte_matrix[byte];
    unsigned int n_cycles = cycle_matrix[byte];
    InstrInfo ret_ir = {ptr, addr_mode, n_bytes, n_cycles, "UDF"};
    strcpy(ret_ir.opcode_mnemonic, opcode_str_matrix[byte]);

    return ret_ir;
}