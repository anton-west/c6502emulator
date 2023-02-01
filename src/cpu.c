#include <stdio.h>
#include <stdarg.h>

#include "cpu.h"
#include "opcodes.h"

void init_cpu(Processor *cpu) {
    cpu_reset(cpu);
}

void cpu_reset(Processor *cpu) {
    //reset vector: $FFFC - $FFFD (LB, HB)
    uint16_t abs_addr = 0xFFFC;
    uint16_t low_byte = cpu_read(cpu, abs_addr);
    uint16_t high_byte = cpu_read(cpu, abs_addr + 1);
    //set pc to value found in reset vector
    cpu->pc = (high_byte << 8) | low_byte;

    //reset registers
    cpu->acc = 0;
    cpu->x_reg = 0;
    cpu->y_reg = 0;

    //The internal state of the CPU now shows that SP is 0xFD,
    //because it got decremented 3 times from 0x00 to 0xFD during reset.
    cpu->sp = 0xFD;
    cpu->status_reg = 0 | flag_U | flag_I;   //unused flag is always high when status reg is pushed to stack, can be high in all the time as it has no effect on cpu operation

    //clear helper variables
    cpu->abs_addr = 0;
    cpu->fetched_value = 0;
    
    //reset takes 8 cycles
    cpu->cycles = 8;
}

//interrupt request
void cpu_irq(Processor *cpu) {
    
    //check interrupt disable is not active
    if (getFlag('I', cpu) == 0) {

        //push pc to stack, HB, then LB
        cpu_write(cpu, 0x1000 | cpu->sp, (cpu->pc >> 8));
        cpu->sp--;
        cpu_write(cpu, 0x1000 | cpu->sp, (cpu->pc) & 0x00FF);
        cpu->sp--;

        //push status reg to stack
        setFlag('B',0,cpu); //B flag is 0 when pushed by hardware interrupt like irq
        setFlag('I',1,cpu);
        cpu_write(cpu, 0x0100 | cpu->sp, cpu->status_reg);
        cpu->sp--;

        //read new program counter from interrupt vector $FFFE - $FFFF (LB, HB)
        uint16_t abs_addr = 0xFFFE;
        uint16_t low_byte = cpu_read(cpu, abs_addr);
        uint16_t high_byte = cpu_read(cpu, abs_addr + 1);
        //set pc to value found in reset vector
        cpu->pc = (high_byte << 8) | low_byte;

        //interrupt takes 7 cycles
        cpu->cycles = 7;
    }
}

//non-maskable interrupt
void cpu_nmi(Processor *cpu) {
    //push pc to stack, HB, then LB
    cpu_write(cpu, 0x1000 | cpu->sp, (cpu->pc >> 8));
    cpu->sp--;
    cpu_write(cpu, 0x1000 | cpu->sp, (cpu->pc) & 0x00FF);
    cpu->sp--;

    //push status reg to stack
    setFlag('B',0,cpu); //B flag is 0 when pushed by hardware interrupt like irq
    setFlag('I',1,cpu);
    cpu_write(cpu, 0x0100 | cpu->sp, cpu->status_reg);
    cpu->sp--;

    //read new program counter from interrupt vector $FFFA - $FFFB (LB, HB)
    uint16_t abs_addr = 0xFFFA;
    uint16_t low_byte = cpu_read(cpu, abs_addr);
    uint16_t high_byte = cpu_read(cpu, abs_addr + 1);
    //set pc to value found in reset vector
    cpu->pc = (high_byte << 8) | low_byte;

    //interrupt takes 8 cycles
    cpu->cycles = 8;
}

void cpu_set_memory(Processor *cpu, uint8_t *memory) {
    cpu->memory = memory;
}

uint8_t cpu_read(Processor *cpu, uint16_t addr) {
    return cpu->memory[addr];
}

uint8_t cpu_write(Processor *cpu, uint16_t addr, uint8_t value) {
    cpu->memory[addr] = value;
    return cpu->memory[addr];
}

int cpu_clock(Processor *cpu, InstrInfo *ir) {

    //if cycle == 0, fetch next instruction at program counter location in memory
    if (cpu->cycles == 0) {
        uint8_t instruction = cpu_read(cpu, cpu->pc);
        InstrInfo ir_temp = decode_instruction(instruction); //pc is incremented here by necessary amount
        (ir_temp.fnc_ptr)(cpu, &ir_temp);
        cpu->cycles += ir_temp.n_cycles;
        *ir = ir_temp;
    }
    cpu->cycles--;
    return cpu->cycles;
}

/*  set status register flag
    if value is nonzero, flag is set to 1
    if value is zero, flag is set to 0

    return 0 on success,
    nonzero if invalid flag was specified
    
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
    (https://www.masswerk.at/6502/6502_instruction_set.html)
*/

int setFlag(const char flag, uint16_t value, Processor *cpu) {

    if (value >= 1) { value = 1; } else { value = 0; }  //clamp value to 1 or 0

    switch (flag) 
    {
    case 'N':
        cpu->status_reg = (cpu->status_reg & ~(1U << 7)) | (value << 7);
        break;
    case 'V':
        cpu->status_reg = (cpu->status_reg & ~(1U << 6)) | (value << 6);
        break;
    case 'B':
        cpu->status_reg = (cpu->status_reg & ~(1U << 4)) | (value << 4);
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

int getFlag(const char flag, Processor *cpu) {
    switch (flag) 
    {
    case 'N':
        return (cpu->status_reg >> 7) & 1U;
    case 'V':
        return (cpu->status_reg >> 6) & 1U;
    case 'B':
        return (cpu->status_reg >> 4) & 1U; 
    case 'D':
        return (cpu->status_reg >> 3) & 1U;
    case 'I':
        return (cpu->status_reg >> 2) & 1U;
    case 'Z':
        return (cpu->status_reg >> 1) & 1U;
    case 'C':
        return cpu->status_reg & 1U; 
        break;
    default:
        return -1;
    }
}
