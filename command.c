#include "command.h"

void showConnections(connectionAvailable* connectionavailable, int numConnections;){
  int i;
  int socket_conn = -1;
  char buffer[255];

  if(numConnections == 0){
    sprintf(buffer, NUM_CONNECTIONS, 0);
    write(1, buffer, strlen(buffer));
  } //if
  else{

    for(i = 0; i < numConnections; i++){
      scanPorts(connectionavailable, i);
    } //for
  } //else
}

void scanPorts(connectionAvailable* connectionavailable, int i){

  int j;
  struct sockaddr_in s_addr;
  struct hostent *hptr;

  for(j = connectionavailable[i].firstPort; j <= connectionavailable[i].lastPort; j++){

      if((htpr = gethostbyname(connectionavailable[i].toConnect)) == NULL){

      } //if

  } //for
}

void connectToPort(uint16_t port) {
  //TODO: Connect command
  printf("in connect\n");
}
