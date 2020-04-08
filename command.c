#include "command.h"

void showConnections(uint16_t port){

  pid_t pid;
  //Pipe
  int fd[2];

  if(initializationPipes(fd)){
    //Mostrar error Pipe Failed
    write(1, PIPE_NOT_CREATED, strlen(PIPE_NOT_CREATED));
  } //if
  else{
    pid = fork();

    if(pid == -1){
      //Tractar error a l'hora de crear el fork
      write(1, FORK_NOT_CREATED, strlen(FORK_NOT_CREATED));
    } //if
    else if(pid == 0){
      //Proces fill

      close(fd[READ_PIPE]);

      dup2(fd[WRITE_PIPE], STDOUT_FILENO);

      execlp(PATH, PATH, "8850", "8860", "127.0.0.1", NULL);

      close(fd[WRITE_PIPE]);
    } //else-if
    else{
      //Proces pare
      int num_ports = 0;
      int aux_num;
      int num_bytes;
      char* buffer = malloc(sizeof(char));
      char* subString;
      int* ports = malloc(sizeof(int));

      close(fd[WRITE_PIPE]);

      while((num_bytes = read(fd[READ_PIPE], buffer, 1024)) != 0){
          subString = strtok(buffer, " ");
          subString = strtok(NULL, " ");
          aux_num = atoi(subString);
          if(aux_num != port){
            num_ports++;
            ports = realloc(ports, sizeof(int) * (num_ports));
            ports[num_ports - 1] = aux_num;
          } //if
      } //while

      close(fd[READ_PIPE]);

      showPorts(ports, num_ports);

      free(buffer);
      free(ports);
    } //else
  } //else
}//func

void connectToPort(uint16_t portToConnect, char* ipToConnect, Info * info_client) {

  connectionInfo ci;

  //Comprovem la validesa del port
  if(portToConnect < atoi(MIN_PORT) && portToConnect > atoi(MAX_PORT)){
    write(1, ERR_PORT_UNAVAILABLE, strlen(ERR_PORT_UNAVAILABLE));
  } //if
  else{
    //Comprovem la validesa de l'adresa IP
    //i la convertim a format binari
    struct in_addr ip_addr;

    if(inet_aton(ipToConnect, &ip_addr) == 0){
        write(1, ERR_PORT_UNAVAILABLE, strlen(ERR_PORT_UNAVAILABLE));
    } //if
    else{
      //Creem el socket
      int sockfd;
      sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

      if(sockfd < 0){
        write(1, ERR_PORT_UNAVAILABLE, strlen(ERR_PORT_UNAVAILABLE));
      } //if
      else{
        //Especifiquem l'adresa del servidor
        struct sockaddr_in s_addr;
        memset(&s_addr, 0, sizeof(s_addr));
        s_addr.sin_family = AF_INET;
        s_addr.sin_port = htons(portToConnect);
        s_addr.sin_addr = ip_addr;

        //I ara ja ens podem connectar
        //al cridar connect, hem de fer un cast:
        //bind espera struct sockaddr *
        //i nosaltres passem struct sockaddr_in *
        if(connect(sockfd, (void *) &s_addr, sizeof(s_addr)) < 0){
          close(sockfd);
          write(1, ERR_PORT_UNAVAILABLE, strlen(ERR_PORT_UNAVAILABLE));
        } //if
        else{

          write(1, "Connecting...\n", strlen("Connecting...\n"));

          sendSocketMSG(sockfd, info_client->cd.userName, 1);

          LLISTABID_vesFinal(&(info_client->connections));
          receiveCD(sockfd, &ci);
          printf("CD received\n");//KILL ME
          LLISTABID_inserir(&(info_client->connections), ci);

          free(ci.userName);
          free(ci.audioDirectory);
          free(ci.ip);
        } //else
      } //else
    } //else
  } //else
}//func

void receiveCD(int sockfd, connectionInfo * ci){
  char* buffer;
  int port;
  int type = 8;

  ci->socket = sockfd;
  receiveSocketMSG(sockfd, &type, &(ci->userName));
  receiveSocketMSG(sockfd, &type, &(ci->audioDirectory));
  receiveSocketMSG(sockfd, &type, &(ci->ip));

  receiveSocketMSG(sockfd, &type, &(buffer));
  port = atoi(buffer);
  ci->port = (uint16_t)port;

  free(buffer);
}//func

void say(char * user, char * data, Info * info_client){
  int found = 0;
  char * message;
  connectionInfo ci;

  message = (char*)malloc(sizeof(char) * (strlen(data) + strlen(info_client->cd.userName) + 5));
  sprintf(message, SEND_MSG, info_client->cd.userName, data);

  if (LLISTABID_esBuida(info_client->connections)) {
    write(1, ERR_NOUSERS, strlen(ERR_NOUSERS));

  }else{
    LLISTABID_vesInici(&(info_client->connections));

    do{
      ci = LLISTABID_consulta(info_client->connections);
      if (strcasecmp(ci.userName, user) == 0) {
        sendSocketMSG(ci.socket, message, 2);
        found = 1;
      }//if

      LLISTABID_avanca(&(info_client->connections));

    }while (!LLISTABID_fi(info_client->connections) && !found);

    if (!found) {
      write(1, ERR_UNKNOWNUSER, strlen(ERR_UNKNOWNUSER));

    }//if
  }//else
  free(message);
}//func

void broadcast(char * data, Info * info_client, int exit){
  char * message;
  connectionInfo ci;

  message = (char*)malloc(sizeof(char) * (strlen(data) + strlen(info_client->cd.userName) + 5));
  sprintf(message, SEND_MSG, info_client->cd.userName, data);

  if (exit) {
    if (!LLISTABID_esBuida(info_client->connections)) {
      //enviem el misatge a tots els usuaris conectats
      LLISTABID_vesInici(&(info_client->connections));

      do{
        ci = LLISTABID_consulta(info_client->connections);

        sendSocketMSG(ci.socket, message, 6);

        LLISTABID_avanca(&(info_client->connections));

      }while (!LLISTABID_fi(info_client->connections));
    }//if
  }else{

    if (LLISTABID_esBuida(info_client->connections)) {
      write(1, ERR_NOUSERS, strlen(ERR_NOUSERS));

    }else{
      //enviem el misatge a tots els usuaris conectats
      LLISTABID_vesInici(&(info_client->connections));

      do{
        ci = LLISTABID_consulta(info_client->connections);

        sendSocketMSG(ci.socket, message, 3);

        LLISTABID_avanca(&(info_client->connections));

      }while (!LLISTABID_fi(info_client->connections));
    }//else
  }//else

  free(message);
}//func

void showAudios(char* userName, Info * info_client){
  int socket;
  char* buffer;
  buffer = malloc(sizeof(char));
  socket = checkUserConnnected(userName, info_client->connections);
  if(socket == -1){
    buffer = realloc(buffer, sizeof(char) * (strlen(userName) + strlen(USER_NO_CONNECTED) + 5));
    sprintf(buffer, USER_NO_CONNECTED, userName);
    write(1, buffer, strlen(buffer));

  } //if
  else{
    //llegir directori d'audios
    readDirectoryUserConnected(socket);
  } //else

  free(buffer);
}//func

void downloadAudios(char * user, char * audio_file, Info * info_client) {
  int socket;
  char* buffer;

  buffer = malloc(sizeof(char));
  socket = checkUserConnnected(user, info_client->connections);

  if(socket == -1){
    buffer = realloc(buffer, sizeof(char) * (strlen(user) + strlen(USER_NO_CONNECTED) + 5));
    sprintf(buffer, USER_NO_CONNECTED, user);
    write(1, buffer, strlen(buffer));

  } //if
  else{

    sendSocketMSG(socket, audio_file, 5);
    getAudioFile(audio_file, info_client->cd.audioDirectory, socket, user);
  } //else
}//func

void exit_server(Info * info_client, Info * info_server){

  LLISTABID_destrueix(&info_client->connections);
  LLISTABID_destrueix(&info_server->connections);
}
