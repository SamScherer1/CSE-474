The program "lcdscreen.c" initializes the GPIO pins used to interface with the LCD screen. The GPIO pins gives the correct sequence of outputs to initialize the screen to 4 bit mode with two lines and a right-scrolling, blinking cursor, then creates a reader FIFO.

The program "write_to_screen.c" creates a writer FIFO that interfaces with "lcdscreen.c" to feed it characters to output.

The program "heartbeat.c" flips a GPIO at a fast rate to view jitter effects from noise and the scheduler

The makefile compiles all these programs into the exe programs "lcd", "writer", and "heartbeat", respectively.
The makefile also cleans up extra files in the folder.