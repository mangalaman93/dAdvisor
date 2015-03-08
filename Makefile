IDIR = include
CC = g++
DEBUG =
CFLAGS = -Wall -std=c++11 -c $(DEBUG) -I$(IDIR)

SDIR = src
ODIR = bin
TDIR = test
LIBS = -lm -lpthread

_DEPS = config.h utils.h guest.h lprocess.h container.h monitor.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = utils.o guest.o lprocess.o container.o monitor.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_TOBJ = util_test.o lprocess_test.o
TOBJ = $(patsubst %,$(ODIR)/%,$(_TOBJ))

$(ODIR)/%_test.o: $(TDIR)/%_test.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $<

all: dir main

dir:
	mkdir -p $(ODIR)

main: $(OBJ)
	$(CC) -I$(IDIR) -o $(ODIR)/demo $^ ex/demo.cpp $(LIBS)

test: clean dir $(TOBJ)
	@$(foreach t,$(TOBJ), $(CC) -o $(patsubst %_test.o,%,$(t))_test\
$(t) -lcppunit $(LIBS) && ./$(patsubst %_test.o,%,$(t))_test;)

clean:
	rm -rf $(ODIR) *~

.PHONY: clean
