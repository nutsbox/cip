CFLAGS=-Wall -pedantic -O2 -Wl,-Bstatic

ODIR=obj
LDIR=lib

LIBS=-ltomcrypt

_DEPS = main.c cip.c nutsbox.c hash.c csiphash.c uuid4.c b64.c base64.c cipher.c
DEPS = $(patsubst %,./%,$(_DEPS))

_OBJ = main.o cip.o nutsbox.o hash.o csiphash.o uuid4.o b64.o base64.o cipher.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	gcc -c -o $@ $<

cip: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS) -Wl,-Bdynamic

.PHONY: clean

clean:
	del /Q $(ODIR)\*.*
