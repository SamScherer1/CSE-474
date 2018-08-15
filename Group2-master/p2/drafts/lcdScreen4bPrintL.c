/*	lcdScreen.c
 * Samuel Scherer
 * 4/5/17
 *
 *	Sets up LCD screen
 */

#include <stdio.h>      // for File IO and printf
#include <string.h>     //for strcat
#include <time.h>       // for usleep


#define GPIO_PIN9 27      // LED pin unused
#define GPIO_PIN10 65      // LED pin unused

int main() {
	// Creates the File pointers to create the gpio file system,
	// set the direction of the GPIO, and change the data stored there.
	FILE *sys, *dir, *sysPWM, *dutyPWM, *periodPWM;

	FILE *val[7];
	
	//function prototypes for setPins
	void setPins(FILE *val[], int input);
	void setPinsEn(FILE *val[], int input, void (*sleeperPtr)(int));

	//function declaration for usleep
	void usleep();
	void (*sleeperPtr)(int);

	sleeperPtr = &usleep;

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
	setPinsEn(val, 3, sleeperPtr); //Function set command 		1100000
	setPinsEn(val, 3, sleeperPtr); //Function set command 		1100000
	setPinsEn(val, 3, sleeperPtr); //Function set command 		1100000
	setPinsEn(val, 2, sleeperPtr); //Function set command (to 4 bit)0100000
	setPinsEn(val, 2, sleeperPtr); //Function set			0100000
	setPinsEn(val, 12, sleeperPtr);//N = bit 4 number of lines, F = bit 3 font	0011000

	setPinsEn(val, 0, sleeperPtr); //display off			0000000
	setPinsEn(val, 8, sleeperPtr);					

	setPinsEn(val, 0, sleeperPtr); //clear display			0000000
	setPinsEn(val, 1, sleeperPtr); //				1000000

	setPinsEn(val, 0, sleeperPtr);//entry mode set
	setPinsEn(val, 6, sleeperPtr);//				0110000

	setPinsEn(val, 0, sleeperPtr); //display on
	setPinsEn(val, 15, sleeperPtr);//				1111000
	
	setPinsEn(val, 100, sleeperPtr); //write L			1100100
	setPinsEn(val, 76, sleeperPtr);//				0011001

	while(1) {
	}


// ============= // 
//   FUNCTIONS   // 
// ============= // 

//move cursor
//when operating in 4 bit mode, send high order nibble, then low order nibble
//setting both numbers to 0 gets the cursor in upper right, column navigation on the first row is still unclear
/*
	setPinsEn(val, 0, sleeperPtr);//sets row (4 is second row)
	setPinsEn(val, 0, sleeperPtr);//sets column (0 is leftmost, 15 is rightmost character)*/

	return 0;
}

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
void setPinsEn(FILE *val[7], int input, void (*sleeperPtr)(int)){
	int inputPlusSixteen;
	inputPlusSixteen = input +16;
	for (int i= 6; i>=0; i--){
		int pinValue;
		pinValue = (input>>(6-i))%2;
		fprintf(val[i], "%d", pinValue);
		fflush(val[i]);
	}
	(*sleeperPtr)(5000);
	for (int i= 6; i>=0; i--){
		int pinValue;
		pinValue = (inputPlusSixteen>>(6-i))%2;
		fprintf(val[i], "%d", pinValue);
		fflush(val[i]);
	}
	(*sleeperPtr)(5000);
	for (int i= 6; i>=0; i--){
		int pinValue;
		pinValue = (input>>(6-i))%2;
		fprintf(val[i], "%d", pinValue);
		fflush(val[i]);
	}
	(*sleeperPtr)(5000);
}

