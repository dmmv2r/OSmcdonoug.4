C = gcc

CFLAGS = -Wall -g

TARGETS = oss user

all: $(TARGETS)

oss: oss.o
	$(C) $(CFLAGS) -o $@ oss.o

user: user.o
	$(C) $(CFLAGS) -o $@ user.o

clean:
	rm -rf *.o $(TARGETS)
