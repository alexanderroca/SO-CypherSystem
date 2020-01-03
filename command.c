#include "command.h"

void showConnections(){

  printf("Tracta forks\n");

  pid_t pid;

  pid = fork();

  if(pid == -1){
    //Tractar error a l'hora de crear el fork
  } //if
  else if(pid == 0){
    //Proces fill
    printf("FILL\n");
    char *argv_list[] = {PATH, MIN_PORT, MAX_PORT, IP_SCRIPT};
    execv(PATH, argv_list); // PIPE per comunicarse amb el pare per mostrar el resultat del fill
  } //else-if
  else{
    //Proces pare
    wait(NULL);
    printf("PARE\n");
  } //else
}

void connectToPort(uint16_t portToConnect, char* ipToConnect) {

  configurationData cd;

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

  write(1, "Connecting...\n", strlen("Connecting...\n"));
  receiveCD(&cd, sockfd);
  printf("socket == %d\n", cd.socket);
  printf("user connected %s\n", cd.userName);//KILL ME
  printf("audio audioDirectory %s\n", cd.audioDirectory);//KILL ME
  printf("ip %s\n", cd.ip);//KILL ME
  printf("port %d\n", cd.port);//KILL ME
  //sprintf(buffer, "%d connected: %s\n", portToConnect, cd.userName);
  //write(1, buffer, strlen(buffer));

}

//MUST BE CONTINUED
void receiveCD(configurationData * cd, int sockfd){
  char* buffer;

  printf("sockfd == %d\n", sockfd);//KILL ME
  buffer = readUntil(sockfd, '\n');
  cd->socket = atoi(buffer);
  printf("read1 == %d\n", cd->socket);//KILL ME

  cd->userName = readUntil(sockfd, '\n');
  printf("read2 == %s\n", cd->userName);//KILL ME

  cd->audioDirectory = readUntil(sockfd, '\n');
  printf("read3 == %s\n", cd->audioDirectory);//KILL ME

  cd->ip = readUntil(sockfd, '\n');
  printf("read4 == %s\n", cd->ip);//KILL ME

  clearBuffer(buffer);
  buffer = readUntil(sockfd, '\n');
  printf("post read until\n");//KILL ME
  cd->port = (uint16_t)atoi(buffer);
  printf("read5 == %d\n", cd->port);//KILL ME

  printf("Llegim el bytes del fitxer d'audio\n");
  getAudioFile("Audio1/Stuck_In_Nostalgia.mp3", sockfd);//KILL ME
}

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
