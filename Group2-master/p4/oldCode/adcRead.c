// adcRead.c
// This file reads ADC output for three ports and prints the measurements.

#include <fcntl.h>
#include <stdio.h>      // for File IO and printf
#include <string.h>     //for strcat
#include <time.h>       // for usleep
#include <sys/stat.h>
#include <unistd.h>

int atoi(char* str);

int main() {

	// Enable ADC ports
	FILE *sys;
	sys = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fprintf(sys, "%s", "cape-bone-iio");
	fflush(sys);
	fclose(sys);
	
	// Open all adc ports and check that files exist
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
	return 0;
}
