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

int print_to_win1(uint8_t* data, int data_n) {
    int line = 1;
    int column = 3;

    for (int i = 0; i < own_min(data_n,64); i++) {
        if (i % 16 == 0 && i != 0) {
            line++;
            column = 3;
        }
        if (i % 4 == 0 && i != 0 && i != 16 && i != 32 && i != 48) {
            mvwprintw(win_1,line, column, "  ");
            column += 2;
        }
        mvwprintw(win_1,line, column, "%02X", data[i]);
        column += 4;
    }
    
    wrefresh(win_1);
    return 0;
}

int print_to_win2(uint8_t* data, int data_n) {
    int line = 1;
    int column = 3;

    for (int i = 0; i < own_min(data_n,64); i++) {
        if (i % 16 == 0 && i != 0) {
            line++;
            column = 3;
        }
        if (i % 4 == 0 && i != 0 && i != 16 && i != 32 && i != 48) {
            mvwprintw(win_2,line, column, "  ");
            column += 2;
        }
        mvwprintw(win_2,line, column, "%02X", data[i]);
        column += 4;
    }
    
    wrefresh(win_2);
    return 0;
}

int print_to_win_sr(uint8_t status_register) {
    mvwprintw(win_sr,1,3, "N:  %d", (status_register & 0x80) > 0);
    mvwprintw(win_sr,2,3, "V:  %d", (status_register & 0x40) > 0);
    mvwprintw(win_sr,3,3, "s:  %d", (status_register & 0x20) > 0);
    mvwprintw(win_sr,4,3, "s:  %d", (status_register & 0x10) > 0);
    mvwprintw(win_sr,5,3, "D:  %d", (status_register & 0x08) > 0);
    mvwprintw(win_sr,6,3, "I:  %d", (status_register & 0x04) > 0);
    mvwprintw(win_sr,7,3, "Z:  %d", (status_register & 0x02) > 0);
    mvwprintw(win_sr,8,3, "C:  %d", (status_register & 0x01) > 0);
    
    wrefresh(win_sr);
    return 0;
}

int print_to_win_cpu(Processor *cpu) {
    mvwprintw(win_cpu,1,3, "AC:  %02X", cpu->acc);
    mvwprintw(win_cpu,2,3, "XR:  %02X", cpu->x_reg);
    mvwprintw(win_cpu,3,3, "YR:  %02X", cpu->y_reg);
    mvwprintw(win_cpu,5,3, "SP:  %02X", cpu->sp);
    mvwprintw(win_cpu,7,3, "PC:  %04X", cpu->pc);
    mvwprintw(win_cpu,8,3, "CY:  %d", cpu->cycles);

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
        mvwprintw(win_decode, 1+i,1, "%s %s", (ir_arr + i)->opcode_mnemonic, addr_mode);
        
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

    

    win_1 = create_newwin(6, 74, starty, startx);
    win_2 = create_newwin(6, 74, starty + 6, startx);
    win_sr = create_newwin(10, 12, starty + 12, startx);
    win_cpu = create_newwin(10, 17, starty + 12, startx + 12);
    win_stack = create_newwin(10, 45, starty + 12, startx + 29);
    win_decode = create_newwin(22, 30, starty, startx + 74);
	/* Show that box 		            */

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
