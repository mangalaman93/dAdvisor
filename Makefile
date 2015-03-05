IDIR = include
CC = g++
DEBUG = -O2
CFLAGS = -Wall -c $(DEBUG) -I$(IDIR)

SDIR = src
ODIR = bin
LIBS = -lm

_DEPS = config.h guest.h lprocess.h container.h monitor.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = guest.o lprocess.o container.o monitor.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $<

all: dir main

dir:
	mkdir -p $(ODIR)

main: $(OBJ)
	$(CC) -I$(IDIR) -o $(ODIR)/demo $^ ex/demo.cpp $(LIBS)

clean:
	rm -rf $(ODIR) *~

.PHONY: clean
