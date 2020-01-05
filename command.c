#include "command.h"

int initializationPipes(int fd[2]){

  if (pipe(fd)==-1){
      //Pipe failed
      return 1;
  }   //if
  if (pipe(fd)==-1){
      //Pipe failed
      return 1;
  } //if

  return 0;
}

void showConnections(){

  printf("Tracta forks\n");

  pid_t pid;

  //Pipes
  int fd1[2]; //Pare-Fill
  int fd2[2]; //Fill-Pare

  if(initializationPipes(&fd1[2])){
    //Mostrar error Pipe Failed
  } //if
  else{
    pid = fork();

    if(pid == -1){
      //Tractar error a l'hora de crear el fork
    } //if
    else if(pid == 0){
      //Proces fill
      close(fd1[1]);
      close(fd2[0]);

      if(initializationPipes(&fd2[2])){
        //Mostrar error Pipe Failed
      } //if
      printf("FILL\n");
      char *argv_list[] = {PATH, MIN_PORT, MAX_PORT, IP_SCRIPT};
      execv(PATH, argv_list); // PIPE per comunicarse amb el pare per mostrar el resultat del fill

      //write();

      close(fd2[1]);
    } //else-if
    else{
      //Proces pare
      wait(NULL);

      //read();

      close(fd1[0]);
      printf("PARE\n");
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
  //getAudioFile("Audio1/Stuck_In_Nostalgia.mp3", sockfd);//KILL ME
}

void say(char * user, char * data, connectedList * cl, configurationData cd){
  int i, found = 0;
  char * message;

  message = (char*)malloc(sizeof(char) * (strlen(data) + strlen(cd.userName) + 5));
  sprintf(message, SAY_MSG, cd.userName, data);
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

/*
void broadcast(char* msg, ThreadServer* ts){
  int i;

  //semafor
  for(i = 0; i < ts->clients.num_sockets; i++){

    if(write(ts->clients->sockets.socket, msg, strlen(msg)) < 0){

      perror("sending failure");
      continue;
    } //if
  } //for
  //fi_ssemafor

}
*/
