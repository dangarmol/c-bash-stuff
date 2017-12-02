CC = gcc
CFLAGS = -Wall -g

OBJETOS = distr.o
PROG = distribucion

all : $(PROG)

$(PROG) : $(OBJETOS)
	gcc -g -o $(PROG) $(OBJETOS)

%.o : %.c
	gcc -c $(CFLAGS) $<

clean :
	-rm $(OBJETOS) $(PROG)
