#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/cpu.h"
#include "../src/opcodes.h"

#define MAX_MEMORY_ADDR 65536

/* A test case that does nothing and succeeds.
static void null_test_success(void **state) {
    (void) state;
*/

static void get_target_address_IMM_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(IMM, &cpu, &ir);

    assert_int_equal(cpu.pc, 2);
    assert_int_equal(trg_addr, 1);
}

static void get_target_address_IMM_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.pc = 0x3232;
    assert_int_equal(cpu.pc, 0x3232); 

    uint16_t trg_addr = get_target_address(IMM, &cpu, &ir);

    assert_int_equal(cpu.pc, 0x3232 + 2);
    assert_int_equal(trg_addr, 0x3232 + 1);
}

static void get_target_address_IMM_3(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.pc = 0xFFFF;
    assert_int_equal(cpu.pc, 0xFFFF); 

    uint16_t trg_addr = get_target_address(IMM, &cpu, &ir);

    assert_int_equal(cpu.pc, 1);    //should roll over from 0xFFFF to 1
    assert_int_equal(trg_addr, 0);  //should roll over from 0xFFFF to 0
}

static void get_target_address_ACC(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(ACC, &cpu, &ir);

    assert_int_equal(cpu.pc, 1);
    assert_int_equal(trg_addr, 0);
}

static void get_target_address_REL_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};

    memory[1] = 69;

    init_cpu(&cpu);
    set_memory(&cpu, memory);

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(REL, &cpu, &ir);

    assert_int_equal(cpu.pc, 0);
    assert_int_equal(trg_addr, 71); //71 = pc + 69 + 2 (2 pc increments in retrieval)

    //here pc should be set to pc = trg_addr in a real program
}

static void get_target_address_REL_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};

    memory[0x0100] = 0x33;

    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.pc = 0x00FF;

    assert_int_equal(cpu.pc, 0x00FF); 

    uint16_t trg_addr = get_target_address(REL, &cpu, &ir);

    assert_int_equal(cpu.pc, 0x00FF);    //does not get increment in this address mode, should be done in opcode implementation
    assert_int_equal(trg_addr, cpu.pc + 0x33 + 2);  //pc + 0x33 + 2 (2 pc increments in retrieval)
}

static void get_target_address_ZPG_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};

    memory[1] = 50;

    init_cpu(&cpu);
    set_memory(&cpu, memory);

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(ZPG, &cpu, &ir);

    assert_int_equal(cpu.pc, 2);
    assert_int_equal(trg_addr, 50);
}

static void get_target_address_ZPG_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};

    memory[1] = 0xFF;

    init_cpu(&cpu);
    set_memory(&cpu, memory);

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(ZPG, &cpu, &ir);

    assert_int_equal(cpu.pc, 2);
    assert_int_equal(trg_addr, 0xFF);
    assert_int_equal(trg_addr & 0xFF00, 0);
}

static void get_target_address_ZPX_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};

    memory[1] = 10;

    init_cpu(&cpu);
    set_memory(&cpu, memory);
    
    cpu.x_reg = 10;

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(ZPX, &cpu, &ir);

    assert_int_equal(cpu.pc, 2);
    assert_int_equal(trg_addr, 20);
    assert_int_equal(trg_addr & 0xFF00, 0);
}

static void get_target_address_ZPX_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};

    memory[1] = 0xFF;

    init_cpu(&cpu);
    set_memory(&cpu, memory);
    
    cpu.x_reg = 10;

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(ZPX, &cpu, &ir);

    assert_int_equal(cpu.pc, 2);
    assert_int_equal(trg_addr, 9);  //0xFF + 10 = 0x09
    assert_int_equal(trg_addr & 0xFF00, 0);
}

static void get_target_address_ZPY_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};

    memory[1] = 10;

    init_cpu(&cpu);
    set_memory(&cpu, memory);
    
    cpu.y_reg = 10;

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(ZPY, &cpu, &ir);

    assert_int_equal(cpu.pc, 2);
    assert_int_equal(trg_addr, 20);
    assert_int_equal(trg_addr & 0xFF00, 0);
}

static void get_target_address_ZPY_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};

    memory[1] = 0xFF;

    init_cpu(&cpu);
    set_memory(&cpu, memory);
    
    cpu.y_reg = 10;

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(ZPY, &cpu, &ir);

    assert_int_equal(cpu.pc, 2);
    assert_int_equal(trg_addr, 9);  //0xFF + 10 = 0x09
    assert_int_equal(trg_addr & 0xFF00, 0);
}

static void get_target_address_ABS(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};

    memory[1] = 0x22;   //low byte
    memory[2] = 0x33;   //high byte

    init_cpu(&cpu);
    set_memory(&cpu, memory);

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(ABS, &cpu, &ir);

    assert_int_equal(cpu.pc, 3);
    assert_int_equal(trg_addr, 0x3322);
}

static void get_target_address_ABX_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};

    memory[1] = 0x22;   //low byte
    memory[2] = 0x33;   //high byte

    init_cpu(&cpu);
    set_memory(&cpu, memory);
    
    cpu.x_reg = 0;

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(ABX, &cpu, &ir);

    assert_int_equal(cpu.pc, 3);
    assert_int_equal(trg_addr, 0x3322);
    assert_int_equal(ir.cycles, 0); //no extra cycle should be added
}

static void get_target_address_ABX_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};

    memory[1] = 0x22;   //low byte
    memory[2] = 0x33;   //high byte

    init_cpu(&cpu);
    set_memory(&cpu, memory);
    
    cpu.x_reg = 0x22;

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(ABX, &cpu, &ir);

    assert_int_equal(cpu.pc, 3);
    assert_int_equal(trg_addr, 0x3322 + 0x22);
    assert_int_equal(ir.cycles, 0); //no extra cycle should be added
}

static void get_target_address_ABX_3(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};

    memory[1] = 0xFF;   //low byte
    memory[2] = 0x33;   //high byte

    init_cpu(&cpu);
    set_memory(&cpu, memory);
    
    cpu.x_reg = 0x01;

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(ABX, &cpu, &ir);

    assert_int_equal(cpu.pc, 3);
    assert_int_equal(trg_addr, 0x33FF + 0x01);
    assert_int_equal(ir.cycles, 1); //extra cycle should be added
}

static void get_target_address_ABY_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};

    memory[1] = 0x22;   //low byte
    memory[2] = 0x33;   //high byte

    init_cpu(&cpu);
    set_memory(&cpu, memory);
    
    cpu.y_reg = 0;

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(ABY, &cpu, &ir);
    assert_int_equal(cpu.pc, 3);
    assert_int_equal(trg_addr, 0x3322);
    assert_int_equal(ir.cycles, 0); //no extra cycle should be added
}

static void get_target_address_ABY_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};

    memory[1] = 0x22;   //low byte
    memory[2] = 0x33;   //high byte

    init_cpu(&cpu);
    set_memory(&cpu, memory);
    
    cpu.y_reg = 0x22;

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(ABY, &cpu, &ir);

    assert_int_equal(cpu.pc, 3);
    assert_int_equal(trg_addr, 0x3322 + 0x22);
    assert_int_equal(ir.cycles, 0); //no extra cycle should be added
}

static void get_target_address_ABY_3(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};

    memory[1] = 0xFF;   //low byte
    memory[2] = 0x33;   //high byte

    init_cpu(&cpu);
    set_memory(&cpu, memory);
    
    cpu.y_reg = 0x01;

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(ABY, &cpu, &ir);

    assert_int_equal(cpu.pc, 3);
    assert_int_equal(trg_addr, 0x33FF + 0x01);
    assert_int_equal(ir.cycles, 1); //extra cycle should be added
}

static void get_target_address_IND(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir= {0};

    memory[1] = 0x22;   //low byte
    memory[2] = 0x33;   //high byte

    memory[0x3322] = 0x55;    //target address is 0x55

    init_cpu(&cpu);
    set_memory(&cpu, memory);

    assert_int_equal(cpu.pc, 0); 

    uint16_t trg_addr = get_target_address(IND, &cpu, &ir);

    assert_int_equal(cpu.pc, 3);
    assert_int_equal(trg_addr, 0x55);
}

static void get_target_address_IDX_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir = {0};

    memory[1] = 0x55;   //read zero page address
    memory[0x55 + 0x35] = 0xAA;
    memory[0x55 + 0x35 + 1] = 0xBB;
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.x_reg = 0x35;
    assert_int_equal(cpu.pc, 0); 
    uint16_t trg_addr = get_target_address(IDX, &cpu, &ir);
    assert_int_equal(cpu.pc, 2);
    assert_int_equal(trg_addr, 0xBBAA);
}

static void get_target_address_IDX_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir = {0};

    memory[1] = 0xFF;   //read zero page address
    memory[(0xFF + 0x35) & 0x00FF] = 0xAA;
    memory[(0xFF + 0x35 + 1) & 0x00FF] = 0xBB;
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.x_reg = 0x35;
    assert_int_equal(cpu.pc, 0); 
    uint16_t trg_addr = get_target_address(IDX, &cpu, &ir);
    assert_int_equal(cpu.pc, 2);
    assert_int_equal(trg_addr, 0xBBAA);
}

static void get_target_address_IDY_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir = {0};

    memory[1] = 0x76;   //read zero page address
    memory[0x76] = 0xAA;
    memory[0x77] = 0xBB;
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.y_reg = 0x35;
    assert_int_equal(cpu.pc, 0); 
    uint16_t trg_addr = get_target_address(IDY, &cpu, &ir);
    assert_int_equal(cpu.pc, 2);
    assert_int_equal(trg_addr, 0xBBAA + 0x35);
    assert_int_equal(ir.cycles, 0);
}

static void get_target_address_IDY_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir = {0};

    memory[1] = 0xFF;   //read zero page address
    memory[0xFF] = 0xAA;
    memory[0x00] = 0xBB;
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.y_reg = 0x35;
    assert_int_equal(cpu.pc, 0); 
    uint16_t trg_addr = get_target_address(IDY, &cpu, &ir);
    assert_int_equal(cpu.pc, 2);
    assert_int_equal(trg_addr, 0xBBAA + 0x35);
    assert_int_equal(ir.cycles, 0);
}

static void get_target_address_IDY_3(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    Ir ir = {0};

    memory[1] = 0xFF;   //read zero page address
    memory[0xFF] = 0xFF;
    memory[0x00] = 0xBB;
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.y_reg = 0x35;
    assert_int_equal(cpu.pc, 0); 
    uint16_t trg_addr = get_target_address(IDY, &cpu, &ir);
    assert_int_equal(cpu.pc, 2);
    assert_int_equal(trg_addr, 0xBBFF + 0x35);
    assert_int_equal(ir.cycles, 1);
}

// LDA TESTS

static void LDA_IMM_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xA9;
    memory[1] = 0xAA;

    Processor cpu = {0};
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    assert_int_equal(cpu.status_reg, 0);

    clock(&cpu);

    assert_int_equal(cpu.acc, 0xAA);
    assert_int_equal(getFlag('N',&cpu), 1);
}

static void LDA_IMM_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xA9;
    memory[1] = 0x00;

    Processor cpu = {0};
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.acc = 0xFF;

    assert_int_equal(cpu.status_reg, 0);
    
    clock(&cpu);

    assert_int_equal(cpu.acc, 0x00);
    assert_int_equal(getFlag('Z',&cpu), 1);
}


static void LDA_ZEP_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xA5;
    memory[1] = 0x55;
    memory[0x55] = 0xF0;

    Processor cpu = {0};;
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    assert_int_equal(cpu.status_reg, 0);

    clock(&cpu);

    assert_int_equal(cpu.acc, 0xF0);
    assert_int_equal(getFlag('N',&cpu), 1);
}

static void LDA_ZEP_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xA5;
    memory[1] = 0x55;
    memory[0x55] = 0x00;

    Processor cpu = {0};;
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.acc = 0xFF;

    assert_int_equal(cpu.status_reg, 0);

    clock(&cpu);

    assert_int_equal(cpu.acc, 0x00);
    assert_int_equal(getFlag('Z',&cpu), 1);
}

static void LDA_ZPX_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xB5;
    memory[1] = 0x55;
    memory[0x55 + 10] = 0xF0;

    Processor cpu = {0};;
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.x_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    clock(&cpu);

    assert_int_equal(cpu.acc, 0xF0);
    assert_int_equal(getFlag('N',&cpu), 1);
}

static void LDA_ZPX_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xB5;
    memory[1] = 0xFF;
    memory[(0xFF + 10) & 0x00FF] = 0xF0;

    Processor cpu = {0};;
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.x_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    clock(&cpu);

    assert_int_equal(cpu.acc, 0xF0);
    assert_int_equal(getFlag('N',&cpu), 1);
}

static void LDA_ABS_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xAD;
    memory[1] = 0x55;
    memory[2] = 0xBB;
    memory[0xBB55] = 0xF0;

    Processor cpu = {0};;
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    assert_int_equal(cpu.status_reg, 0);

    clock(&cpu);

    assert_int_equal(cpu.acc, 0xF0);
    assert_int_equal(getFlag('N',&cpu), 1);
}

static void LDA_ABS_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xAD;
    memory[1] = 0xFF;
    memory[2] = 0xFF;
    memory[0xFFFF] = 0xF0;

    Processor cpu = {0};;
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    assert_int_equal(cpu.status_reg, 0);

    clock(&cpu);

    assert_int_equal(cpu.acc, 0xF0);
    assert_int_equal(getFlag('N',&cpu), 1);
}

static void LDA_ABX_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xBD;
    memory[1] = 0x55;
    memory[2] = 0xBB;
    memory[0xBB55 + 10] = 0xF0;

    Processor cpu = {0};;
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.x_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    clock(&cpu);

    assert_int_equal(cpu.acc, 0xF0);
    assert_int_equal(getFlag('N',&cpu), 1);
}

static void LDA_ABX_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xAD;
    memory[1] = 0xFF;
    memory[2] = 0xFF;
    memory[0xFFFF] = 0xF0;

    Processor cpu = {0};;
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.x_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    clock(&cpu);

    assert_int_equal(cpu.acc, 0xF0);
    assert_int_equal(getFlag('N',&cpu), 1);
}

static void LDA_ABY_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xB9;
    memory[1] = 0x55;
    memory[2] = 0xBB;
    memory[0xBB55 + 10] = 0xF0;

    Processor cpu = {0};;
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.y_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    clock(&cpu);

    assert_int_equal(cpu.acc, 0xF0);
    assert_int_equal(getFlag('N',&cpu), 1);
}

static void LDA_ABY_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xB9;
    memory[1] = 0xFF;
    memory[2] = 0x55;
    memory[0x55FF+10] = 0xF0;

    Processor cpu = {0};;
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.y_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    clock(&cpu);

    assert_int_equal(cpu.acc, 0xF0);
    assert_int_equal(getFlag('N',&cpu), 1);
}

static void LDA_IDX_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xA1;
    memory[1] = 0x50;
    memory[0x50+10] = 0xBB;
    memory[0x51+10] = 0xAA;
    memory[0xAABB] = 0xF0;
    Processor cpu = {0};
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.x_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    clock(&cpu);

    assert_int_equal(cpu.acc, 0xF0);
    assert_int_equal(getFlag('N',&cpu), 1);
}

static void LDA_IDX_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xA1;
    memory[1] = 0xFF;
    memory[(0xFF+10) & 0x00FF] = 0xBB;
    memory[(0xFF+10 + 1) & 0x00FF] = 0xAA;
    memory[0xAABB] = 0xF0;
    Processor cpu = {0};
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.x_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    clock(&cpu);

    assert_int_equal(cpu.acc, 0xF0);
    assert_int_equal(getFlag('N',&cpu), 1);
}

static void LDA_IDY_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xB1;
    memory[1] = 0x55;
    memory[0x55] = 0x60;
    memory[0x56] = 0xAA;
    memory[0xAA60 + 10] = 0xF1;


    Processor cpu = {0};
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.y_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    clock(&cpu);

    assert_int_equal(cpu.acc, 0xF1);
    assert_int_equal(getFlag('N',&cpu), 1);
}

static void LDA_IDY_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[5] = 0xB1;
    memory[6] = 0xFF;
    memory[0xFF] = 0x60;
    memory[0x00] = 0xAA;
    memory[0xAA60 + 10] = 0xF1;

    Processor cpu = {0};
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);

    cpu.pc = 5;
    cpu.y_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    clock(&cpu);

    assert_int_equal(cpu.acc, 0xF1);
    assert_int_equal(getFlag('N',&cpu), 1);
}

int main(void) {
    
    const struct CMUnitTest tests[] = {
        //get_target_address() tests
        cmocka_unit_test(get_target_address_IMM_1),
        cmocka_unit_test(get_target_address_IMM_2),
        cmocka_unit_test(get_target_address_IMM_3),
        cmocka_unit_test(get_target_address_ACC),
        cmocka_unit_test(get_target_address_REL_1),
        cmocka_unit_test(get_target_address_REL_2),
        cmocka_unit_test(get_target_address_ZPG_1),
        cmocka_unit_test(get_target_address_ZPG_2),
        cmocka_unit_test(get_target_address_ZPX_1),
        cmocka_unit_test(get_target_address_ZPX_2),
        cmocka_unit_test(get_target_address_ZPY_1),
        cmocka_unit_test(get_target_address_ZPY_2),
        cmocka_unit_test(get_target_address_ABS),
        cmocka_unit_test(get_target_address_ABX_1),
        cmocka_unit_test(get_target_address_ABX_2),
        cmocka_unit_test(get_target_address_ABX_3),
        cmocka_unit_test(get_target_address_ABY_1),
        cmocka_unit_test(get_target_address_ABY_2),
        cmocka_unit_test(get_target_address_ABY_3),
        cmocka_unit_test(get_target_address_IND),
        cmocka_unit_test(get_target_address_IDX_1),
        cmocka_unit_test(get_target_address_IDX_2),
        cmocka_unit_test(get_target_address_IDY_1),
        cmocka_unit_test(get_target_address_IDY_2),
        cmocka_unit_test(get_target_address_IDY_3),

        //LDA tests
        cmocka_unit_test(LDA_IMM_1),
        cmocka_unit_test(LDA_IMM_2),
        cmocka_unit_test(LDA_ZEP_1),
        cmocka_unit_test(LDA_ZEP_2),
        cmocka_unit_test(LDA_ZPX_1),
        cmocka_unit_test(LDA_ZPX_2),
        cmocka_unit_test(LDA_ABS_1),
        cmocka_unit_test(LDA_ABS_2),
        cmocka_unit_test(LDA_ABX_1),
        cmocka_unit_test(LDA_ABX_2),
        cmocka_unit_test(LDA_ABY_1),
        cmocka_unit_test(LDA_ABY_2),
        cmocka_unit_test(LDA_IDX_1),
        cmocka_unit_test(LDA_IDX_2),
        cmocka_unit_test(LDA_IDY_1),
        cmocka_unit_test(LDA_IDY_2),


    };
    return cmocka_run_group_tests(tests, NULL, NULL);
    
}
