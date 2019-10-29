#include "command.h"

void showConnections(connectionAvailalble* connectionavailalble, int numConnections;){
  int i;
  int socket_conn = -1;
  char buffer[255];

  if(numConnections == 0){
    sprintf(buffer, NUM_CONNECTIONS, 0);
    write(1, buffer, strlen(buffer));
  } //if
  else{

    for(i = 0; i < numConnections; i++){
      scanPorts(connectionavailalble, i);
    } //for
  } //else
}

void scanPorts(connectionAvailalble* connectionavailalble, int i){

  int j;
  struct sockaddr_in s_addr;
  struct hostent *hptr;

  for(j = connectionavailalble[i].firstPort; j <= connectionavailalble[i].lastPort; j++){

      if((htpr = gethostbyname(connectionavailalble[i].toConnect)) == NULL){

      } //if

  } //for
}
