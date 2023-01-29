#ifndef CPU_H
#define CPU_H

#include "stdint.h"

//struct to hold information of opcode execution

typedef struct Processor {
    uint16_t pc;
    uint8_t sp;

    /* 
    7  bit  0
    ---- ----
    NVsB DIZC
    |||| ||||
    |||| |||+- Carry
    |||| ||+-- Zero
    |||| |+--- Interrupt Disable
    |||| +---- Decimal
    ||++------ No CPU effect, see: the B flag
    |+-------- Overflow
    +--------- Negative
    */

    /*
    The break flag (B) is not an actual flag implemented in a register, and rather appears only,
    when the status register is pushed onto or pulled from the stack.
    When pushed, it will be 1 when transfered by a BRK or PHP instruction,
    and zero otherwise (i.e., when pushed by a hardware interrupt).
    When pulled into the status register (by PLP or on RTI), it will be ignored.
    In other words, the break flag will be inserted,
    whenever the status register is transferred to the stack by software (BRK or PHP),
    and will be zero, when transferred by hardware.
    Since there is no actual slot for the break flag, it will be always ignored, when retrieved
    (PLP or RTI). The break flag is not accessed by the CPU at anytime and there is no internal representation.
    Its purpose is more for patching,
    to discern an interrupt caused by a BRK instruction from a normal interrupt initiated by hardware.
    */

    uint8_t status_reg;

    uint8_t x_reg;
    uint8_t y_reg;

    uint8_t acc;

    uint8_t *memory;

    unsigned int cycles; //the current instruction cycle that the cpu is on
    uint16_t abs_addr; //set this to the absolute address that the next instruction will act upon
    uint8_t fetched_value;  //this is the value that is returned when reading the value at memory location abs_addr

} Processor;

typedef enum address_mode {UDF, ACC, ABS, ABX, ABY, IMM, IMP, IND, IDX, IDY, REL, ZPG, ZPX, ZPY} address_mode;

typedef struct InstructionInfo {
    void (*fnc_ptr) (Processor *cpu, struct InstructionInfo *ir);      //equivalent to opcode function ptr signature
    address_mode addr_mode;
    unsigned int n_bytes;
    unsigned int n_cycles;
    char opcode_mnemonic[4];
} InstrInfo;

void init_cpu(Processor *cpu);

void cpu_set_memory(Processor *cpu, uint8_t *memory);

uint8_t cpu_read(Processor *cpu, uint16_t addr);

uint8_t cpu_write(Processor *cpu, uint16_t addr, uint8_t value);

int cpu_clock(Processor *cpu, InstrInfo *ir);

int setFlag(const char flag, uint16_t value, Processor *cpu);

int getFlag(const char flag, Processor *cpu);

#endif
