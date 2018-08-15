// adcRead.c

#include <fcntl.h>
#include <stdio.h>      // for File IO and printf
#include <string.h>     //for strcat
#include <time.h>       // for usleep
#include <sys/stat.h>
#include <unistd.h>

int atoi(char* str);

int main() {

	//Enable ADC ports
	FILE *sys;
	sys = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fprintf(sys, "%s", "cape-bone-iio");
	fflush(sys);
	fclose(sys);
	
	//Read ADC port
	FILE* sys2 = fopen("../sys/devices/ocp.3/helper.15/AIN0", "r");
	if (sys2 == NULL) {
		printf("File is null!\n");
	}	
		
	// read in sensor value from file
	char buffer[100];
	int success = fscanf(sys2, "%s", buffer);
	fflush(sys2);
	fclose(sys2);

	// print to terminal
	printf("%d\n", atoi(buffer));
	return 0;
}
