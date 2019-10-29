#ifndef _COMMAND_H
#define  _COMMAND_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>

#include "io.h"

#define NUM_CONNECTIONS "Connections availables: %d\n"

void showConnections(connectionAvailalble* connectionavailalble, int numConnections;);
void connect(uint16_t port);
void say(char* userName, char* text);
void broadcast(char* text);
void showAudios(char* userName);
void download(char* userName);  //Falta determinar l'audio
void exit();

#endif
