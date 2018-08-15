#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>     //for strcat
#include <time.h>       // for usleep
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define SIG_USER 22
#define SEARCH 1
#define GO 2
#define DONE 3
#define SPIN 4

// function declarations
void setPins(FILE *val[], int input);
int atoi(char[]);
int mean(int* values, int length);
int max(int a, int b);
void sleepWithInterrupts(int sec);


// GPIO file pointers are a global variable
FILE *val[7];


int currentValue = 0;
int maxValue = 0;
int counter = 0;
int timeAveragedOutputs[4];

int tempValue1[10] = {};
int tempValue2[10] = {};
int tempValue3[10] = {};
int tempValue4[10] = {};



int mean(int* values, int length) {
	int sum = 0;
	for (int i = 0; i < length; i++) {
		sum = sum + values[i];
	}
	sum = sum/length;
	return sum;
}

int max(int a, int b) {
	if (a > b) {
		return a;
	} else {
		return b;
	}
}


void sleepWithInterrupts(int sec) { //add commment HERE
	struct timespec timeout;

	timeout.tv_sec = sec;
	timeout.tv_nsec = 0;

	while (nanosleep(&timeout, &timeout) && errno == EINTR);
}


/* Callback function that is called every time the timer interrupts this program. */
void sig_handler(int signo) {
	// Buffers to read ADC output into
	char buffer1[100];
	char buffer2[100];
	char buffer3[100];
	char buffer4[100];
	

	//Read ADC port
	FILE* adc1 = fopen("../sys/devices/ocp.3/helper.15/AIN0", "r");
	FILE* adc2 = fopen("../sys/devices/ocp.3/helper.15/AIN1", "r");
	FILE* adc3 = fopen("../sys/devices/ocp.3/helper.15/AIN2", "r");
	FILE* adc4 = fopen("../sys/devices/ocp.3/helper.15/AIN3", "r");
	if ((adc1 == NULL) | (adc2 == NULL) | (adc3 == NULL) | (adc4 == NULL)) {
		printf("File is null!\n");
	}	
		
	// Read in sensor value from file
	
	fscanf(adc1, "%s", buffer1);
	fscanf(adc2, "%s", buffer2);
	fscanf(adc3, "%s", buffer3);
	fscanf(adc4, "%s", buffer4);
	
	fflush(adc1); fflush(adc2); fflush(adc3); fflush(adc4);
	fclose(adc1); fclose(adc2); fclose(adc3); fclose(adc4);

	// Print to terminal
	//printf("ADCval1: %d, ADCval2: %d, ADCval3: %d\n, ADCval4: %d\n", atoi(buffer1), atoi(buffer2), atoi(buffer3), atoi(buffer4));

	

	
	// add the current sensor reading to the array of recent readings
	tempValue1[counter] = atoi(buffer1);
	tempValue2[counter] = atoi(buffer2);
	tempValue3[counter] = atoi(buffer3);
	tempValue4[counter] = atoi(buffer4);


	// 
	if (counter == 9){
		timeAveragedOutputs[0] = mean(tempValue1, 10);
		timeAveragedOutputs[1] = mean(tempValue2, 10);
		timeAveragedOutputs[2] = mean(tempValue3, 10);
		timeAveragedOutputs[3] = mean(tempValue4, 10);
		currentValue = timeAveragedOutputs[1];
		maxValue = max(maxValue, currentValue);
		printf("Current value: %d\n", currentValue);
		counter = 0;
	}
	counter++;
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

	//             lsb                     msb
	int pins[7] = {66, 67, 68, 69, 44, 45, 26};

	// initializes pins
	for (int i = 0; i < 7; i++) {
		sys = fopen("/sys/class/gpio/export", "w");
		fprintf(sys, "%d", pins[i]);
		fflush(sys);
		fclose(sys);
		printf("Opened pin %d... \n", pins[i]);
	}

	// set the gpio to output
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
	setPins(val, 0);
	int state = SEARCH;
	int closestObjectValue = 0;
	//int startValue; // for later

	while (state != DONE) {
		// spin and record values, hold minimum value
		// go forward until sensors detect we are close
			// distance decrease, go back to search
		// done


		if (state == SEARCH) {
			
			printf("Just entered SEARCH.\n");
			maxValue = 0;
			setPins(val, 10); // turnls
			sleepWithInterrupts(5); // wait for five seconds
			closestObjectValue = maxValue; // find closest object
			printf("Closest object value is: %d\n", closestObjectValue);
			
			state = SPIN;
		} else if (state == SPIN) {
			printf("Just entered SPIN.\n");
			setPins(val, 10);
			if ((currentValue > closestObjectValue - 200)) {
				state = GO;
			}
		} else if (state == GO) {
			printf("Just entered GO.\n");
			setPins(val, 6);
			
			if (timeAveragedOutputs[2] > 950) {
				state = DONE;
				printf("Just entered DONE.\n");
				setPins(val, 0);
				printf("Should have turned off motors.\n");
				sleepWithInterrupts(1);
				return 0;
			}
		
		} else if (state == DONE) {
			printf("Just entered DONE.\n");
			setPins(val, 0);
			sleepWithInterrupts(10);
		} else {
			printf("Something went wrong.\n");
		}
		
	}

	setPins(val, 0);
	printf("Should have turned off motors.\n");
	sleepWithInterrupts(1);
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
