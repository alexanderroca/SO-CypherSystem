
#include "tcpServer.h"

sem_t mutexExclusioUsersList;
int* end;
char * user_input;
char * buffer;

int stop = 0;

Info info_client;
Info info_server;

void sig_handler() {
  printf("in sig_handler\n");//KILL ME

  sem_wait(&mutexExclusioUsersList);
  *end = checkCommand(CMD_EXIT, &info_client);
  exit_server(&info_client, &info_server);
  printf("passa el exitserver\n");
  sem_post(&mutexExclusioUsersList);

  if(buffer != NULL){
    free(buffer);
  }

  if (user_input != NULL) {
    free(user_input);
  }

  deleteInfo();
  sem_destroy(&mutexExclusioUsersList);
  exit(0);
}

void deleteInfo(){
  free(info_client.cd.userName);
  free(info_client.cd.audioDirectory);
  free(info_client.cd.ip);

  free(info_server.cd.userName);
  free(info_server.cd.audioDirectory);
  free(info_server.cd.ip);
}

void copyCD(Info * info, configurationData * cd){
  info->cd.userName = (char*)malloc(sizeof(char) * (strlen(cd->userName) + 1));
  info->cd.ip = (char*)malloc(sizeof(char) * (strlen(cd->ip) + 1));
  info->cd.audioDirectory = (char*)malloc(sizeof(char) * (strlen(cd->audioDirectory) + 1));

  strcpy(info->cd.userName, cd->userName);
  strcpy(info->cd.ip, cd->ip);
  strcpy(info->cd.audioDirectory, cd->audioDirectory);

  info->cd.socket = cd->socket;
  info->cd.port = cd->port;
  info->cd.numConnections = cd->numConnections;
}

int serverClient(configurationData cd){

  pthread_t t_server;
  int estat = 0;
  int aux = 0;
  end = &aux;
  //ThreadServer ts;

  //clients = initializationClients();

  write(1, WELCOME, strlen(WELCOME));

  copyCD(&info_client, &cd);
  copyCD(&info_server, &cd);

  free(cd.userName);
  free(cd.audioDirectory);
  free(cd.connectionavailable -> toConnect);
  free(cd.connectionavailable);
  free(cd.ip);

  printf("info_client cd.userName = %s\n", info_client.cd.userName);//KILL ME

  info_client.connections = LLISTABID_crea();
  info_server.connections = LLISTABID_crea();

  //ts.clients = clients;
  //ts.cd = cd;

  sem_init(&mutexExclusioUsersList, 0, 1);

  estat = pthread_create(&t_server, NULL, userAsServer, &info_server);

  if(estat != 0){
    perror("pthread_create");
    return -1;
  }  //if

  info_client.id_thread = t_server;

  userAsClient();

  sem_wait(&mutexExclusioUsersList);
  exit_server(&info_client, &info_server);
  sem_post(&mutexExclusioUsersList);

  deleteInfo();
  sem_destroy(&mutexExclusioUsersList);

  return 0;
}

//Usuari com a client
void userAsClient(){

  printf("in user as client\n");//KILL ME
  user_input = (char*)malloc(sizeof(char));
  buffer = (char*)malloc(sizeof(char) * (strlen(info_client.cd.userName) + 5));
  //SEM_constructor_with_name(&sem_clientServer, ftok("tcpServer.c", atoi("clientServer")));

  while (!(*end)) {

    printf("waiting intput\n");
    //SEM_wait(&sem_clientServer);
    sprintf(buffer, "$%s: ", info_client.cd.userName);
    write(1, buffer, strlen(buffer));

    user_input = readUntil(0, '\n');

    if (strlen(user_input)) {
      *end = checkCommand(user_input, &info_client);
    }else{

      write(1, ERR_UNKNOWNCMD, strlen(ERR_UNKNOWNCMD));
    }//else

    printf("post check\n");
    //SEM_signal(&sem_clientServer);

  }//while
}//func

//Servidor dedicat per client connectat al serverClient
void *dedicatedServer(void *arg){
  int connected = 1;
  int msg_type = 0;
  char * message;
  char * show_message;
  char * client_name;

  connectionInfo * ci = (connectionInfo*) arg;
  show_message = (char*)malloc(sizeof(char));
  client_name = (char*)malloc(sizeof(char) * (strlen(ci->userName) + 5));
  sprintf(client_name, USERCLIENT, ci->userName);

  write(1, "Soc server dedicat\n", strlen("Soc server dedicat\n")); //KILL ME
  printf("Num socket: %d\n", ci->socket); //KILL ME

  while (connected) {
    printf("waiting to receive message....\n");//KILL ME
    receiveSocketMSG(ci->socket, &msg_type, &(message));
    printf("post msg received message == %s\n", message);//KILL ME
    connected = DSMsgHandler(message, msg_type, client_name, ci);
    msg_type = 0;
    free(message);
  }

  free(show_message);
  free(client_name);
  printf("DS thread closed\n");//KILL ME
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
    case 6:
      connected = 0;
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
  sem_init(&mutexExclusioUsersList, 0, 1);
  int ret_w;
  Info * info_server = (Info *) arg;
  connectionInfo ci;
  connectionInfo DS_ci;
  connectionInfo ci_test;//KILL ME
  int connect = 1;
  char *user_connected;

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
  fcntl(sockfd, F_SETFL, O_NONBLOCK); /* Change the socket into non-blocking state	*/

  while(!(*end)){
    struct sockaddr_in c_addr;
    socklen_t c_len = sizeof(c_addr);

    //Al cridar accept hem de fer el mateix cast que per bind,
    //i pel mateix motiu
    int newsock = accept(sockfd, (void *) &c_addr, &c_len);
    if(newsock > 0){
      //Posar semafor
      //sem_wait(&mutexExclusioUserConnect);
      //ts->clients.sockets[ts->clients.num_sockets].socket = newsock;
      ret_w = write(newsock, H_CONOK, strlen(H_CONOK));
      printf("ret_w == %d\n", ret_w);

      if (ret_w >= 0) {

          //signal(SIGALRM, sigalrm_handler);
          //alarm(1);
          //while(!stop);
          printf("user_to connect: %s\n", user_connected);
          receiveSocketMSG(newsock, &connect, &user_connected);
          printf("user_to connect: %s\n", user_connected);

          if(user_connected != NULL){

            sem_wait(&mutexExclusioUsersList);
            ci.socket = newsock;
            printf("ci.socket == %d\n", ci.socket);
            printf("pre access list\n");
            LLISTABID_vesFinal(&(info_server->connections));
            LLISTABID_inserir(&(info_server->connections), ci);
            ci_test = LLISTABID_consulta(info_server->connections);//KILL ME

            printf("File descriptor socket: %d\n", ci_test.socket);//KILL ME

            sendConfirmationReply(newsock, info_server->cd);
            LLISTABID_vesFinal(&(info_server->connections));
            DS_ci = LLISTABID_consulta(info_server->connections);
            sem_post(&mutexExclusioUsersList);
            pthread_create(&t_dedicatedServer, NULL, dedicatedServer, &DS_ci); //Creacio del thread del nou client, cal passar ts al thread
          } //if
        //Fins aqui
        //sem_post(&mutexExclusioUserConnect); //Falta destruir el semafor quan es fa exit o Ctrl-C
      }//if
    } //if
  }//While(1)

  free(ci.userName);
  free(ci.audioDirectory);
  free(ci.ip);

  printf("USER SERVER END\n");
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
