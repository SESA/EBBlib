IXPDIR=../../../libixp-0.5
CFLAGS=-g -Wall -I$(IXPDIR)/include
OBJ=main.o fs.o
LIBS=-L$(IXPDIR)/lib -lixp
prefix=/usr/local

ebbfs: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) $(LIBS) -o $@

%.o: %.c ebbfs.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) ebbfs

