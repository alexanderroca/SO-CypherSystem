#include "command.h"

sem_t mutexExclusioBroadcast;

void showConnections(){

  pid_t pid;
  //Pipe
  int fd[2];

  printf("Tracta forks\n");

  write(1, TESTING, strlen(TESTING));

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

      execlp(PATH_NAME, PATH, MIN_PORT, MAX_PORT, IP_SCRIPT, NULL);

      close(fd[WRITE_PIPE]);

      exit(1);
    } //else-if
    else{
      //Proces pare
      int num_ports = 0;
      int num_bytes;
      char* buffer = malloc(sizeof(char));
      int* ports = malloc(sizeof(int));

      close(fd[WRITE_PIPE]);

      num_bytes = read(fd[READ_PIPE], buffer, 1024);
      printf("buffer pipe: %s\n", buffer);
      num_ports++;
      ports = realloc(ports, sizeof(int) * (num_ports));

      ports[num_ports - 1] = atoi(buffer);

      close(fd[READ_PIPE]);

      printf("PORT: %d\n", ports[0]);

      printf("S'HA LLEGIT EL SCRIPT\n");

      showPorts(ports, num_ports);

      free(buffer);
      free(ports);
    } //else
  } //else
}

void connectToPort(uint16_t portToConnect, char* ipToConnect, connectedList * cl) {

  //configurationData cd;

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

  cl->num_connected++;
  write(1, "Connecting...\n", strlen("Connecting...\n"));
  receiveCD(&(cl->info[cl->num_connected - 1]), sockfd);
  printf("socket == %d\n", cl->info[cl->num_connected - 1].socket);
  printf("user connected %s\n", cl->info[cl->num_connected - 1].userName);//KILL ME
  printf("audio audioDirectory %s\n", cl->info[cl->num_connected - 1].audioDirectory);//KILL ME
  printf("ip %s\n", cl->info[cl->num_connected - 1].ip);//KILL ME
  printf("port %d\n", cl->info[cl->num_connected - 1].port);//KILL ME
  //sprintf(buffer, "%d connected: %s\n", portToConnect, cd.userName);
  //write(1, buffer, strlen(buffer));

}

void receiveCD(connectedInfo * ci, int sockfd){
  char* buffer;

  ci->socket = sockfd;

  ci->userName = receiveSocketMSG(sockfd);

  ci->audioDirectory = receiveSocketMSG(sockfd);

  ci->ip = receiveSocketMSG(sockfd);

  buffer = receiveSocketMSG(sockfd);
  ci->port = (uint16_t)atoi(buffer);

  //printf("Llegim el bytes del fitxer d'audio\n");
  getAudioFile("Hymn of the Soviet Union - Russian Red Army Choir.mp3", ci->audioDirectory, sockfd, ci->userName);//KILL ME
}

void say(char * user, char * data, connectedList * cl, configurationData cd){
  int i, found = 0;
  char * message;

  message = (char*)malloc(sizeof(char) * (strlen(data) + strlen(cd.userName) + 5));
  sprintf(message, SEND_MSG, cd.userName, data);
  printf("in say\n");//KILL ME

  if (cl->num_connected == 0) {
    write(1, ERR_NOUSERS, strlen(ERR_NOUSERS));
  }else{
    for (i = 0; i < cl->num_connected && !found; i++) {
      printf("user-%s cl-%s\n", user, cl->info[i].userName);
      if (strcasecmp(cl->info[i].userName, user) == 0) {
        printf("MSG to send == %s\n", message);
        sendSocketMSG(cl->info[i].socket, message, 2);
        found = 1;

      }//if
    }//for

    if (!found) {
      write(1, ERR_UNKNOWNUSER, strlen(ERR_UNKNOWNUSER));

    }//if
  }//else
}//func

void broadcast(char * data, connectedList * cl, configurationData cd){
  int i;
  char * message;

  message = (char*)malloc(sizeof(char) * (strlen(data) + strlen(cd.userName) + 5));
  sprintf(message, SEND_MSG, cd.userName, data);

  //semafor
  if (cl->num_connected == 0) {
    write(1, ERR_NOUSERS, strlen(ERR_NOUSERS));
  }else{
    //enviem el misatge a tots els usuaris conectats
    for(i = 0; i < cl->num_connected; i++){
      printf("sending broadcast to socket %d\n", cl->info[i].socket);
      sendSocketMSG(cl->info[i].socket, message, 3);
    } //for
  }//else
  //fi_ssemafor
}

void showAudios(char* userName, connectedList connected_list){

  connectedInfo connected_info;
  char* buffer = malloc(sizeof(char));

  connected_info = checkUserConnnected(userName, connected_list);
  if(connected_info.socket == 0){
    sprintf(buffer, USER_NO_CONNECTED, userName);
    write(1, buffer, strlen(buffer));
  } //if
  else{
    //llegir directori d'audios
    sprintf(buffer, SHOW_USER_AUDIO, userName);
    write(1, buffer, strlen(buffer));
    readDirectoryUserConnected(connected_info.socket);
  } //else

  free(buffer);

}
