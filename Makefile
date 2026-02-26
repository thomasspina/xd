CC = gcc
CFLAGS = -Wall -std=c11
PROJ = cxd

main: cxd.c
	$(CC) $(CFLAGS) -o bin/$(PROJ) cxd.c

clean:
	rm -rf bin
	rm -rf $(PROJ).dSYM
