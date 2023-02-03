//uncomment to supress messages
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wreturn-type"

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










int fetch_target_value(Processor *cpu, InstrInfo *ir) {

    int extra_cycle = 0;

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
            if((trg_addr & 0xFF00) != ((cpu->pc + 2)& 0xFF00)) {
                extra_cycle = 1;  //page boundary crossed, add 1
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
                extra_cycle = 1;    //extra cycle
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
                extra_cycle = 1;    //extra cycle
            }
            cpu->pc += 3;
            cpu->abs_addr =  trg_addr;
            cpu->fetched_value = cpu_read(cpu, cpu->abs_addr);
            break;
        }
    
    case IND:
        {
            uint16_t ptr_low = cpu_read(cpu, cpu->pc + 1);
            uint16_t ptr_high = cpu_read(cpu, cpu->pc + 2);
            uint16_t ptr = ptr_low + (ptr_high << 8);

            //page boundary hardware bug simulation
            if (ptr_low == 0x00FF) {
                cpu->abs_addr = (cpu_read(cpu, ptr & 0xFF00) << 8) | cpu_read(cpu, ptr + 0);
            } else {
                cpu->abs_addr = (cpu_read(cpu, ptr +1) << 8) | cpu_read(cpu, ptr + 0);
            }

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
                extra_cycle = 1;    //extra cycle
            }
            cpu->pc += 2;
            cpu->abs_addr = trg_addr;
            cpu->fetched_value = cpu_read(cpu, cpu->abs_addr);
            break;
        }

    default:
        break;
    }
    return extra_cycle;
}


void I_ADC(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "ADC");
    
    ir->n_cycles += fetch_target_value(cpu, ir);
    uint16_t result = (uint16_t) cpu->acc + (uint16_t) cpu->fetched_value + (uint16_t) getFlag('C', cpu);
    
    setFlag('C', result > 255, cpu);
    setFlag('Z', (result & 0x00FF) == 0, cpu);
    setFlag('N', result & 0x80, cpu);
    setFlag('V', ( ( (uint16_t)cpu->acc ^ (uint16_t)result ) & ~( (uint16_t)cpu->acc ^ (uint16_t)cpu->fetched_value ) ) & 0x0080, cpu);
    cpu->acc = (uint8_t) (result & 0x00FF);
}
void I_AND(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "AND");
    
    ir->n_cycles += fetch_target_value(cpu, ir);
    uint8_t and_comparison = cpu->fetched_value & cpu->acc;
    cpu->acc = and_comparison;
    setFlag('N', and_comparison & 0x80, cpu);
    setFlag('Z', and_comparison == 0, cpu);
}
void I_ASL(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "ASL");  

    ir->n_cycles += fetch_target_value(cpu, ir);
    uint8_t carry_flag_value = cpu->fetched_value & 0x80; //carry flag value is the bit that is getting shifted out
    uint8_t new_value = cpu->fetched_value << 1;if
    
    (ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   //TODO: fix the abs_addr writing somehow

    setFlag('N', new_value & 0x80, cpu);
    setFlag('Z', new_value == 0, cpu);
    setFlag('C', carry_flag_value, cpu);
}
void I_BCC(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BCC");
    
    int extra_cycle = fetch_target_value(cpu, ir);
    if (getFlag('C', cpu) == 0) {
        ir->n_cycles += 1 + extra_cycle;
        cpu->pc = cpu->abs_addr;
    } else {
        cpu->pc += 2;
    }
}
void I_BCS(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BCS");
    int extra_cycle = fetch_target_value(cpu, ir);
    if (getFlag('C', cpu) == 1) {
        ir->n_cycles += 1 + extra_cycle; //if carry set and branch occours, add extra cycle
        cpu->pc = cpu->abs_addr;
    } else {
        cpu->pc += 2;
    }
}
void I_BEQ(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BEQ");
    int extra_cycle = fetch_target_value(cpu, ir);
    if (getFlag('Z', cpu) == 1) {
        ir->n_cycles += 1 + extra_cycle;
        cpu->pc = cpu->abs_addr;
    } else {
        cpu->pc += 2;
    }
}
void I_BIT(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BIT");
    
    ir->n_cycles += fetch_target_value(cpu, ir);

    uint8_t result = cpu->acc & cpu->fetched_value;
    setFlag('N', cpu->fetched_value & 0x80, cpu);
    setFlag('V', cpu->fetched_value & 0x40, cpu);
    setFlag('Z', result == 0, cpu);
}
void I_BMI(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BMI");
    int extra_cycle = fetch_target_value(cpu, ir);
    if (getFlag('N', cpu) == 1) {
        ir->n_cycles += 1 + extra_cycle;
        cpu->pc = cpu->abs_addr;
    } else {
        cpu->pc += 2;
    }
}
void I_BNE(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BNE");
    int extra_cycle = fetch_target_value(cpu, ir);
    if (getFlag('Z', cpu) == 0) {
        ir->n_cycles += 1 + extra_cycle;
        cpu->pc = cpu->abs_addr;     
    } else {
        cpu->pc += 2;
    }
}
void I_BPL(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BPL");

    int extra_cycle = fetch_target_value(cpu, ir);
    if (getFlag('N', cpu) == 0) {
        ir->n_cycles += 1 + extra_cycle;
        cpu->pc = cpu->abs_addr;
    } else {
        cpu->pc += 2;
    }
}
void I_BRK(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BRK");

    setFlag('I', 1, cpu);
    cpu_write(cpu, 0x0100 | cpu->sp, ((cpu->pc+2)>>8) & 0x00FF);
    cpu->sp--;
    cpu_write(cpu, 0x0100 | cpu->sp, (cpu->pc+2) & 0x00FF);
    cpu->sp--;

    setFlag('B', 1, cpu);
    cpu_write(cpu, 0x0100 | cpu->sp, cpu->status_reg);
    cpu->sp--;
    setFlag('B', 0, cpu);

    //read reset vector for pc
    cpu->pc = (uint16_t)cpu_read(cpu, 0xFFFE) | ((uint16_t)cpu_read(cpu, 0xFFFF) << 8);
}
void I_BVC(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BVC");
    int extra_cycle = fetch_target_value(cpu, ir);
    if (getFlag('V', cpu) == 0) {
        ir->n_cycles += 1 + extra_cycle;
        cpu->pc = cpu->abs_addr;
    } else {
        cpu->pc += 2;
    }
}
void I_BVS(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "BVS");
    int extra_cycle = fetch_target_value(cpu, ir);
    if (getFlag('V', cpu) == 1) {
        ir->n_cycles += 1 + extra_cycle;
        cpu->pc = cpu->abs_addr;
    } else {
        cpu->pc += 2;
    }
}
void I_CLC(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "CLC");
    setFlag('C', 0, cpu);   //clear carry
    cpu->pc++;
}
void I_CLD(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "CLD");
    setFlag('D', 0, cpu);   //clear decimal
    cpu->pc++;
}
void I_CLI(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "CLI");
    setFlag('I', 0, cpu);   //clear interrupt disable
    cpu->pc++;
}
void I_CLV(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "CLV");
    setFlag('V', 0, cpu);   //clear overflow
    cpu->pc++;
}
void I_CMP(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "CMP");
    ir->n_cycles += fetch_target_value(cpu, ir);
    uint8_t cmp_result = cpu->acc - cpu->fetched_value;

    setFlag('N', cmp_result & 0x80, cpu);
    setFlag('Z', (cpu->acc == cpu->fetched_value) ? 1 : 0, cpu);
    setFlag('C', (cpu->acc >= cpu->fetched_value) ? 1 : 0, cpu);
}
void I_CPX(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "CPX");
    ir->n_cycles += fetch_target_value(cpu, ir);
    uint8_t cmp_result = cpu->x_reg - cpu->fetched_value;
    setFlag('N', cmp_result & 0x80, cpu);
    setFlag('Z', (cpu->x_reg == cpu->fetched_value) ? 1 : 0, cpu);
    setFlag('C', (cpu->x_reg >= cpu->fetched_value) ? 1 : 0, cpu);
}
void I_CPY(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "CPY");
    ir->n_cycles += fetch_target_value(cpu, ir);
    uint8_t cmp_result = cpu->y_reg - cpu->fetched_value;
    setFlag('N', cmp_result & 0x80, cpu);
    setFlag('Z', (cpu->y_reg == cpu->fetched_value) ? 1 : 0, cpu);
    setFlag('C', (cpu->y_reg >= cpu->fetched_value) ? 1 : 0, cpu);
}
void I_DEC(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "DEC");

    ir->n_cycles += fetch_target_value(cpu, ir);
    uint8_t new_value = cpu->fetched_value - 1;
    cpu_write(cpu, cpu->abs_addr, new_value);
    setFlag('N', new_value & 0x80, cpu);
    setFlag('Z', new_value == 0, cpu);
}
void I_DEX(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "DEX");
    cpu->x_reg--;
    cpu->pc++;
    setFlag('N', cpu->x_reg & 0x80, cpu);
    setFlag('Z', cpu->x_reg == 0, cpu);
}
void I_DEY(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "DEY");
    cpu->y_reg--;
    cpu->pc++;
    setFlag('N', cpu->y_reg & 0x80, cpu);
    setFlag('Z', cpu->y_reg == 0, cpu);
}
void I_EOR(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "EOR");
    ir->n_cycles += fetch_target_value(cpu, ir);
    uint8_t and_comparison = cpu->fetched_value ^ cpu->acc;
    cpu->acc = and_comparison;
    setFlag('N', and_comparison & 0x80, cpu);
    setFlag('Z', and_comparison == 0, cpu);
}
void I_INC(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "INC");
    ir->n_cycles += fetch_target_value(cpu, ir);
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
    cpu->pc++;
}
void I_INY(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "INY");
    cpu->y_reg++;
    setFlag('N', cpu->y_reg & 0x80, cpu);
    setFlag('Z', cpu->y_reg == 0, cpu);
    cpu->pc++;
}
void I_JMP(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "JMP");
    ir->n_cycles += fetch_target_value(cpu, ir);
    cpu->pc=cpu->abs_addr;
}
void I_JSR(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "JSR");
    //JSR is a very wierd instruction, it is 3 bytes long so last byte of abs addr is pushed onto stack
    //logically this should instead be pc+3, i.e. next instruction. Instead, when returning with RTS,
    //1 is added to the returned pc and everything works
    cpu_write(cpu, 0x0100 | cpu->sp, ((cpu->pc + 2) >> 8) & 0x00FF );
    cpu->sp--;
    cpu_write(cpu, 0x0100 | cpu->sp, (cpu->pc + 2) & 0x00FF );
    cpu->sp--;
    ir->n_cycles += fetch_target_value(cpu, ir);
    cpu->pc=cpu->abs_addr;;
}
void I_LDA(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "LDA");  
    ir->n_cycles += fetch_target_value(cpu, ir);
    cpu->acc = cpu->fetched_value;
    setFlag('N', cpu->fetched_value & 0x80, cpu);
    setFlag('Z', cpu->fetched_value == 0, cpu);
}
void I_LDX(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "LDX");  
    ir->n_cycles += fetch_target_value(cpu, ir);
    cpu->x_reg = cpu->fetched_value;
    setFlag('N', cpu->fetched_value & 0x80, cpu);
    setFlag('Z', cpu->fetched_value == 0, cpu);
}
void I_LDY(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "LDY");  
    ir->n_cycles += fetch_target_value(cpu, ir);
    cpu->y_reg = cpu->fetched_value;
    setFlag('N', cpu->fetched_value & 0x80, cpu);
    setFlag('Z', cpu->fetched_value == 0, cpu);
}
void I_LSR(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "LSR");  

    ir->n_cycles += fetch_target_value(cpu, ir);
    uint8_t carry_flag_value = cpu->fetched_value & 0x01; //carry flag value is the bit that is getting shifted out
    uint8_t new_value = cpu->fetched_value >> 1;
    if (ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   

    setFlag('N', 0, cpu);
    setFlag('Z', new_value == 0, cpu);
    setFlag('C', carry_flag_value, cpu);
}
void I_NOP(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "NOP");
    ir->n_cycles += fetch_target_value(cpu, ir);
    //cpu->pc += ir->n_bytes;
}
void I_ORA(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "ORA");
    ir->n_cycles += fetch_target_value(cpu, ir);
    uint8_t or_comparison = cpu->fetched_value | cpu->acc;
    cpu->acc = or_comparison;
    setFlag('N', or_comparison & 0x80, cpu);
    setFlag('Z', or_comparison == 0, cpu);
}
void I_PHA(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "PHA");
    cpu_write(cpu, 0x0100 | cpu->sp, cpu->acc);
    cpu->sp--;
    cpu->pc++;
}
void I_PHP(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "PHP");
    cpu_write(cpu, 0x0100 | cpu->sp, cpu->status_reg | (flag_U | flag_B));
    cpu->sp--;
    cpu->pc++;
}
void I_PLA(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "PLA");
    cpu->sp++;
    cpu->acc = cpu_read(cpu, 0x0100 | cpu->sp);
    setFlag('N', cpu->acc & 0x80, cpu);
    setFlag('Z', cpu->acc == 0, cpu);
    cpu->pc++;
}
void I_PLP(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "PLP");
    cpu->sp++;
    cpu->status_reg = cpu_read(cpu, 0x0100 | cpu->sp);

    cpu->status_reg &= ~(flag_B); //ignore break flag
    cpu->status_reg |= flag_U; //set unused flag
    cpu->pc++;
}
void I_ROL(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "ROL");  
    ir->n_cycles += fetch_target_value(cpu, ir);
    uint8_t carry_flag_value = cpu->fetched_value & 0x80; //carry flag value is the bit that is getting shifted out
    uint8_t new_value = (cpu->fetched_value << 1) | getFlag('C', cpu); //shift left and insert old carry value
    if (ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   

    setFlag('N', new_value & 0x80, cpu);
    setFlag('Z', new_value == 0, cpu);
    setFlag('C', carry_flag_value, cpu);    //set shifted out bit as new carry
}  
void I_ROR(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "ROR");  
    ir->n_cycles += fetch_target_value(cpu, ir);
    uint8_t new_carry_flag_value = cpu->fetched_value & 0x01; //carry flag value is the bit that is getting shifted out
    uint8_t new_value = (cpu->fetched_value >> 1) | ((getFlag('C',cpu)) << 7); //shift right and insert bit zero on left
    if (ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   

    setFlag('N', new_value & 0x80, cpu);
    setFlag('Z', new_value == 0, cpu);
    setFlag('C', new_carry_flag_value, cpu);    //set shifted out bit as new carry}
}
void I_RTI(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "RTI");
    cpu->sp++;
    cpu->status_reg = cpu_read(cpu, 0x0100 | cpu->sp);
    //B ignored
    cpu->status_reg &= ~(flag_B);
    cpu->status_reg |= flag_U;

    cpu->sp++;
    uint16_t low_byte = cpu_read(cpu, 0x0100 | cpu->sp);
    cpu->sp++;
    uint16_t high_byte = cpu_read(cpu, 0x0100 | cpu->sp);

    cpu->pc = (high_byte << 8) | low_byte;
}
void I_RTS(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "RTS");
    cpu->sp++;
    uint16_t low_byte = cpu_read(cpu, 0x0100 | cpu->sp);
    cpu->sp++;
    uint16_t high_byte = cpu_read(cpu, 0x0100 | cpu->sp);

    uint16_t trg_addr = (high_byte << 8) | low_byte;
    cpu->pc=trg_addr;
    cpu->pc++;          //need to increment by one, otherwise returning to wrong address due behaviour of JSR
}
void I_SBC(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "SBC");
    ir->n_cycles += fetch_target_value(cpu, ir);

    uint16_t value_inv = ((uint16_t)cpu->fetched_value) ^ 0x00FF;

    uint16_t result = (uint16_t) cpu->acc + value_inv + (uint16_t) getFlag('C', cpu);
    setFlag('C', result > 255, cpu);
    setFlag('Z', (result & 0x00FF) == 0, cpu);
    setFlag('N', result & 0x80, cpu);
    setFlag('V', ( ( result ^ (uint16_t)cpu->acc ) & ( ( result ^ (uint16_t)value_inv ) ) & 0x0080), cpu);
    cpu->acc = (uint8_t) (result & 0x00FF);
}
void I_SEC(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "SEC");
    cpu->pc += 1;
    setFlag('C', 1, cpu);   //set carry bit
}
void I_SED(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "SED");
    cpu->pc += 1;
    setFlag('D', 1, cpu);   //set decimal bit
}
void I_SEI(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "SEI");
    cpu->pc += 1;
    setFlag('I', 1, cpu);   //set interrupt disable bit
}
void I_STA(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "STA");  
    fetch_target_value(cpu, ir);
    cpu_write(cpu, cpu->abs_addr, cpu->acc);
}
void I_STX(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "STX");  
    ir->n_cycles += fetch_target_value(cpu, ir);
    cpu_write(cpu, cpu->abs_addr, cpu->x_reg);
}
void I_STY(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "STY");  
    ir->n_cycles += fetch_target_value(cpu, ir);
    cpu_write(cpu, cpu->abs_addr, cpu->y_reg);
}
void I_TAX(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "TAX");

    cpu->x_reg = cpu->acc;
    setFlag('N', cpu->x_reg & 0x80, cpu);
    setFlag('Z', cpu->x_reg == 0, cpu);
    cpu->pc++;
}
void I_TAY(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "TAY");

    cpu->y_reg = cpu->acc;
    setFlag('N', cpu->y_reg & 0x80, cpu);
    setFlag('Z', cpu->y_reg == 0, cpu);
    cpu->pc++;
}
void I_TSX(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "TSX");

    cpu->x_reg = cpu->sp;
    setFlag('N', cpu->x_reg & 0x80, cpu);
    setFlag('Z', cpu->x_reg == 0, cpu);
    cpu->pc++;
}
void I_TXA(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "TXA");

    cpu->acc = cpu->x_reg;
    setFlag('N', cpu->x_reg & 0x80, cpu);
    setFlag('Z', cpu->x_reg == 0, cpu);
    cpu->pc++;
}
void I_TXS(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "TXS");
    cpu->sp = cpu->x_reg;
    cpu->pc++;
}
void I_TYA(Processor *cpu, InstrInfo *ir) {
    strcpy(ir->opcode_mnemonic, "TYA");

    cpu->acc = cpu->y_reg;
    setFlag('N', cpu->y_reg & 0x80, cpu);
    setFlag('Z', cpu->y_reg == 0, cpu);
    cpu->pc += 1;
}

void U_DEF(Processor *cpu, InstrInfo *ir) {
    //do nothing
    strcpy(ir->opcode_mnemonic, "UDF");
    cpu->acc=cpu->acc;
}

// ILLEGAL OPCODES

void I_ALR(Processor *cpu, InstrInfo *ir) {
    ir->n_cycles += fetch_target_value(cpu, ir);
    uint8_t and_result = cpu->acc & cpu->fetched_value;

    uint8_t carry_flag_value = and_result & 0x01; //carry flag value is the bit that is getting shifted out
    uint8_t new_value = and_result >> 1;
    if (ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   

    setFlag('N', 0, cpu);
    setFlag('Z', new_value == 0, cpu);
    setFlag('C', carry_flag_value, cpu);

}
void I_ANC(Processor *cpu, InstrInfo *ir) {
    ir->n_cycles += fetch_target_value(cpu, ir);
    uint8_t and_result = cpu->acc & cpu->fetched_value;
    cpu->acc = and_result;
    setFlag('N', and_result & 0x80, cpu);
    setFlag('Z', and_result == 0, cpu);
    setFlag('C', and_result & 0x80, cpu);

}
void I_ANE(Processor *cpu, InstrInfo *ir) { ir; } //unstable, do no implement
void I_ARR(Processor *cpu, InstrInfo *ir) { ir; }
void I_DCP(Processor *cpu, InstrInfo *ir) {
    int debug = 0;
    if(cpu->pc == 0xE949) {
        debug = 0;
    }

    fetch_target_value(cpu,ir);
    cpu_write(cpu, cpu->abs_addr, cpu->fetched_value-1);
    uint8_t cmp_result = cpu->acc - (cpu->fetched_value-1);

    setFlag('N', cmp_result & 0x80, cpu);
    setFlag('Z', cmp_result==0 ? 1 : 0, cpu);
    setFlag('C', (cpu->acc >= (cpu->fetched_value-1)) ? 1 : 0, cpu);

    if(debug) {
        fprintf(stderr, "M: %02X, A: %02X, cmp: %02X, N: %d, Z: %d, C: %d\n", cpu_read(cpu, cpu->abs_addr), cpu->acc, cmp_result, getFlag('N', cpu), getFlag('Z', cpu), getFlag('C', cpu));
    }

}
void I_ISC(Processor *cpu, InstrInfo *ir) {
    fetch_target_value(cpu,ir);
    cpu_write(cpu, cpu->abs_addr, cpu->fetched_value+1);


    uint16_t value_inv = ((uint16_t)cpu->fetched_value+1) ^ 0x00FF;

    uint16_t result = (uint16_t) cpu->acc + value_inv + (uint16_t) getFlag('C', cpu);
    setFlag('C', result > 255, cpu);
    setFlag('Z', (result & 0x00FF) == 0, cpu);
    setFlag('N', result & 0x80, cpu);
    setFlag('V', ( ( result ^ (uint16_t)cpu->acc ) & ( ( result ^ (uint16_t)value_inv ) ) & 0x0080), cpu);
    cpu->acc = (uint8_t) (result & 0x00FF);
}
void I_LAS(Processor *cpu, InstrInfo *ir) {
    ir->n_cycles += fetch_target_value(cpu,ir);

    uint8_t result = cpu->fetched_value & cpu->sp;
    cpu->acc = result;
    cpu->x_reg = result;
    cpu->y_reg = result;

    setFlag('N', result & 0x80, cpu);
    setFlag('N', result == 0, cpu);
}
void I_LAX(Processor *cpu, InstrInfo *ir) {
    ir->n_cycles += fetch_target_value(cpu,ir);

    cpu->acc = cpu->fetched_value;
    cpu->x_reg = cpu->fetched_value;

    setFlag('N', cpu->fetched_value & 0x80, cpu);
    setFlag('Z', cpu->fetched_value == 0, cpu);
}
void I_LXA(Processor *cpu, InstrInfo *ir) { ir; } //unstable, do no implement
void I_RLA(Processor *cpu, InstrInfo *ir) {
    fetch_target_value(cpu, ir);

    uint8_t carry_flag_value = cpu->fetched_value & 0x80; //carry flag value is the bit that is getting shifted out
    uint8_t new_value = (cpu->fetched_value << 1) | getFlag('C', cpu); //shift left and insert old carry value
    if (ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   

    cpu->acc = cpu->acc & new_value;

    setFlag('N', cpu->acc & 0x80, cpu);
    setFlag('Z', cpu->acc == 0, cpu);
    setFlag('C', carry_flag_value, cpu);    //set shifted out bit as new carry
}
void I_RRA(Processor *cpu, InstrInfo *ir) {
    fetch_target_value(cpu, ir);
    uint8_t new_value = (cpu->fetched_value >> 1) | ((getFlag('C',cpu)) << 7); //shift right and insert bit zero on left
    if (ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   
    setFlag('C', cpu->fetched_value & 0x01, cpu);


    uint16_t result = (uint16_t) cpu->acc + (uint16_t) new_value + (uint16_t) getFlag('C', cpu);
    
    setFlag('C', result > 255, cpu);
    setFlag('Z', (result & 0x00FF) == 0, cpu);
    setFlag('N', result & 0x80, cpu);
    setFlag('V', ( ( (uint16_t)cpu->acc ^ (uint16_t)result ) & ~( (uint16_t)cpu->acc ^ (uint16_t)new_value ) ) & 0x0080, cpu);
    cpu->acc = (uint8_t) (result & 0x00FF);
}
void I_SAX(Processor *cpu, InstrInfo *ir) {
    ir->n_cycles += fetch_target_value(cpu,ir);
    cpu_write(cpu, cpu->abs_addr, cpu->acc & cpu->x_reg);
}
void I_SBX(Processor *cpu, InstrInfo *ir) { ir; }
void I_SHA(Processor *cpu, InstrInfo *ir) { ir; }
void I_SHX(Processor *cpu, InstrInfo *ir) { ir; }
void I_SHY(Processor *cpu, InstrInfo *ir) { ir; }
void I_SLO(Processor *cpu, InstrInfo *ir) {
    fetch_target_value(cpu, ir);

    uint8_t carry_flag_value = cpu->fetched_value & 0x80; //carry flag value is the bit that is getting shifted out
    uint8_t new_value = cpu->fetched_value << 1;
    
    if(ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   //TODO: fix the abs_addr writing somehow

    cpu->acc = cpu->acc | new_value;

    setFlag('N', cpu->acc & 0x80, cpu);
    setFlag('Z', cpu->acc == 0, cpu);
    setFlag('C', carry_flag_value, cpu);
}
void I_SRE(Processor *cpu, InstrInfo *ir) {
    fetch_target_value(cpu, ir);
    uint8_t carry_flag_value = cpu->fetched_value & 0x01; //carry flag value is the bit that is getting shifted out
    uint8_t new_value = cpu->fetched_value >> 1;
    if (ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   

    cpu->acc = cpu->acc ^ new_value;

    setFlag('N', cpu->acc & 0x80, cpu);
    setFlag('Z', cpu->acc == 0, cpu);
    setFlag('C', carry_flag_value, cpu);
}
void I_TAS(Processor *cpu, InstrInfo *ir) { ir; }
void I_JAM(Processor *cpu, InstrInfo *ir) { ir; }
void I_USB(Processor *cpu, InstrInfo *ir) {
    ir->n_cycles += fetch_target_value(cpu, ir);

    uint16_t value_inv = ((uint16_t)cpu->fetched_value) ^ 0x00FF;

    uint16_t result = (uint16_t) cpu->acc + value_inv + (uint16_t) getFlag('C', cpu);
    setFlag('C', result > 255, cpu);
    setFlag('Z', (result & 0x00FF) == 0, cpu);
    setFlag('N', result & 0x80, cpu);
    setFlag('V', ( ( result ^ (uint16_t)cpu->acc ) & ( ( result ^ (uint16_t)value_inv ) ) & 0x0080), cpu);
    cpu->acc = (uint8_t) (result & 0x00FF);
}

opcode opcode_matrix[] = {
                            I_BRK,	I_ORA,	I_JAM,	I_SLO,	I_NOP,	I_ORA,	I_ASL,	I_SLO,	I_PHP,	I_ORA,	I_ASL,	I_ANC,	I_NOP,	I_ORA,	I_ASL,	I_SLO,
                            I_BPL,	I_ORA,	I_JAM,	I_SLO,	I_NOP,	I_ORA,	I_ASL,	I_SLO,	I_CLC,	I_ORA,	I_NOP,	I_SLO,	I_NOP,	I_ORA,	I_ASL,	I_SLO,
                            I_JSR,	I_AND,	I_JAM,	I_RLA,	I_BIT,	I_AND,	I_ROL,	I_RLA,	I_PLP,	I_AND,	I_ROL,	I_ANC,	I_BIT,	I_AND,	I_ROL,	I_RLA,
                            I_BMI,	I_AND,	I_JAM,	I_RLA,	I_NOP,	I_AND,	I_ROL,	I_RLA,	I_SEC,	I_AND,	I_NOP,	I_RLA,	I_NOP,	I_AND,	I_ROL,	I_RLA,
                            I_RTI,	I_EOR,	I_JAM,	I_SRE,	I_NOP,	I_EOR,	I_LSR,	I_SRE,	I_PHA,	I_EOR,	I_LSR,	I_ALR,	I_JMP,	I_EOR,	I_LSR,	I_SRE,
                            I_BVC,	I_EOR,	I_JAM,	I_SRE,	I_NOP,	I_EOR,	I_LSR,	I_SRE,	I_CLI,	I_EOR,	I_NOP,	I_SRE,	I_NOP,	I_EOR,	I_LSR,	I_SRE,
                            I_RTS,	I_ADC,	I_JAM,	I_RRA,	I_NOP,	I_ADC,	I_ROR,	I_RRA,	I_PLA,	I_ADC,	I_ROR,	I_ARR,	I_JMP,	I_ADC,	I_ROR,	I_RRA,
                            I_BVS,	I_ADC,	I_JAM,	I_RRA,	I_NOP,	I_ADC,	I_ROR,	I_RRA,	I_SEI,	I_ADC,	I_NOP,	I_RRA,	I_NOP,	I_ADC,	I_ROR,	I_RRA,
                            I_NOP,	I_STA,	I_NOP,	I_SAX,	I_STY,	I_STA,	I_STX,	I_SAX,	I_DEY,	I_NOP,	I_TXA,	I_ANE,	I_STY,	I_STA,	I_STX,	I_SAX,
                            I_BCC,	I_STA,	I_JAM,	I_SHA,	I_STY,	I_STA,	I_STX,	I_SAX,	I_TYA,	I_STA,	I_TXS,	I_TAS,	I_SHY,	I_STA,	I_SHX,	I_SHA,
                            I_LDY,	I_LDA,	I_LDX,	I_LAX,	I_LDY,	I_LDA,	I_LDX,	I_LAX,	I_TAY,	I_LDA,	I_TAX,	I_LXA,	I_LDY,	I_LDA,	I_LDX,	I_LAX,
                            I_BCS,	I_LDA,	I_JAM,	I_LAX,	I_LDY,	I_LDA,	I_LDX,	I_LAX,	I_CLV,	I_LDA,	I_TSX,	I_LAS,	I_LDY,	I_LDA,	I_LDX,	I_LAX,
                            I_CPY,	I_CMP,	I_NOP,	I_DCP,	I_CPY,	I_CMP,	I_DEC,	I_DCP,	I_INY,	I_CMP,	I_DEX,	I_SBX,	I_CPY,	I_CMP,	I_DEC,	I_DCP,
                            I_BNE,	I_CMP,	I_JAM,	I_DCP,	I_NOP,	I_CMP,	I_DEC,	I_DCP,	I_CLD,	I_CMP,	I_NOP,	I_DCP,	I_NOP,	I_CMP,	I_DEC,	I_DCP,
                            I_CPX,	I_SBC,	I_NOP,	I_ISC,	I_CPX,	I_SBC,	I_INC,	I_ISC,	I_INX,	I_SBC,	I_NOP,	I_USB,	I_CPX,	I_SBC,	I_INC,	I_ISC,
                            I_BEQ,	I_SBC,	I_JAM,	I_ISC,	I_NOP,	I_SBC,	I_INC,	I_ISC,	I_SED,	I_SBC,	I_NOP,	I_ISC,	I_NOP,	I_SBC,	I_INC,	I_ISC,
};                      

address_mode address_mode_matrix[] = {
                                        IMP,	IDX,	UDF,	IDX,	ZPG,	ZPG,	ZPG,	ZPG,	IMP,	IMM,	ACC,	IMM,	ABS,	ABS,	ABS,	ABS,
                                        REL,	IDY,	UDF,	IDY,	ZPX,	ZPX,	ZPX,	ZPX,	IMP,	ABY,	IMP,	ABY,	ABX,	ABX,	ABX,	ABX,
                                        ABS,	IDX,	UDF,	IDX,	ZPG,	ZPG,	ZPG,	ZPG,	IMP,	IMM,	ACC,	IMM,	ABS,	ABS,	ABS,	ABS,
                                        REL,	IDY,	UDF,	IDY,	ZPX,	ZPX,	ZPX,	ZPX,	IMP,	ABY,	IMP,	ABY,	ABX,	ABX,	ABX,	ABX,
                                        IMP,	IDX,	UDF,	IDX,	ZPG,	ZPG,	ZPG,	ZPG,	IMP,	IMM,	ACC,	IMM,	ABS,	ABS,	ABS,	ABS,
                                        REL,	IDY,	UDF,	IDY,	ZPX,	ZPX,	ZPX,	ZPX,	IMP,	ABY,	IMP,	ABY,	ABX,	ABX,	ABX,	ABX,
                                        IMP,	IDX,	UDF,	IDX,	ZPG,	ZPG,	ZPG,	ZPG,	IMP,	IMM,	ACC,	IMM,	IND,	ABS,	ABS,	ABS,
                                        REL,	IDY,	UDF,	IDY,	ZPX,	ZPX,	ZPX,	ZPX,	IMP,	ABY,	IMP,	ABY,	ABX,	ABX,	ABX,	ABX,
                                        IMM,	IDX,	IMM,	IDX,	ZPG,	ZPG,	ZPG,	ZPG,	IMP,	IMM,	IMP,	IMM,	ABS,	ABS,	ABS,	ABS,
                                        REL,	IDY,	UDF,	IDY,	ZPX,	ZPX,	ZPY,	ZPY,	IMP,	ABY,	IMP,	ABY,	ABX,	ABX,	ABY,	ABY,
                                        IMM,	IDX,	IMM,	IDX,	ZPG,	ZPG,	ZPG,	ZPG,	IMP,	IMM,	IMP,	IMM,	ABS,	ABS,	ABS,	ABS,
                                        REL,	IDY,	UDF,	IDY,	ZPX,	ZPX,	ZPY,	ZPY,	IMP,	ABY,	IMP,	ABY,	ABX,	ABX,	ABY,	ABY,
                                        IMM,	IDX,	IMM,	IDX,	ZPG,	ZPG,	ZPG,	ZPG,	IMP,	IMM,	IMP,	IMM,	ABS,	ABS,	ABS,	ABS,
                                        REL,	IDY,	UDF,	IDY,	ZPX,	ZPX,	ZPX,	ZPX,	IMP,	ABY,	IMP,	ABY,	ABX,	ABX,	ABX,	ABX,
                                        IMM,	IDX,	IMM,	IDX,	ZPG,	ZPG,	ZPG,	ZPG,	IMP,	IMM,	IMP,	IMM,	ABS,	ABS,	ABS,	ABS,
                                        REL,	IDY,	UDF,	IDY,	ZPX,	ZPX,	ZPX,	ZPX,	IMP,	ABY,	IMP,	ABY,	ABX,	ABX,	ABX,	ABX,
};

unsigned int byte_matrix[] = {1,2,0,2,2,2,2,2,1,2,1,2,3,3,3,3,2,2,0,2,2,2,2,2,1,3,1,3,3,3,3,3,3,2,0,2,2,2,2,2,1,2,1,2,3,3,3,3,2,2,0,2,2,2,2,2,1,3,1,3,3,3,3,3,1,2,0,2,2,2,2,2,1,2,1,2,3,3,3,3,2,2,0,2,2,2,2,2,1,3,1,3,3,3,3,3,1,2,0,2,2,2,2,2,1,2,1,2,3,3,3,3,2,2,0,2,2,2,2,2,1,3,1,3,3,3,3,3,2,2,2,2,2,2,2,2,1,2,1,2,3,3,3,3,2,2,0,2,2,2,2,2,1,3,1,3,3,3,3,3,2,2,2,2,2,2,2,2,1,2,1,2,3,3,3,3,2,2,0,2,2,2,2,2,1,3,1,3,3,3,3,3,2,2,2,2,2,2,2,2,1,2,1,2,3,3,3,3,2,2,0,2,2,2,2,2,1,3,1,3,3,3,3,3,2,2,2,2,2,2,2,2,1,2,1,2,3,3,3,3,2,2,0,2,2,2,2,2,1,3,1,3,3,3,3,3,};

unsigned int cycle_matrix[] = {7,6,0,8,3,3,5,5,3,2,2,2,4,4,6,6,2,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7,6,6,0,8,3,3,5,5,4,2,2,2,4,4,6,6,2,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7,6,6,0,8,3,3,5,5,3,2,2,2,3,4,6,6,2,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7,6,6,0,8,3,3,5,5,4,2,2,2,5,4,6,6,2,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7,2,6,2,6,3,3,3,3,2,2,2,2,4,4,4,4,2,6,0,6,4,4,4,4,2,5,2,5,5,5,5,5,2,6,2,6,3,3,3,3,2,2,2,2,4,4,4,4,2,5,0,5,4,4,4,4,2,4,2,4,4,4,4,4,2,6,2,8,3,3,5,5,2,2,2,2,4,4,6,6,2,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7,2,6,2,8,3,3,5,5,2,2,2,2,4,4,6,6,2,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7,};

char *opcode_str_matrix[] = {       
                                "BRK",	"ORA",	"JAM",	"SLO",	"NOP",	"ORA",	"ASL",	"SLO",	"PHP",	"ORA",	"ASL",	"ANC",	"NOP",	"ORA",	"ASL",	"SLO",
                                "BPL",	"ORA",	"JAM",	"SLO",	"NOP",	"ORA",	"ASL",	"SLO",	"CLC",	"ORA",	"NOP",	"SLO",	"NOP",	"ORA",	"ASL",	"SLO",
                                "JSR",	"AND",	"JAM",	"RLA",	"BIT",	"AND",	"ROL",	"RLA",	"PLP",	"AND",	"ROL",	"ANC",	"BIT",	"AND",	"ROL",	"RLA",
                                "BMI",	"AND",	"JAM",	"RLA",	"NOP",	"AND",	"ROL",	"RLA",	"SEC",	"AND",	"NOP",	"RLA",	"NOP",	"AND",	"ROL",	"RLA",
                                "RTI",	"EOR",	"JAM",	"SRE",	"NOP",	"EOR",	"LSR",	"SRE",	"PHA",	"EOR",	"LSR",	"ALR",	"JMP",	"EOR",	"LSR",	"SRE",
                                "BVC",	"EOR",	"JAM",	"SRE",	"NOP",	"EOR",	"LSR",	"SRE",	"CLI",	"EOR",	"NOP",	"SRE",	"NOP",	"EOR",	"LSR",	"SRE",
                                "RTS",	"ADC",	"JAM",	"RRA",	"NOP",	"ADC",	"ROR",	"RRA",	"PLA",	"ADC",	"ROR",	"ARR",	"JMP",	"ADC",	"ROR",	"RRA",
                                "BVS",	"ADC",	"JAM",	"RRA",	"NOP",	"ADC",	"ROR",	"RRA",	"SEI",	"ADC",	"NOP",	"RRA",	"NOP",	"ADC",	"ROR",	"RRA",
                                "NOP",	"STA",	"NOP",	"SAX",	"STY",	"STA",	"STX",	"SAX",	"DEY",	"NOP",	"TXA",	"ANE",	"STY",	"STA",	"STX",	"SAX",
                                "BCC",	"STA",	"JAM",	"SHA",	"STY",	"STA",	"STX",	"SAX",	"TYA",	"STA",	"TXS",	"TAS",	"SHY",	"STA",	"SHX",	"SHA",
                                "LDY",	"LDA",	"LDX",	"LAX",	"LDY",	"LDA",	"LDX",	"LAX",	"TAY",	"LDA",	"TAX",	"LXA",	"LDY",	"LDA",	"LDX",	"LAX",
                                "BCS",	"LDA",	"JAM",	"LAX",	"LDY",	"LDA",	"LDX",	"LAX",	"CLV",	"LDA",	"TSX",	"LAS",	"LDY",	"LDA",	"LDX",	"LAX",
                                "CPY",	"CMP",	"NOP",	"DCP",	"CPY",	"CMP",	"DEC",	"DCP",	"INY",	"CMP",	"DEX",	"SBX",	"CPY",	"CMP",	"DEC",	"DCP",
                                "BNE",	"CMP",	"JAM",	"DCP",	"NOP",	"CMP",	"DEC",	"DCP",	"CLD",	"CMP",	"NOP",	"DCP",	"NOP",	"CMP",	"DEC",	"DCP",
                                "CPX",	"SBC",	"NOP",	"ISC",	"CPX",	"SBC",	"INC",	"ISC",	"INX",	"SBC",	"NOP",	"USB",	"CPX",	"SBC",	"INC",	"ISC",
                                "BEQ",	"SBC",	"JAM",	"ISC",	"NOP",	"SBC",	"INC",	"ISC",	"SED",	"SBC",	"NOP",	"ISC",	"NOP",	"SBC",	"INC",	"ISC",
};

//TODO: design proper decoding
InstrInfo decode_instruction(uint8_t byte) {

    opcode ptr = opcode_matrix[byte];
    address_mode addr_mode = address_mode_matrix[byte];
    unsigned int n_bytes = byte_matrix[byte];
    unsigned int n_cycles = cycle_matrix[byte];
    InstrInfo ret_ir = {ptr, addr_mode, n_bytes, n_cycles, 0, 0, 0, 0,"UDF"};
    strcpy(ret_ir.opcode_mnemonic, opcode_str_matrix[byte]);

    return ret_ir;
}

/* Disassemble one opcode with info being returned in an InstrInfo struct.
 * Struct will contain necessary info for printing debug information.
 * Pass as argument ptr to byte in memory that corresponds to valid opcode
 */
InstrInfo disassemble(uint8_t *mem, uint16_t abs_addr) {
    opcode ptr = opcode_matrix[*(mem + abs_addr)];
    address_mode addr_mode = address_mode_matrix[*(mem + abs_addr)];
    unsigned int n_bytes = byte_matrix[*(mem + abs_addr)];
    unsigned int n_cycles = cycle_matrix[*(mem + abs_addr)];

    uint8_t byte_1 = *(mem + abs_addr);
    uint8_t byte_2 = 0;
    uint8_t byte_3 = 0;     //populate the following 2 bytes with values from memory, if they are part of instruction, this allows for easy disassembly later

    if (n_bytes == 3) {
        byte_2 = *(mem + abs_addr + 1);
        byte_3 = *(mem + abs_addr + 2);
    } else if (n_bytes == 2) {
        byte_2 = *(mem + abs_addr + 1);
    }

    InstrInfo ret_ir = {ptr, addr_mode, n_bytes, n_cycles, byte_1 ,byte_2, byte_3, abs_addr, "UDF"};
    strcpy(ret_ir.opcode_mnemonic, opcode_str_matrix[*(mem + abs_addr)]);

    return ret_ir;
}