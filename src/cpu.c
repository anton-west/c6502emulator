#include <stdio.h>
#include <stdarg.h>

#include "cpu.h"
#include "opcodes.h"

void init_cpu(Processor *cpu) {

    //read pc from reset vector (FFFC LB - FFFD HB)
    uint16_t pc_low = cpu_read(cpu, 0xFFFC);
    uint16_t pc_high = cpu_read(cpu, 0xFFFD);
    cpu->pc = (pc_low & 0x00FF) | (pc_high << 8);
    cpu->pc = 0;
    
    cpu->sp=0xFD;
    cpu->status_reg=0;    //TODO: this should go here: 0 | 0x20; is how status register is initialized on actual 6502

    cpu->x_reg=0;
    cpu->y_reg=0;

    cpu->acc=0;

    cpu->cycles=0;
    cpu->abs_addr=0;
    cpu->fetched_value=0;
}

void reset_cpu(Processor *cpu) {
    
    //read pc from reset vector (FFFC LB - FFFD HB)
    uint16_t pc_low = cpu_read(cpu, 0xFFFC);
    uint16_t pc_high = cpu_read(cpu, 0xFFFD);
    cpu->pc = (pc_low & 0x00FF) | (pc_high << 8);

    cpu->acc = 0;
    cpu->x_reg = 0;
    cpu->y_reg = 0;

    cpu->sp = 0xFD;
    cpu->status_reg = 0;

    //reset take 7 to 8 cycles, TODO: check exact value!
    cpu->cycles = 8;
}

void cpu_set_memory(Processor *cpu, uint8_t *memory) {
    cpu->memory = memory;
}

uint8_t cpu_read(Processor *cpu, uint16_t addr) {
    return cpu->memory[addr];
}

uint8_t cpu_write(Processor *cpu, uint16_t addr, uint8_t value) {
    cpu->memory[addr] = value;
    return cpu->memory[addr];
}

int cpu_clock(Processor *cpu, InstrInfo *ir) {

    //if cycle == 0, fetch next instruction at program counter location in memory
    if (cpu->cycles == 0) {
        uint8_t instruction = cpu_read(cpu, cpu->pc);
        InstrInfo ir_temp = decode_instruction(instruction); //pc is incremented here by necessary amount
        (ir_temp.fnc_ptr)(cpu, &ir_temp);
        cpu->cycles += ir_temp.n_cycles;
        *ir = ir_temp;
    }
    cpu->cycles--;
    return cpu->cycles;
}

/*  set status register flag
    if value is nonzero, flag is set to 1
    if value is zero, flag is set to 0

    return 0 on success,
    nonzero if invalid flag was specified
    
    7  bit  0
    ---- ----
    NVsB DIZC
    |||| ||||
    |||| |||+- Carry
    |||| ||+-- Zero
    |||| |+--- Interrupt Disable
    |||| +---- Decimal
    ||++------ No CPU effect, see: the B flag
    |+-------- Overflow
    +--------- Negative
*/
int setFlag(const char flag, uint16_t value, Processor *cpu) {

    if (value >= 1) { value = 1; } else { value = 0; }  //clamp value to 1 or 0

    switch (flag) 
    {
    case 'N':
        cpu->status_reg = (cpu->status_reg & ~(1U << 7)) | (value << 7);
        break;
    case 'V':
        cpu->status_reg = (cpu->status_reg & ~(1U << 6)) | (value << 6);
        break;
    case 'B':
        cpu->status_reg = (cpu->status_reg & ~(1U << 4)) | (value << 4);
        break;
    case 'D':
        cpu->status_reg = (cpu->status_reg & ~(1U << 3)) | (value << 3);
        break;
    case 'I':
        cpu->status_reg = (cpu->status_reg & ~(1U << 2)) | (value << 2);
        break;
    case 'Z':
        cpu->status_reg = (cpu->status_reg & ~(1U << 1)) | (value << 1);
        break;
    case 'C':
        cpu->status_reg = (cpu->status_reg & ~(1U << 0)) | (value << 0);
        break;
    default:
        return -1;
    }
    return 0;
}

int getFlag(const char flag, Processor *cpu) {
    switch (flag) 
    {
    case 'N':
        return (cpu->status_reg >> 7) & 1U;
    case 'V':
        return (cpu->status_reg >> 6) & 1U;
    case 'D':
        return (cpu->status_reg >> 3) & 1U;
    case 'I':
        return (cpu->status_reg >> 2) & 1U;
    case 'Z':
        return (cpu->status_reg >> 1) & 1U;
    case 'C':
        return cpu->status_reg & 1U; 
        break;
    default:
        return -1;
    }
}
