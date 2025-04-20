# Makefile for scroll
#
# D Provine, 17 November 2012

SOURCES = scroll.c model.c view.c
OBJECTS = scroll.o model.o view.o
DRIVERS = LEDots.o
INCLUDE = -I/usr/lib/gcc/x86_64-redhat-linux/3.4.6/include
LIBRARY = -lncurses
CFLAGS  = -g -Wall
COMPILER= gcc
PROGRAM = scroll

.SUFFIXES:
.SUFFIXES: .c .o

.c.o: ; $(COMPILER) $(CFLAGS) -c $< $(INCLUDE)

$(PROGRAM) : $(OBJECTS)
	$(COMPILER) -o $(PROGRAM) $(CFLAGS) $(OBJECTS) $(DRIVERS) $(LIBRARY)

clean: ; /bin/rm -f $(PROGRAM) $(OBJECTS) depend

# handle dependencies
depend : $(SOURCES)
	gcc -M $(SOURCES) > depend

include depend
