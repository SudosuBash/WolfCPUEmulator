CC = gcc
CFLAGS = -g -Iinclude
LDFLAGS = 
SRCS = main.c $(wildcard src/*.c) $(wildcard src/**/*.c)
OBJS = $(SRCS:.c=.o)
TARGET = app.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm *.o 
	rm *\*.o 
	rm *\*\*.o