#ifndef _DATASTRUCT_H_
#define _DATASTRUCT_H_

#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>

//defines for io.c
#define ENTER_SEP '\n'
#define AUDIO_FILE ".mp3"
#define CMD_CONNECT "CONNECT"
#define CMD_SAY "SAY"
#define CMD_BROADCAST "BROADCAST"
#define CMD_DOWNLOAD "DOWNLOAD"
#define CMD_SHOW "SHOW"
#define CMD_EXIT "EXIT"
#define CMD_SHOW_AUDIOS "AUDIOS"
#define CMD_SHOW_CONNECTIONS "CONNECTIONS"
#define ERR_UNKNOWNCMD "Error, unknown command.\n"
#define ERR_2FEWARGS "Error, too few arguments or wrong argument order provided to execute command.\n"
#define ERR_2MANYARGS "Error, too many arguments or wrong argument order provided to execute command.\n"
#define ERR_PORT "Error, invalid port value.\n"
#define ERR_MESSAGE "Error, no message found (specified by using \" \")\n"
#define ERR_ORDER "Error, wrong argment order.\n"
#define ERR_NOAUDIO "Error, no audio file extension found in audio file parameter. Audio file must not contain spaces!\n"
#define ERR_NOSPEACHMARKS "Error, message must end with speachmarks.\n"

//defines for moin.c
#define FILE_NOT_FOUND_ERR "Error, invalid path\n"
#define ERR_ARGUMENTS "ERROR, invalid arguments\n"
#define STARTUP "Starting Trinity...\n"

//defines for command.c
#define NUM_CONNECTIONS "Connections availables: %d\n"

typedef struct {
	char* toConnect;
	uint16_t firstPort;
	uint16_t lastPort;
}connectionAvailable;

typedef struct {
	char* userName;
	char* audioDirectory;
	char* ip;
	uint16_t port;
	connectionAvailable* connectionavailable;
	int numConnections;
}configurationData;

#endif
