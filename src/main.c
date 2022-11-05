#include <stdio.h>
#include <stdint.h>

#include "cpu.h"
#include "opcodes.h"

#define MAX_MEMORY_ADDR 65536

int main(void) {
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0x69;
    memory[1] = 0x65;
    memory[2] = 0x75;
    memory[3] = 0x6D;
    memory[4] = 0x7D;
    memory[5] = 0x79;
    memory[6] = 0x61;
    memory[7] = 0x71;

    Processor cpu;

    init_cpu(&cpu);
    set_memory(&cpu, memory);

    for (int i = 0; i<8; i++) {
        uint8_t byte = read(&cpu);
        opcode ptr;
        ptr = decode(byte);
        ptr(byte, &cpu);
    }
}
