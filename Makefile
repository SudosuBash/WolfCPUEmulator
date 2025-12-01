CC = gcc
CFLAGS = -g -Iinclude
LDFLAGS = 
SRCS = main.c $(wildcard src/*.c) $(wildcard src/**/*.c)
OBJS = $(SRCS:.c=.o)
TARGET = emulator.out

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	@rm -rf *.o *.out */*.o */*/*.o
	@echo Successfully deleted all the files.