#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wreturn-type"

#include <stdio.h>

#include "opcodes.h"
#include "cpu.h"

/* Sets cpu->abs_addr and cpu->fetched_value to correct values
*/
void fetch_target_value(address_mode mode, Processor *cpu, Ir *ir) {
    switch (mode)
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
                ir->cycles++;
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
                ir->cycles += 1;    //extra cycle
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
                ir->cycles += 1;    //extra cycle
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
                ir->cycles += 1;    //extra cycle
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


int I_ADC(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="ADC";
    switch (byte)
    {
    case 0x69:
        ir->addr_mode=IMM; ir->bytes=2; ir->cycles=2;
        break;
    case 0x65:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=3;
        break;
    case 0x75:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=4;
        break;
    case 0x6D:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=4;
        break;
    case 0x7D:
        ir->addr_mode=ABX; ir->bytes=3; ir->cycles=4;
        break;
    case 0x79:
        ir->addr_mode=ABY; ir->bytes=3; ir->cycles=4;
        break;
    case 0x61:
        ir->addr_mode=IDX; ir->bytes=2; ir->cycles=6;
        break;
    case 0x71:
        ir->addr_mode=IDY; ir->bytes=2; ir->cycles=5;
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    uint16_t result = (uint16_t) cpu->acc + (uint16_t) cpu->fetched_value + (uint16_t) getFlag('C', cpu);
    
    setFlag('C', result > 255, cpu);
    setFlag('Z', (result & 0x00FF) == 0, cpu);
    setFlag('N', result & 0x80, cpu);
    setFlag('V', ( ( (uint16_t)cpu->acc ^ (uint16_t)result ) & ~( (uint16_t)cpu->acc ^ (uint16_t)cpu->fetched_value ) ) & 0x0080, cpu);
    cpu->acc = (uint8_t) (result & 0x00FF);
}
int I_AND(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="AND";
    switch (byte)
    {
    case 0x29:
        ir->addr_mode=IMM; ir->bytes=2; ir->cycles=2;
        break;
    case 0x25:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=3;
        break;
    case 0x35:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=4;
        break;
    case 0x2D:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=4;
        break;
    case 0x3D:
        ir->addr_mode=ABX; ir->bytes=3; ir->cycles=4;
        break;
    case 0x39:
        ir->addr_mode=ABY; ir->bytes=3; ir->cycles=4;
        break;
    case 0x21:
        ir->addr_mode=IDX; ir->bytes=2; ir->cycles=6;
        break;
    case 0x31:
        ir->addr_mode=IDY; ir->bytes=2; ir->cycles=5;
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    uint8_t and_comparison = cpu->fetched_value & cpu->acc;
    cpu->acc = and_comparison;
    setFlag('N', and_comparison & 0x80, cpu);
    setFlag('Z', and_comparison == 0, cpu);
}
int I_ASL(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="ASL";  
    switch (byte)
    {
    case 0x0A:
        ir->addr_mode=ACC; ir->bytes=1; ir->cycles=2;
        break;
    case 0x06:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=5;
        break;
    case 0x16:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=6;
        break;
    case 0x0E:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=6;
        break;
    case 0x1E:
        ir->addr_mode=ABX; ir->bytes=3; ir->cycles=7;
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    uint8_t carry_flag_value = cpu->fetched_value & 0x80; //carry flag value is the bit that is getting shifted out
    uint8_t new_value = cpu->fetched_value << 1;if
    
    (ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   //TODO: fix the abs_addr writing somehow

    setFlag('N', new_value & 0x80, cpu);
    setFlag('Z', new_value == 0, cpu);
    setFlag('C', carry_flag_value, cpu);
}
int I_BCC(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="BCC";
    switch (byte)
    {
    case 0x90:
        ir->addr_mode=REL; ir->bytes=2; ir->cycles=1;   //TODO: check if this should be 1 or 2, doc is confusing
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    if (getFlag('C', cpu) == 0) {
        cpu->pc = cpu->abs_addr;
    }
}
int I_BCS(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="BCS";
    switch (byte)
    {
    case 0xB0:
        ir->addr_mode=REL; ir->bytes=2; ir->cycles=1;   //TODO: check if this should be 1 or 2, doc is confusing
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    if (getFlag('C', cpu) == 1) {
        cpu->pc = cpu->abs_addr;
    }
}
int I_BEQ(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="BEQ";
    switch (byte)
    {
    case 0xF0:
        ir->addr_mode=REL; ir->bytes=2; ir->cycles=1;   //TODO: check if this should be 1 or 2, doc is confusing
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    if (getFlag('Z', cpu) == 1) {
        cpu->pc = cpu->abs_addr;
    }
}
int I_BIT(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="BIT";
    switch (byte)
    {
    case 0x24:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=3;
        break;
    case 0x2C:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=4;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);

    uint8_t result = cpu->acc & cpu->fetched_value;
    setFlag('N', cpu->fetched_value & 0x80, cpu);
    setFlag('V', cpu->fetched_value & 0x40, cpu);
    setFlag('Z', result == 0, cpu);
}
int I_BMI(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="BMI";
    switch (byte)
    {
    case 0x30:
        ir->addr_mode=REL; ir->bytes=2; ir->cycles=1;   //TODO: check if this should be 1 or 2, doc is confusing
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    if (getFlag('N', cpu) == 1) {
        cpu->pc = cpu->abs_addr;
    }
}
int I_BNE(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="BNE";
    switch (byte)
    {
    case 0xD0:
        ir->addr_mode=REL; ir->bytes=2; ir->cycles=1;   //TODO: check if this should be 1 or 2, doc is confusing
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    if (getFlag('Z', cpu) == 0) {
        cpu->pc = cpu->abs_addr;
    }
}
int I_BPL(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="BPL";
    switch (byte)
    {
    case 0x10:
        ir->addr_mode=REL; ir->bytes=2; ir->cycles=1;   //TODO: check if this should be 1 or 2, doc is confusing
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    if (getFlag('N', cpu) == 0) {
        cpu->pc = cpu->abs_addr;
    }
}
int I_BRK(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="BRK";
    switch (byte)
    {
    case 0x00:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=7;
        break;
    default:
        break;
    }
    setFlag('I', 1, cpu);
    cpu_write(cpu, cpu->sp, cpu->pc+2);
    cpu->sp--;
    cpu_write(cpu, cpu->sp, cpu->status_reg);
    cpu->sp--;
}
int I_BVC(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="BVC";
    switch (byte)
    {
    case 0x50:
        ir->addr_mode=REL; ir->bytes=2; ir->cycles=1;   //TODO: check if this should be 1 or 2, doc is confusing
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    if (getFlag('V', cpu) == 0) {
        cpu->pc = cpu->abs_addr;
    }
}
int I_BVS(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="BVC";
    switch (byte)
    {
    case 0x70:
        ir->addr_mode=REL; ir->bytes=2; ir->cycles=1;   //TODO: check if this should be 1 or 2, doc is confusing
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    if (getFlag('V', cpu) == 01) {
        cpu->pc = cpu->abs_addr;
    }
}
int I_CLC(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="CLC";
    switch (byte)
    {
    case 0x18:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    cpu->pc += 1;
    setFlag('C', 0, cpu);   //clear carry
}
int I_CLD(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="CLD";
    switch (byte)
    {
    case 0xD8:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    setFlag('D', 0, cpu);   //clear decimal
}
int I_CLI(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="CLI";
    switch (byte)
    {
    case 0x58:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    setFlag('I', 0, cpu);   //clear interrupt disable
}
int I_CLV(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="CLV";
    switch (byte)
    {
    case 0xB8:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    setFlag('V', 0, cpu);   //clear overflow
}
int I_CMP(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="CMP";
    switch (byte)
    {
    case 0xC9:
        ir->addr_mode=IMM; ir->bytes=2; ir->cycles=2;
        break;
    case 0xC5:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=3;
        break;
    case 0xD5:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=4;
        break;
    case 0xCD:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=4;
        break;
    case 0xDD:
        ir->addr_mode=ABX; ir->bytes=3; ir->cycles=4;
        break;
    case 0xD9:
        ir->addr_mode=ABY; ir->bytes=3; ir->cycles=4;
        break;
    case 0xC1:
        ir->addr_mode=IDX; ir->bytes=2; ir->cycles=6;
        break;
    case 0xD1:
        ir->addr_mode=IDY; ir->bytes=2; ir->cycles=5;
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    uint8_t cmp_result = cpu->acc - cpu->fetched_value;

    setFlag('N', cmp_result & 0x80, cpu);
    setFlag('Z', (cpu->acc == cpu->fetched_value) ? 1 : 0, cpu);
    setFlag('C', (cpu->acc >= cpu->fetched_value) ? 1 : 0, cpu);
}
int I_CPX(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="CPX";
    switch (byte)
    {
    case 0xE0:
        ir->addr_mode=IMM; ir->bytes=2; ir->cycles=2;
        break;
    case 0xE4:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=3;
        break;
    case 0xEC:
        ir->addr_mode=ABS; ir->bytes=2; ir->cycles=4;
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    uint8_t cmp_result = cpu->x_reg - cpu->fetched_value;
    setFlag('N', cmp_result & 0x80, cpu);
    setFlag('Z', (cpu->x_reg == cpu->fetched_value) ? 1 : 0, cpu);
    setFlag('C', (cpu->x_reg >= cpu->fetched_value) ? 1 : 0, cpu);
}
int I_CPY(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="CPY";
    switch (byte)
    {
    case 0xC0:
        ir->addr_mode=IMM; ir->bytes=2; ir->cycles=2;
        break;
    case 0xC4:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=3;
        break;
    case 0xCC:
        ir->addr_mode=ABS; ir->bytes=2; ir->cycles=4;
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    uint8_t cmp_result = cpu->y_reg - cpu->fetched_value;
    setFlag('N', cmp_result & 0x80, cpu);
    setFlag('Z', (cpu->y_reg == cpu->fetched_value) ? 1 : 0, cpu);
    setFlag('C', (cpu->y_reg >= cpu->fetched_value) ? 1 : 0, cpu);
}
int I_DEC(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="DEC";
    switch (byte)
    {
    case 0xC6:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=5;
        break;
    case 0xD6:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=6;
        break;
    case 0xCE:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=6;
        break;
    case 0xDE:
        ir->addr_mode=ABX; ir->bytes=3; ir->cycles=7;
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    uint8_t new_value = cpu->fetched_value - 1;
    cpu_write(cpu, cpu->abs_addr, new_value);
    setFlag('N', new_value & 0x80, cpu);
    setFlag('Z', new_value == 0, cpu);
}
int I_DEX(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="DEX";
    switch (byte)
    {
    case 0xCA:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    cpu->x_reg--;
    setFlag('N', cpu->x_reg & 0x80, cpu);
    setFlag('Z', cpu->x_reg == 0, cpu);
}
int I_DEY(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="DEY";
    switch (byte)
    {
    case 0x88:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    cpu->y_reg--;
    setFlag('N', cpu->y_reg & 0x80, cpu);
    setFlag('Z', cpu->y_reg == 0, cpu);
}
int I_EOR(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="EOR";
    switch (byte)
    {
    case 0x49:
        ir->addr_mode=IMM; ir->bytes=2; ir->cycles=2;
        break;
    case 0x45:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=3;
        break;
    case 0x55:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=4;
        break;
    case 0x4D:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=4;
        break;
    case 0x5D:
        ir->addr_mode=ABX; ir->bytes=3; ir->cycles=4;
        break;
    case 0x59:
        ir->addr_mode=ABY; ir->bytes=3; ir->cycles=4;
        break;
    case 0x41:
        ir->addr_mode=IDX; ir->bytes=2; ir->cycles=6;
        break;
    case 0x51:
        ir->addr_mode=IDY; ir->bytes=2; ir->cycles=5;
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    uint8_t and_comparison = cpu->fetched_value ^ cpu->acc;
    cpu->acc = and_comparison;
    setFlag('N', and_comparison & 0x80, cpu);
    setFlag('Z', and_comparison == 0, cpu);
}
int I_INC(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="INC";
    switch (byte)
    {
    case 0xE6:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=5;
        break;
    case 0xF6:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=6;
        break;
    case 0xEE:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=6;
        break;
    case 0xFE:
        ir->addr_mode=ABX; ir->bytes=3; ir->cycles=7;
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    uint8_t new_value = cpu->fetched_value + 1;
    cpu_write(cpu, cpu->abs_addr, new_value);
    setFlag('N', new_value & 0x80, cpu);
    setFlag('Z', new_value == 0, cpu);
}
int I_INX(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="INX";
    switch (byte)
    {
    case 0xE8:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    cpu->x_reg++;
    setFlag('N', cpu->x_reg & 0x80, cpu);
    setFlag('Z', cpu->x_reg == 0, cpu);
}
int I_INY(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="INY";
    switch (byte)
    {
    case 0xC8:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    cpu->y_reg++;
    setFlag('N', cpu->y_reg & 0x80, cpu);
    setFlag('Z', cpu->y_reg == 0, cpu);
}
int I_JMP(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="JMP";
    switch (byte)
    {
    case 0x4C:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=3;
        break;
    case 0x6C:
        ir->addr_mode=IND; ir->bytes=3; ir->cycles=5;
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    cpu->pc=cpu->abs_addr;
}
int I_JSR(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="JSR";
    switch (byte)
    {
    case 0x20:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=6;
        break;
    default:
        break;
    }
    cpu_write(cpu, cpu->sp, (cpu->pc >> 8) & 0x00FF );
    cpu->sp--;
    cpu_write(cpu, cpu->sp, cpu->pc & 0x00FF );
    cpu->sp--;
    fetch_target_value(ir->addr_mode, cpu, ir);
    cpu->pc=cpu->abs_addr;;
}
int I_LDA(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="LDA";  
    switch (byte)
    {
    case 0xA9:
        ir->addr_mode=IMM; ir->bytes=2; ir->cycles=2;
        break;
    case 0xA5:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=3;
        break;
    case 0xB5:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=4;
        break;
    case 0xAD:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=4;
        break;
    case 0xBD:
        ir->addr_mode=ABX; ir->bytes=3; ir->cycles=4;
        break;
    case 0xB9:
        ir->addr_mode=ABY; ir->bytes=3; ir->cycles=4;
        break;
    case 0xA1:
        ir->addr_mode=IDX; ir->bytes=2; ir->cycles=6;
        break;
    case 0xB1:
        ir->addr_mode=IDY; ir->bytes=2; ir->cycles=5;
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    cpu->acc = cpu->fetched_value;
    setFlag('N', cpu->fetched_value & 0x80, cpu);
    setFlag('Z', cpu->fetched_value == 0, cpu);
}
int I_LDX(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="LDX";  
    switch (byte)
    {
    case 0xA2:
        ir->addr_mode=IMM; ir->bytes=2; ir->cycles=2;
        break;
    case 0xA6:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=3;
        break;
    case 0xB6:
        ir->addr_mode=ZPY; ir->bytes=2; ir->cycles=4;
        break;
    case 0xAE:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=4;
        break;
    case 0xBE:
        ir->addr_mode=ABY; ir->bytes=3; ir->cycles=4;
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    cpu->x_reg = cpu->fetched_value;
    setFlag('N', cpu->fetched_value & 0x80, cpu);
    setFlag('Z', cpu->fetched_value == 0, cpu);
}
int I_LDY(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="LDY";  
    switch (byte)
    {
    case 0xA0:
        ir->addr_mode=IMM; ir->bytes=2; ir->cycles=2;
        break;
    case 0xA4:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=3;
        break;
    case 0xB4:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=4;
        break;
    case 0xAC:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=4;
        break;
    case 0xBC:
        ir->addr_mode=ABX; ir->bytes=3; ir->cycles=4;
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    cpu->y_reg = cpu->fetched_value;
    setFlag('N', cpu->fetched_value & 0x80, cpu);
    setFlag('Z', cpu->fetched_value == 0, cpu);
}
int I_LSR(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="LSR";  
    switch (byte)
    {
    case 0x4A:
        ir->addr_mode=ACC; ir->bytes=1; ir->cycles=2;
        break;
    case 0x46:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=5;
        break;
    case 0x56:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=6;
        break;
    case 0x4E:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=6;
        break;
    case 0x5E:
        ir->addr_mode=ABX; ir->bytes=3; ir->cycles=7;
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    uint8_t carry_flag_value = cpu->fetched_value & 0x01; //carry flag value is the bit that is getting shifted out
    uint8_t new_value = cpu->fetched_value >> 1;
    if (ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   

    setFlag('N', 0, cpu);
    setFlag('Z', new_value == 0, cpu);
    setFlag('C', carry_flag_value, cpu);
}
int I_NOP(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="NOP";
    switch (byte)
    {
    case 0xEA:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    cpu->pc++;
}
int I_ORA(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="ORA";
    switch (byte)
    {
    case 0x09:
        ir->addr_mode=IMM; ir->bytes=2; ir->cycles=2;
        break;
    case 0x05:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=3;
        break;
    case 0x15:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=4;
        break;
    case 0x0D:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=4;
        break;
    case 0x1D:
        ir->addr_mode=ABX; ir->bytes=3; ir->cycles=4;
        break;
    case 0x19:
        ir->addr_mode=ABY; ir->bytes=3; ir->cycles=4;
        break;
    case 0x01:
        ir->addr_mode=IDX; ir->bytes=2; ir->cycles=6;
        break;
    case 0x11:
        ir->addr_mode=IDY; ir->bytes=2; ir->cycles=5;
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    uint8_t or_comparison = cpu->fetched_value | cpu->acc;
    cpu->acc = or_comparison;
    setFlag('N', or_comparison & 0x80, cpu);
    setFlag('Z', or_comparison == 0, cpu);
}
int I_PHA(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="PHA";
    switch (byte)
    {
    case 0x48:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=3;
        break;
    default:
        break;
    }
    cpu_write(cpu, (uint16_t)cpu->sp, cpu->acc);
    cpu->sp--;
}
int I_PHP(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="PHP";
    switch (byte)
    {
    case 0x08:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=3;
        break;
    default:
        break;
    }
    cpu_write(cpu, (uint16_t)cpu->sp, cpu->status_reg | 0x30);
    cpu->sp--;
}
int I_PLA(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="PLA";
    switch (byte)
    {
    case 0x68:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=4;
        break;
    default:
        break;
    }
    cpu->acc = cpu_read(cpu, (uint16_t)cpu->sp);
    cpu->sp++;
    setFlag('N', cpu->acc & 0x80, cpu);
    setFlag('Z', cpu->acc == 0, cpu);
}
int I_PLP(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="PHP";
    switch (byte)
    {
    case 0x28:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=3;
        break;
    default:
        break;
    }
    cpu->status_reg = cpu_read(cpu, (uint16_t)cpu->sp) & 0xCF;
    cpu->sp++;
}
int I_ROL(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="ROL";  
    switch (byte)
    {
    case 0x2A:
        ir->addr_mode=ACC; ir->bytes=1; ir->cycles=2;
        break;
    case 0x26:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=5;
        break;
    case 0x36:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=6;
        break;
    case 0x2E:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=6;
        break;
    case 0x3E:
        ir->addr_mode=ABX; ir->bytes=3; ir->cycles=7;
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    uint8_t carry_flag_value = cpu->fetched_value & 0x80; //carry flag value is the bit that is getting shifted out
    uint8_t new_value = (cpu->fetched_value << 1) | getFlag('C', cpu); //shift left and insert old carry value
    if (ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   

    setFlag('N', new_value & 0x80, cpu);
    setFlag('Z', new_value == 0, cpu);
    setFlag('C', carry_flag_value, cpu);    //set shifted out bit as new carry
}  
int I_ROR(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="ROR";  
    switch (byte)
    {
    case 0x6A:
        ir->addr_mode=ACC; ir->bytes=1; ir->cycles=2;
        break;
    case 0x66:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=5;
        break;
    case 0x76:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=6;
        break;
    case 0x6E:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=6;
        break;
    case 0x7E:
        ir->addr_mode=ABX; ir->bytes=3; ir->cycles=7;
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    uint8_t carry_flag_value = cpu->fetched_value & 0x01; //carry flag value is the bit that is getting shifted out
    uint8_t new_value = (cpu->fetched_value >> 1) | ((cpu->fetched_value & 0x01 ) << 7); //shift right and insert bit zero on left
    if (ir->addr_mode == ACC) {cpu->acc = new_value;} else {cpu_write(cpu, cpu->abs_addr, new_value);}   

    setFlag('N', new_value & 0x80, cpu);
    setFlag('Z', new_value == 0, cpu);
    setFlag('C', carry_flag_value, cpu);    //set shifted out bit as new carry}
}
int I_RTI(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="RTI";
    switch (byte)
    {
    case 0x40:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=6;
        break;
    default:
        break;
    }
    cpu->sp++;
    cpu->status_reg = cpu_read(cpu, cpu->sp) & 0xCB;    //interrupt flag ignored and b flags
    cpu->sp++;
    cpu->pc = cpu_read(cpu, cpu->sp);
}
int I_RTS(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="RTS";
    switch (byte)
    {
    case 0x60:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=6;
        break;
    default:
        break;
    }
    cpu->sp++;
    uint16_t low_byte = cpu_read(cpu, cpu->sp);
    uint16_t high_byte = cpu_read(cpu, cpu->sp);

    uint16_t trg_addr = low_byte | ((high_byte << 8)  & 0xFF00);
    cpu->pc=trg_addr;
    cpu->pc++;
}
int I_SBC(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="SBC";
    switch (byte)
    {
    case 0xE9:
        ir->addr_mode=IMM; ir->bytes=2; ir->cycles=2;
        break;
    case 0xE5:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=3;
        break;
    case 0xF5:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=4;
        break;
    case 0xED:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=4;
        break;
    case 0xFD:
        ir->addr_mode=ABX; ir->bytes=3; ir->cycles=4;
        break;
    case 0xF9:
        ir->addr_mode=ABY; ir->bytes=3; ir->cycles=4;
        break;
    case 0xE1:
        ir->addr_mode=IDX; ir->bytes=2; ir->cycles=6;
        break;
    case 0xF1:
        ir->addr_mode=IDY; ir->bytes=2; ir->cycles=5;
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);

    uint16_t value_inv = ((uint16_t)cpu->fetched_value) ^ 0x00FF;

    uint16_t result = (uint16_t) cpu->acc + value_inv + (uint16_t) getFlag('C', cpu);
    setFlag('C', result > 255, cpu);
    setFlag('Z', (result & 0x00FF) == 0, cpu);
    setFlag('N', result & 0x80, cpu);
    setFlag('V', ( ( (uint16_t)cpu->acc ^ (uint16_t)result ) & ~( (uint16_t)cpu->acc ^ (uint16_t)cpu->fetched_value ) ) & 0x0080, cpu);
    cpu->acc = (uint8_t) (result & 0x00FF);
}
int I_SEC(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="SEC";
    switch (byte)
    {
    case 0x38:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    cpu->pc += 1;
    setFlag('C', 1, cpu);   //set carry bit
}
int I_SED(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="SED";
    switch (byte)
    {
    case 0xF8:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    setFlag('D', 1, cpu);   //set decimal bit
}
int I_SEI(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="SEI";
    switch (byte)
    {
    case 0x78:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    setFlag('I', 1, cpu);   //set interrupt disable bit
}
int I_STA(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="STA";  
    switch (byte)
    {
    case 0x85:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=3;
        break;
    case 0x95:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=4;
        break;
    case 0x8D:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=4;
        break;
    case 0x9D:
        ir->addr_mode=ABX; ir->bytes=3; ir->cycles=5;
        break;
    case 0x99:
        ir->addr_mode=ABY; ir->bytes=3; ir->cycles=5;
        break;
    case 0x81:
        ir->addr_mode=IDX; ir->bytes=2; ir->cycles=6;
        break;
    case 0x91:
        ir->addr_mode=IDY; ir->bytes=2; ir->cycles=6;
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    cpu_write(cpu, cpu->abs_addr, cpu->acc);
}
int I_STX(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="STX";  
    switch (byte)
    {
    case 0x86:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=3;
        break;
    case 0x96:
        ir->addr_mode=ZPY; ir->bytes=2; ir->cycles=4;
        break;
    case 0x8E:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=4;
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    cpu_write(cpu, cpu->abs_addr, cpu->x_reg);
}
int I_STY(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="STY";  
    switch (byte)
    {
    case 0x84:
        ir->addr_mode=ZPG; ir->bytes=2; ir->cycles=3;
        break;
    case 0x94:
        ir->addr_mode=ZPX; ir->bytes=2; ir->cycles=4;
        break;
    case 0x8C:
        ir->addr_mode=ABS; ir->bytes=3; ir->cycles=4;
        break;
    default:
        break;
    }
    fetch_target_value(ir->addr_mode, cpu, ir);
    cpu_write(cpu, cpu->abs_addr, cpu->y_reg);
}
int I_TAX(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="TAX";
    switch (byte)
    {
    case 0xAA:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    cpu->x_reg = cpu->acc;
    setFlag('N', cpu->x_reg & 0x80, cpu);
    setFlag('Z', cpu->x_reg == 0, cpu);
}
int I_TAY(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="TAY";
    switch (byte)
    {
    case 0xA8:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    cpu->y_reg = cpu->acc;
    setFlag('N', cpu->y_reg & 0x80, cpu);
    setFlag('Z', cpu->y_reg == 0, cpu);
}
int I_TSX(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="TSX";
    switch (byte)
    {
    case 0xBA:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    cpu->x_reg = cpu->sp;
    setFlag('N', cpu->x_reg & 0x80, cpu);
    setFlag('Z', cpu->x_reg == 0, cpu);
}
int I_TXA(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="TXA";
    switch (byte)
    {
    case 0x8A:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    cpu->acc = cpu->x_reg;
    setFlag('N', cpu->x_reg & 0x80, cpu);
    setFlag('Z', cpu->x_reg == 0, cpu);
}
int I_TXS(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="TXS";
    switch (byte)
    {
    case 0x9A:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    cpu->sp = cpu->x_reg;
}
int I_TYA(uint8_t byte, Processor *cpu, Ir *ir) {
    ir->opcode_mnemonic="TYA";
    switch (byte)
    {
    case 0x98:
        ir->addr_mode=IMP; ir->bytes=1; ir->cycles=2;
        break;
    default:
        break;
    }
    cpu->acc = cpu->y_reg;
    setFlag('N', cpu->y_reg & 0x80, cpu);
    setFlag('Z', cpu->y_reg == 0, cpu);
}

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
