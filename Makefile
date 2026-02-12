CC = gcc

CFLAGS = -Iinclude

SRCS = login.c

TARGET = login

all:
		$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)
clean:
		rm -f $(TARGET)