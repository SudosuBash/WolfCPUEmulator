CC = gcc
CFLAGS = -g -Iinclude
LDFLAGS = 
SRCS = main.c $(wildcard src/*.c) $(wildcard src/**/*.c)
OBJS = $(SRCS:.c=.o)
TARGET = emulator.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	@for /r %%d in (*.o *.exe) do del /Q "%%d"
	@echo Successfully deleted all the files.