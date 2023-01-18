#ifndef CPU_H
#define CPU_H

#include "stdint.h"

typedef struct Processor {
    uint16_t pc;
    uint8_t sp;

    /* 
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
    uint8_t status_reg;

    uint8_t x_reg;
    uint8_t y_reg;

    uint8_t acc;

    uint8_t *memory;

    unsigned int cycles;
} Processor;

void init_cpu(Processor *cpu);

void set_memory(Processor *cpu, uint8_t *memory);

uint8_t cpu_fetch(Processor *cpu);

uint8_t cpu_read(Processor *cpu, uint16_t addr);

uint8_t cpu_write(Processor *cpu, uint16_t addr, uint8_t value);

int cpu_clock(Processor *cpu);

int setFlag(const char flag, uint16_t value, Processor *cpu);

int getFlag(const char flag, Processor *cpu);

#endif
