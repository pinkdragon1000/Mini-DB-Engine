CC=gcc
CFLAGS=-I. -g

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

database: database.o db.o
	$(CC) -o database database.o db.o

clean:
	rm *.o; rm database
	