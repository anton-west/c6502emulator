#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "display.h"
#include "logging.h"
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

    uint8_t memory[ MAX_MEMORY_ADDR ] = {0};

    //print help message
    if(strcmp("h", argv[1]) == 0 || strcmp("help", argv[1]) == 0 || strcmp("-h", argv[1]) == 0 || strcmp("--h", argv[1]) == 0 || strcmp("--help", argv[1]) == 0) {
        fprintf(stderr, "       Use -b [filename] to read instructions from a binary file\n");
        fprintf(stderr, "       Use -f [filename] to read instructions from a text file\n");
        exit(1);
    }

    int enable_log = 0;
    FILE *logptr = NULL;
    //check if any of the arguments passed are "--log" to enable logging to "log.csv"
    for(int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--log") == 0) {
            enable_log = 1;
            logptr = fopen("log.csv", "w");
            if (logptr == NULL) {
                fprintf(stderr,"ERROR: could not open file \"log.csv\", %s\n", strerror(errno));
            }
        }
    }

    //open file for reading 6502 program data
    FILE *ptr;
    char *filename;
    int binary_mode = 0;
    int file_mode_specified = 0;     //set to true when valid file opening option is found, otherwise, exit
    for(int i = 0; i < argc - 1; i++) {
        if (strcmp(argv[i], "-b") == 0) {
            filename = argv[i + 1];
            binary_mode = 1;
            file_mode_specified = 1;
        } else if(strcmp(argv[i], "-f") == 0) {
            filename = argv[i + 1];
            file_mode_specified = 1;
        }
    }

    if(!file_mode_specified) {
        fprintf(stderr, "ERROR: Invalid options specified\n");
        fprintf(stderr, "       Use -b [filename] to read instructions from a binary file\n");
        fprintf(stderr, "       Use -f [filename] to read instructions from a text file\n");
        exit(1);
    }

    ptr = fopen(filename, "r");
    if (ptr == NULL) {
        fprintf(stderr,"ERROR: could not open file \"%s\", %s\n", filename, strerror(errno));
        exit(1);
    }

    //read binary file into memory array
    if (binary_mode) {
        size_t n_read = fread((memory),sizeof(char)*MAX_MEMORY_ADDR,1, ptr);
        if(n_read == 0) {
            fprintf(stderr,"ERROR: could not properly read from program file\n");
            exit(1);
        }
    }

    //decode text file into memory array
    //hexcodes (e.g. the text "A9 BB 02 FF" get transformed to corresponding bytes)
    if (!binary_mode) {
        size_t i = 0;
        int n_read = 1;
        while (n_read != EOF)
        {
            char str[3] = {0};
            n_read = fscanf(ptr, "%s", str);
            unsigned long byte = strtoul(str,NULL,16);
            memory[i] = byte;
            i++;
        }
    }

    InstrInfo current_instruction = {0};
    Processor cpu = {0};
        
    cpu_set_memory(&cpu, memory);
    
    init_cpu(&cpu);

    start_display();

    cpu.pc = 0xC000;

    char c;
    int cont = 1;

    //main execution loop starts here
    while (cont) {

        //clear screens
        werase(win_decode);

        //print info to display
        print_to_win(win_1, memory, 0x0000, 64);
        print_to_win(win_2, memory, 0x0040, 64);

        print_to_win_sr(cpu.status_reg);
        print_to_win_cpu(&cpu);
        print_to_win_stack(memory, cpu.sp);

        print_disassembly(memory, cpu.pc-10 , cpu.pc);
        
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

        //if cpu clock == 0, print to log
        if(enable_log && cpu.cycles == 0) {
            InstrInfo info_out = disassemble(memory, cpu.pc);
            print_ir(logptr, &info_out, &cpu);
        }
        

    }

    close_display();
    return 0;
}
