IDIR = include
CC = g++
DEBUG =
CFLAGS = -Wall -std=c++11 $(DEBUG) -I$(IDIR)

SDIR = src
ODIR = bin
TDIR = test
LIBS = -lm -lpthread

_DEPS = config.h utils.h guest.h lprocess.h monitor.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = utils.o guest.o lprocess.o monitor.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_TOBJ = util_test.o lprocess_test.o
TOBJ = $(patsubst %,$(ODIR)/%,$(_TOBJ))

$(ODIR)/%_test.o: $(TDIR)/%_test.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: dir main compiletest

dir:
	mkdir -p $(ODIR)

main: $(OBJ)
	$(CC) $(CFLAGS) -I$(IDIR) -o $(ODIR)/demo $^ ex/demo.cpp $(LIBS)
	$(CC) $(CFLAGS) -I$(IDIR) -o $(ODIR)/client $^ ex/client.cpp $(LIBS)
	$(CC) $(CFLAGS) -I$(IDIR) -o $(ODIR)/server $^ ex/server.cpp $(LIBS)
	$(CC) $(CFLAGS) -I$(IDIR) -o $(ODIR)/snort $^ ex/snort.cpp $(LIBS)
	$(CC) $(CFLAGS) -I$(IDIR) -o $(ODIR)/usage $^ ex/usage.cpp $(LIBS)

compiletest: $(TOBJ)
	@$(foreach t,$(TOBJ), $(CC) -o $(patsubst %_test.o,%,$(t))_test\
$(t) -lcppunit $(LIBS);)

test: all
	@$(foreach t,$(TOBJ), ./$(patsubst %_test.o,%,$(t))_test;)

clean:
	rm -rf $(ODIR) *~

.PHONY: clean
