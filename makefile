CFLAGS := -std=c99
CC=gcc
EXEC=Eighty-Sixer
OBJS=*.o
SOURCES=*.c

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) -std=c99

.c.o:
	$(CC) -c *.c

clean:
	rm -f $(OBJS) $(EXEC) a.out
