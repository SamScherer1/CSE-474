/*	blinky.c
 * Adrian Prananda Lincong Li
 * 30 / Sept / 2015
 *
 *	Blinks and external LED from gpio0[8] = x70 pin 27
 */

#include <stdio.h>      // for File IO and printf
#include <time.h>       // for usleep

#define GPIO_PIN 66      // LED pin
#define GPIO_PIN1 67      // LED pin
#define GPIO_PIN2 68      // LED pin

int main() {
	int count = 0;
	// Creates the File pointers to create the gpio file system,
	// set the direction of the GPIO, and change the data stored there.
	FILE *sys, /**sys1, *sys2,*/ *dir, *val, *val1, *val2;

	// Specifies the file that the pointer will be used for (w = //write)
	sys = fopen("/sys/class/gpio/export", "w");
	fprintf(sys, "%d", GPIO_PIN);
	// Clears the FILE stream for sys file object.
	fflush(sys);
	fclose(sys);

	sys = fopen("/sys/class/gpio/export", "w");
	fprintf(sys, "%d", GPIO_PIN1);
	fflush(sys);
	fclose(sys);

	sys = fopen("/sys/class/gpio/export", "w");
	fprintf(sys, "%d", GPIO_PIN2);
	fflush(sys);
	fclose(sys);

	
	// prints value inside quotes into given file.
	// %d means it will print some variable as a decimal.
	// GPIO_PIN is the variable refered to by %d.
	
	

	
	
	//Set the gpio to output
	dir = fopen("/sys/class/gpio/gpio66/direction", "w");
	fseek(dir, 0, SEEK_SET);
	fprintf(dir, "%s", "out");
	fflush(dir);
	fclose(dir);

	dir = fopen("/sys/class/gpio/gpio67/direction", "w");
	fseek(dir, 0, SEEK_SET);
	fprintf(dir, "%s", "out");
	fflush(dir);
	fclose(dir);

	dir = fopen("/sys/class/gpio/gpio68/direction", "w");
	fseek(dir, 0, SEEK_SET);
	fprintf(dir, "%s", "out");
	fflush(dir);
	fclose(dir);

	//Opens the file that controls if the pin is high or low
	val = fopen("/sys/class/gpio/gpio66/value", "w");
	fseek(val, 0, SEEK_SET);

	val1 = fopen("/sys/class/gpio/gpio67/value", "w");
	fseek(val1, 0, SEEK_SET);

	val2 = fopen("/sys/class/gpio/gpio68/value", "w");
	fseek(val2, 0, SEEK_SET);

	//Blinks the LED
	while(1) {
		if ((count %2) == 1){
			fprintf(val, "%d", 1);
			fflush(val);
		}else{
			fprintf(val, "%d", 0);
			fflush(val);	
		}

		if (((count/2)%2) == 1){
			fprintf(val1, "%d", 1);
			fflush(val1);
		}else{
			fprintf(val1, "%d", 0);
			fflush(val1);	
		}

		if (((count/4)%2) == 1){
			fprintf(val2, "%d", 1);
			fflush(val2);
		}else{
			fprintf(val2, "%d", 0);
			fflush(val2);	
		}
		
		//fprintf(val1, "%d", 1);
		//fflush(val1);
      		// delays for given length of time in microseconds.
		count = count +1;
     		usleep(500000);		
	}

   // close all files
	//fclose(sys);
	//fclose(sys1);
	fclose(val);
	//fclose(dir1);
	fclose(val1);
	fclose(val2);
	return 0;
}
