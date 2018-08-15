/*	lcdScreen.c
 * Samuel Scherer, Tremaine Ng, Irina Tolkova
 * 4/5/17
 *
 *	Interface with 19988 Optrex DmcSeries LCD screen
 *	uses 4 bit mode but has access to all pins for 8 bit mode
 */	

#include <stdio.h>      // for File IO and printf
#include <string.h>     // for strcat
#include <time.h>       // for usleep

#define NUM_PINS 11
#define PIN_RS 0
#define PIN_RW 1
#define PIN_EN 2

// function declarations ====================================================================
// directory creation
void createDirectoryName(char* directoryName, int pinNumber);
void createDirectoryNameDirection(char* directoryName, int pinNumber);
void createDirectoryNameValue(char* directoryName, int pinNumber);
// pin manipulation
void setEn(FILE *val[NUM_PINS], char bit);
void flipEn(FILE *val[NUM_PINS]);
void setRSRW(FILE *val[], char* bits);
void setDataBus4b(FILE *val[], char* bits);
void setDataBus8b(FILE *val[], char* bits);
/*
void setPins(FILE *val[], int input);
void setPinsEn(FILE *val[], int input);
void setPins2(char* DB, char* control, FILE *val[NUM_PINS]);
void setPins3(char* DB, char* control, FILE *val[NUM_PINS]);
*/

void usleep();


// DB pins, index 0 = DB0, ... index 7 = DB7
int PIN_DB[8] = { 3, 4, 5, 6, 7, 8, 9, 10 };	// all pins accessible, only last 4 used in 4 bit operation


int main() {
	// creates file pointers (val stores file pointers for all of the necessary pins)
	FILE *sys, *dir, *val[NUM_PINS];

	// pins in respective order of location in val[] ====================================
						// functions in respective order
	int pins[NUM_PINS] = {69, 45, 44, 	// 0-2:  register select, read/write, enable
			48, 49, 117, 115,	// 3-6:  DB 0-3
			27, 46, 47, 26};	// 7-10: DB 4-7	used in 4 bit mode

	// initialize pins
	sys = fopen("/sys/class/gpio/export", "w");
	for (int i = 0; i < NUM_PINS; i++){
		fprintf(sys, "%d", pins[i]);
		fflush(sys);
	}
	fclose(sys);
	
	// create directory names and set GPIOs to output
	for (int i = 0; i < NUM_PINS; i++){
		// create directory name
		char directoryName[80];	
		createDirectoryNameDirection(directoryName, pins[i]);

		// print to directory
		dir = fopen(directoryName, "w");
		fseek(dir, 0, SEEK_SET);
		fprintf(dir, "%s", "out");
		fflush(dir);
		fclose(dir);
	}


	// open the files that control if the pin is high or low
	for (int i = 0; i < NUM_PINS; i++){
		// create directory name		
		char directoryName[80];
		createDirectoryNameValue(directoryName, pins[i]);
		//printf("%s \n", directoryName);
		
		// print value
		val[i] = fopen(directoryName, "w");
		fseek(val[i], 0, SEEK_SET);
	}

	// power on
	usleep(500000);
	
	// INITIALIZATION ===========================================================================
	// in 4 bit mode send high order nibble then low order nibble

	// 4 bit initialization		// respective bit order to function, x = don't care
	
	setEn(val, '0');	// begin at 0
	setRSRW(val, "00");
	// function set
	setDataBus4b(val, "0010");	// 8 bit mode > 4 bit mode
	// always use 4 bit double instructions after this
	setDataBus4b(val, "0010");	// 0 - 0 - 1 - Bits
	setDataBus4b(val, "1000");	// NumLines - DotMatrixSize - x - x
	// Display ON
	setDataBus4b(val, "0000");
	setDatabus4b(val, "1111");	// 1 - Display - Cursor - Blink
	// Entry mode set
	setDataBus4b(val, "0001");
	setDataBus4b(val, "0100");	// Shift - Right/Left - x - x
	
	// set to write mode
	setRSRW(val, "11");
	// print L
	setDataBus4b(val, "0100");
	setDataBus4b(val, "1100");
	
/*


// set to write mode
	setRSRW(val, "10");
	// print L
	setDataBus4b(val, "0100");
	setDataBus4b(val, "1100");

// set to write mode
	setRSRW(val, "01");
	// print L
	setDataBus4b(val, "0100");
	setDataBus4b(val, "1100");*/


	char* myfifo = "/tmp/myfifo/";	// fifo folder address
	
	return 0; 
}



// ============= //
//   FUNCTIONS   //
// ============= //

// create string representing directory name
void createDirectoryName(char* directoryName, int pinNumber) {
	char pinNum[80];
	memset(directoryName, 0, 80);	// reset directory name
	strcat(directoryName, "/sys/class/gpio/gpio");
	sprintf(pinNum, "%d", pinNumber);
	strcat(directoryName, pinNum);
}

// create string representing directory/direction
void createDirectoryNameDirection(char* directoryName, int pinNumber) {
	char pinNum[80];
	memset(directoryName, 0, 80);	// reset directory name
	strcat(directoryName, "/sys/class/gpio/gpio");
	sprintf(pinNum, "%d", pinNumber);
	strcat(directoryName, pinNum);
	strcat(directoryName, "/direction");
}

// create string representing directory/value
void createDirectoryNameValue(char* directoryName, int pinNumber) {
	char pinNum[80];
	memset(directoryName, 0, 80);	// reset directory name
	strcat(directoryName, "/sys/class/gpio/gpio");
	sprintf(pinNum, "%d", pinNumber);
	strcat(directoryName, pinNum);
	strcat(directoryName, "/value");
}

// set register select and read/write
void setRSRW(FILE *val[NUM_PINS], char* bits) {
	//printf("In setRS\n");	
	//printf("bits[0]: %c\n", bits[0]);
	fprintf(val[PIN_RS], "%c", bits[0]);
	fflush(val[PIN_RS]);
	
	//printf("bits[1]: %c\n", bits[1]);
	fprintf(val[PIN_RW], "%c", bits[1]);
	fflush(val[PIN_RW]);

	//printf("Finished\n");
}

void setEn(FILE *val[NUM_PINS], char bit) {
	fprintf(val[PIN_EN], "%c", bit);
	fflush(val[PIN_EN]);
}

// flip the enable pin for screen's internal computation
void flipEn(FILE *val[NUM_PINS]) {
	setEn(val, '1');	// flip to 1
	// set busy pin to input
	// wait shortly so pin is set to 1
	// when busy pin off, revert to output
	// undo enable
	usleep(5000);
	setEn(val, '0');
	usleep(5000);
}

// set upper 4 data bus pin values (low index = most significant bits)
// for use in 4 bit operation, then flips enable
// must be passed string of 4 or more chars or out of bounds will happen! TODO check for insufficient size
void setDataBus4b(FILE *val[NUM_PINS], char* bits) {
	for (int i = 0; i < 4; i++) {	// lower order bits unused
		fprintf(val[PIN_DB[i]], "%c", '0');
		fflush(val[PIN_DB[i]]);
	}
	for (int i = 4; i < 8; i++) {	// higher order bits used
		// assign cooresponding bits DB indexes in val
		fprintf(val[PIN_DB[i]], "%c", bits[3-i]);
		fflush(val[PIN_DB[i]]);
	}
	flipEn(val);
}

// set all data bus pin values (low index = most significant bits)
// for use in 8 bit operation, then flips enable
// must be passed a string of 8 or more chars or out of bounds will happen! TODO check for insufficient size
void setDataBus8b(FILE *val[NUM_PINS], char* bits) {
	for (int i = 0; i < 8; i++) {
		// assign cooresponding bits DB indexes in val
		fprintf(val[PIN_DB[i]], "%c", bits[7-i]);
		fflush(val[PIN_DB[i]]);
	}
	flipEn(val);
}




