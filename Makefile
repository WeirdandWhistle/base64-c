CC=gcc
TARGET=main
CFLAGS=-Wall -Wextra

CFILES=$(shell (find -name '*.c'))
OFILES=$(subst .c,.o,$(CFILES))

$(TARGET) : $(OFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OFILES)

.c :
	$(CC) $(CFLAGS) -c -o $@ $<