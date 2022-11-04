#include <stdio.h>
#include <stdint.h>

#include "cpu.h"

#define MAX_MEMORY_ADDR 65536

int main(void) {
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[10] = 123;

    Processor cpu;

    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.pc = 10;
    cpu.sp = 11;
    cpu.status_reg = 12;

    printf("%d\n", cpu.memory[cpu.pc]);
    printf("%d\n", cpu.memory[cpu.pc + 1]);
}
