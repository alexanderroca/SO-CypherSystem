#ifndef _COMMAND_H
#define  _COMMAND_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#include "io.h"

int initializationPipes(int fd[2]);
void showPorts(int* ports, int num_ports);
void showConnections(uint16_t port);
void connectToPort(uint16_t port, char* ipToConnect, Info * info_client);
void say(char * user, char * data, Info * info_client);
void broadcast(char * data, Info * info_client);
void receiveCD(int sockfd, connectionInfo * ci);
void showAudios(char* userName, Info * info_client);
void downloadAudios(char * user, char * audio_file, Info * info_client);
void exit();

#endif
