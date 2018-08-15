//
//  mkfifo_test.c
//  
// Takes user input (characters) and passes them into a fifo queue that will be read by lcdScreen.c

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE 32

int main() {
	char buf[MAX_LINE]; // buffer for pipe
	char c; //stores input character
	int n; // stores newline

	// create file and initialize the pipe
    char * myfifo = "/tmp/myfifo";
    mkfifo(myfifo, 0666);

	while (1) {
		// get character input from terminal		
		printf("Enter a character: ");

		// get one character
		c = (char) fgetc(stdin);
		buf[0] = c;
		buf[1] = '\0'; //terminates the string

		// read and ignore newline
		n = fgetc(stdin);
		
		printf("Got the input: %s\n", buf);

        // open fifo and write input to fifo
        int fd = open(myfifo, O_WRONLY);
        write(fd, buf, sizeof(buf));
        close(fd);

		// pause
		usleep(5000);
	}
	
	// "close" pipe
	unlink(myfifo);
    
    return 0;
}
