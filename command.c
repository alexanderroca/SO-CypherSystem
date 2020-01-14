#include "command.h"

void showConnections(uint16_t port){

  pid_t pid;
  //Pipe
  int fd[2];

  printf("Tracta forks\n");

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
}

void connectToPort(uint16_t portToConnect, char* ipToConnect, Info * info_client) {

  //configurationData cd;
  connectionInfo ci;

  //Comprovem la validesa del port
  if(portToConnect < atoi(MIN_PORT) && portToConnect > atoi(MAX_PORT)){
    fprintf(stderr, "Error: %d es un port invalid\n", portToConnect);
    exit(EXIT_FAILURE);
  } //if

  //Comprovem la validesa de l'adresa IP
  //i la convertim a format binari
  struct in_addr ip_addr;

  if(inet_aton(ipToConnect, &ip_addr) == 0){
    fprintf(stderr, "inet_Aton (%s): %s\n", ipToConnect, strerror(errno));
    exit(EXIT_FAILURE);
  } //if

  //Creem el socket
  int sockfd;
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if(sockfd < 0){
    perror("socket TCP");
    exit(EXIT_FAILURE);
  } //if

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
    char buff[128];
    perror("conenct");
    int bytes = sprintf(buff, "errno says: %s\n", strerror(errno)); // molt útil
    write(1, buff, bytes);
    close(sockfd);
    exit(EXIT_FAILURE);
  } //if

  //cl->num_connected++;
  write(1, "Connecting...\n", strlen("Connecting...\n"));
  LLISTABID_vesFinal(&(info_client->connections));
  ci = receiveCD(sockfd);
  LLISTABID_inserir(&(info_client->connections), ci);

  printf("socket == %d\n", ci.socket);
  printf("user connected %s\n", ci.userName);//KILL ME
  printf("audio audioDirectory %s\n", ci.audioDirectory);//KILL ME
  printf("ip %s\n", ci.ip);//KILL ME
  printf("port %d\n", ci.port);//KILL ME
  //sprintf(buffer, "%d connected: %s\n", portToConnect, cd.userName);
  //write(1, buffer, strlen(buffer));

}//func

connectionInfo receiveCD(int sockfd){
  char* buffer;
  int type = 8;
  connectionInfo ci;

  buffer = (char*)malloc(sizeof(char));
  ci.socket = sockfd;

  ci.userName = receiveSocketMSG(sockfd, &type);

  ci.audioDirectory = receiveSocketMSG(sockfd, &type);

  ci.ip = receiveSocketMSG(sockfd, &type);

  buffer = receiveSocketMSG(sockfd, &type);
  ci.port = (uint16_t)atoi(buffer);

  printf("type post receive == %d\n", type);//KILL ME

  //printf("Llegim el bytes del fitxer d'audio\n");
  //getAudioFile("Hymn of the Soviet Union - Russian Red Army Choir.mp3", ci.audioDirectory, sockfd, ci.userName);//KILL ME
  return ci;
}//func

void say(char * user, char * data, Info * info_client){
  int found = 0;
  char * message;
  connectionInfo ci;

  message = (char*)malloc(sizeof(char) * (strlen(data) + strlen(info_client->cd.userName) + 5));
  sprintf(message, SEND_MSG, info_client->cd.userName, data);
  printf("in say\n");//KILL ME

  if (LLISTABID_esBuida(info_client->connections)) {
    write(1, ERR_NOUSERS, strlen(ERR_NOUSERS));

  }else{
    LLISTABID_vesInici(&(info_client->connections));

    do{
      ci = LLISTABID_consulta(info_client->connections);

      if (strcasecmp(ci.userName, user) == 0) {
        printf("MSG to send == %s\n", message);
        sendSocketMSG(ci.socket, message, 2);
        found = 1;
      }//if

    }while (!LLISTABID_fi(info_client->connections) && !found);

    if (!found) {
      write(1, ERR_UNKNOWNUSER, strlen(ERR_UNKNOWNUSER));

    }//if
  }//else
}//func

void broadcast(char * data, Info * info_client){
  char * message;
  connectionInfo ci;

  message = (char*)malloc(sizeof(char) * (strlen(data) + strlen(info_client->cd.userName) + 5));
  sprintf(message, SEND_MSG, info_client->cd.userName, data);

  //semafor
  if (LLISTABID_esBuida(info_client->connections)) {
    write(1, ERR_NOUSERS, strlen(ERR_NOUSERS));
  }else{
    //enviem el misatge a tots els usuaris conectats
    LLISTABID_vesInici(&(info_client->connections));

    do{
      ci = LLISTABID_consulta(info_client->connections);
      printf("sending broadcast to socket %d\n", ci.socket);//KILL ME
      sendSocketMSG(ci.socket, message, 3);

    }while (!LLISTABID_fi(info_client->connections));

  }//else
  //fi_ssemafor
}//func

void showAudios(char* userName, Info * info_client){

  connectionInfo ci;
  char* buffer = malloc(sizeof(char));

  ci = checkUserConnnected(userName, info_client->connections);
  if(ci.socket == -1){
    sprintf(buffer, USER_NO_CONNECTED, userName);
    write(1, buffer, strlen(buffer));
  } //if
  else{
    //llegir directori d'audios
     readDirectoryUserConnected(ci.socket);
  } //else

  free(buffer);
}//func
