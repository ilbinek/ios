CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic

all: proj2

proj2: proj2.o
	$(CC) $(CFLAGS) proj2.o -o proj2 -pthread

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.o
	rm proj2

zip:
	zip xpupak01.zip *.c Makefile