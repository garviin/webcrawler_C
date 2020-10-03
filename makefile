CC=gcc
CFLAGS= -ggdb3 -std=c99 -Wall -Wextra
DEPS= include/crawler.h include/helper_functions.h include/paths.h include/html_parser.h
OBJ= src/crawler.o src/helper_functions.o src/paths.o src/html_parser.o

%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

crawler: $(OBJ)
		$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
		rm -f $(OBJ) *.o