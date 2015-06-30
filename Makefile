SRCDIR = src
IDIR = include
TESTDIR = tests
OUTDIR = output
CC=gcc
CFLAGS=-g

ODIR=obj
LDIR =lib

LIBS=-lcrypto


all: encrypt decrypt tests
	@echo "Build complete!"
	@echo "Running tests!"
	./tests/unit_test

$(ODIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p obj
	$(CC) -c -o $@ $< $(CFLAGS)

encrypt: $(ODIR)/encrypt.o
	$(CC) -o encrypt $(ODIR)/encrypt.o $(CFLAGS) $(LIBS)

decrypt: $(ODIR)/decrypt.o
	$(CC) -o decrypt $(ODIR)/decrypt.o $(CFLAGS) $(LIBS)

tests: $(TESTDIR)/test.o
	@mkdir -p $(OUTDIR)
	$(CC) -o $(TESTDIR)/unit_test $(TESTDIR)/test.o $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	rm -f encrypt decrypt
	rm -f $(TESTDIR)/unit_test $(TESTDIR)/*.o
	rm -rf ./$(ODIR)
	rm -rf ./$(OUTDIR)

