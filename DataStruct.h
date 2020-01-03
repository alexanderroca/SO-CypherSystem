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
#include <sys/wait.h>

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
#define TRANSFER_SUCCESS "File transfer completed for id: %d\n"
#define TRANSFER_FAILURE "Error reading %s\n"
#define RECIEVING_FILE "Receiving file...\n"
#define ERROR_OPENING_FILE "Error opening file\n"
#define RECIEVING_DATA "Received: %Le Mb"
#define TRANSFER_SUCCES_ONCLIENT "File OK....Completed\n"

#define MT_CONNECTION 0x01

////////////////////Defines for message sending protocol////////////////////////

//All messages must be composed of PROTOCOL_MESSAGE
#define PROTOCOL_MESSAGE "%X %s %d %s\n" //Message Type, Headder, Length, Data

//MT stands for message type
#define MT_CONNECTION 0x01
#define MT_SAY 0x02
#define MT_BROADCAST 0x03
#define MT_SHOWAUDIO 0x04
#define MT_DOWNAUDIO 0x05
#define MT_EXIT 0x06

//Headers
#define H_TRNAME "[TR_NAME]"
#define H_MSG "[MSG]"
#define H_MSGOK "[MSGOK]"
#define H_BROAD "[BROADCAST]"
#define H_SHOWAUDIO "[SHOW_AUDIOS]"
#define H_LISTAUDIO "[LIST_AUDIOS]"
#define H_AUDIORES "[AUDIO_RSPNS]"
#define H_AUDIOREQ "[AUDIO_RQST]"
#define H_AUDIOKO "[AUDIO_KO]"
#define H_EXIT "[]"
#define H_CONOK "[CONOK]"
#define H_CONKO "[CONKO]"
#define H_MD5OK "[MD5OK]"
#define H_MD5KO "[MD5KO]"
#define H_EOF "[EOF]"

////////////////////////////////////////////////////////////////////////////////

//defines for main.c
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
	int socket;
	char* userName;
	char* audioDirectory;
	char* ip;
	uint16_t port;
	connectionAvailable* connectionavailable;
	int numConnections;
}configurationData;

typedef struct {
	int socket;
	char * user;
}userSocket;

/*
typedef struct{

}
*/
#endif
