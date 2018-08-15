/*	testShiftRegister.c
 * Samuel Scherer
 * 4/23/17
 *
 *	loads shift register and tests output
 */

#include <fcntl.h>
#include <stdio.h>      // for File IO and printf
#include <string.h>     //for strcat
#include <time.h>       // for usleep
#include <sys/stat.h>
#include <unistd.h>

void usleep();
void msleep();
void setPins(FILE *val[3], int input);
void sendBits(FILE *val[3], int input);
void setPinsEn(FILE *val[3], int input);
void sendBitsEn(FILE *val[3], int input);
int flipASCII(int x);

int main() {

	FILE *sys, *dir;
	FILE *val[3];
	
	// pins and connections
	// 46 +1 NOT SRCLEAR (when false clear register)
	// 47 +2 SRCLK shift values on posedge
	// 26 +4 RCLK store in storage register on posedge
	// 44 +8 NOT output enable
	// 45 +16 SER input

	// 66 highest
	// 67 next highest
	// 68 lowest
	int pins[3] = {66, 67, 68};
	int count = 0;
	for (int i = 0; i < 3; i++){
		int a = i;
		sys = fopen("/sys/class/gpio/export", "w");
		fprintf(sys, "%d", pins[i]);
		fflush(sys);
		fclose(sys);
	}

	//Set the gpio to output
	for (int i = 0; i < 3; i++){
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
	for (int i = 0; i < 3; i++){
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
	usleep(500000);
	
	setPinsEn(val, 12);
	setPinsEn(val, 12);
	setPinsEn(val, 12);
	setPinsEn(val, 4);
	setPinsEn(val, 4);
	setPinsEn(val, 3);

	setPinsEn(val, 0);
	setPinsEn(val, 1);

	setPinsEn(val, 0);
	setPinsEn(val, 8);	
	
	setPinsEn(val, 0);
	setPinsEn(val, 7);

	setPinsEn(val, 0);
	setPinsEn(val, 15);


	int charIn = (int) 'm';
	setPinsEn(val, flipASCII(charIn/16) + 64 + 32);
	setPinsEn(val, flipASCII(charIn % 16) + 64);
	

	/*
	setPinsEn(val, 2);
	setPinsEn(val, 2);
	setPinsEn(val, 8);
	setPinsEn(val, 0);
	setPinsEn(val, 14);
	setPinsEn(val, 0);
	setPinsEn(val, 6);
	*/
	//setPinsEn(val,5);
	//usleep(1000000);

	//setPins(val, 0); //must start by setting pins to 0 or the previous pin setup will persist
	//sendBits(val,0);

	// SRCLK +1
	// RCLK +2
	// SER +4
/*
	sendBits(val, 11);
	usleep(10000);
	sendBits(val, 0);
	*/
	return 0;
}


//flipASCII
int flipASCII(int x) {
	int bitOne = (x>>3)%2;
	int bitTwo = (x>>2)%2;
	int bitThree = (x>>1)%2;
	int bitFour = (x>>0)%2;
	int result = 1 * bitOne + 2 * bitTwo + 4 * bitThree + 8 * bitFour;
	printf("In flipASCII. Received: %d, returned: %d\n", x, result);
	return result;
}



//sets pins
void setPins(FILE *val[3], int input){
	for (int i= 2; i>=0; i--){
		int pinValue;
		pinValue = (input>>(2-i))%2;
		fprintf(val[i], "%d", pinValue);
		fflush(val[i]);
	}
}

void sendBits(FILE *val[3], int input){
//reset
	
	setPins(val, 0); //00000
	usleep(5000);
	setPins(val, 2); //00010
	usleep(5000);

	for (int i = 0; i <= 7; i++){
		
		//input 1 and shift
		if (((input>>i)%2)==1){
			setPins(val, 4); // 10001
			usleep(5000);
			setPins(val, 5); // 10011
			usleep(5000);
		}

		//input 0 and shift
		else { 
			setPins(val, 0); // 00001
			usleep(5000);
			setPins(val, 1); // 00011
			usleep(5000);
		}
	}

	// flip storage clock
	//setPins(val, 7);
	//usleep(5000);
	/*setPins(val, 11);
	usleep(5000);*/
	// enable read
	setPins(val, 0);
	usleep(5000);
	setPins(val, 2);  //testing not disabling output between data entries
	usleep(5000);
	
}
	// pins and connections
	// 46 +1 NOT SRCLEAR (when false clear register)
	// 47 +2 SRCLK shift values on posedge
	// 26 +4 RCLK store in storage register on posedge
	// 44 +8 NOT output enable
	// 45 +16 SER input

/*
void sendBitsEn(FILE *val[3], int input){
	
	// add 2^4 = 16 to flip enable pin to 1	
	int inputPlusSixteen = input + 16;
	
	sendBits(val, input);
	usleep(5000);
	sendBits(val, inputPlusSixteen);
	usleep(5000);
	//sendBits(val, input);
	//usleep(5000);
} */


void setPinsEn(FILE *val[3], int input){
	
	// add 2^4 = 16 to flip enable pin to 1	
	int inputPlusSixteen = input + 16;
	
	sendBits(val, input);
	usleep(5000);
	sendBits(val, inputPlusSixteen);
	usleep(5000);
	sendBits(val, input);
	usleep(5000);
}


