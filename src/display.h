#ifndef DISPLAY_H
#define DISPLAY_H

#include "cpu.h"

int start_display();

int close_display();

int print_to_win1(uint8_t* data, int data_n);
int print_to_win2(uint8_t* data, int data_n);
int print_to_win_sr(uint8_t status_register);
int print_to_win_cpu(Processor *cpu);
int print_to_win_stack(uint8_t *stack, uint8_t stack_pointer);

#endif
