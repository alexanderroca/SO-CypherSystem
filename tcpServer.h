#ifndef _TCPSERVER_H
#define  _TCPSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h> /* Added for the nonblocking socket */

#include "command.h"
#include "io.h"

int serverClient(configurationData cd);
void userAsClient();
void *userAsServer(void *arg);
void sendConfirmationReply(int sockfd, configurationData cd);
void copyCD(Info * info, configurationData * cd);
void deleteInfo();

//function used by dedicated server to handle received messages
int DSMsgHandler(char * message, int type, char * client_name, connectionInfo * ci);

#endif
