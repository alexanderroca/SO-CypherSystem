#ifndef _TCPSERVER_H
#define  _TCPSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/shm.h>
//#include "semaphore_v2.h"

#include "command.h"
#include "io.h"

#define PORT 8550
#define WELCOME "Starting trinity...\n"
#define USERCLIENT "$%s: "

int serverClient(configurationData cd);
void *userAsClient(void *arg);
void *userAsServer(void *arg);

#endif
