#include <ncurses.h>
#include <string.h>

#include "display.h"
#include "cpu.h"

WINDOW *win_1;
WINDOW *win_2;
WINDOW *win_sr;
WINDOW *win_cpu;
WINDOW *win_stack;
WINDOW *win_decode;

int get_starty(int height) { 
    return (LINES - height) / 2;
}

int own_min(int a, int b) {
    if (a <= b) {
        return a;
    } else {
        return b;
    }
}

int own_max(int a, int b) {
    if (a >= b) {
        return a;
    } else {
        return b;
    }
}

WINDOW *get_win_1() {
    return win_1;
}
WINDOW *get_win_2() {
    return win_2;
}
WINDOW *get_win_sr() {
    return win_sr;
}
WINDOW *get_win_cpu() {
    return win_cpu;
}
WINDOW *get_win_stack() {
    return win_stack;
}
WINDOW *get_win_decode() {
    return win_decode;
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters 
					            * for the vertical and horizontal
					            * lines			                */
	wrefresh(local_win);	    /* Show that box 		        */

	return local_win;
}

void destroy_win(WINDOW *local_win)
{	
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}

int print_to_win(WINDOW *win, uint8_t* data, unsigned int data_start, int data_n) {
    int line = 1;
    int column = 10;

    for (int i = 0; i < own_min(data_n,64); i++) {
        if (i % 16 == 0 && i != 0) {
            line++;
            column = 10;
        }
        if (i % 4 == 0 && i != 0 && i != 16 && i != 32 && i != 48) {
            mvwprintw(win,line, column, "  ");
            column += 2;
        }

        if (i == 0 || i == 16 || i == 32 || i == 48) {
            mvwprintw(win, line, column - 7, "$%04x:", (data_start + i));
        }
        mvwprintw(win, line, column, "%02X", data[data_start + i]);
        column += 4;
    }
    
    wrefresh(win);
    return 0;
}

int print_to_win_sr(uint8_t status_register) {

    int flag_n = (status_register & 0x80) > 0;
    int flag_v = (status_register & 0x40) > 0;
    int flag_s = (status_register & 0x20) > 0;
    int flag_b = (status_register & 0x10) > 0;
    int flag_d = (status_register & 0x08) > 0;
    int flag_i = (status_register & 0x04) > 0;
    int flag_z = (status_register & 0x02) > 0;
    int flag_c = (status_register & 0x01) > 0;

    mvwprintw(win_sr,1,3, "N:  %d", flag_n);
    mvwprintw(win_sr,2,3, "V:  %d", flag_v);
    mvwprintw(win_sr,3,3, "s:  %d", flag_s);
    mvwprintw(win_sr,4,3, "B:  %d", flag_b);
    mvwprintw(win_sr,5,3, "D:  %d", flag_d);
    mvwprintw(win_sr,6,3, "I:  %d", flag_i);
    mvwprintw(win_sr,7,3, "Z:  %d", flag_z);
    mvwprintw(win_sr,8,3, "C:  %d", flag_c);
    
    if (flag_n) {
        wattrset(win_sr, A_STANDOUT);
    }
    mvwprintw(win_sr,1,7, "%d", flag_n);
    wattrset(win_sr, A_NORMAL);

    if (flag_v) {
        wattrset(win_sr, A_STANDOUT);
    }
    mvwprintw(win_sr,2,7, "%d", flag_v);
    wattrset(win_sr, A_NORMAL);

    if (flag_s) {
        wattrset(win_sr, A_STANDOUT);
    }
    mvwprintw(win_sr,3,7, "%d", flag_s);
    wattrset(win_sr, A_NORMAL);
    
    if (flag_b) {
        wattrset(win_sr, A_STANDOUT);
    }
    mvwprintw(win_sr,4,7, "%d", flag_b);
    wattrset(win_sr, A_NORMAL);
    
    if (flag_d) {
        wattrset(win_sr, A_STANDOUT);
    }
    mvwprintw(win_sr,5,7, "%d", flag_d);
    wattrset(win_sr, A_NORMAL);
    
    if (flag_i) {
        wattrset(win_sr, A_STANDOUT);
    }
    mvwprintw(win_sr,6,7, "%d", flag_i);
    wattrset(win_sr, A_NORMAL);

    if (flag_z) {
        wattrset(win_sr, A_STANDOUT);
    }
    mvwprintw(win_sr,7,7, "%d", flag_z);
    wattrset(win_sr, A_NORMAL);

    if (flag_c) {
        wattrset(win_sr, A_STANDOUT);
    }
    mvwprintw(win_sr,8,7, "%d", flag_c);
    wattrset(win_sr, A_NORMAL);
    
    wrefresh(win_sr);
    return 0;
}

int print_to_win_cpu(Processor *cpu) {
    mvwprintw(win_cpu,1,2, "AC: $%02X (%03d)", cpu->acc, cpu->acc);
    mvwprintw(win_cpu,2,2, "XR: $%02X (%03d)", cpu->x_reg, cpu->x_reg);
    mvwprintw(win_cpu,3,2, "YR: $%02X (%03d)", cpu->y_reg, cpu->y_reg);
    mvwprintw(win_cpu,5,2, "SP: $%02X", cpu->sp);
    mvwprintw(win_cpu,7,2, "PC: $%04X", cpu->pc);
    mvwprintw(win_cpu,8,2, "CY: %d", cpu->cycles);

    wrefresh(win_cpu);
    return 0;
}

int print_to_win_stack(uint8_t *stack, uint8_t stack_pointer) {

    int line = 8;
    int column = 14;
    
    uint8_t i = 0;

    mvwprintw(win_stack,8,2, "SP $01%02X -> ", stack_pointer);

    while(i < 40) {
        if((stack_pointer + i) == 0xFF) {
            break;
        }

        mvwprintw(win_stack,line,column,"  %02X", stack[(uint16_t)(0x0100 | (stack_pointer + i))]);
        line--;

        if(i % 8 == 7) {
            column += 4;
            line = 8;
        }

        i++;
    }
    wrefresh(win_stack);
    return 0;
}

int mvwprintw_ir(WINDOW* win, int y_coord, int x_coord, InstrInfo *ir) {
    box(win, 0 , 0);	
    //remember, 6502 is little-endian, byte 3 is before byte 2 when looking at addresses
    switch (ir->addr_mode)
    {
    case ACC:
        mvwprintw(win, y_coord,x_coord, "$%04X   %s A", ir->abs_addr, ir->opcode_mnemonic);
        break;
    case ABS:
        mvwprintw(win, y_coord,x_coord, "$%04X   %s $%02X%02X {ABS}", ir->abs_addr, ir->opcode_mnemonic, ir->byte_3, ir->byte_2);
        break;
    case ABX:
        mvwprintw(win, y_coord,x_coord, "$%04X   %s $%02X%02X, X {ABX}", ir->abs_addr, ir->opcode_mnemonic, ir->byte_3, ir->byte_2);
        break;
    case ABY:
        mvwprintw(win, y_coord,x_coord, "$%04X   %s $%02X%02X, Y {ABY}", ir->abs_addr, ir->opcode_mnemonic, ir->byte_3, ir->byte_2);
        break;
    case IMM:
        mvwprintw(win, y_coord,x_coord, "$%04X   %s #$%02X {IMM}", ir->abs_addr, ir->opcode_mnemonic, ir->byte_2);
        break;
    case IMP:
        mvwprintw(win, y_coord,x_coord, "$%04X   %s {IMP}", ir->abs_addr, ir->opcode_mnemonic);
        break;
    case IND:
        mvwprintw(win, y_coord,x_coord, "$%04X   %s ($%02X%02X) {IND}", ir->abs_addr, ir->opcode_mnemonic, ir->byte_3, ir->byte_2);
        break;
    case IDX:
        mvwprintw(win, y_coord,x_coord, "$%04X   %s ($%02X, X) {IDX}", ir->abs_addr, ir->opcode_mnemonic, ir->byte_2);
        break;
    case IDY:
        mvwprintw(win, y_coord,x_coord, "$%04X   %s ($%02X), Y {IDY}", ir->abs_addr, ir->opcode_mnemonic, ir->byte_2);
        break;
    case REL:
        //requires block and step wise declaration due to bits acting weird when written inline
        {   
            uint16_t adr = (uint16_t)ir->abs_addr & 0xFFFF;
            uint8_t byte2 = (uint16_t)ir->byte_2;
            uint16_t byte2_ext = ((byte2 & 0x80) > 0) ? (0xFF00 | byte2) : byte2;
            uint16_t result = adr + byte2_ext + 2;
            mvwprintw(win, y_coord,x_coord, "$%04X   %s $%02X [$%04X] {REL}", ir->abs_addr, ir->opcode_mnemonic, byte2, result);
        }
        break;
    case ZPG:
        mvwprintw(win, y_coord,x_coord, "$%04X   %s $%02X {ZPG}", ir->abs_addr, ir->opcode_mnemonic, ir->byte_2);
        break;
    case ZPX:
        mvwprintw(win, y_coord,x_coord, "$%04X   %s $%02X, X {ZPG}", ir->abs_addr, ir->opcode_mnemonic, ir->byte_2);
        break;
    case ZPY:
        mvwprintw(win, y_coord,x_coord, "$%04X   %s $%02X, Y {ZPG}", ir->abs_addr, ir->opcode_mnemonic, ir->byte_2);
        break;
    default:
        mvwprintw(win, y_coord,x_coord, "$%04X   %s {UDF}", ir->abs_addr, ir->opcode_mnemonic);
        break;
    }
    return 0;
}

/* As disassembly is done on the fly from "behind the pc and up to few instructions after, if 
 * the "start" address lands on data byte instead of opcode byte, then strange things can happen and
 * disassembly can be done wrong. Most often data byte is mapped to undefined instruction and start pointer
 * is moved forward until it lands on valid instruction. However, this does not always happen
 * 
 * This means that one should not trust instructions before the pc to be disassembled correctly
 * instructions on pc and forward are always disassembled correctly
 * 
 * If pc instruction is not shown(highlighted in window) then dissasembly has gone wrong
 */
int print_disassembly(uint8_t *memory, uint16_t start, uint16_t pc) {
    
    //start at valid region in memory, in nes system 0x8000 and 0xC000 forward is reserved for prog data
    //0 for now so that multiply program displays properly
    int abs_addr = own_max(start, 0);
    int passed_pc = 0;

    //print 20 instruction
    for (size_t i = 0; i < 20; i++)
    {   
        /* This if statement is to make sure that if we get really unlucky and interpret data bytes before pc as valid instructions,
         * we might jump over the pc and completely miss interpreting the instruction at pc. Therefore we check if we have landed or passed
         * pc, and then set the abs_address to pc, so that instruction at pc is definetely interpreted correctly(all instructions afterward 
         * are also interpreted correctly this way, as long as opcode logic is correct)
         */
        if(abs_addr >= pc && passed_pc == 0) {
            passed_pc = 1;
            abs_addr = pc;
            wattrset(win_decode, A_STANDOUT);
        } else {
            wattroff(win_decode, A_STANDOUT);
        }
        InstrInfo ir = disassemble(memory, abs_addr);
        mvwprintw_ir(win_decode, 1+i, 1, &ir);

        abs_addr += ir.n_bytes;
        if(ir.addr_mode == UDF) {
            abs_addr += 1;
        }

    }

    wrefresh(win_decode);
    return 0;
}

int start_display() {

	int startx, starty;
	initscr();			/* Start curses mode 		*/
    curs_set(0);
    noecho();
    refresh();

	cbreak();			/* Line buffering disabled, Pass on
					 * everty thing to me 		*/
	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/

	starty = (LINES - (6+6+6+10)) / 2;	/* Calculating for a center placement */
	startx = (COLS - (81 + 31)) / 2;	/* of the window		*/

    

    win_1 = create_newwin(6, 81, starty + 6, startx);
    win_2 = create_newwin(6, 81, starty + 12, startx);
    win_sr = create_newwin(10, 12, starty + 18, startx);
    win_cpu = create_newwin(10, 17, starty + 18, startx + 12);
    win_stack = create_newwin(10, 52, starty + 18, startx + 29);
    win_decode = create_newwin(22, 31, starty + 6, startx + 81);
	/* Show that box 		            */

    mvwprintw(stdscr, starty, startx, "SPACE: cycle 1 instruction");
    mvwprintw(stdscr, starty+1, startx, "s:     step 1 cycle");
    mvwprintw(stdscr, starty+2, startx, "r:     reset");
    mvwprintw(stdscr, starty+3, startx, "i:     IRQ");
    mvwprintw(stdscr, starty+4, startx, "n:     NMI");
    mvwprintw(stdscr, starty+5, startx, "q:     quit");

    refresh();

    return 0;
}

int close_display() {
   
    destroy_win(win_1);
    destroy_win(win_2);
    destroy_win(win_sr);
    destroy_win(win_cpu);
    destroy_win(win_stack);
    destroy_win(win_decode);
		
	endwin();			/* End curses mode		  */
	return 0;
}
