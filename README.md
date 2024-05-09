# c6502emulator
6502 cpu emulator implemented using C.

## Dependencies
Make sure you have `ncurses` available on your system.

## Compiling
`make` - compile

`make run` - compile and run

The Makefile instructs the emulator to load in the program in file multiply.txt.
The file contains a small program that multiplies the numbers 10 and 3. The result
is stored in memory address $0003

## Running

`c6502 -f [file]` - load program from ascii text file

`c6502 -b [file]` - load program by directly reading binary, (i.e. no translation from ascii to bytevalues)

`--log` - enable instruction logging to a file called log.csv

The rightmost window contains the disassembly of the memory region around the program counter.
the highlighted instruction is the current instruction that program counter is pointing to.

The two top windows showcase the contents of memory in different regions.

The leftmost bottom window contains the status register. The window next to it displays
the values of the internal registers of the 6502. The window next to it displays the stack.
