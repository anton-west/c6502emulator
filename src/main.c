#include <stdio.h>
#include <stdint.h>
#include <ncurses.h>

//#include "cpu.h"

#define MAX_MEMORY_ADDR 65536

int main()
{	
	//initscr();			/* Start curses mode 		  */
	//endwin();			/* End curses mode		  */
    printf("hello world\n");
	

    
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

    return 0;

}










































  