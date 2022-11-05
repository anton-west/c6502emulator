#include "cpu.h"

void init_cpu(Processor* cpu) {
    cpu->pc=0;
    cpu->sp=0;
    cpu->status_reg=0;

    cpu->x_reg=0;
    cpu->y_reg=0;

    cpu->acc=0;
}

void set_memory(Processor *cpu, uint8_t *memory) {
    cpu->memory = memory;
}

uint8_t cpu_read(Processor *cpu) {
    uint16_t index = cpu->pc;
    return cpu->memory[index];
}