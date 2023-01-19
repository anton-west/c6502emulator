#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "display.h"

#include "cpu.h"

#define MAX_MEMORY_ADDR 65536

int main(int argc, char *argv[]) {	
    

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    
    memory[0] = 0x0024;
    memory[1] = 0x0002;
    memory[2] = 0x0001;
    
    memory[0] = 0xA9;
    memory[1] = 0xAA;
    memory[2] = 0xA9;
    memory[3] = 0xFF;

    Processor cpu = {0};

    init_cpu(&cpu);
    cpu_set_memory(&cpu, memory);

    start_display();

    char c;
    int cont = 1;
    while (cont) {

        //print info to display
        print_to_win1(memory, 64);
        print_to_win2(memory+2, 64);

        print_to_win_sr(cpu.status_reg);
        print_to_win_cpu(&cpu);
        print_to_win_stack(memory, cpu.sp);
        
        refresh();

        c = getch();

        switch (c)
        {
        case 'q':
            cont = 0;
            break;
        
        case ' ':
            cpu_clock(&cpu);
        default:
            break;
        }

    }

    close_display();
    return 0;
}





























    //uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    /*
    memory[0] = 0x0024;
    memory[1] = 0x0002;
    memory[2] = 0x0001;
    
    memory[0] = 0xA9;
    memory[1] = 0xAA;
    memory[2] = 0xA9;
    memory[3] = 0x0F;
    */
    //Processor cpu;

    //init_cpu(&cpu);
    //cpu.acc = 0x00;
    //set_memory(&cpu, memory);
    /*
    //do 50 clock cycles
    for (int i = 0; i < 20; i++) {
        clock(&cpu);
    }
    printf("%x\n", cpu.acc);
    printf("%x\n", cpu.status_reg);

    setFlag('N', 1, &cpu);
    printf("%x\n", cpu.status_reg);

    setFlag('V', 1, &cpu);
    printf("%x\n", cpu.status_reg);

    setFlag('D', 1, &cpu);
    printf("%x\n", cpu.status_reg);

    setFlag('I', 1, &cpu);
    printf("%x\n", cpu.status_reg);

    setFlag('Z', 1, &cpu);
    printf("%x\n", cpu.status_reg);

    setFlag('C', 1, &cpu);
    printf("%x\n", cpu.status_reg);




    setFlag('C', 0, &cpu);
    printf("%x\n", cpu.status_reg);

    setFlag('Z', 0, &cpu);
    printf("%x\n", cpu.status_reg);

    setFlag('I', 0, &cpu);
    printf("%x\n", cpu.status_reg);

    setFlag('D', 0, &cpu);
    printf("%x\n", cpu.status_reg);

    setFlag('V', 0, &cpu);
    printf("%x\n", cpu.status_reg);

    setFlag('N', 0, &cpu);
    printf("%x\n", cpu.status_reg);
    */
