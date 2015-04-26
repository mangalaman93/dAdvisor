IDIR = include
CC = g++
DEBUG =
CFLAGS = -Wall -std=c++11 $(DEBUG) -I$(IDIR)

SDIR = src
ODIR = bin
TDIR = test
LIBS = -lm -lpthread

_DEPS = config.h utils.h guest.h monitor.h cgroup.h container.h lprocess.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = utils.o guest.o monitor.o cgroup.o container.o lprocess.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_TOBJ = cgroup_test container_test lprocess_test
TOBJ = $(patsubst %,$(ODIR)/%,$(_TOBJ))

$(ODIR)/%_test: $(TDIR)/%_test.cpp $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: dir main

dir:
	mkdir -p $(ODIR)

main: $(TOBJ)

clean:
	rm -rf $(ODIR) *~

.PHONY: clean
