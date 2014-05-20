# Makefile
TAR = ilda
CC = gcc -O6 -Wall
L = -lm -lirand -licsv -liutil
OBJS = main.o ilda.o

all : $(TAR)
$(TAR): $(OBJS)
	  $(CC) -o $@ $(OBJS) $(L)
clean:
	  rm -f $(OBJS) $(TAR) *~ *.bak

.c.o:	  $(CC) -c $<om


main.o: main.h ilda.o
ilda.o: ilda.h
