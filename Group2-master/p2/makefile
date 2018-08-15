# Makefile for project2

all: lcd writer heartbeat

lcd: lcdScreen.c
	gcc -std=c99 -o lcd lcdScreen.c

writer:  write_to_screen.c
	gcc -o writer write_to_screen.c

heartbeat: heartbeat.c
	gcc -o heartbeat heartbeat.c

clean:
	rm -rf *.o lcd writer heartbeat
