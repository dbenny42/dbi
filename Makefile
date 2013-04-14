# variables
CC = gcc
CFLAGS = -g "-Wall"

# targets
EXES = selconds
OBJS = project2.o


# name doesn't matter; going to be wrapped in a shell, anyway.

$(EXES): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $?

%.o: %.c
	$(CC) $(CFLAGS) -c $<


.PHONY: clean

clean:
	rm -f *~ *.o $(EXES)
