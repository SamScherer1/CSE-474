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
	//define count which determines the state of leds/tone of buzzer
	int count = 0;

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




	//Blinks the LED
	while(1) {
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
		
		//Shorten period by 100,000 microseconds for each count starting at 1,000,000
		fprintf(periodPWM, "%d", (1000000-(100000*(count%8))));
		fflush(periodPWM);

		count = count +1;//increment count
		if (count == 8) //reset count to not overflow count if run for a long time
		{
			count = 0;
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
