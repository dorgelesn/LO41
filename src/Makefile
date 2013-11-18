CC=gcc
CFLAGS=-W -Wall -pedantic -pthreads
LDFLAGS=
EXEC=central pilote gt

all: $(EXEC)

pilote: pilote.o libmsq.o libshm.o
	$(CC) -o $@ $^ $(LDFLAGS)

central: central.o libshm.o libmsq.o
	$(CC) -o $@ $^ $(LDFLAGS)

gt: gt.o libmsq.o libshm.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
