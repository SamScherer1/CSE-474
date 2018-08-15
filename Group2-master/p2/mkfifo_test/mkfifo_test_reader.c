//
//  mkfifo_test_reader.c
//  


#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_BUF 32

int main() {
    char* myfifo = "/tmp/myfifo";
    char buf[MAX_BUF];
    
    /* open, read, and display the message from the FIFO */
    while(1) {
        int fd = open(myfifo, O_RDONLY);
        read(fd, buf, MAX_BUF);
        printf("%s\n", buf);
		buf = "";
		
        close(fd);
    }
    return 0;
}
