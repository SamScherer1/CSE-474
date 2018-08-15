This project includes two parts:

in lcd folder:
	Loadable kernel module lcd.c and header lcd.h
	Makefile
	Test .c files, writes text to kernel module node

in calculator folder:
	calculator user space code

The loadable kernel module defined by lcd.c and lcd.h create a driver that can be accessed through a node
in the /dev/ folder in the beaglebone (requires mknod, print kernel message after insmod to see instruction).
Opening this file will initialize the LCD display and writing text to it will print that string.
The '\n' character will change to a new line, and this will also automatically be added if the text writes off
the side of the screen. A new line being input on the second line (or overflowing text again) will clear 
THe display and begin at the first line again.

The calculator interfaces with the kernel node. The user writes a calculator operation consisting of
+, -, *, / (follows order of operations, no spaces) and the lcd display will print the operation, then the answer
on the next line, wait a short amount of time then clear the display. Future operations will implicitly use
the last value produced if a one sided operation (like +2 as opposed to 2+2) is at the beginning.
Special cases of dividing by zero, adding a value to infinity and multiplying infinity by 0 are covered.