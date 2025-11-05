FLAGS = main.c -o main.out
clean:
	rm -rf *.out
all:
	gcc main.c -Wall -g -I./cpu -o main