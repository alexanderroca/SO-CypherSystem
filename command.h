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
#define PATH_NAME "show_connections_v2.sh"
#define PATH_NAME "show_connections_v2.sh"
#define MIN_PORT "8550"
#define MAX_PORT "8560"
#define IP_SCRIPT "127.0.0.1"
#define READ_PIPE 0
#define WRITE_PIPE 1
#define USER_NO_CONNECTED "%s not connected\n"

int initializationPipes(int fd[2]);
void showPorts(int* ports, int num_ports);
void showConnections(uint16_t port);
void connectToPort(uint16_t port, char* ipToConnect, Info * info_client);
void say(char * user, char * data, Info * info_client);
void broadcast(char * data, Info * info_client);
connectionInfo receiveCD(int sockfd);
void showAudios(char* userName, Info * info_client);
void download(char* userName, char * audio_file);
void exit();

#endif
