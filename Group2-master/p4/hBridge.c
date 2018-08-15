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


// function declarations
void setPins(FILE *val[], int input);
void usleep();

int main() {
	// Creates the File pointers to create the gpio file system,
	// set the direction of the GPIO, and change the data stored there.
	FILE *sys, *dir;
	FILE *val[7];
//                     lsb                     msb
	int pins[7] = {66, 67, 68, 69, 44, 45, 26};
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
		//printf("%s \n", directoryName);
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
		//printf("%s \n", directoryName);
		val[i] = fopen(directoryName, "w");
		fseek(val[i], 0, SEEK_SET);

	}
	
	//power on
	while(1){
		setPins(val, 6);
		usleep(5000000);
		setPins(val, 0);
		usleep(5000000);
		setPins(val, 9);
		usleep(5000000);
		setPins(val, 0);
		usleep(5000000);
		setPins(val, 10);
		usleep(5000000);
		setPins(val, 0);
		usleep(5000000);
		setPins(val, 5);
		usleep(5000000);
		printf("Should move one iteration.\n");
	}
	return 0;
}


// ============= // 
//   FUNCTIONS   // 
// ============= // 


//sets pins
void setPins(FILE *val[7], int input){
	for (int i= 0; i<7; i++){
		int pinValue;
		pinValue = (input>>(i))%2;
		fprintf(val[i], "%d", pinValue);
		fflush(val[i]);
	}
}


// need to do
// store "recent adc values" field
// 



