#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include <ncurses.h>


WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

int main(int argc, char *argv[]) {	
    
    WINDOW *win_1;
    WINDOW *win_2;
    WINDOW *win_sr;
    WINDOW *win_cpu;
    WINDOW *win_extra;

	int startx, starty, width, height;
	int ch;

	initscr();			/* Start curses mode 		*/
    refresh();

	cbreak();			/* Line buffering disabled, Pass on
					 * everty thing to me 		*/
	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/

	height = 5;
	width = 74;
	starty = 0;	/* Calculating for a center placement */
	startx = 0;	/* of the window		*/

    win_1 = create_newwin(6, 74, 0, 0);
    win_2 = create_newwin(6, 74, 6, 0);
    win_sr = create_newwin(10, 12, 12, 0);
    win_cpu = create_newwin(10, 17, 12, 12);
    win_extra = create_newwin(10, 45, 12, 29);
	/* Show that box 		            */
    mvwprintw(win_1,1,1, "  FF  FF  FF  FF    FF  FF  FF  FF    FF  FF  FF  FF    FF  FF  FF  FF  ");
    mvwprintw(win_1,2,1, "  FF  FF  FF  FF    FF  FF  FF  FF    FF  FF  FF  FF    FF  FF  FF  FF  ");
    mvwprintw(win_1,3,1, "  FF  FF  FF  FF    FF  FF  FF  FF    FF  FF  FF  FF    FF  FF  FF  FF  ");
    mvwprintw(win_1,4,1, "  FF  FF  FF  FF    FF  FF  FF  FF    FF  FF  FF  FF    FF  FF  FF  FF  ");

    mvwprintw(win_2,1,1, "  FF  FF  FF  FF    FF  FF  FF  FF    FF  FF  FF  FF    FF  FF  FF  FF  ");
    mvwprintw(win_2,2,1, "  FF  FF  FF  FF    FF  FF  FF  FF    FF  FF  FF  FF    FF  FF  FF  FF  ");
    mvwprintw(win_2,3,1, "  FF  FF  FF  FF    FF  FF  FF  FF    FF  FF  FF  FF    FF  FF  FF  FF  ");
    mvwprintw(win_2,4,1, "  FF  FF  FF  FF    FF  FF  FF  FF    FF  FF  FF  FF    FF  FF  FF  FF  ");

    mvwprintw(win_sr,1,3, "N:  1");
    mvwprintw(win_sr,2,3, "V:  1");
    mvwprintw(win_sr,3,3, "s:  0");
    mvwprintw(win_sr,4,3, "s:  0");
    mvwprintw(win_sr,5,3, "D:  1");
    mvwprintw(win_sr,6,3, "I:  1");
    mvwprintw(win_sr,7,3, "Z:  1");
    mvwprintw(win_sr,8,3, "C:  1");

    mvwprintw(win_cpu,1,3, "AC:  0xFF");
    mvwprintw(win_cpu,2,3, "XR:  0xFF");
    mvwprintw(win_cpu,3,3, "YR:  0xFF");
    mvwprintw(win_cpu,5,3, "SP:  0xFF");
    mvwprintw(win_cpu,7,3, "PC:  0xFFFF");

    wrefresh(win_1);
    wrefresh(win_2);
    wrefresh(win_sr);
    wrefresh(win_cpu);
    wrefresh(win_extra);
    refresh();
    getch();

	destroy_win(win_1);
    destroy_win(win_2);
    destroy_win(win_sr);
    destroy_win(win_cpu);
    destroy_win(win_extra);
		
	endwin();			/* End curses mode		  */
	return 0;
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters 
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

void destroy_win(WINDOW *local_win)
{	
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners 
	 * and so an ugly remnant of window. 
	 */
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	/* The parameters taken are 
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window 
	 * 3. rs: character to be used for the right side of the window 
	 * 4. ts: character to be used for the top side of the window 
	 * 5. bs: character to be used for the bottom side of the window 
	 * 6. tl: character to be used for the top left corner of the window 
	 * 7. tr: character to be used for the top right corner of the window 
	 * 8. bl: character to be used for the bottom left corner of the window 
	 * 9. br: character to be used for the bottom right corner of the window
	 */
	wrefresh(local_win);
	delwin(local_win);
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
