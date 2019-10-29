#ifndef _IO_H
#define  _IO_H

#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <limits.h>

#define ENTER_SEP '\n'

#define FILE_NOT_FOUND_ERR "Error, invalid path"

typedef struct {
	char* toConnect;
	uint16_t firstPort;
	uint16_t lastPort;
}connectionAvailalble;

typedef struct {
	char* userName;
	char* audioDirectory;
	char* ip;
	uint16_t port;
	connectionAvailalble* connectionavailalble;
	int numConnections;
}configurationData;

int checkEOF(int fd);
char* readUntil(int fd, char end);
int readConfigurationFile(char* path, configurationData* cd);

#endif
