all: main

main.o: main.c io.h
	gcc main.c -c -Wall -Wextra

io.o: io.c io.h DataStruct.h
	gcc io.c -c -Wall -Wextra

command.o: command.c command.h io.h
	gcc command.c -c -Wall -Wextra

tcpServer.o: tcpServer.c tcpServer.h command.h
	gcc tcpServer.c -c -Wall -Wextra

LlistaBid.o: LlistaBid.c LlistaBid.h DataStruct.h
	gcc LlistaBid.c -c -Wall -Wextra

main: main.o io.o command.o tcpServer.o LlistaBid.o
	gcc main.o io.o command.o tcpServer.o LlistaBid.o -o Practica -lpthread -Wall -Wextra

clean:
	rm *.o

tar:
	cd .. && tar -cvf Practica_F1.tar F1/
