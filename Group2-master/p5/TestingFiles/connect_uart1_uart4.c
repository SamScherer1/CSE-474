/* CONNECT UART1 AND UART4
 *	May 29th, 2017
 *
 *	Circuitry: connect RX/TX of UART ports 1 and 4.
 *	This script sets up the BeagleBone UART ports 1 and 4.
 *	It then indefinitely writes the string "abc" to UART1 and reads from UART4, once per second.
 */

#include <fcntl.h>
#include <stdio.h>      // for File IO and printf
#include <string.h>     //for strcat
#include <time.h>       // for usleep
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>


// function declarations
void usleep();

int main() {
	// initializes file pointer to /slots
	FILE *sys;

	// pointers for UART files and buffers
	int uart1; int uart4;


	// enable UART1 ===============================================
	// RX: P9_26	TX: P9_24
	sys = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fprintf(sys, "%s", "BB-UART1");	// enable UART1
	usleep(100000);
	fflush(sys);
	fprintf(sys, "%s", "BB-UART4");	// enable UART4
	usleep(100000);
	fflush(sys);
	fclose(sys);

	// open both ports
	uart1 = open("/dev/ttyO1", O_RDWR | O_NOCTTY); 
	uart4 = open("/dev/ttyO4", O_RDWR | O_NOCTTY);

	if (uart1 < 0 || uart4 < 0) {
		printf("Error -- ports failed to open!");
	}

	// create a termios structure to store uart settings info
	struct termios uart_settings;
	
	// clear memory
	bzero(&uart_settings, sizeof(uart_settings));

	// specify settings
	uart_settings.c_cflag = B4800 | CS8 | CLOCAL | CREAD;
	uart_settings.c_iflag = IGNPAR | ICRNL;
	uart_settings.c_oflag = 0;
	uart_settings.c_lflag = 0;

	uart_settings.c_cc[VTIME] = 0.5;
	uart_settings.c_cc[VMIN] = 0;

	// flush uart files and set attributes
	tcflush(uart1, TCIFLUSH);
	tcflush(uart4, TCIFLUSH);

	tcsetattr(uart1, TCSANOW, &uart_settings);
	tcsetattr(uart4, TCSANOW, &uart_settings);

	// write and read in a while loop
	char buf1[100] = {};
	char buf4[100] = {};

	strcpy(buf1, "abc");

	usleep(100000);




	while(1) {
		/*
		printf("--\n");
		
		// write
		int error = write(uart1, &buf1, 20);
		if (error < 0) {
			printf("ERROR -- failed to write!\n");
		} else {
			printf("After write.\n");
		}
		
		usleep(100000);
		*/
		// read
		int readReturn = read(uart1, &buf4, 20);
		if (readReturn > 0) {
			printf("Reading from buffer: %s \n", buf4);
		} else {
			printf("Nothing to read. Read returns: %d\n", readReturn);
		}
		usleep(1000000);

		
	}

	close(uart1);
	close(uart4);


}


