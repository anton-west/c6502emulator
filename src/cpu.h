#ifndef CPU_H
#define CPU_H

#include "stdint.h"

typedef struct Cpu {
    uint16_t pc;
    uint8_t sp;
    uint8_t s_reg; //OBS! only 6 bits are used

    uint8_t x_reg;
    uint8_t y_reg;

    uint8_t acc;

    uint8_t* memory;
} Cpu;

void init_cpu(Cpu* cpu);

void init_memory(Cpu* cpu, uint8_t const* memory);
#endif
