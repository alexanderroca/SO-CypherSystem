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
  char buffer[100] = " ";
  int num;

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
  num = read(4, &cd, sizeof(configurationData)); //Canviar el primer parametre pel num_socket corresponent
  printf("Num bytes llegits: %d\n", num);
  sprintf(buffer, "%d connected: %s\n", portToConnect, cd.userName);
  write(1, buffer, strlen(buffer));
}
