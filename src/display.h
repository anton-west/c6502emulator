#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <ncurses.h>
#include "cpu.h"
#include "opcodes.h"

WINDOW *get_win_1();
WINDOW *get_win_2();
WINDOW *get_win_sr();
WINDOW *get_win_cpu();
WINDOW *get_win_stack();
WINDOW *get_win_decode();

int start_display();

int close_display();

//print to win_1 and win_2
int print_to_win(WINDOW *win, uint8_t* data, unsigned int data_start, int data_n);

int print_to_win_sr(uint8_t status_register);
int print_to_win_cpu(Processor *cpu);
int print_to_win_stack(uint8_t *stack, uint8_t stack_pointer);
int print_disassembly(uint8_t *memory, uint16_t start, uint16_t pc);

#endif
