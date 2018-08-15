/* bluetooth.c
 * Tremaine Ng
 * 5/27/2017
 *
 *	interfaces with a RN-42 bluetooth module
 */

#include <fcntl.h>
#include <stdio.h>      // for File IO and printf
#include <string.h>     //for strcat
#include <time.h>       // for usleep
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <signal.h>

// function declarations
//void usleep();

int main() {
	// initializes file pointer to /slots
	FILE *sys;

	// pointers for UART files and buffers
	int uart1;


	// enable uart1 ===============================================
	// RX: P9_26	TX: P9_24
	sys = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	usleep(5000);
	fprintf(sys, "%s", "BB-UART4");	// enable UART1
	usleep(100000);
	fflush(sys);
	fclose(sys);

	// open both ports
	uart1 = open("/dev/ttyO4", O_RDWR | O_NOCTTY); 

	if (uart1 < 0) {
		printf("Error -- ports failed to open!");
	}

	// create a termios structure to store uart settings info
	struct termios uart_settings;
	
	// clear memory
	bzero(&uart_settings, sizeof(uart_settings));

	// specify settings
	uart_settings.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
	uart_settings.c_iflag = IGNPAR | ICRNL;
	uart_settings.c_oflag = 0;
	uart_settings.c_lflag = 0;

	uart_settings.c_cc[VTIME] = 0;
	uart_settings.c_cc[VMIN] = 1;	// number of characters to send per word

	// flush uart files and set attributes
	tcflush(uart1, TCIFLUSH);

	tcsetattr(uart1, TCSANOW, &uart_settings);

	// create buffers for data in and out
	
	/*
	strcpy(buf_in, "$$$");

	// write "$$$" to Bluetooth and see if it responds???
	int error = write(uart1, &buf_in, 3);
	if (error < 0) {
		printf("ERROR -- failed to write!\n");
	} else {
		printf("After write.\n");
	}
	*/
	usleep(5000);
	


	// GPIO stuff
	// creates the file pointers for the directory and GPIOs
	FILE *dir;
	FILE *val[7];
	FILE *dutyPWM, *periodPWM;

	//             lsb                     msb
	int pins[7] = {66, 67, 68, 69, 44, 45, 26};

	// initializes pins
	for (int i = 0; i < 7; i++) {
		sys = fopen("/sys/class/gpio/export", "w");
		usleep(5000);
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
	setPins(val, 10);	// make stop
	
	usleep(100000);
	
	// adc read stuff
	//Enable ADC ports
	sys = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	usleep(50000);
	if (sys < 0) {
		printf("could not enable ADC slots\n");
	} else printf("enabled ADC slots\n");
	fprintf(sys, "%s", "cape-bone-iio");
	fflush(sys);
	fclose(sys);
	
	
	// PWM stuff
	sys = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	usleep(50000);
	if (sys < 0) {
		printf("could not enable PWM slots\n");
	} else printf("enabled PWM slots\n");
	fprintf(sys, "%s", "am33xx_pwm");
	fflush(sys);
	fprintf(sys, "%s", "bone_pwm_P9_14");
	fflush(sys);
	fclose(sys);
	
	usleep(1000000);
	dutyPWM = fopen("/sys/devices/ocp.3/pwm_test_P9_14.16/duty", "w");
	//usleep(1000000);
	if (dutyPWM < 0) {
		printf("could not open PWM duty\n");
	} else printf("opened PWM duty\n");
	fprintf(dutyPWM, "%d", 1000*200);
	//printf("1\n");
	fflush(dutyPWM);
	//printf("2\n");
	fclose(dutyPWM);
	//printf("3\n");
	
	
	usleep(1000000);
	periodPWM = fopen("/sys/devices/ocp.3/pwm_test_P9_14.16/period", "w");
	//printf("4\n");
	//usleep(1000000);
	if (periodPWM < 0) {
		printf("could not open PWM period\n");
	} else printf("opened PWM period\n");
	fprintf(periodPWM, "%d", 1000*500);	// period based on our audio frequency in lab 1
	fflush(periodPWM);
	fclose(periodPWM); 
	
	// read
	usleep(5000000);
	setPins(val, 9);
	
	usleep(5000000);
	setPins(val, 0);

	return(0);
}	

/*
	int readReturn = 0;
	int start = 0;
	while (!start) {
		usleep(10000);
		char buf_read[100] = {};
		int readResult = read(uart1, buf_read, 5);
		if (readResult > 0 && buf_read[0] == 'g') {
			start = 1;
		}
		printf("In loop, waiting for a go command.");
		
	}
	setPins(val, 10);
	for (int ii = 0; ii < 5000; ii++) {

		char buf_in[100] = {};
		char buf_out[100] = {};

		//readReturn = read(uart1, &buf_out, 20);
		readReturn = 1;
		if (readReturn > 0) {
			// read in sensor value from file
			FILE* sys2 = fopen("/sys/devices/ocp.3/helper.15/AIN0", "r");
			if (sys2 == NULL) {
				printf("File is null!\n");
			}	
			char buffer[5] = {};
			int success = fscanf(sys2, "%s", buffer);
			fflush(sys2);
			fclose(sys2);
			printf("%d\n", atoi(buffer));
			// write sensor value to terminal connected to bluetooth
			int error = write(uart1, buffer, 5);
			//int error2 = write(uart1,'a',20);//Trying to add a terminator
			if (error < 0) {
				printf("ERROR -- failed to write!\n");
			} else {
				printf("After write.\n");
			}
			printf("Reading from buffer: %s\n", buf_out);
			usleep(500);



			
			strcpy(buf_in, buf_out);
			int error = write(uart1, &buf_in, 20);
			if (error < 0) {
				printf("ERROR -- failed to write!\n");
			} else {
				printf("After write.\n");
			}
			
			
			
			//usleep(500);
		}
	}
	setPins(val, 0);
	while (1) {
		char buf_out[100] = {};
		readReturn = read(uart1, &buf_out, 20);
	
		if (buf_out[0] == 'f') {
				setPins(val, 6);
		} else if (buf_out[0] == 'b') {
			setPins(val, 9);
		} else if (buf_out[0] == 'l') {
			setPins(val, 10);
		} else if (buf_out[0] == 'r') {
			setPins(val, 5);
		} else if (buf_out[0] == 's') {
			setPins(val, 0);
		}
		usleep(100000);
	}
	usleep(100000);

	close(uart1);
} */

	// config the device within 60 seconds (configuration timer)
	// slave mode (SM, 0) - other devices discover and connect to module
	// pretty sure slave mode is default


	// send "$$$" to enter command mode
	// (should get the response "CMD")
	
	// send "ST,255" to set configuration timer to not expire
	// check if timer is in configuration mode -- should blink red at 10Hz
	
	// default BB baud rate 115,200 (from elinux.org)

	// send "D" tom buffer:  
	// to get basic settings information
	// (should respond by sending info)
	
	// send "GK" to show current connection status
	// (1, 0, 0 if connected, 0, 0, 0 if disconnected)

// sets pins
void setPins(FILE *val[7], int input){
	for (int i= 0; i<7; i++){
		int pinValue;
		pinValue = (input>>(i))%2;
		fprintf(val[i], "%d", pinValue);
		fflush(val[i]);
	}
}

void sleepWithInterrupts(int sec) { //add commment HERE
	struct timespec timeout;

	timeout.tv_sec = sec;
	timeout.tv_nsec = 0;

	while (nanosleep(&timeout, &timeout) && errno == EINTR);
}


