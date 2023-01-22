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
    
    //open file to read in program code
    if (argc < 3) {
        fprintf(stderr, "ERROR: Not enough arguments, input file not specified!\n");
        fprintf(stderr, "       Use -b [filename] to read instructions from a binary file\n");
        fprintf(stderr, "       Use -f [filename] to read instructions from a text file\n");
        exit(1);
    }

    FILE *ptr;
    char *filename = argv[2];
    ptr = fopen(filename, "r");
    if (ptr == NULL) {
        fprintf(stderr,"ERROR: could not open file \"%s\", %s\n", filename, strerror(errno));
        exit(1);
    }
    
    //0xA9, 2, 0x69, 5, 0x38, 0xE9, 3
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};

    //read binary file into memory array
    if (strcmp(argv[1], "-b") == 0) {
        fread(memory,sizeof(char)*10,1, ptr);
    }

    //decode text file into memory array
    //hexcodes (e.g. "A9 BB 02 FF" get transformed to corresponding bytes)
    if (strcmp(argv[1], "-f") == 0) {
        size_t i = 0;
        while (!feof(ptr))
        {
            char str[3] = {0};
            fscanf(ptr, "%s", str);
            unsigned long byte = strtoul(str,NULL,16);
            memory[i] = byte;
            i++;
        }
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
