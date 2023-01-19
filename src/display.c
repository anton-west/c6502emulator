#include <ncurses.h>

#include "display.h"
#include "cpu.h"

WINDOW *win_1;
WINDOW *win_2;
WINDOW *win_sr;
WINDOW *win_cpu;
WINDOW *win_stack;

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
    int column = 1;

    for (int i = 0; i < own_min(data_n,64); i++) {
        if (i % 16 == 0 && i != 0) {
            line++;
            column = 1;
        }
        if (i % 4 == 0 && i != 0 && i != 16) {
            mvwprintw(win_1,line, column, "  ");
            column += 2;
        }
        mvwprintw(win_1,line, column, "  %02X", data[i]);
        column += 4;
    }
    
    wrefresh(win_1);
    return 0;
}

int print_to_win2(uint8_t* data, int data_n) {
    int line = 1;
    int column = 1;

    for (int i = 0; i < own_min(data_n,64); i++) {
        if (i % 16 == 0 && i != 0) {
            line++;
            column = 1;
        }
        if (i % 4 == 0 && i != 0 && i != 16) {
            mvwprintw(win_2,line, column, "  ");
            column += 2;
        }
        mvwprintw(win_2,line, column, "  %02X", data[i]);
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

int start_display() {

	int startx, starty;
	initscr();			/* Start curses mode 		*/
    curs_set(0);
    refresh();

	cbreak();			/* Line buffering disabled, Pass on
					 * everty thing to me 		*/
	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/

	starty = (LINES - (6+6+10)) / 2;	/* Calculating for a center placement */
	startx = (COLS - (74)) / 2;	/* of the window		*/

    

    win_1 = create_newwin(6, 74, starty, startx);
    win_2 = create_newwin(6, 74, starty + 6, startx);
    win_sr = create_newwin(10, 12, starty + 12, startx);
    win_cpu = create_newwin(10, 17, starty + 12, startx + 12);
    win_stack = create_newwin(10, 45, starty + 12, startx + 29);
	/* Show that box 		            */

    uint8_t arr[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21};

    print_to_win1(arr, 21);

    print_to_win2(arr, 21);

    print_to_win_sr(0x55);

    Processor cpu = {0};
    init_cpu(&cpu);
    cpu.acc = 0xAC;
    cpu.x_reg = 0xF8;
    cpu.y_reg = 0x55;
    cpu.pc = 0xABCD;
    cpu.sp = 0xF1;
    print_to_win_cpu(&cpu);

    uint8_t stacky[0x0200] = {0};
    stacky[0x01FF] = 0; stacky[0x01FF-1] = 1; stacky[0x01FF-2] = 2; stacky[0x01FF-3] = 3; stacky[0x01FF-4] = 4; stacky[0x01FF-5] = 5; stacky[0x01FF-6] = 6; stacky[0x01FF-7] = 7; stacky[0x01FF-7] = 7;
    print_to_win_stack(stacky, 0xFF-39);

    refresh();
    getch();

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