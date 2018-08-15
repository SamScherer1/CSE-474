/* lcdScreen.c
 * Samuel Scherer
 * 4/5/17
 *
 *	Sets up LCD screen
 */

#include <fcntl.h>
#include <stdio.h>      // for File IO and printf
#include <string.h>     //for strcat
#include <time.h>       // for usleep
#include <sys/stat.h>
#include <unistd.h>


#define GPIO_PIN9 27      // LED pin unused
#define GPIO_PIN10 65      // LED pin unused
#define MAX_BUF 3


// function declarations
void setPins(FILE *val[], int input);
void setPinsEn(FILE *val[], int input);
void usleep();

int main() {
	// Creates the File pointers to create the gpio file system,
	// set the direction of the GPIO, and change the data stored there.
	FILE *sys, *dir;
	FILE *val[7];

	int pins[7] = {69, 45, 44, 26, 47, 46, 27};
	int count = 0;
	for (int i = 0; i < 7; i++){
		int a = i;
		sys = fopen("/sys/class/gpio/export", "w");
		fprintf(sys, "%d", pins[i]);
		fflush(sys);
		fclose(sys);
	}

	//Set the gpio to output
	for (int i = 0; i < 7; i++){
		char directoryName[80];
		char pinNum[80];
		memset(directoryName, 0, 80);
		strcat(directoryName, "/sys/class/gpio/gpio");
		sprintf(pinNum, "%d", pins[i]);
		strcat(directoryName, pinNum);
		strcat(directoryName, "/direction");
		printf("%s \n", directoryName);
		dir = fopen(directoryName, "w");
		fseek(dir, 0, SEEK_SET);
		fprintf(dir, "%s", "out");
		fflush(dir);
		fclose(dir);
	}

	//Open the files that control if the pin is high or low
	for (int i = 0; i < 7; i++){
		char directoryName[80];
		char pinNum[80];
		memset(directoryName, 0, 80);
		strcat(directoryName, "/sys/class/gpio/gpio");
		sprintf(pinNum, "%d", pins[i]);
		strcat(directoryName, pinNum);
		strcat(directoryName, "/value");
		printf("%s \n", directoryName);
		val[i] = fopen(directoryName, "w");
		fseek(val[i], 0, SEEK_SET);

	}
	
	//power on
	usleep(500000);//more than 15 ms
	
	//initialize board (4 bit)
	setPinsEn(val, 3); //Function set command 		1100000
	setPinsEn(val, 3); //Function set command 		1100000
	setPinsEn(val, 3); //Function set command 		1100000
	setPinsEn(val, 2); //Function set command (to 4 bit)0100000
	setPinsEn(val, 2); //Function set			0100000
	setPinsEn(val, 12); //N = bit 4 number of lines, F = bit 3 font	0011000

	setPinsEn(val, 0); //display off			0000000
	setPinsEn(val, 8);					

	setPinsEn(val, 0); //clear display			0000000
	setPinsEn(val, 1); //				1000000

	setPinsEn(val, 0); //entry mode set
	setPinsEn(val, 6); //				0110000

	setPinsEn(val, 0); //display on
	setPinsEn(val, 15); //				1111000


	// create a pipe (same file as in write_to_screen.c) and a buffer for reading the pipe
    char* myfifo = "/tmp/myfifo";
    char buf[MAX_BUF];

	// keep track of the number of characters printed so far
    int charsInLine = 0;
    
	// open, read, and display the message from the FIFO
    while(1) {
		// read input
        int fd = open(myfifo, O_RDONLY);
        read(fd, buf, MAX_BUF);
		int charIn = (int)buf[0];

		// if we pass in "\n" and we are on the first line, go to the next line
		if ((charIn == 10) && (charsInLine < 16)) {
			setPinsEn(val, 12);
			setPinsEn(val, 0);
			charsInLine = 16;
		}
		
		// if we pass in "\n" and we are on the second, clear screen and reset
		else if ((charIn == 10) && (charsInLine >= 16)) {
			setPinsEn(val, 0);
			setPinsEn(val, 1);
			charsInLine = 0;
		}
		
		// otherwise, just print the character
		else {
			// set pins
			setPinsEn(val, charIn/16 + 64 + 32);
			setPinsEn(val, (charIn % 16) + 64);
	
			// update the number of characters
			charsInLine++;

			// move to line 2 when first line full, and clear everything and reset when both are full
			if (charsInLine == 16) {	
				setPinsEn(val, 12);
				setPinsEn(val, 0);
			} else if (charsInLine == 33) {
				setPinsEn(val, 0);
				setPinsEn(val, 1);
				charsInLine = 0;
			}
		}
		
		// print to console and close 
        printf("%d: %d %d\n", charIn, charIn/16 + 64 + 32, (charIn % 16) + 64);
		usleep(5000);
        close(fd);
    }
	return 0;
}


// ============= // 
//   FUNCTIONS   // 
// ============= // 

//move cursor
//when operating in 4 bit mode, send high order nibble, then low order nibble
//setting both numbers to 0 gets the cursor in upper right, column navigation on the first row is still unclear
/*
	setPinsEn(val, 0);//sets row (4 is second row)
	setPinsEn(val, 0);//sets column (0 is leftmost, 15 is rightmost character)*/


//sets pins
void setPins(FILE *val[7], int input){
	for (int i= 6; i>=0; i--){
		int pinValue;
		pinValue = (input>>(6-i))%2;
		fprintf(val[i], "%d", pinValue);
		fflush(val[i]);
	}
}

//sets pins, flicks enable up and down
void setPinsEn(FILE *val[7], int input){
	
	// add 2^4 = 16 to flip enable pin to 1	
	int inputPlusSixteen = input + 16;
	
	// print to all pins with EN = 0
	for (int i= 6; i>=0; i--){
		int pinValue;
		pinValue = (input>>(6-i))%2;
		fprintf(val[i], "%d", pinValue);
		fflush(val[i]);
	}
	usleep(5000);

	// print to all pins with EN = 1	
	for (int i= 6; i>=0; i--){
		int pinValue;
		pinValue = (inputPlusSixteen>>(6-i))%2;
		fprintf(val[i], "%d", pinValue);
		fflush(val[i]);
	}
	usleep(5000);

	// print to all pins with EN = 0	
	for (int i= 6; i>=0; i--){
		int pinValue;
		pinValue = (input>>(6-i))%2;
		fprintf(val[i], "%d", pinValue);
		fflush(val[i]);
	}
	usleep(5000);
}
