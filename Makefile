CC = gcc
CFLAGS = -Wall -std=c11
PROJ = xd

main: xd.c
	$(CC) $(CFLAGS) -o bin/$(PROJ) xd.c

clean:
	rm -rf bin
	rm -rf $(PROJ).dSYM