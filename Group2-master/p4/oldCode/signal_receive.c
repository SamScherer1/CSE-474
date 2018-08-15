#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>     //for strcat
#include <time.h>       // for usleep
#include <sys/stat.h>
#include <fcntl.h>
#define SIG_USER 22

// function declarations
void setPins(FILE *val[], int input);
int atoi(char[]);

// GPIO file pointers are a global variable
FILE *val[7];
FILE *reset;
char stop = '0';

/* Callback function that is called every time the timer interrupts this program. */
void sig_handler(int signo) {
	//Read ADC port
	FILE* adc1 = fopen("../sys/devices/ocp.3/helper.15/AIN0", "r");
	FILE* adc2 = fopen("../sys/devices/ocp.3/helper.15/AIN1", "r");
	FILE* adc3 = fopen("../sys/devices/ocp.3/helper.15/AIN2", "r");
	if (adc1 == NULL | adc2 == NULL | adc3 == NULL) {
		printf("File is null!\n");
	}	
		
	// Read in sensor value from file
	char buffer1[100]; char buffer2[100]; char buffer3[100];
	fscanf(adc1, "%s", buffer1);
	fscanf(adc2, "%s", buffer2);
	fscanf(adc3, "%s", buffer3);
	
	fflush(adc1); fflush(adc2); fflush(adc3);
	fclose(adc1); fclose(adc2); fclose(adc3);

	// Print to terminal
	printf("ADC value 1: %d, ADC value 2: %d, ADC value 3: %d\n", atoi(buffer1), atoi(buffer2), atoi(buffer3));
}


/* Main: initializes and ets up GPIO pins for motor, then drives back and forth. */ 
int main(void) {

	// checks signal properties for correctly connecting to a timer.
    if (signal(SIG_USER, sig_handler) == SIG_ERR)
  	printf("\ncan't catch SIGINT\n");
    
	// enable ADC ports
    FILE *sysADC;
    sysADC = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fprintf(sysADC, "%s", "cape-bone-iio");
	fflush(sysADC);
	fclose(sysADC);

	// creates the file pointers for the directory and GPIOs
	FILE *sys, *dir;

	// GPIO pin numbers on beaglebone
	//             lsb                     msb
	int pins[7] = {66, 67, 68, 69, 44, 45, 26};
	int resetPin = 20;
	int count = 0;

	// initialize pins ===============================
	sys = fopen("/sys/class/gpio/export", "w");
	for (int i = 0; i < 7; i++) {
		//sys = fopen("/sys/class/gpio/export", "w");
		fprintf(sys, "%d", pins[i]);
		fflush(sys);
		//fclose(sys);
		printf("Opened pin %d... \n", pins[i]);
	}

	// initialize reset pin
	fprintf(sys, "%d", resetPin);
	fflush(sys);
	fclose(sys);
	printf("Opened reset pin %d... \n", resetPin);

	// set GPIO to output ============================
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

	// set reset GPIO to input
	char directoryName[80];
	char pinNum[80];
	memset(directoryName, 0, 80);
	strcat(directoryName, "/sys/class/gpio/gpio");
	sprintf(pinNum, "%d", resetPin);
	strcat(directoryName, pinNum);
	strcat(directoryName, "/direction");
	printf("%s \n", directoryName);
	dir = fopen(directoryName, "w");
	fseek(dir, 0, SEEK_SET);
	fprintf(dir, "%s", "in");
	fflush(dir);
	fclose(dir);

	// open the files that control if the pin is high or low
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

	// open file that reads input pin
	memset(directoryName, 0, 80);
	strcat(directoryName, "/sys/class/gpio/gpio");
	sprintf(pinNum, "%d", resetPin);
	strcat(directoryName, pinNum);
	strcat(directoryName, "/value");
	printf("%s \n", directoryName);
	reset = fopen(directoryName, "r");
	fseek(reset, 0, SEEK_SET);

	// drive forwards and backwards in a loop, should terminate after 10 seconds
	// 6 - spin towards breadboard
	// 9 - spin towards beaglebone
	for (int i = 0; i < 10; i++) {
		setPins(val, 6);
    	usleep(500000);
    	setPins(val, 9);
    	usleep(500000);
		
		// check reset value
		char buffer[5];
		fscanf(reset, "%s", buffer);
		printf("buffer = %s \n", buffer);
		printf("buffer[0] = %c \n", buffer[0]);
		stop = buffer[0];
		printf("from main loop stop = %c \n", stop);
		fflush(reset);
		if (stop == '1') {
			setPins(val, 0);
			return 0;
		}
    }

	return 0;
}


// sets pins
void setPins(FILE *val[7], int input){
	for (int i= 0; i<7; i++){
		int pinValue;
		pinValue = (input>>(i))%2;
		fprintf(val[i], "%d", pinValue);
		fflush(val[i]);
	}
}
