all: main

main.o: main.c io.h
	gcc main.c -c -Wall -Wextra

io.o: io.c io.h
	gcc io.c -c -Wall -Wextra

main: main.o io.o
	gcc main.o io.o -o Practica -Wall -Wextra

clean:
	rm *.o main.

tar:
	tar -cvzf main.tar *.c*.h  makefile
