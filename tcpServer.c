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
  char * buffer;
  int exit = 0;
  connectedList connected_list;

  connected_list.num_connected = 0;
  connected_list.info = (connectedInfo*)malloc(sizeof(connectedInfo));
  buffer = (char*)malloc(sizeof(char) * (strlen(cd->userName) + 5));
  //SEM_constructor_with_name(&sem_clientServer, ftok("tcpServer.c", atoi("clientServer")));

  while (!exit) {

    //SEM_wait(&sem_clientServer);
    sprintf(buffer, "$%s: ", cd->userName);
    write(1, buffer, strlen(buffer));

    user_input = readUntil(0, '\n');

    if (strlen(user_input)) {
      exit = checkCommand(user_input, *cd, &connected_list);

    }else{

      write(1, ERR_UNKNOWNCMD, strlen(ERR_UNKNOWNCMD));
    }//else

    //SEM_signal(&sem_clientServer);

  }//while

  free(buffer);

  return NULL;
}

//Servidor dedicat per client connectat al serverClient
void *dedicatedServer(void *arg){
  int connected = 1;
  char * message;
  char * show_message;
  char * client_name;

  configurationData *cd = (configurationData *) arg;
  show_message = (char*)malloc(sizeof(char));
  client_name = (char*)malloc(sizeof(char) * (strlen(cd->userName) + 5));
  sprintf(client_name, USERCLIENT, cd->userName);

  write(1, "Soc server dedicat\n", strlen("Soc server dedicat\n")); //KILL ME
  printf("Num socket: %d\n", cd->socket); //KILL ME

  while (connected) {
    message = receiveSocketMSG(cd->socket);
    printf("post msg received\n");//KILL ME
    write(1, message, strlen(message));
    write(1, "\n", strlen("\n"));
    write(1, client_name, strlen(client_name));
    clearBuffer(message);
  }

  free(show_message);

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
      ts->clients.sockets[ts->clients.num_sockets - 1].userName = ts->cd.userName;

      sendConfirmationReply(newsock, ts->cd);
      pthread_create(&t_dedicatedServer, NULL, dedicatedServer, &ts->clients.sockets[ts->clients.num_sockets - 1]); //Creacio del thread del nou client, cal passar ts al thread
      //Fins aqui
    } //else
  }//While(1)

  return (void *) 0;
}

void sendConfirmationReply(int sockfd, configurationData cd){
  char buffer[255];

  //envia userName
  sendSocketMSG(sockfd, cd.userName, 0);

  //envia audioDirectory
  sendSocketMSG(sockfd, cd.audioDirectory, 0);

  //envia ip
  sendSocketMSG(sockfd, cd.ip, 0);

  //envia port
  itoa((int)cd.port, buffer);
  sendSocketMSG(sockfd, buffer, 0);
  //readAudioFile("Audio/Stuck_In_Nostalgia.mp3", sockfd);
}
