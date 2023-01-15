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

static void lda(void **state) {
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    memory[0] = 0xA9;
    memory[1] = 0xAA;

    Processor cpu;
    
    init_cpu(&cpu);
    set_memory(&cpu, memory);
    
    //clock(&cpu);

    assert_int_equal(cpu.acc, 0xAA);
}

int main(void) {
    
    const struct CMUnitTest tests[] = {
        //cmocka_unit_test(null_test_success),
        cmocka_unit_test(lda),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
    
}