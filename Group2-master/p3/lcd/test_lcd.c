#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#define NEW_CHAR_DIR "/dev/lcd"

int main(void)
{
	int size_buf, fd;
	char read_buf[100], write_buf[100];
	fd = open(NEW_CHAR_DIR, O_RDWR);
	if (fd < 0)
	{
		printf("File %s cannot be opened\n", NEW_CHAR_DIR);
		exit(1);
	}
	
	strcpy(write_buf, "HELLO, this is a very very long line and it should go overrrrrrrr to the next line.");
	write(fd,write_buf, sizeof(write_buf));
	
	strcpy(write_buf, "clear");
	write(fd,write_buf, sizeof(write_buf));
	
	strcpy(write_buf, "HELLO");
	write(fd,write_buf, sizeof(write_buf));
	
	printf("Successfully wrote to device file\n");

	read(fd, read_buf, sizeof(read_buf));
	printf("Successfully read from device file\n");
	printf("The words are: %s\n", read_buf);

	close(fd);
	return 0;
}

