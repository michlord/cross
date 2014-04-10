OBJS = test.o solver.o board.o words.o
CFILES = test.c solver.c board.c words.c
CC = gcc
CFLAGS = -c -std=c99 -Wall -Werror -lm
PKGCONF = `pkg-config --cflags --libs glib-2.0`
EXE = cross.exe
all: $(EXE)
$(EXE):	$(OBJS)
	$(CC) $(OBJS) $(PKGCONF) -o $(EXE)
test.o: test.c
	$(CC) $(CFLAGS) $(PKGCONF) test.c
solver.o: solver.c
	$(CC) $(CFLAGS) $(PKGCONF) solver.c
board.o: board.c
	$(CC) $(CFLAGS) $(PKGCONF) board.c
words.o: words.c
	$(CC) $(CFLAGS) $(PKGCONF) words.c
clean: 
	rm -f $(EXE) $(OBJS)
