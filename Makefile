CC = gcc
CFLAGS = -Iinclude
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
	del $(TARGET) $(OBJS) src\**\*.o 2>NUL