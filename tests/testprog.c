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
    Processor cpu;
    Ir ir;
    
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
    Processor cpu;
    Ir ir;
    
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
    Processor cpu;
    Ir ir;
    
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
    Processor cpu;
    Ir ir;
    
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
    Processor cpu;
    Ir ir;

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
    Processor cpu;
    Ir ir;

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
    Processor cpu;
    Ir ir;

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
    Processor cpu;
    Ir ir;

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
    Processor cpu;
    Ir ir;

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
    Processor cpu;
    Ir ir;

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
    Processor cpu;
    Ir ir;

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
    Processor cpu;
    Ir ir;

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
    Processor cpu;
    Ir ir;

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
    Processor cpu;
    Ir ir;

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
    Processor cpu;
    Ir ir;

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
    Processor cpu;
    Ir ir;

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
    Processor cpu;
    Ir ir = {0};

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
    Processor cpu;
    Ir ir = {0};

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
    Processor cpu;
    Ir ir = {0};

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
    Processor cpu;
    Ir ir;

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
}

/*
static void lda(void **state) {
    (void) state;

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xA9;
    memory[1] = 0xAA;

    Processor cpu;
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);
    
    clock(&cpu);

    assert_int_equal(cpu.acc, 0xAA);
}
*/

int main(void) {
    
    const struct CMUnitTest tests[] = {
        //cmocka_unit_test(null_test_success),
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
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
    
}
