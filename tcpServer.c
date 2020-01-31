#include "tcpServer.h"

sem_t mutexExclusioUserConnect;

//Creem dos threads
int serverClient(configurationData cd){

  pthread_t t_client, t_server;
  int estat = 0;
  Info info_client, info_server;
  //ThreadServer ts;
  //semaphore sem_clientServer;

  //clients = initializationClients();

  write(1, WELCOME, strlen(WELCOME));

  info_client.cd = cd;
  info_server.cd = cd;

  info_client.connections = LLISTABID_crea();
  info_server.connections = LLISTABID_crea();

  //ts.clients = clients;
  //ts.cd = cd;

  //SEM_constructor_with_name(&sem_clientServer, ftok("tcpServer.c", atoi("clientServer")));

  //SEM_init(&sem_clientServer, 1);

  estat = pthread_create(&t_client, NULL, userAsClient, &info_client);
  if(estat != 0){
    perror("pthread_create");
    return -1;
  }  //if

  estat = pthread_create(&t_server, NULL, userAsServer, &info_server);

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

  //semaphore sem_clientServer;
  Info * info_client = (Info *) arg;
  char * user_input;
  char * buffer;
  int exit = 0;
  //connectedList connected_list;

  //connected_list.num_connected = 0;
  //connected_list.info = (connectedInfo*)malloc(sizeof(connectedInfo));
  user_input = (char*)malloc(sizeof(char));
  buffer = (char*)malloc(sizeof(char) * (strlen(info_client->cd.userName) + 5));
  //SEM_constructor_with_name(&sem_clientServer, ftok("tcpServer.c", atoi("clientServer")));

  while (!exit) {

    printf("waiting intput\n");
    //SEM_wait(&sem_clientServer);
    sprintf(buffer, "$%s: ", info_client->cd.userName);
    write(1, buffer, strlen(buffer));

    user_input = readUntil(0, '\n');

    if (strlen(user_input)) {
      exit = checkCommand(user_input, info_client);

    }else{

      write(1, ERR_UNKNOWNCMD, strlen(ERR_UNKNOWNCMD));
    }//else

    printf("post check\n");
    //SEM_signal(&sem_clientServer);

  }//while

  free(buffer);
  free(user_input);

  return NULL;
}

//Servidor dedicat per client connectat al serverClient
void *dedicatedServer(void *arg){
  int connected = 1;
  int msg_type = 0;
  char * message;
  char * show_message;
  char * client_name;

  connectionInfo * ci = (connectionInfo*) arg;
  message = (char*)malloc(sizeof(char));
  show_message = (char*)malloc(sizeof(char));
  client_name = (char*)malloc(sizeof(char) * (strlen(ci->userName) + 5));
  sprintf(client_name, USERCLIENT, ci->userName);

  write(1, "Soc server dedicat\n", strlen("Soc server dedicat\n")); //KILL ME
  printf("Num socket: %d\n", ci->socket); //KILL ME

  while (connected) {
    printf("waiting to receive message....\n");//KILL ME
    receiveSocketMSG(ci->socket, &msg_type, message);
    printf("post msg received message == %s\n", message);//KILL ME
    connected = DSMsgHandler(message, msg_type, client_name, ci);
  }

  free(show_message);
  free(message);

  return NULL;
}

int DSMsgHandler(char * message, int type, char * client_name, connectionInfo * ci){
  printf("message DSM == %s\n", message);
  int connected = 1;//in  message exit turn this to false
  char * path;

  path = (char*)malloc(sizeof(char));

	switch (type) {

		case 2://MSG
		case 3://BROADCAST
			write(1, message, strlen(message));
			write(1, "\n", strlen("\n"));
			write(1, client_name, strlen(client_name));
		break;
		case 4://SHOW_AUDIOS
			replyDirectoryUserConnected(ci->audioDirectory, ci->socket);
      write(1, client_name, strlen(client_name));
		break;
		case 5://DOWNLOAD_AUDIOS
      path = realloc(path, sizeof(char) * (strlen(message) + strlen(ci->audioDirectory) + 5));
      sprintf(path, "%s/%s", ci->audioDirectory, message);
      printf("reading audio file path: %s\n", path);//KILL ME
      readAudioFile(path, ci->socket);
      write(1, client_name, strlen(client_name));
		break;
    default:
      write(1, "Error, Default DSMsgHandler triggered\n",
        strlen("Error, Default DSMsgHandler triggered\n"));
    break;
	}//switch

  free(path);
  return connected;
}//func

//Usuari com a servidor
void *userAsServer(void *arg){

  pthread_t t_dedicatedServer;
  sem_init(&mutexExclusioUserConnect, 0, 1);
  Info * info_server = (Info *) arg;
  connectionInfo ci;
  connectionInfo DS_ci;
  connectionInfo ci_test;//KILL ME

  ci.userName = (char*)malloc(sizeof(char));
  ci.audioDirectory = (char*)malloc(sizeof(char));
  ci.ip = (char*)malloc(sizeof(char));

  setupCI(info_server->cd, &ci);

  int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(sockfd < 0){
    perror("socket");
  } //if

  struct sockaddr_in s_addr;
  memset(&s_addr, 0, sizeof(s_addr));
  s_addr.sin_family = AF_INET;
  s_addr.sin_port = htons(info_server->cd.port);
  s_addr.sin_addr.s_addr = INADDR_ANY;

  if(bind(sockfd, (void *) &s_addr, sizeof(s_addr)) < 0){
    perror("bind");
    return (void *) -1;
  } //if

  listen(sockfd, 5);

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
      //sem_wait(&mutexExclusioUserConnect);
      //ts->clients.sockets[ts->clients.num_sockets].socket = newsock;
      ci.socket = newsock;
      printf("ci.socket == %d\n", ci.socket);
      printf("pre access list\n");
      LLISTABID_vesFinal(&(info_server->connections));
      LLISTABID_inserir(&(info_server->connections), ci);
      ci_test = LLISTABID_consulta(info_server->connections);//KILL ME

      printf("File descriptor socket: %d\n", ci_test.socket);//KILL ME
      //ts->clients.num_sockets++;
      //ts->clients.sockets = (configurationData*)realloc(ts->clients.sockets, sizeof(int) * (ts->clients.num_sockets + 1));
      //ts->clients.sockets[ts->clients.num_sockets - 1].userName = ts->cd.userName;
      //ts->clients.sockets[ts->clients.num_sockets - 1].audioDirectory = ts->cd.audioDirectory;

      sendConfirmationReply(newsock, info_server->cd);
      LLISTABID_vesFinal(&(info_server->connections));
      DS_ci = LLISTABID_consulta(info_server->connections);
      pthread_create(&t_dedicatedServer, NULL, dedicatedServer, &DS_ci); //Creacio del thread del nou client, cal passar ts al thread
      //Fins aqui
      //sem_post(&mutexExclusioUserConnect); //Falta destruir el semafor quan es fa exit o Ctrl-C
    } //else
  }//While(1)

  free(ci.userName);
  free(ci.audioDirectory);
  free(ci.ip);

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
  //readAudioFile("Audio/Russian Red Army Choir.mp3", sockfd);
}
