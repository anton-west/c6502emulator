#include "cpu.h"
#include "opcodes.h"

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

uint8_t read(Processor *cpu) {
    uint16_t index = cpu->pc;
    return cpu->memory[index];
}

int clock(Processor *cpu) {
    static int cycles = 0;

    //if cycle == 0, fetch next instruction at program counter location in memory
    if (cycles == 0) {
        uint8_t instruction = read(cpu);
        cpu->pc += 1;   //read from memory, so increment pc
        opcode *ptr = decode_instruction(instruction);
    }

}

/* set status register flag
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
int setFlags(const char* flags, int n_flags, uint16_t value, Processor *cpu) {
    if (value >= 1) {
        value = 1;
    } else {
        value = 0;
    }

    for (int i = 0; flags[i] != '\0'; i++) {
        char flag = flags[i];

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
}