#include "cpu.h"

void init_cpu(Cpu* cpu) {
    cpu->pc=0;
    cpu->sp=0;
    cpu->s_reg=0;

    cpu->x_reg=0;
    cpu->y_reg=0;

    cpu->acc=0;
}

void init_memory(Cpu* cpu, uint8_t const* memory) {
    cpu->memory = memory;
}