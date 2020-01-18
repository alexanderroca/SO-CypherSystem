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
#include <semaphore.h>

//defines tcpServer
#define PORT 8550
#define WELCOME "\nStarting trinity...\n\n"
#define USERCLIENT "$%s: "

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
#define TESTING "Testing ...\n"
#define CONNECTIONS_AVAILABLE "\n%d connections available\n\n"
#define PIPE_NOT_CREATED "Pipe couldn't be created\n"
#define FORK_NOT_CREATED "Fork couldn't be created\n"
#define FILE_TRANSFER_KO "Transfer corrupted\n"
#define FILE_TRANSFER_OK "[%s] %s downloaded\n"
#define SHOW_USER_AUDIO "[%s] Audios\n"
#define MD5SUM "md5sum"
#define MD5SUM_FAILED "md5sum command error\n"
#define MD5SUM_COMMAND "md5sum %s"
#define AUDIO_LIST_TITLE "\n------Audios Found------\n"
#define AUDIO_NO_LIST_TITLE "\n------Audios Not Found------\n"

#define MT_CONNECTION 0x01

////////////////////Defines for message sending protocol////////////////////////

//All messages must be composed of PROTOCOL_MESSAGE
#define PROTOCOL_MESSAGE "%X %s %d %s\n" //Message Type, Headder, Length, Data

//MT stands for message type
#define MT_CONFIGDATA 0x00
#define MT_CONNECTION 0x01
#define MT_SAY 0x02
#define MT_BROADCAST 0x03
#define MT_SHOWAUDIO 0x04
#define MT_DOWNAUDIO 0x05
#define MT_EXIT 0x06

//Headers
#define H_CONFIGDATA "[CD]"
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
#define H_DOWNAUDIO "[DOWNAUDIO]"
#define H_MD5OK "[MD5OK]"
#define H_MD5KO "[MD5KO]"
#define H_EOF "[EOF]"
#define H_FILEKO "[FILEKO]\0"
#define H_FILEOK "[FILEOK]\0"

//other defines
#define FILL_SHOWAUDIO "Show Audio Requested\n"
#define FILL_DOWNAUDIO "Audio Download Requested\n"

////////////////////////////////////////////////////////////////////////////////

//defines for main.c
#define FILE_NOT_FOUND_ERR "Error, invalid path\n"
#define ERR_ARGUMENTS "ERROR, invalid arguments\n"

//defines for command.c
#define READ_PIPE 0
#define WRITE_PIPE 1

#define SEND_MSG "[%s] %s"
#define NUM_CONNECTIONS "Connections availables: %d\n"
#define ERR_NOUSERS "Error, no users connected.\n"
#define ERR_UNKNOWNUSER "Error, user unknown.\n"
#define PATH "./show_connections_v2.sh"
#define PATH_NAME "show_connections_v2.sh"
#define MIN_PORT "8550"
#define MAX_PORT "8560"
#define IP_SCRIPT "127.0.0.1"
#define USER_NO_CONNECTED "%s not connected\n"

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


//---------------------------------LIST----------------------------------------
#define ELEMENT_INDEFINIT -1

//Struct in list
typedef struct{
	int socket;
	char * userName;
	char * audioDirectory;
	char * ip;
	uint16_t port;
}connectionInfo;

typedef struct _Node{
  connectionInfo ci;
  struct _Node *seg;
  struct _Node *ant;
}Node;

typedef struct{
  Node *pri;
  Node *ult;
  Node *pdi;
}LlistaBid;
//-----------------------------------------------------------------------------

typedef struct{
	configurationData cd;
	LlistaBid connections;
}Info;

#endif
