#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "display.h"

#include "cpu.h"

#define MAX_MEMORY_ADDR 65536

int main(int argc, char *argv[]) {	
    
    //open binary file to read in program code
    if (argc < 3) {
        fprintf(stderr, "ERROR: Not enough arguments, input file not specified!\n");
        fprintf(stderr, "       Use -b [filename] to read instructions from a binary file\n");
        fprintf(stderr, "       Use -f [filename] to read instructions from a text file\n");
        exit(1);
    }

    int fd;
    char *filename = argv[2];
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr,"ERROR: could not open file \"%s\", %s\n", filename, strerror(errno));
        exit(1);
    }
    
    //0xA9, 2, 0x69, 5, 0x38, 0xE9, 3
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};

    //read binary file into memory array
    if (strcmp(argv[1], "-b") == 0) {
        read(fd,memory,10);
    }

    //decode text file into memory array
    //hexcodes (e.g. "A9 BB 02 FF")
    if (strcmp(argv[1], "-f") == 0) {
        
    }

    Processor cpu = {0};

    init_cpu(&cpu);
    cpu_set_memory(&cpu, memory);

    start_display();

    char c;
    int cont = 1;
    while (cont) {

        //print info to display
        print_to_win1(memory, 64);
        print_to_win2(memory+256, 64);

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

        case 'Q':
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
