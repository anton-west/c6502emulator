#include <ncurses.h>
#include <string.h>

#include "display.h"
#include "cpu.h"

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
    int column = 8;
    
    uint8_t i = 0;

    mvwprintw(win_stack,8,2, "SP -> ");

    while(i < 40) {
        mvwprintw(win_stack,line,column,"  %02X", stack[(uint16_t)(0x0100 | (stack_pointer + i))]);
        line--;
        
        if((stack_pointer + i) == 0xFF) {
            break;
        }

        if(i % 8 == 7) {
            column += 4;
            line = 8;
        }

        i++;
    }
    wrefresh(win_stack);
    return 0;
}

//TODO: implement proper decoding of machine code
int print_and_decode(InstrInfo *ir_arr, size_t n_ir) {
    
    for (size_t i = 0; i < n_ir; i++) {

        //all the addr modes: UDF, ACC, ABS, ABX, ABY, IMM, IMP, IND, IDX, IDY, REL, ZPG, ZPX, ZPY
        char addr_mode[10];
        switch ((ir_arr + i)->addr_mode)
        {
        case ACC:
            strcpy(addr_mode,"ACC");
            break;
        case ABS:
            strcpy(addr_mode,"ABS");
            break;
        case ABX:
            strcpy(addr_mode,"ABX");
            break;
        case ABY:
            strcpy(addr_mode,"ABY");
            break;
        case IMM:
            strcpy(addr_mode,"IMM");
            break;
        case IMP:
            strcpy(addr_mode,"IMP");
            break;
        case IND:
            strcpy(addr_mode,"IND");
            break;
        case IDX:
            strcpy(addr_mode,"IDX");
            break;
        case IDY:
            strcpy(addr_mode,"IDY");
            break;
        case REL:
            strcpy(addr_mode,"REL");
            break;
        case ZPG:
            strcpy(addr_mode,"ZPG");
            break;
        case ZPX:
            strcpy(addr_mode,"ZPX");
            break;
        case ZPY:
            strcpy(addr_mode,"ZPY");
            break;
        default:
            strcpy(addr_mode, "UDF");
            break;
        }
        mvwprintw(win_decode, 1,1, "current instruction:");
        mvwprintw(win_decode, 2+i,1, "  %s %s", (ir_arr + i)->opcode_mnemonic, addr_mode);
        
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

	starty = (LINES - (6+6+10)) / 2;	/* Calculating for a center placement */
	startx = (COLS - (74 + 30)) / 2;	/* of the window		*/

    

    win_1 = create_newwin(6, 81, starty, startx);
    win_2 = create_newwin(6, 81, starty + 6, startx);
    win_sr = create_newwin(10, 12, starty + 12, startx);
    win_cpu = create_newwin(10, 17, starty + 12, startx + 12);
    win_stack = create_newwin(10, 45, starty + 12, startx + 29);
    win_decode = create_newwin(22, 30, starty, startx + 81);
	/* Show that box 		            */

    mvwprintw(stdscr, starty-6, startx, "SPACE: cycle 1 instruction");
    mvwprintw(stdscr, starty-5, startx, "s:     step 1 cycle");
    mvwprintw(stdscr, starty-4, startx, "r:     reset");
    mvwprintw(stdscr, starty-3, startx, "i:     IRQ");
    mvwprintw(stdscr, starty-2, startx, "n:     NMI");
    mvwprintw(stdscr, starty-1, startx, "q:     quit");

    refresh();

    return 0;
}

int close_display() {
    destroy_win(win_1);
    destroy_win(win_2);
    destroy_win(win_sr);
    destroy_win(win_cpu);
    destroy_win(win_stack);
		
	endwin();			/* End curses mode		  */
	return 0;
}
