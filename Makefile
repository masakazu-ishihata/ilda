# Makefile
TAR = ilda
CC = gcc -O6 -Wall
L = -lm -lirand -licsv -litime
OBJS = main.o ilda_main.o ilda_vb.o ilda_cgs.o ilda_io.o

all : $(TAR)
$(TAR): $(OBJS)
	  $(CC) -o $@ $(OBJS) $(L)
clean:
	  rm -f $(OBJS) $(TAR) *~ *.bak

.c.o:	  $(CC) -c $<om


main.o: main.h ilda.h
ilda_main.o: ilda_main.h
ilda_vb.o: ilda_vb.h ilda_main.o
ilda_cgs.o: ilda_cgs.h ilda_main.o
ilda_io.o: ilda_io.h ilda_main.o
