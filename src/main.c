#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "display.h"

#include "cpu.h"

/*
 *    Style guide: The most important thing here is consistency!
 *
 *    All macros and constants in caps: MAX_BUFFER_SIZE, TRACKING_ID_PREFIX.
 *    Struct names and typedef's in camelcase: GtkWidget, TrackingOrder.
 *    Functions that operate on structs: classic C style: gtk_widget_show(), tracking_order_process().
 *    Pointers: nothing fancy here: GtkWidget *foo, TrackingOrder *bar.
 *    Global variables: just don't use global variables. They are evil.
 *    Functions that are there, but shouldn't be called directly, or have obscure uses, or whatever: one or more underscores at the beginning: _refrobnicate_data_tables(), _destroy_cache().
 *
 */

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
    
    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};
    uint8_t temp_memory[ MAX_MEMORY_ADDR ] = {0};

    //read binary file into memory array
    if (strcmp(argv[1], "-b") == 0) {
        fread((temp_memory),sizeof(char)*MAX_MEMORY_ADDR,1, ptr);
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

    for (int i = 0; i < 0x4000; i++ ) {
        memory[0x8000 + i] = temp_memory[0x0010 + i];
        memory[0xC000 + i] = temp_memory[0x0010 + i];
    }

    InstrInfo current_instruction = {0};
    
    Processor cpu = {0};
        
    cpu_set_memory(&cpu, memory);
    
    init_cpu(&cpu);
    cpu.pc = 0x0040;

    start_display();

    char c;
    int cont = 1;
    int extra_offset = 0;
    while (cont) {

        //clear screens
        werase(win_decode);

        
        //print info to display
        print_to_win(win_1, memory, 0, 64);
        print_to_win(win_2, memory, 0x0040, 64);

        print_to_win_sr(cpu.status_reg);
        print_to_win_cpu(&cpu);
        print_to_win_stack(memory, cpu.sp);

        print_disassembly(memory, cpu.pc);
        
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
            do {
                cpu_clock(&cpu, &current_instruction);
            } while (cpu.cycles != 0);
            break;

        case 's':
            cpu_clock(&cpu, &current_instruction);
            break;
        
        case 'r':
            cpu_reset(&cpu);
            break;

        case 'i':
            cpu_irq(&cpu);
            break;

        case 'n':
            cpu_nmi(&cpu);

        default:
            break;
        }

    }

    close_display();
    return 0;
}
