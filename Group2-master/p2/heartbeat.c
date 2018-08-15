/*	blinky.c
 * Adrian Prananda Lincong Li
 * 30 / Sept / 2015
 *
 *	Blinks and external LED from gpio0[8] = x70 pin 27
 */

#include <stdio.h>      // for File IO and printf
#include <time.h>       // for usleep

#define GPIO_PIN 20      // LED pin

void usleep();

int main() {
	// Creates the File pointers to create the gpio file system,
	// set the direction of the GPIO, and change the data stored there.
	FILE *sys, *dir, *val;

	// Specifies the file that the pointer will be used for (w = //write)
	sys = fopen("/sys/class/gpio/export", "w");
	fprintf(sys, "%d", GPIO_PIN);
	// Clears the FILE stream for sys file object.
	fflush(sys);
	fclose(sys);

	//Set the gpio to output
	dir = fopen("/sys/class/gpio/gpio20/direction", "w");
	fseek(dir, 0, SEEK_SET);
	fprintf(dir, "%s", "out");
	fflush(dir);
	fclose(dir);


	//Opens the file that controls if the pin is high or low
	val = fopen("/sys/class/gpio/gpio20/value", "w");
	fseek(val, 0, SEEK_SET);


	//Blinks the LED
	int count = 0;
	while(1) {
		if ((count %2) == 1){
			fprintf(val, "%d", 1);
			fflush(val);
		}else{
			fprintf(val, "%d", 0);
			fflush(val);	
		}
		
      	// delays for given length of time in microseconds.
		count = count +1;
     	usleep(0.1);	
	}

	fclose(val);

	return 0;
}
