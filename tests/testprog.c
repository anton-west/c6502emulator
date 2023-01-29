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

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,5,4,3};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=IMM;
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 2);
    assert_int_equal(cpu.abs_addr, 1);
    assert_int_equal(cpu.fetched_value, 5);
}

static void get_target_address_IMM_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=IMM;
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.pc = 0x3232;
    assert_int_equal(cpu.pc, 0x3232); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 0x3232 + 2);
    assert_int_equal(cpu.abs_addr, 0x3232 + 1);
}

static void get_target_address_IMM_3(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=IMM;
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.pc = 0xFFFF;
    assert_int_equal(cpu.pc, 0xFFFF); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 1);    //should roll over from 0xFFFF to 1
    assert_int_equal(cpu.abs_addr, 0);  //should roll over from 0xFFFF to 0
}

static void get_target_address_ACC(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=ACC;
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 1);
    assert_int_equal(cpu.abs_addr, 0);
}

static void get_target_address_REL_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=REL;

    memory[1] = 69;

    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 0);
    assert_int_equal(cpu.abs_addr, 71); //71 = pc + 69 + 2 (2 pc increments in retrieval)

    //here pc should be set to pc = trg_addr in a real program
}

static void get_target_address_REL_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=REL;

    memory[0x0100] = 0xF0;

    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.pc = 0x00FF;

    assert_int_equal(cpu.pc, 0x00FF); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 0x00FF);    //does not get increment in this address mode, should be done in opcode implementation
    assert_int_equal(cpu.abs_addr, cpu.pc - 16 + 2);  //pc + 0x33 + 2 (2 pc increments in retrieval)
}

static void get_target_address_ZPG_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=ZPG;

    memory[1] = 50;

    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 2);
    assert_int_equal(cpu.abs_addr, 50);
}

static void get_target_address_ZPG_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=ZPG;

    memory[1] = 0xFF;

    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 2);
    assert_int_equal(cpu.abs_addr, 0xFF);
    assert_int_equal(cpu.abs_addr & 0xFF00, 0);
}

static void get_target_address_ZPX_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=ZPX;

    memory[1] = 10;
    

    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    
    cpu.x_reg = 10;

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 2);
    assert_int_equal(cpu.abs_addr, 20);
    assert_int_equal(cpu.abs_addr & 0xFF00, 0);
}

static void get_target_address_ZPX_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=ZPX;

    memory[1] = 0xFF;

    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    
    cpu.x_reg = 10;

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 2);
    assert_int_equal(cpu.abs_addr, 9);  //0xFF + 10 = 0x09
    assert_int_equal(cpu.abs_addr & 0xFF00, 0);
}

static void get_target_address_ZPY_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=ZPY;

    memory[1] = 10;

    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    
    cpu.y_reg = 10;

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 2);
    assert_int_equal(cpu.abs_addr, 20);
    assert_int_equal(cpu.abs_addr & 0xFF00, 0);
}

static void get_target_address_ZPY_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=ZPY;

    memory[1] = 0xFF;

    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    
    cpu.y_reg = 10;

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 2);
    assert_int_equal(cpu.abs_addr, 9);  //0xFF + 10 = 0x09
    assert_int_equal(cpu.abs_addr & 0xFF00, 0);
}

static void get_target_address_ABS(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=ABS;

    memory[1] = 0x22;   //low byte
    memory[2] = 0x33;   //high byte

    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 3);
    assert_int_equal(cpu.abs_addr, 0x3322);
}

static void get_target_address_ABX_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=ABX;

    memory[1] = 0x22;   //low byte
    memory[2] = 0x33;   //high byte

    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    
    cpu.x_reg = 0;

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 3);
    assert_int_equal(cpu.abs_addr, 0x3322);
    assert_int_equal(ir.n_cycles, 0); //no extra cycle should be added
}

static void get_target_address_ABX_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=ABX;

    memory[1] = 0x22;   //low byte
    memory[2] = 0x33;   //high byte

    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    
    cpu.x_reg = 0x22;

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 3);
    assert_int_equal(cpu.abs_addr, 0x3322 + 0x22);
    assert_int_equal(ir.n_cycles, 0); //no extra cycle should be added
}

static void get_target_address_ABX_3(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=ABX;

    memory[1] = 0xFF;   //low byte
    memory[2] = 0x33;   //high byte

    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    
    cpu.x_reg = 0x01;

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 3);
    assert_int_equal(cpu.abs_addr, 0x33FF + 0x01);
    assert_int_equal(ir.n_cycles, 1); //extra cycle should be added
}

static void get_target_address_ABY_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=ABY;

    memory[1] = 0x22;   //low byte
    memory[2] = 0x33;   //high byte

    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    
    cpu.y_reg = 0;

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);
    assert_int_equal(cpu.pc, 3);
    assert_int_equal(cpu.abs_addr, 0x3322);
    assert_int_equal(ir.n_cycles, 0); //no extra cycle should be added
}

static void get_target_address_ABY_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=ABY;

    memory[1] = 0x22;   //low byte
    memory[2] = 0x33;   //high byte

    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    
    cpu.y_reg = 0x22;

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 3);
    assert_int_equal(cpu.abs_addr, 0x3322 + 0x22);
    assert_int_equal(ir.n_cycles, 0); //no extra cycle should be added
}

static void get_target_address_ABY_3(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=ABY;

    memory[1] = 0xFF;   //low byte
    memory[2] = 0x33;   //high byte

    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    
    cpu.y_reg = 0x01;

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 3);
    assert_int_equal(cpu.abs_addr, 0x33FF + 0x01);
    assert_int_equal(ir.n_cycles, 1); //extra cycle should be added
}

static void get_target_address_IND(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir= {0};
    ir.addr_mode=IND;

    memory[1] = 0x22;   //low byte
    memory[2] = 0x33;   //high byte

    memory[0x3322] = 0x55;    //target address is 0x55

    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    assert_int_equal(cpu.pc, 0); 

    fetch_target_value(&cpu, &ir);

    assert_int_equal(cpu.pc, 3);
    assert_int_equal(cpu.abs_addr, 0x55);
}

static void get_target_address_IDX_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir = {0};
    ir.addr_mode=IDX;

    memory[1] = 0x55;   //read zero page address
    memory[0x55 + 0x35] = 0xAA;
    memory[0x55 + 0x35 + 1] = 0xBB;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.x_reg = 0x35;
    assert_int_equal(cpu.pc, 0); 
    fetch_target_value(&cpu, &ir);
    assert_int_equal(cpu.pc, 2);
    assert_int_equal(cpu.abs_addr, 0xBBAA);
}

static void get_target_address_IDX_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir = {0};
    ir.addr_mode=IDX;

    memory[1] = 0xFF;   //read zero page address
    memory[(0xFF + 0x35) & 0x00FF] = 0xAA;
    memory[(0xFF + 0x35 + 1) & 0x00FF] = 0xBB;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.x_reg = 0x35;
    assert_int_equal(cpu.pc, 0); 
    fetch_target_value(&cpu, &ir);
    assert_int_equal(cpu.pc, 2);
    assert_int_equal(cpu.abs_addr, 0xBBAA);
}

static void get_target_address_IDY_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir = {0};
    ir.addr_mode=IDY;

    memory[1] = 0x76;   //read zero page address
    memory[0x76] = 0xAA;
    memory[0x77] = 0xBB;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.y_reg = 0x35;
    assert_int_equal(cpu.pc, 0); 
    fetch_target_value(&cpu, &ir);
    assert_int_equal(cpu.pc, 2);
    assert_int_equal(cpu.abs_addr, 0xBBAA + 0x35);
    assert_int_equal(ir.n_cycles, 0);
}

static void get_target_address_IDY_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir = {0};
    ir.addr_mode=IDY;

    memory[1] = 0xFF;   //read zero page address
    memory[0xFF] = 0xAA;
    memory[0x00] = 0xBB;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.y_reg = 0x35;
    assert_int_equal(cpu.pc, 0); 
    fetch_target_value(&cpu, &ir);
    assert_int_equal(cpu.pc, 2);
    assert_int_equal(cpu.abs_addr, 0xBBAA + 0x35);
    assert_int_equal(ir.n_cycles, 0);
}

static void get_target_address_IDY_3(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    InstrInfo ir = {0};
    ir.addr_mode=IDY;

    memory[1] = 0xFF;   //read zero page address
    memory[0xFF] = 0xFF;
    memory[0x00] = 0xBB;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.y_reg = 0x35;
    assert_int_equal(cpu.pc, 0); 
    fetch_target_value(&cpu, &ir);
    assert_int_equal(cpu.pc, 2);
    assert_int_equal(cpu.abs_addr, 0xBBFF + 0x35);
    assert_int_equal(ir.n_cycles, 1);
}

// LDA TESTS

static void LDA_IMM_1(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xA9;
    memory[1] = 0xAA;

    Processor cpu = {0};
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    assert_int_equal(cpu.status_reg, 0);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

    assert_int_equal(cpu.acc, 0xAA);
    assert_int_equal(getFlag('N',&cpu), 1);
}

static void LDA_IMM_2(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xA9;
    memory[1] = 0x00;

    Processor cpu = {0};
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.acc = 0xFF;

    assert_int_equal(cpu.status_reg, 0);
    
    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

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
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    assert_int_equal(cpu.status_reg, 0);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

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
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.acc = 0xFF;

    assert_int_equal(cpu.status_reg, 0);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

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
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.x_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

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
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.x_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

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
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    assert_int_equal(cpu.status_reg, 0);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

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
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    assert_int_equal(cpu.status_reg, 0);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

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
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.x_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

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
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.x_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

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
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.y_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

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
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.y_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

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
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.x_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

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
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.x_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

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
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.y_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

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
    
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.pc = 5;
    cpu.y_reg = 10;

    assert_int_equal(cpu.status_reg, 0);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

    assert_int_equal(cpu.acc, 0xF1);
    assert_int_equal(getFlag('N',&cpu), 1);
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/////////       ACTUAL TESTS            ///////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

//transfer instructions

static void LDA_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    Processor cpu = {0};
    memory[0] = 0xA9;
    memory[1] = 0xF0;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

    assert_int_equal(cpu.acc, 0xF0);
    assert_int_equal(getFlag('N',&cpu), 1);
    assert_int_equal(getFlag('Z',&cpu), 0);
}

static void LDA_2(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0x01,0};
    Processor cpu = {0};
    memory[0] = 0xA9;
    memory[1] = 0x00;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

    assert_int_equal(cpu.acc, 0x00);
    assert_int_equal(getFlag('N',&cpu), 0);
    assert_int_equal(getFlag('Z',&cpu), 1);
}

static void LDX_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0xA2;
    memory[1] = 0xF0;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

    assert_int_equal(cpu.x_reg, 0xF0);
    assert_int_equal(getFlag('N',&cpu), 1);
    assert_int_equal(getFlag('Z',&cpu), 0);
}

static void LDX_2(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0x01,0};
    Processor cpu = {0};
    memory[0] = 0xA2;
    memory[1] = 0x00;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

    assert_int_equal(cpu.x_reg, 0x00);
    assert_int_equal(getFlag('N',&cpu), 0);
    assert_int_equal(getFlag('Z',&cpu), 1);
}

static void LDY_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0xA0;
    memory[1] = 0xF0;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

    assert_int_equal(cpu.y_reg, 0xF0);
    assert_int_equal(getFlag('N',&cpu), 1);
    assert_int_equal(getFlag('Z',&cpu), 0);
}

static void LDY_2(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0x01,0};
    Processor cpu = {0};
    memory[0] = 0xA0;
    memory[1] = 0x00;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);

    assert_int_equal(cpu.y_reg, 0x00);
    assert_int_equal(getFlag('N',&cpu), 0);
    assert_int_equal(getFlag('Z',&cpu), 1);
}

static void STA_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x85;
    memory[1] = 0x02;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.acc = 0xFF;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(memory[2], 0xFF);
    assert_int_equal(cpu.status_reg, 0);
}

static void STX_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x86;
    memory[1] = 0x02;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.x_reg = 0xFF;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(memory[2], 0xFF);
    assert_int_equal(cpu.status_reg, 0);
}

static void STY_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x84;
    memory[1] = 0x02;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.y_reg = 0xFF;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(memory[2], 0xFF);
    assert_int_equal(cpu.status_reg, 0);
}

static void TAX_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0xAA;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.acc = 0xFF;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.x_reg, 0xFF);
    assert_int_equal(getFlag('N',&cpu), 1);
    assert_int_equal(getFlag('Z',&cpu), 0);
}

static void TAY_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0xA8;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.acc = 0xFF;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.y_reg, 0xFF);
    assert_int_equal(getFlag('N',&cpu), 1);
    assert_int_equal(getFlag('Z',&cpu), 0);
}

static void TSX_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0xBA;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.sp = 0xFF;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.x_reg, 0xFF);
    assert_int_equal(getFlag('N',&cpu), 1);
    assert_int_equal(getFlag('Z',&cpu), 0);
}

static void TXA_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x8A;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.x_reg = 0xFF;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.acc, 0xFF);
    assert_int_equal(getFlag('N',&cpu), 1);
    assert_int_equal(getFlag('Z',&cpu), 0);
}

static void TXS_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x9A;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.x_reg = 0xFF;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.sp, 0xFF);
    assert_int_equal(cpu.status_reg, 0);
}

static void TYA_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x98;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.y_reg = 0xFF;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.acc, 0xFF);
    assert_int_equal(getFlag('N',&cpu), 1);
    assert_int_equal(getFlag('Z',&cpu), 0);
}

// shift and rotate instructions

static void ASL_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x0A;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.acc = 0x88;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.acc, 0x10);
    assert_int_equal(getFlag('N',&cpu), 0);
    assert_int_equal(getFlag('Z',&cpu), 0);
    assert_int_equal(getFlag('C',&cpu), 1);
}


static void LSR_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x4A;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.acc = 0x88;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.acc, 0x44);
    assert_int_equal(getFlag('N',&cpu), 0);
    assert_int_equal(getFlag('Z',&cpu), 0);
    assert_int_equal(getFlag('C',&cpu), 0);
}

static void LSR_2(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x4A;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.acc = 0x8F;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.acc, 0x47);
    assert_int_equal(getFlag('N',&cpu), 0);
    assert_int_equal(getFlag('Z',&cpu), 0);
    assert_int_equal(getFlag('C',&cpu), 1);
}

static void ROL_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x2A;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.acc = 0x88;
    setFlag('C', 0, &cpu);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.acc, 0x10);
    assert_int_equal(getFlag('N',&cpu), 0);
    assert_int_equal(getFlag('Z',&cpu), 0);
    assert_int_equal(getFlag('C',&cpu), 1);
}

static void ROL_2(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x2A;
    memory[1] = 0x2A;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.acc = 0x88;
    InstrInfo temp_ir={0};
    cpu_clock(&cpu, &temp_ir);
    cpu_clock(&cpu, &temp_ir);
    cpu_clock(&cpu, &temp_ir);
    cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.acc, 0x21);
    assert_int_equal(getFlag('N',&cpu), 0);
    assert_int_equal(getFlag('Z',&cpu), 0);
    assert_int_equal(getFlag('C',&cpu), 0);
}

static void ROR_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x6A;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.acc = 0x88;
    setFlag('C', 0, &cpu);

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.acc, 0x44);
    assert_int_equal(getFlag('N',&cpu), 0);
    assert_int_equal(getFlag('Z',&cpu), 0);
    assert_int_equal(getFlag('C',&cpu), 0);
}

static void ROR_2(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x6A;
    memory[1] = 0x6A;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.acc = 0x88;

    InstrInfo temp_ir={0};
    cpu_clock(&cpu, &temp_ir);
    cpu_clock(&cpu, &temp_ir);
    cpu_clock(&cpu, &temp_ir);
    cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.acc, 0x22);
    assert_int_equal(getFlag('N',&cpu), 0);
    assert_int_equal(getFlag('Z',&cpu), 0);
    assert_int_equal(getFlag('C',&cpu), 0);
}

// bitwise logical operations

static void AND_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x29;
    memory[1] = 0x33;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.acc = 0x0F;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.acc, 0x03);
    assert_int_equal(getFlag('N',&cpu), 0);
    assert_int_equal(getFlag('Z',&cpu), 0);
}

static void EOR_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x49;
    memory[1] = 0x22;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.acc = 0x0F;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.acc, 0x2D);
    assert_int_equal(getFlag('N',&cpu), 0);
    assert_int_equal(getFlag('Z',&cpu), 0);
}

static void ORA_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x09;
    memory[1] = 0x22;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.acc = 0x0F;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.acc, 0x2F);
    assert_int_equal(getFlag('N',&cpu), 0);
    assert_int_equal(getFlag('Z',&cpu), 0);
}

static void PHA_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x48;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.acc = 0x68;
    cpu.sp = 0x0F;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(memory[0x0F], 0x68);
    assert_int_equal(cpu.sp, 0x0F-1);
}

static void PHP_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x08;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);

    cpu.status_reg = 0xCF;
    
    cpu.sp = 0x0F;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(memory[0x0F], 0xFF);
    assert_int_equal(cpu.sp, 0x0F-1);
}

static void PLA_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x68;
    memory[0x0A] = 0x12;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    
    cpu.sp = 0x0A;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.acc, 0x12);
    assert_int_equal(cpu.sp, 0x0A+1);
}

static void PLP_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x28;
    memory[0x0A] = 0xFF;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    
    cpu.sp = 0x0A;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.status_reg, 0xCF);
    assert_int_equal(cpu.sp, 0x0A+1);
}

static void ADC_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0x69;
    memory[1] = 10;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    
    cpu.acc = 10;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.acc, 20);
}

static void SBC_1(void **state) {
    (void) state;
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0,0,0};
    Processor cpu = {0};
    memory[0] = 0xE9;
    memory[1] = 5;
    cpu_set_memory(&cpu, memory);
    init_cpu(&cpu);
    setFlag('C',1,&cpu);    //prepare by setting carry
    cpu.acc = 15;

    InstrInfo temp_ir={0}; cpu_clock(&cpu, &temp_ir);
    
    assert_int_equal(cpu.acc, 10);
}






///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/////////       END TESTS               ///////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
int main(void) {
    
    const struct CMUnitTest tests[] = {
        //fetch_target_value() tests
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

        //first LDA tests
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

        //actual opcode tests go here

        //transfer tests
        cmocka_unit_test(LDA_1),
        cmocka_unit_test(LDA_2),
        cmocka_unit_test(LDX_1),
        cmocka_unit_test(LDX_2),
        cmocka_unit_test(LDY_1),
        cmocka_unit_test(LDY_2),
        cmocka_unit_test(STA_1),
        cmocka_unit_test(STX_1),
        cmocka_unit_test(STY_1),
        cmocka_unit_test(TAX_1),
        cmocka_unit_test(TAY_1),
        cmocka_unit_test(TSX_1),
        cmocka_unit_test(TXA_1),
        cmocka_unit_test(TXS_1),
        cmocka_unit_test(TYA_1),

        //shift and rotate tests
        cmocka_unit_test(ASL_1),
        cmocka_unit_test(LSR_1),
        cmocka_unit_test(LSR_2),
        cmocka_unit_test(ROL_1),
        cmocka_unit_test(ROL_2),
        cmocka_unit_test(ROR_1),
        cmocka_unit_test(ROR_2),

        //logical bitwise test
        cmocka_unit_test(AND_1),
        cmocka_unit_test(EOR_1),
        cmocka_unit_test(ORA_1),

        //stack operations
        cmocka_unit_test(PHA_1),
        cmocka_unit_test(PHP_1),
        cmocka_unit_test(PLA_1),
        cmocka_unit_test(PLP_1),

        cmocka_unit_test(ADC_1),
        cmocka_unit_test(SBC_1),


    };
    return cmocka_run_group_tests(tests, NULL, NULL);
    
}
