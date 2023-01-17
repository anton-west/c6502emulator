#include <stdio.h>

#include "cpu.h"
#include "opcodes.h"

void init_cpu(Processor* cpu) {
    cpu->pc=0;
    cpu->sp=0;
    cpu->status_reg=0;

    cpu->x_reg=0;
    cpu->y_reg=0;

    cpu->acc=0;

    cpu->cycles=0;
}

void set_memory(Processor *cpu, uint8_t *memory) {
    cpu->memory = memory;
}

uint8_t read(Processor *cpu, uint16_t addr) {
    return cpu->memory[addr];
}

uint8_t write(Processor *cpu, uint16_t addr, uint8_t value) {
    cpu->memory[addr] = value;
    return cpu->memory[addr];
}

/*tells cpu to read value from memory at program counter index
uint8_t read(Processor *cpu) {
    return cpu->memory[cpu->pc];
}
*/

//fetches byte from memory and increments program counter by 1
uint8_t fetch(Processor *cpu) {
    uint8_t read_byte = read(cpu, cpu->pc);
    cpu->pc += 1;
    return read_byte;
}

int clock(Processor *cpu) {

    //if cycle == 0, fetch next instruction at program counter location in memory
    if (cpu->cycles == 0) {
        Ir ir = {0};
        uint8_t instruction = read(cpu, cpu->pc);
        opcode ptr = decode_instruction(instruction); //pc is incremented here by necessary amount
        (ptr)(instruction, cpu, &ir);
        cpu->cycles += ir.cycles;
    }
    cpu->cycles --;
    return cpu->cycles;
}

/*  set status register flag
    if value is nonzero, flag is set to 1
    if value is zero, flag is set to 0

    return 0 on success,
    nonzero if invalid flag was specified
    
    7  bit  0
    ---- ----
    NVss DIZC
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
