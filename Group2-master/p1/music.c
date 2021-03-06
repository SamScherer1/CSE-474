/*	music.c
 * Samuel Scherer, Tremaine Ng, Irina Tolkova
 * 5 / April / 2017
 *
 *	Blinks 3 external leds in counting order from 0-7 (GPIO 66:68)
 *      Plays tones from external buzzer from PWM pin EHRPWM1A
 *      Tones get higher in frequency as count goes up
 */

#include <stdio.h>      // for File IO and printf
#include <time.h>       // for usleep

#define GPIO_PIN0 66      // LED pin
#define GPIO_PIN1 67      // LED pin
#define GPIO_PIN2 68      // LED pin

int main() {
	// Creates the File pointers to create the gpio and pwm file system,
	// set the direction of the GPIO, and change the data stored there.
	// Set duty and period with *dutyPWM, *periodPWM

	FILE *sys, *dir, *val0, *val1, *val2, *sysPWM, *dutyPWM, *periodPWM;

	// Specifies the file that the pointer will be used for (w = //write)

	sysPWM = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fseek(sysPWM, 0, SEEK_CUR);
	fprintf(sysPWM, "%s", "am33xx_pwm");
	fflush(sysPWM);
	fprintf(sysPWM, "%s", "bone_pwm_P9_14");
	fflush(sysPWM);
	fclose(sysPWM);
	usleep(500000);

	sys = fopen("/sys/class/gpio/export", "w");
	fprintf(sys, "%d", GPIO_PIN0);
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


	//Set pin duty to 20000
	
	dutyPWM = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/duty", "w");
	fprintf(dutyPWM, "%d", 20000);
	fflush(dutyPWM);
	fclose(dutyPWM);

	//Open period pin

	periodPWM = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/period", "w");


	//Set the gpio pins to output

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
	val0 = fopen("/sys/class/gpio/gpio66/value", "w");
	fseek(val0, 0, SEEK_SET);

	val1 = fopen("/sys/class/gpio/gpio67/value", "w");
	fseek(val1, 0, SEEK_SET);

	val2 = fopen("/sys/class/gpio/gpio68/value", "w");
	fseek(val2, 0, SEEK_SET);

	// Put song notes in array, and calculate period
	//int notesHz[8] = {262, 277, 294, 311, 330, 349, 370, 392};
	// G, G, A, G, C, B, 0, 0
	// G, G, A, G, D, C, 0, 0
	// G, G, G^, E, C, B, A, 0
	// F, F, E, C, D, C, 0, 0
	int notesHz[32] = {392, 392, 440, 392, 523, 494, 1, 1,
			   392, 392, 440, 392, 587, 523, 1, 1,
			   392, 392, 784, 660, 523, 494, 440, 1,
			   350, 350, 330, 262, 294, 262, 1, 1};
	int notesPeriod[32];

	int i;
	for (i = 0; i < 32; i++) {
		notesPeriod[i] = 1000 * 1000 * 1000/notesHz[i];
	}


	// Define count which determines the state of leds/tone of buzzer
	int count = 0;
	int musicCount = 0;


	// Overall loop for the program
	int ii;	
	for (ii = 0; ii < 1 * 32; ii++) {
		if ((count %2) == 1){ //light led0 if count is an odd number
			fprintf(val0, "%d", 1);
			fflush(val0);
		}else{
			fprintf(val0, "%d", 0);
			fflush(val0);	
		}

		if (((count/2)%2) == 1){ //light led1 if half(integer division) of count is an odd number
			fprintf(val1, "%d", 1);
			fflush(val1);
		}else{
			fprintf(val1, "%d", 0);
			fflush(val1);	
		}

		if (((count/4)%2) == 1){ //light led2 if a quarter(integer division) of count is an odd number
			fprintf(val2, "%d", 1);
			fflush(val2);
		}else{
			fprintf(val2, "%d", 0);
			fflush(val2);	
		}
		
		// play segment of "happy birthday" song
		fprintf(periodPWM, "%d", notesPeriod[musicCount]);
		fflush(periodPWM);

		// increment counters
		count = count +1;
		musicCount = musicCount + 1;

		// To avoid eventual overflow, reset counters.
		if (count == 8)
		{
			count = 0;
		}
		if (musicCount == 32)
		{
			musicCount = 0;
		}
     		usleep(500000);	//wait a bit to make changes visible	
	}

	// close all open files
	fclose(val0);
	fclose(val1);
	fclose(val2);
	fclose(periodPWM);
	
	return 0;
}
