# Makefile for project1

CC=arm-linux-gnueabihf-gcc
SOURCES= music.c
EXECUTABLE = music_exe

music_exe: music.c
	$(CC) -o music_exe music.c

.PHONY: clean

clean:
	rm -f *.o *~ $(EXECUTABLE)

