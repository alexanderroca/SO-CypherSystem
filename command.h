#ifndef _COMMAND_H
#define  _COMMAND_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#include "io.h"

#define PATH "./show_connections_v2.sh"
#define MIN_PORT "8550"
#define MAX_PORT "8560"
#define IP_SCRIPT "127.0.0.1"

void showConnections(connectionAvailable* connectionavailable, int numConnections);
void connectToPort(uint16_t port);
void say(char* userName, char* text);
void broadcast(char* text);
void showAudios(char* userName);
void download(char* userName, char * audio_file);
void exit();

#endif
