#ifndef CPU_H
#define CPU_H

#include "stdint.h"

typedef struct Processor {
    uint16_t pc;
    uint8_t sp;
    uint8_t status_reg; //OBS! only 6 bits are used

    uint8_t x_reg;
    uint8_t y_reg;

    uint8_t acc;

    uint8_t *memory;
} Processor;

void init_cpu(Processor *cpu);

void set_memory(Processor *cpu, uint8_t *memory);

uint8_t* read(Processor *cpu, uint8_t *memory);
#endif
