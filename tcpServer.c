#include "tcpServer.h"

Clients initializationClients(){

  Clients clients;
  clients.num_sockets = 0;
  clients.sockets = malloc(sizeof(configurationData));

  return clients;
}

//Creem dos threads
int serverClient(configurationData cd){

  Clients clients;
  pthread_t t_client, t_server;
  int estat = 0;
  ThreadServer ts;  //Posar-ho en una memoria compartida
  //semaphore sem_clientServer;

  clients = initializationClients();

  write(1, WELCOME, strlen(WELCOME));

  ts.clients = clients;
  ts.cd = cd;

  //SEM_constructor_with_name(&sem_clientServer, ftok("tcpServer.c", atoi("clientServer")));

  //SEM_init(&sem_clientServer, 1);

  estat = pthread_create(&t_client, NULL, userAsClient, &cd);
  if(estat != 0){
    perror("pthread_create");
    return -1;
  }  //if

  estat = pthread_create(&t_server, NULL, userAsServer, &ts);

  if(estat != 0){
    perror("pthread_create");
    return -1;
  }  //if

  pthread_join(t_client, NULL);
  pthread_join(t_server, NULL);

  //SEM_destructor(&sem_clientServer);


  return 0;
}

//Usuari com a client
void *userAsClient(void *arg){

  write(1, "Soc client\n", strlen("Soc client\n"));

  //semaphore sem_clientServer;
  configurationData *cd = (configurationData *) arg;
  char * user_input;
  char buffer[100] = " ";
  int exit = 0;

  //SEM_constructor_with_name(&sem_clientServer, ftok("tcpServer.c", atoi("clientServer")));

  while (!exit) {

    //SEM_wait(&sem_clientServer);
    sprintf(buffer, "$%s: ", cd->userName);
    write(1, buffer, strlen(buffer));

    user_input = readUntil(0, '\n');

    if (strlen(user_input)) {
      exit = checkCommand(user_input, *cd);

    }else{

      write(1, ERR_UNKNOWNCMD, strlen(ERR_UNKNOWNCMD));
    }//else

    //SEM_signal(&sem_clientServer);

  }//while

  return NULL;
}

//Servidor dedicat per client connectat al serverClient
void *dedicatedServer(void *arg){
  int connected = 1;

  configurationData *cd = (configurationData *) arg;

  write(1, "Soc server dedicat\n", strlen("Soc server dedicat\n")); //KILL ME
  printf("Num socket: %d\n", cd->socket); //KILL ME

  while (connected) {

  }


  return NULL;
}

//Usuari com a servidor
void *userAsServer(void *arg){

  pthread_t t_dedicatedServer;

  ThreadServer *ts = (ThreadServer *) arg;
  write(1, "Soc server\n", strlen("Soc server\n")); //KILL ME

  printf("PORT: %d\n", ts->cd.port);//KILL ME

  //semaphore sem_clientServer;

  int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(sockfd < 0){
    perror("socket");
  } //if

  struct sockaddr_in s_addr;
  memset(&s_addr, 0, sizeof(s_addr));
  s_addr.sin_family = AF_INET;
  s_addr.sin_port = htons(ts->cd.port);
  s_addr.sin_addr.s_addr = INADDR_ANY;

  if(bind(sockfd, (void *) &s_addr, sizeof(s_addr)) < 0){
    perror("bind");
    return (void *) -1;
  } //if

  //SEM_constructor_with_name(&sem_clientServer, ftok("tcpServer.c", atoi("clientServer")));

  //Falta incloure semafors, zona critica a l'hora d'escoltar com a server a transmetre com a client
  //while(1){
    //SEM_wait(&sem_clientServer);
    listen(sockfd, 5);
    //SEM_signal(&sem_clientServer);
//  } //while

  while(1){
    struct sockaddr_in c_addr;
    socklen_t c_len = sizeof(c_addr);

    //Al cridar accept hem de fer el mateix cast que per bind,
    //i pel mateix motiu
    int newsock = accept(sockfd, (void *) &c_addr, &c_len);

    if(newsock < 0){
      perror("accept");
      exit(EXIT_FAILURE);
    } //if
    else{
      //Posar semafor
      ts->clients.sockets[ts->clients.num_sockets].socket = newsock;
      printf("File descriptor socket: %d ", ts->clients.sockets[ts->clients.num_sockets].socket);
      ts->clients.num_sockets++;
      printf("- NUM Clients connectats: %d\n", ts->clients.num_sockets);
      ts->clients.sockets = (configurationData*)realloc(ts->clients.sockets, sizeof(int) * (ts->clients.num_sockets + 1));

      sendConfirmationReply(newsock, ts->cd);
      printf("post send\n");//KILL ME
      pthread_create(&t_dedicatedServer, NULL, dedicatedServer, &ts->clients.sockets[ts->clients.num_sockets - 1]); //Creacio del thread del nou client, cal passar ts al thread
      //Fins aqui
    } //else


  }

  return (void *) 0;
}

//MUST BE CONTINUED
void sendConfirmationReply(int sockfd, configurationData cd){
  char buffer[255];
  char * message;

  message = (char*)malloc(sizeof(char));
  clearBuffer(buffer);

  printf("Envio reply de confirmacio de connexio del socket %d\n", sockfd);
  //envia num socket
  clearBuffer(buffer);
  itoa(sockfd, buffer);
  message = realloc(message, sizeof(char) * (strlen(buffer) + 1));
  strcpy(message, buffer);
  message[strlen(message)] = '\n';
  printf("message 1 == %s\n", message);//KILL ME
  write(sockfd, message, strlen(message));
  clearBuffer(message);

  message = realloc(message, sizeof(char) * (strlen(cd.userName) + 1));
  strcpy(message, cd.userName);
  message[strlen(message)] = '\n';
  printf("message 2 == %s\n", message);//KILL ME
  write(sockfd, message, strlen(message));
  clearBuffer(message);

  message = realloc(message, sizeof(char) * (strlen(cd.audioDirectory) + 1));
  strcpy(message, cd.audioDirectory);
  message[strlen(message)] = '\n';
  printf("message 3 == %s\n", message);//KILL ME
  write(sockfd, message, strlen(message));
  clearBuffer(message);

  message = realloc(message, sizeof(char) * (strlen(cd.ip) + 1));
  strcpy(message, cd.ip);
  message[strlen(message)] = '\n';
  printf("message 4 == %s\n", message);//KILL ME
  write(sockfd, message, strlen(message));
  clearBuffer(message);

  clearBuffer(buffer);
  itoa((int)cd.port, buffer);
  message = realloc(message, sizeof(char) * (strlen(buffer) + 1));
  strcpy(message, buffer);
  message[strlen(message)] = '\n';
  printf("message 5 == %s\n", message);//KILL ME
  write(sockfd, message, strlen(message));
  clearBuffer(message);

  readAudioFile("Audio/Stuck_In_Nostalgia.mp3", sockfd);

  free(message);
}
