#include "tcpServer.h"

void initializationClients(Clients clients){
  clients.num_sockets = 0;
  clients.sockets = malloc(sizeof(int));
}

//Creem dos threads
int serverClient(configurationData cd){

  Clients clients;
  pthread_t t_client, t_server;
  int estat = 0;
  //semaphore sem_clientServer;

  initializationClients(clients);

  write(1, WELCOME, strlen(WELCOME));

  //SEM_constructor_with_name(&sem_clientServer, ftok("tcpServer.c", atoi("clientServer")));

  //SEM_init(&sem_clientServer, 1);

  estat = pthread_create(&t_client, NULL, userAsClient, &cd);
  if(estat != 0){
    perror("pthread_create");
    return -1;
  }  //if

  estat = pthread_create(&t_server, NULL, userAsServer, NULL);
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

//Usuari com a servidor
void *userAsServer(void *arg){

  Clients clients;

  write(1, "Soc server\n", strlen("Soc server\n")); //KILL ME

  //semaphore sem_clientServer;
  int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(sockfd < 0){
    perror("socket");
  } //if

  struct sockaddr_in s_addr;
  memset(&s_addr, 0, sizeof(s_addr));
  s_addr.sin_family = AF_INET;
  s_addr.sin_port = htons(PORT);
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
      clients.sockets[clients.num_sockets] = newsock;
      printf("File descriptor socket: %d ", clients.sockets[clients.num_sockets]);
      clients.num_sockets++;
      printf("- NUM Clients connectats: %d\n", clients.num_sockets);
      clients.sockets = (int*)realloc(clients.sockets, sizeof(int) * (clients.num_sockets + 1));
    } //else


  }

  return (void *) 0;
}
