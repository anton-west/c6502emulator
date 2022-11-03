#include <stdio.h>
#include <stdint.h>

#include "cpu.h"

#define MAX_MEMORY_ADDR 65536

int main(void) {
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[10] = 123;

    Cpu cpu;

    init_cpu(&cpu);
    init_memory(&cpu, memory);

    cpu.pc = 10;
    cpu.sp = 11;
    cpu.s_reg = 12;

    printf("%d\n", cpu.memory[cpu.pc]);
}
