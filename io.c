#include "io.h"

/******************************************************************************
* <Description>
* itoa (integer to ASCII) gets the vale of an integer and converts it to a
* string.
*
* @author: Victor Blasco <victor.blasco@students.salle.url.edu>
* @version: 2.0
*
* @param: n integer that will be converted to string.
* @param: s string to which the result wil be recorded.
******************************************************************************/
void itoa(int n, char* s){
		int i, sign;

		if ((sign = n) < 0)  /* record sign */
				n = -n;          /* make n positive */
		i = 0;
		do {       /* generate digits in reverse order */
				s[i++] = n % 10 + '0';   /* get next digit */
		} while ((n /= 10) > 0);     /* delete it */
		if (sign < 0)
				s[i++] = '-';
		s[i] = '\0';
		reverse(s);
}//func

/******************************************************************************
* <Description>
* reverse gets all the characters in a string and reverses the order in wich
* they are written (the firs character will be the last, the second one wiil+
* become second to last etc.).
*
* @author: Victor Blasco <victor.blasco@students.salle.url.edu>
* @version: 2.0
*
* @param: s string in which the converted integer will be stored
*****************************************************************************/
void reverse(char* s){
		int i, j;
		char c;

		for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
				c = s[i];
				s[i] = s[j];
				s[j] = c;
		}
}//func

/******************************************************************************
* <Description>
* clearBuffer sets all the characters of a given string to '\0'
*
* @author: Victor Blasco <victor.blasco@students.salle.url.edu>
* @version: 2.0
*
* @param: buffer char* that requires resetting
*****************************************************************************/
void clearBuffer(char* buffer){
		int i, length;

		length = strlen(buffer);

		for (i = 0; i < length; i++) {
				buffer[i] = '\0';
		}
}//func

//DEPRECATED
char *get_message(int fd, char delimiter) {
	char *msg = (char *) malloc(1);
	char current;
	int i = 0;

	while (read(fd, &current, 1) > 0) {

		msg[i] = current;
		msg = (char *) realloc(msg, ++i + 1);

		if (current == delimiter)
			break;
	}

	msg[i] = '\0';

	return msg;
}//func

/******************************************************************************
* <Description>
* readUntil reads from a given file descriptor until it finds a given stopping
* character or the end of the string. Once at the stopping character it
* substitutes the stopping characte by a '\0'
*
* @author: Alexander Roca <alexander.roca@students.salle.url.edu>
* @version: 1.0
*
* @param: fd file descriptor from which the information is read.
* @param: end stopping character
******************************************************************************/
char* readUntil(int fd, char end) {
	int i = 0;
	char c = '\r';
	char* string = (char*)malloc(sizeof(char));

	while (c != end) {
		read(fd, &c, sizeof(char));
		if (c != end) {
			string = (char*)realloc(string, sizeof(char) * (i + 2));
			string[i] = c;
		}
		i++;
	}

	string[i - 1] = '\0';
	return string;
}//func

/******************************************************************************
* <Description>
* readConfigurationFile reads the configuration.txt file based on the files'
* path and stores the information from the file in the configurationData data
* structure.
*
* @authors: Alexander Roca <alexander.roca@students.salle.url.edu>
* @version: 1.4
*
* @param: path string containing the path to the file.
* @param: cd configurationData structure where all the info is stored.
*
* @return: returns -1 in case of error 0 if everything worked correctly.
******************************************************************************/
int readConfigurationFile(char* path, configurationData* cd) {

	int fd = open(path, O_RDONLY), i = 0;
	char * aux;

	cd->numConnections = 0;

	if (fd < 0) {
		write(1, FILE_NOT_FOUND_ERR, strlen(FILE_NOT_FOUND_ERR));
		return -1;
	} //if
	else {

		cd->userName = readUntil(fd, ENTER_SEP);
		cd->audioDirectory = readUntil(fd, ENTER_SEP);
		cd->ip = readUntil(fd, ENTER_SEP);
		aux = readUntil(fd, ENTER_SEP);
		cd->port = atoi(aux);

		cd->connectionavailable = malloc(sizeof(connectionAvailable));

		while (1) {

			cd->connectionavailable[i].toConnect = readUntil(fd, ENTER_SEP);
			aux = readUntil(fd, ENTER_SEP);
			cd->connectionavailable[i].firstPort = atoi(aux);
			aux = readUntil(fd, ENTER_SEP);
			cd->connectionavailable[i].lastPort = atoi(aux);

			if (checkEOF(fd) == 1)
				break;

			i++;
			cd->connectionavailable = (connectionAvailable*)realloc(cd->connectionavailable, (i + 1) * sizeof(connectionAvailable));
			cd->numConnections++;
		} //while

		close(fd);
		return 0;
	} //else
}//func

/******************************************************************************
* <Description>
* executeMD5sum execute md5sum command by file name
*
* @authors: Alexander Roca <alexander.roca@students.salle.url.edu>
* @version: 1.4
*
* @param: string that contains the file name.
*
* @return: returns string, if it's NULL something went wrong otherwise it contains md5 result
******************************************************************************/
void executeMD5sum(char* file_name, char* md5sum){
	pid_t pid;
	int i = 0;

	//Pipe
	int fd[2];

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

			dup2(fd[WRITE_PIPE], 1);

			execlp(MD5SUM, MD5SUM, file_name, NULL);

			close(fd[WRITE_PIPE]);

			exit(1);
		} //else-if
		else{
			//Proces pare
			int num_bytes;
			char buffer[1024];

			close(fd[WRITE_PIPE]);

			do{
				num_bytes = read(fd[READ_PIPE], buffer, 1024);
			} while(num_bytes != 0); //do-while

			for (i = 0; buffer[i] != ' '; i++) {
				md5sum[i] = buffer[i];
			}//for
			md5sum[32] = '\0';

			close(fd[READ_PIPE]);
		} //else
	} //else
}//func

/******************************************************************************
* <Description>
* readAudioFile reads the audio file of certain user, that we know by the
* socket, we read the file by chunks of 255 bytes and then we send this data
* to the client that request this file.
*
* @authors: Alexander Roca <alexander.roca@students.salle.url.edu>
* @version: 1.4
*
* @param: path string containing the path to the file.
* @param: int that contains the socket
*
* @return: returns -1 in case of error 0 if everything worked correctly.
******************************************************************************/
int readAudioFile(char* path, int socket){

	/* First read file in chunks of 256 bytes */
	char buff[255];
	int fd = open(path, O_RDONLY);
	char * md5sum_file;
	int status = 0;

	struct stat st;

	if(fd < 0){
		//send KO
		sendServerCheck(socket, 5, " ", 0, 0);
		status = 1;
	}	//if
	else{
		//send OK
		int i, trunk_st_size;
		int num_bytes;

		stat(path, &st);

		trunk_st_size = (int) st.st_size / 255;

		i = 0;

		while(i < (trunk_st_size * 255)){
			bzero(buff, 255);
			num_bytes = read(fd, buff, 255);
			sendServerCheck(socket, 5, buff, num_bytes, 1);
			usleep(1000);
			i += 255;
		}	//while

		if(i < st.st_size){
			bzero(buff, 255);
			i = st.st_size - (trunk_st_size * 255);
			num_bytes = read(fd, buff, i);
			sendServerCheck(socket, 5, buff, i, 1);
		}	//if

		md5sum_file = (char*)malloc(sizeof(char) * 36);

		executeMD5sum(path, md5sum_file);

		if(md5sum_file != NULL){
			sendEOFProtocol(socket);
			write(socket, md5sum_file, 32);
		}
		else{
			write(1, MD5SUM_FAILED, strlen(MD5SUM_FAILED));

		}//else
		free(md5sum_file);

	}	//else

	close(fd);
	return status;
}//func

int getAudioFile(char* fileName, char* directoryUserConnected, int socket, char* usernameConnected){

	int status = 0;
	int type = 5;
	int num_bytes = 0;
	char* md5sum_file;
	char* path = NULL;
	char* response = NULL;
	char* buffer = NULL;

	path = (char*)malloc(sizeof(char) * (strlen(directoryUserConnected) + strlen(fileName) + 2));
	sprintf(path, "%s/%s", directoryUserConnected, fileName);

	num_bytes = receiveSocketMSG(socket, &type, &(response));

	if(strcmp(H_AUDIOKO, response) == 0)
			status = 1;
	else{
		int fd = open(path, O_CREAT|O_WRONLY,0644);

		if(fd < 0){
			write(1, ERROR_OPENING_FILE, strlen(ERROR_OPENING_FILE));
		}	//if
		else{
			//escrivim primer batch de bytes
			write(fd, response, num_bytes);
			free(response);
			write(1, "\nDownloading File, Please wait...\n",
				strlen("\nDownloading File, Please wait...\n"));

			while(1){
				num_bytes = receiveSocketMSG(socket, &type, &(response));
				if(num_bytes == 0){ //if(strcmp(buffer, H_EOF) == 0)
						break;
				}
				write(fd, response, num_bytes);
				free(response);

			}	//while

			md5sum_file = (char*)malloc(sizeof(char) * 36);

			executeMD5sum(path, md5sum_file);

			if(strcmp(response, md5sum_file) == 0){
				buffer = (char*)malloc(sizeof(char) *
								(strlen(FILE_TRANSFER_OK) + strlen(usernameConnected) + strlen(fileName) + 2));

				sprintf(buffer, FILE_TRANSFER_OK, usernameConnected, fileName);
				write(1, buffer, strlen(response));
				free(buffer);

			}	//if
			else{
				write(1, FILE_TRANSFER_KO, strlen(FILE_TRANSFER_KO));

			}//else
		}	//else
		close(fd);

	}	//else

	free(path);
	free(response);

	return status;
}//func

void sendEOFProtocol(int sockfd){
	char * message;
	int length = 32;

	message = (char*)malloc(sizeof(char) * (strlen(H_EOF) + 10));
	sprintf(message, "%X %s %d ", MT_DOWNAUDIO, H_EOF, length);
	write(sockfd, message, strlen(message));

}//func

/******************************************************************************
* <Description>
* sendSocketMSG has diferent scenarios for each kind of message that can be
* sent via sockets, type 0 corresponds to server configuration messages, type 1
* corresponds to connection messages, type 2 corresponds to normal message
* sending, type 3 corresponds to broadcast messages, type 4 corresponds to show
* audios messages, type 5 corresponds to download messages and finally type 6
* corresponds to exit messages.
*
* @authors: Victor Blasco <victor.blasco@students.salle.url.edu>
* @version: 1.4
*
* @param: sockfd int that identifies the socket to which the message will be sent
* @param: data string of data that will be sent
* @param: type int that idnetifies which kind of message will be sent
*
* @return: returns 1
******************************************************************************/
int sendSocketMSG(int sockfd, char * data, int type){
	char* message;
	char c_length[5];
	int length = 0;

	switch (type) {
		case 0:
			//sending configuration data type messages
			length = strlen(data);
			itoa(length, c_length);
			message = (char*)malloc(sizeof(char) * (length + strlen(H_CONFIGDATA) + 10));

			sprintf(message, PROTOCOL_MESSAGE, MT_CONFIGDATA, H_CONFIGDATA, c_length, data);
			write(sockfd, message, strlen(message));

			free(message);
			break;
		case 1:
			//CONNECT
			length = strlen(data);
			itoa(length, c_length);
			message = (char*)malloc(sizeof(char) * (length + strlen(H_TRNAME) + strlen(c_length) + 9));

			sprintf(message, PROTOCOL_MESSAGE, MT_CONNECTION, H_TRNAME, c_length, data);
			write(sockfd, message, strlen(message));

			free(message);
			break;
		case 2:
			//SEND_MSG
			length = strlen(data);
			itoa(length, c_length);
			message = (char*)malloc(sizeof(char) * (length + strlen(H_MSG) + 10));

			sprintf(message, PROTOCOL_MESSAGE, MT_SAY, H_MSG, c_length, data);
			write(sockfd, message, strlen(message));

			free(message);
			break;
		case 3:
			//BROADCAST
			length = strlen(data);
			itoa(length, c_length);
			message = (char*)malloc(sizeof(char) * (length + strlen(H_BROAD) + 10));

			sprintf(message, PROTOCOL_MESSAGE, MT_BROADCAST, H_BROAD, c_length, data);
			write(sockfd, message, strlen(message));

			free(message);
			break;
		case 4:
			//SHOW_AUDIOS
			itoa(length, c_length);

			if (data == NULL) {
				message = (char*)malloc(sizeof(char) * (length + strlen(H_SHOWAUDIO) + 10));

				sprintf(message, PROTOCOL_MESSAGE, MT_SHOWAUDIO, H_SHOWAUDIO, c_length, " ");
				write(sockfd, message, strlen(message));

			}else{
				length = strlen(data);
				itoa(length, c_length);
				message = (char*)malloc(sizeof(char) * (length + strlen(H_LISTAUDIO) + 10));

				sprintf(message, PROTOCOL_MESSAGE, MT_SHOWAUDIO, H_LISTAUDIO, c_length, data);+
				write(sockfd, message, strlen(message));
			}//else

			free(message);
			break;
		case 5:
			//DOWNLOAD_AUDIOS
			length = strlen(data);
			itoa(length, c_length);
			message = (char*)malloc(sizeof(char) * (length + strlen(H_AUDIOREQ) + 10));

			sprintf(message, PROTOCOL_MESSAGE, MT_DOWNAUDIO, H_AUDIOREQ, c_length, data);
			write(sockfd, message, strlen(message));

			free(message);
			break;

		case 6:
			//EXIT
			length = strlen(data);
			itoa(length, c_length);
			message = (char*)malloc(sizeof(char) * (length + strlen(H_EXIT) + 10));

			sprintf(message, PROTOCOL_MESSAGE, MT_EXIT, H_EXIT, c_length, data);
			write(sockfd, message, strlen(message));

			free(message);
			break;

		default:
			write(1, "Error Default criteria met in sendSocketMSG\n",
				strlen("Error Default criteria met in sendSocketMSG\n"));
			break;
	}//switch

	return 1;
}//func

/******************************************************************************
* <Description>
* receiveSocketMSG is used to decypher the contents received via a given socket
* firstly it divides the received string in tis components and once it knows what
* kind of message it has received it proceeds to extract the useful data and
* return it via the data parameter.
*
* @authors: Victor Blasco <victor.blasco@students.salle.url.edu>
* @version: 1.4
*
* @param: sockfd socket through which the message will be received
* @param: int pointer type so calling function can identify which type of
*					message has been received
* @param: char double pointer to return the useful data which has been received
*
* @return: length integer representing total length of received data
******************************************************************************/
int receiveSocketMSG(int sockfd, int * type, char ** data){
	char * buffer, * aux;
	int i, c, num_camps, type_int, length;
	char **ptr; //ptr[0] = Type ptr[1] = header ptr[2] = length
	*data = malloc(sizeof(char));

	length = 0;

	if ((*type) == 5) {
		num_camps = 4;
		type_int = 5;

		//Allocate memory for data types
		ptr = (char**)malloc(sizeof(char*) * 4);

		//Allocate msg type to ptr[0]
		ptr[0] = (char*)malloc(sizeof(char));
		ptr[0] = "5";

		buffer = readUntil(sockfd, ' ');//read MSG type

		//Allocate msg Headder to ptr[1]
		buffer = readUntil(sockfd, ' ');//read Headder
		ptr[1] = (char*)malloc(sizeof(char) * (strlen(buffer) + 2));
		strcpy(ptr[1], buffer);

		//Allocate length to ptr[2]
		buffer = readUntil(sockfd, ' ');//read length
		ptr[2] = (char*)malloc(sizeof(char) * (strlen(buffer) + 2));
		strcpy(ptr[2], buffer);

		length = atoi(ptr[2]);

		//Allocate data to ptr[3]
		ptr[3] = (char*)malloc(sizeof(char) * (length + 2));
		read(sockfd, ptr[3], length);

	}else{
		buffer = readUntil(sockfd, '\n');
		ptr = (char**)malloc(sizeof(char*));

		//Didvidim la info sparada per espais en strings diferents
		for (c = 0, aux = strtok (buffer, " ");
						aux != NULL; aux = strtok (NULL, " "), c++){

			ptr = realloc (ptr, sizeof (char *) * (c + 2));
			ptr[c] = aux;
		}//for

		num_camps = c;
		type_int = atoi(ptr[0]);
		*type = type_int;
		length = atoi(ptr[2]);

	}//else

	switch (type_int) {

		case 0://RECEIVE CD
		case 1://CONNECT
		case 2://RECEIVE MSG
		case 3://RECEIVE BROADCAST
		case 6://RECEIVE EXIT
			//separem el missatge sencer en les diferents parts separades per " "
			*data = realloc(*data, sizeof(char) * (length + 1));

			//guardem totes la info rebuda en un sol string on nomes hi ha dades utils
			strcpy(*data, ptr[3]);
			for (c = 4; c < num_camps; c++) {
				sprintf(*data, "%s %s", *data, ptr[c]);
			}//for

			break;
		case 4:
		//SHOW AUDIOS
			if (strcmp(ptr[1], H_SHOWAUDIO) == 0) {
				*data = realloc(*data, sizeof(char) * (strlen(FILL_SHOWAUDIO) + 1));
				strcpy(*data, FILL_SHOWAUDIO);

			}else{
				if(strcmp(ptr[3], H_AUDIOKO) == 0){
					*data = realloc(*data, sizeof(char) * (strlen(H_AUDIOKO) + 1));
					strcpy(*data, H_AUDIOKO);

				}	//if
				else{
					*data = realloc(*data, sizeof(char) * (length + 1));

					//guardem totes la info rebuda en un sol string on nomes hi ha dades utils
					strcpy(*data, ptr[3]);

					for (c = 4; c < num_camps; c++) {
						sprintf(*data, "%s %s", *data, ptr[c]);

					}//for
				}	//else
			}//else

			break;
		case 5:
		//DOWNLOAD AUDIOS
			if (strcmp(ptr[1], H_AUDIOKO) == 0) {
				*data = realloc(*data, sizeof(char) * (strlen(H_AUDIOKO) + 2));
				strcpy(*data, H_AUDIOKO);

			}else{
				if (strcmp(ptr[1], H_AUDIOREQ) == 0) {
					*data = realloc(*data, sizeof(char) * (strlen(ptr[3]) + 2));
					strcpy(*data, ptr[3]);

				}else{
					if (strcmp(ptr[1], H_AUDIORES) == 0) {
						*data = realloc(*data, sizeof(char) * (length + 5));

						for (i = 0; i < length; i++) {
							data[0][i] = ptr[3][i];
						}//for

					}else{
						if (strcmp(ptr[1], H_EOF) == 0) {
							ptr[3][32] = '\0';
							length = 0;
							*data = realloc(*data, sizeof(char) * (strlen(ptr[3]) + 2));
							strcpy(*data, ptr[3]);

						}else{
							//cas que no existeixi el audio file demanat
							write(1, ERR_UNKNOWNFILE, strlen(ERR_UNKNOWNFILE));
						}//else EOF
					}//else AUDIO_RES
				}//else AUDIO_REQ
			}//else AUDIO_KO
			break;
		default:
		write(1, "Error Default criteria met in receiveSocketMSG\n",
			strlen("Error Default criteria met in receiveSocketMSG\n"));
		break;
	}//switch

	//Alliberem Memoria
	for (c = 0; c < num_camps; c++) {
		ptr[c] = NULL;
	}
	free(ptr);

	return length;
}//func

/******************************************************************************
* <Description>
* sendServerCheck sends messages to confirm the reception of a given message
* has been received correctly such as MSGOK
*
* @authors: Victor Blasco <victor.blasco@students.salle.url.edu>
* @version: 1.4
*
* @param: sockfd int that identifies the socket to which the message will be sent
* @param: data string of data that will be sent
* @param: type int that idnetifies which kind of message will be sent
* @param: length int that identifies the length of the message
* @param: ok int that will detemrmine i¡f the message to be sent is of kind OK
*  				or KO
*
* @return: returns 1
******************************************************************************/
int sendServerCheck(int sockfd, int type, char * data, int length, int ok){
	char * message;
	char c_length[5];

	itoa(length, c_length);

	switch (type) {
		case 1:
			if (ok) {
				message = (char*)malloc(sizeof(char) * (strlen(H_CONOK) + length + 10));
				sprintf(message, PROTOCOL_MESSAGE, MT_CONNECTION, H_CONOK, c_length, data);

			}else{
				message = (char*)malloc(sizeof(char) * (strlen(H_CONKO) + length + 10));
				sprintf(message, PROTOCOL_MESSAGE, MT_CONNECTION, H_CONKO, c_length, data);

			}

			write(sockfd, message, strlen(message));
			free(message);

		break;
		case 2://Only possible response MSGOK?
		case 3://Same reply for say & broadcast
			message = (char*)malloc(sizeof(char) * (strlen(H_MSGOK) + 10));
			sprintf(message, PROTOCOL_MESSAGE, MT_SAY, H_MSGOK, "0", " ");

			write(sockfd, message, strlen(message));
			free(message);
		break;

		case 5:
			if (ok) {
				message = (char*)malloc(sizeof(char) * (strlen(H_AUDIORES) + 10));
				sprintf(message, "%X %s %d ", type, H_AUDIORES, length);

				write(sockfd, message, strlen(message));
				write(sockfd, data, length);

			}else{
				message = (char*)malloc(sizeof(char) * (strlen(H_AUDIOKO) + 10));
				sprintf(message, PROTOCOL_MESSAGE, MT_DOWNAUDIO, H_AUDIOKO, "0", " ");

				write(sockfd, message, strlen(message));

			}//else

			free(message);

		break;
		case 6://exit
			if (ok) {
				message = (char*)malloc(sizeof(char) * (strlen(H_CONOK) + 6));
				sprintf(message, PROTOCOL_MESSAGE, MT_EXIT, H_CONOK, "0", " ");

			}else{
				message = (char*)malloc(sizeof(char) * (strlen(H_CONKO) + 6));
				sprintf(message, PROTOCOL_MESSAGE, MT_EXIT, H_CONKO, "0", " ");
			}

			write(sockfd, message, strlen(message));
			free(message);

		break;
		default:
			write(1, "Error Default criteria met in sendServerCheck\n",
				strlen("Error Default criteria met in sendServerCheck\n"));

		break;
	}//switch
	return 1;

}//func

/******************************************************************************
* <Description>
* receiveServerCheck receives all the messages which are replies to a previous
* message such as MSGOK or MSGKO
*
* @authors: Victor Blasco <victor.blasco@students.salle.url.edu>
* @version: 1.4
*
* @param: sockfd int that identifies the socket throuhg which the message will
*					be received
* @param: data string of data that has been received
*
******************************************************************************/
int receiveServerCheck(int sockfd, char ** data){
	char * buffer, * aux;
	char check;
	int c, length, ok, num_camps;
	char **ptr; //ptr[0] = header ptr[1] = length ptr[2] = data

	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	ok = read(sockfd, &check, sizeof(char));

	if (ok < 1) {
		sleep(2);
	}

	ok = read(sockfd, &check, sizeof(char));

	if (ok > 0){
		buffer = readUntil(sockfd, '\n');
		ptr = (char**)malloc(sizeof(char*));

		for (c = 0, aux = strtok (buffer, " ");
						aux != NULL; aux = strtok (NULL, " "), c++){

			ptr = realloc (ptr, (sizeof(char *) * (c + 2)) + 2);
			ptr[c] = aux;
		}//for

		length = atoi(ptr[1]);
		num_camps = c;

		*data = realloc(*data, (sizeof(char) * (length + 1)) + 1);
		//guardem totes la info rebuda en un sol string on nomes hi ha dades utils
		strcpy(*data, ptr[2]);

		for (c = 3; c < num_camps; c++) {
			sprintf(*data, "%s %s", *data, ptr[c]);

		}//for
	}//if

	return ok;
}//func

/******************************************************************************
* <Description>
* checkEOF searches if the end of file for a given file descriptor has been
* reached
*
* @author: Alexander Roca <alexander.roca@students.salle.url.edu>
* @version: 1.2
*
* @param: fd file descriptor from the file which wants to be checked
*
* @return: returns 1 in case of end of file found and 0 if not
******************************************************************************/
int checkEOF(int fd) {
	char c;
	ssize_t n = read(fd, &c, sizeof(char));

	if (n == 0) return 1;

	lseek(fd, -1, SEEK_CUR);
	return 0;

}//func

/******************************************************************************
* <Description>
* Check Command looks at the 1st word input by the user and calls a specific
* function depending on the value of this string if unknown it will display an
* error message. It also divides user_input in words into a string array named
* ptr to handle the data in a easier manner.
*
* @author: Victor Blasco <victor.blasco@students.salle.url.edu>
* @version: 1.2
*
* @param: user_input data recalled from the keyboard as input by the user
* @param: cd configuration data sed to send info to the functions that need it
******************************************************************************/
int  checkCommand(char * user_input, Info * info_client) {

	char *aux, **ptr;
	int c, exit;

	c = 0;
	exit = 0;

	ptr = (char**) malloc (sizeof (char *));
	aux = (char*) malloc(sizeof(char) * (strlen(user_input) + 1));
	assert (ptr != NULL);

	//Llegim l'input de l'usuari i el guardem paraula a paraula a ptr
	for (c = 0, aux = strtok (user_input, " ");
					aux != NULL; aux = strtok (NULL, " "), c++){

		ptr = realloc (ptr, sizeof (char *) * ((c + 2) + 1));
		ptr[c] = (char*) malloc(sizeof(char) * (strlen(aux) + 1));
		ptr[c] = aux;
	}

	//mirem la primera paraula llegida i actuem en consequencia
	if (strcasecmp(ptr[0], CMD_CONNECT) == 0) {

		checkCMDConnect(ptr, c, info_client);
	}else{//if connect
		if (strcasecmp(ptr[0], CMD_SAY) == 0) {

			checkCMDSay(ptr, c, info_client);
		}else{//if say
			if (strcasecmp(ptr[0], CMD_BROADCAST) == 0) {

				checkCMDBroadcast(ptr, c, info_client);
			}else{//if broadcast
				if (strcasecmp(ptr[0], CMD_DOWNLOAD) == 0) {

					checkCMDDownload(ptr, c, info_client);
				}else{//if download
					if (strcasecmp(ptr[0], CMD_EXIT) == 0) {

						exit = checkCMDExit(c, info_client);
					}else{//if exit
						if (strcasecmp(ptr[0], CMD_SHOW) == 0) {

							checkCMDShow(ptr, c, info_client);
						}else{//if show

							write(1, ERR_UNKNOWNCMD, strlen(ERR_UNKNOWNCMD));
						}//else show
					}//else exit
				}//else dowload
			}//else broadcast
		}//else say
	}//else connect

	return exit;
}//func

//DEPRECATED
void stringToUpper(char * string) {
	unsigned int i;

	for (i = 0; i < strlen(string); i++) {
		string[i] = toupper(string[i]);
	}//for
}//func

/******************************************************************************
* <Description>
* checkCMDConnect does the apropiate checks to consider the Connect command as
* valid, if all the checks are succesfull the apropiate function is called, if
* not an error message is displayed accordingly.
*
* @author: Victor Blasco <victor.blasco@students.salle.url.edu>
* @version: 1.0
*
* @param: ptr is a string array with all the words (separated by spaces) that
					have been read from user_input
* @param: c integer that store the number of word in the ptr array.
******************************************************************************/
void checkCMDConnect(char **ptr, int c, Info * info_client) {
	int port = atoi(ptr[1]);
	if (c != 2) {

		if (c < 2) {

			write(1, ERR_2FEWARGS, strlen(ERR_2FEWARGS));
		}else{

			write(1, ERR_2MANYARGS, strlen(ERR_2MANYARGS));
		}
	}else{

		if (atoi(ptr[1]) != 0) {

			if(port == info_client->cd.port){
				write(1, ERR_PORT_SELF, strlen(ERR_PORT_SELF));
			}	//if
			else{
				if(alreadyConnected(info_client, ptr[1])){
					connectToPort(atoi(ptr[1]), "127.0.0.1", info_client, ptr[1]);
				}else{
					write(1, ALREADY_CONNECTED, strlen(ALREADY_CONNECTED));
				}//else
			}	//else
		}	//if
		else{
			write(1, ERR_PORT, strlen(ERR_PORT));
		}//else
	}//else
}//func

int alreadyConnected(Info * info_client, char * port){

	int status = 1;
	connectionInfo ci;

	 LLISTABID_inici(info_client->connections);
	 while(!LLISTABID_fi(info_client->connections)){

		  ci = LLISTABID_consulta(info_client->connections);
			if(ci.port == atoi(port)){
				status = 0;
				break;
			}
		 LLISTABID_avanca(&info_client->connections);
	 }	//while

	 return status;
}//func

/******************************************************************************
* <Description>
* checkCMDSay does the apropiate checks to consider the Say command as
* valid, if all the checks are succesfull the apropiate function is called, if
* not an error message is displayed accordingly.
*
* @author: Victor Blasco <victor.blasco@students.salle.url.edu>
* @version: 2.0
*
* @param: ptr is a string array with all the words (separated by spaces) that
					have been read from user_input
* @param: c integer that store the number of word in the ptr array.
******************************************************************************/
void checkCMDSay(char **ptr, int c, Info * info_client){

	char *user, *message;
	int i = 0;
	int ok = 1;

	user = (char*)malloc(sizeof(char));
	message = (char*)malloc(sizeof(char));

	//comprovem que el nombre d'arguments sigui suficient
	if (c < 3) {

		write(1, ERR_2FEWARGS, strlen(ERR_2FEWARGS));
		ok = 0;
	}else{
		//cas que s'ntrodueixi text abans que el no d'usuari
		if (ptr[1][0] == '"') {
			write(1, ERR_2FEWARGS, strlen(ERR_2FEWARGS));
			ok = 0;
		}else{
			i = 2;
			user = (char*)realloc(user, sizeof(char) * (strlen(ptr[1]) + 1));
			strcpy(user, ptr[1]);

			//recorrem totes les paraules fins trobar la que inicia el text o ens quedem sense
			while (i < c && ptr[i][0] != '"') {

				user = (char*)realloc(user, sizeof(char) * (strlen(user) + strlen(ptr[i]) + 2));
				sprintf(user, "%s %s", user, ptr[i]);
				i++;
			}

			//comprovem que l'usuari haigi posat text (entre " ")
			if (i == c) {

				write(1, ERR_2FEWARGS, strlen(ERR_2FEWARGS));
				ok = 0;
			}else{

				message = realloc(message, sizeof(char) * (strlen(ptr[i]) + 1));
				strcpy(message, ptr[i]);
				i++;

				while (i < c) {

					//comprovem que no hi haigi mes arguments dels necesaris
					if(ok && message[strlen(message) - 1] == '"'){
						write(1, ERR_2MANYARGS, strlen(ERR_2MANYARGS));
						ok = 0;
					}

					message = (char*)realloc(message, sizeof(char) * (strlen(message) + strlen(ptr[i]) + 2));
					sprintf(message, "%s %s", message, ptr[i]);
					i++;
				}//while

				//Comprovem que el missatge acaba en "
				if (ok && message[strlen(message) - 1] != '"') {
					write(1, ERR_NOSPEACHMARKS, strlen(ERR_NOSPEACHMARKS));
					ok = 0;
				}//if
			}//else
		}//else
	}//else

	if (strlen(message) > 80) {
		write(1, "Please send shorter messages.\n",
				strlen("Please send shorter messages.\n"));
		ok = 0;
	}//if

	if (ok) {
		say(user, message, info_client);
	}//if

	free(message);
	free(user);
}//func

/******************************************************************************
* <Description>
* checkCMDBroadcast does the apropiate checks to consider the broadcast command
* as valid, if all the checks are succesfull the apropiate function is called,
* if not an error message is displayed accordingly.
*
* @author: Victor Blasco <victor.blasco@students.salle.url.edu>
* @version: 1.0
*
* @param: ptr is a string array with all the words (separated by spaces) that
					have been read from user_input
* @param: c integer that store the number of word in the ptr array.
******************************************************************************/
void checkCMDBroadcast(char **ptr, int c, Info * info_client) {

	char *message;
	int i, ok;

	ok = 1;
	message = (char*)malloc(sizeof(char));

	if (c < 2) {
		ok = 0;
		write(1, ERR_2FEWARGS, strlen(ERR_2FEWARGS));

	}else{

		if (ptr[1][0] == '"') {
			message = realloc(message, sizeof(char) * (strlen(ptr[1]) + 1));
			strcpy(message, ptr[1]);
			i = 2;

			while (i < c) {

				//comprovem que no hi haigi mes arguments dels necesaris
				if(message[strlen(message) - 1] == '"'){

					write(1, ERR_2MANYARGS, strlen(ERR_2MANYARGS));
					ok = 0;
				}

				message = (char*)realloc(message, sizeof(char) * (strlen(message) + strlen(ptr[i]) + 2));
				sprintf(message, "%s %s", message, ptr[i]);
				i++;
			}//while
		}else{

			ok = 0;
			write(1, ERR_MESSAGE, strlen(ERR_MESSAGE));
		}//else
	}//else

	if (ok && message[strlen(message) - 1] != '"') {
		write(1, ERR_NOSPEACHMARKS, strlen(ERR_NOSPEACHMARKS));
		ok = 0;
	}//if

	if (ok) {

		broadcast(message, info_client, 0);
	}//if

	free(message);
}//func

/******************************************************************************
* <Description>
* checkCMDDownload does the apropiate checks to consider the Download command as
* valid, if all the checks are succesfull the apropiate function is called, if
* not an error message is displayed accordingly.
*
* @author: Victor Blasco <victor.blasco@students.salle.url.edu>
* @version: 1.0
*
* @param: ptr is a string array with all the words (separated by spaces) that
					have been read from user_input
* @param: c integer that store the number of word in the ptr array.
******************************************************************************/
void checkCMDDownload(char **ptr, int c, Info * info_client) {

	char *user, *audio_file;
	int i, ok;

	ok = 1;
	user = (char*)malloc(sizeof(char));
	audio_file = (char*)malloc(sizeof(char));

	if (c < 3) {

		ok = 0;
		write(1, ERR_2FEWARGS, strlen(ERR_2FEWARGS));
	}else{

		if (strstr(ptr[1], AUDIO_FILE) != NULL) {

			ok = 0;
			write(1, ERR_ORDER, strlen(ERR_ORDER));
		}else{

			user = realloc(user, sizeof(char) * (strlen(ptr[2]) + 1));
			strcpy(user, ptr[1]);
			i = 2;
			while (i < (c - 1)) {

				user = (char*)realloc(user, sizeof(char) * (strlen(user) + strlen(ptr[i]) + 2));
				sprintf(user, "%s %s", user, ptr[i]);
				i++;
			}//while

			if (strstr(ptr[i], AUDIO_FILE) != NULL) {

				audio_file = realloc(audio_file, sizeof(char) * (strlen(ptr[i]) + 2));
				strcpy(audio_file, ptr[i]);
			}else{

				ok = 0;
				write(1, ERR_NOAUDIO, strlen(ERR_NOAUDIO));
				write(1, ERR_FILETERMINATION, strlen(ERR_FILETERMINATION));
				write(1, AUDIO_FILE, strlen(AUDIO_FILE));
			}//else
		}//else
	}//else

	if(ok){
		downloadAudios(user, audio_file, info_client);
	}//if

	free(user);
	free(audio_file);
}//func

/******************************************************************************
* <Description>
* In checkCMDExit the user input has already been checked as a valid command
* therefore this function only checks if some aditional parameters have been
* added in which case the command is rejected (returning 0).
*
* @author: Victor Blasco <victor.blasco@students.salle.url.edu>
* @version: 1.0
*
* @param: c integer that store the number of word in the ptr array.
*
* @return: exit integer used as boolean, returns 1 if command is valid,
*				 	 if not returns 0
******************************************************************************/
int checkCMDExit(int c, Info * info_client){
	int exit = 0;

	if (c > 1) {

		write(1, ERR_2MANYARGS, strlen(ERR_2MANYARGS));
	}else{
		broadcast(" ", info_client, 1);
		exit = 1;
		write(1, DISCONNECTING, strlen(DISCONNECTING));
	}//else

	return exit;
}//func

/******************************************************************************
* <Description>
* checkCMDShow does the apropiate checks to consider the Show Audios &
* Show Connections command as valid, if all the checks are succesfull the
* apropiate function is called, if not an error message is displayed
* accordingly.
*
* @author: Victor Blasco <victor.blasco@students.salle.url.edu>
* @version: 1.0
*
* @param: ptr is a string array with all the words (separated by spaces) that
					have been read from user_input
* @param: c integer that store the number of word in the ptr array.
* @param: cd configurationData contains all the data needed for the functions
*					called in checkCMDShow
******************************************************************************/
void checkCMDShow(char **ptr, int c, Info * info_client){

	char *user;
	int i;

	user = (char*)malloc(sizeof(char));
	if (c < 2) {

		write(1, ERR_2FEWARGS, strlen(ERR_2FEWARGS));
	}else{

		if (strcasecmp(ptr[1], CMD_SHOW_CONNECTIONS) == 0) {

			if(c > 2){

				write(1, ERR_2MANYARGS, strlen(ERR_2MANYARGS));
			}else{
				showConnections(info_client->cd.port);
			}//else
		}else{

			if (strcasecmp(ptr[1], CMD_SHOW_AUDIOS) == 0) {

				if (c < 3) {

					write(1, ERR_2FEWARGS, strlen(ERR_2FEWARGS));
				}else{

					user = realloc(user, sizeof(char) * (strlen(ptr[2]) + 1));
					strcpy(user, ptr[2]);
					i = 3;

					while(i < c) {

						user = (char*)realloc(user, sizeof(char) * (strlen(ptr[i]) + strlen(user) + 2));
						sprintf(user, "%s %s", user, ptr[i]);
						i++;
					}//while

					showAudios(user, info_client);
				}
			}else{

				write(1, ERR_UNKNOWNCMD, strlen(ERR_UNKNOWNCMD));
			}//else
		}//else
	}//else

	free(user);

}//func

/******************************************************************************
* <Description>
* checkUserConnnected searches through the list of connected users and look to
* any of their userNames coincide with the functions userName parameter, if the
* user is found in the list it returns the users socket file descriptor, if not
* it returns -1
*
* @author: Alexander Roca <alexander.roca@students.salle.url.edu>
* @version: 1.5
*
* @param: userName name of the user to be found
* @param: list, list through which the user will be searched
*
* @return: returns -1 if the user was not found or the users socket file
* 				 descriptor if he was found
******************************************************************************/
int checkUserConnnected(char* userName, LlistaBid list){
	connectionInfo ci;
	int socket;
	int found = 0;

	if (LLISTABID_esBuida(list)) {
		write(1, ERR_NOUSERS, strlen(ERR_NOUSERS));
	}else{
		LLISTABID_vesInici(&list);

		do {
			ci = LLISTABID_consulta(list);

			if (strcmp(userName, ci.userName) == 0) {
				socket = ci.socket;
				found = 1;
			}

			LLISTABID_avanca(&list);
		} while(!LLISTABID_fi(list) && !found);
	}//else

	if (!found) {
		socket = -1;
	}//if

  return socket;
}//func


void replyDirectoryUserConnected(char* directory_name, int socket){

  char buffer[1024];
  pid_t pid;
  //Pipe
  int fd[2];

	initString(buffer, 1024);

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

      dup2(fd[WRITE_PIPE], 1);

      execlp("ls", "ls", directory_name, "-1", NULL);

      close(fd[WRITE_PIPE]);

      exit(1);
    } //else-if
    else{
      //Proces pare
      int num_bytes;

      close(fd[WRITE_PIPE]);

      num_bytes = read(fd[READ_PIPE], buffer, 1024);
			if(num_bytes == 0)
				sprintf(buffer, H_AUDIOKO);
			else
				createAudioListMSG(buffer);

			sendSocketMSG(socket, buffer, 4);

      close(fd[READ_PIPE]);
    } //else
  } //else
}//func

/******************************************************************************
* <Description>
* createAudioListMSG creates a sendable message out of the list of elements
* created by the audio file search by substituting all the '\n' with blank spaces
*
* @author: Alexander Roca <alexander.roca@students.salle.url.edu>
* @version: 1.5
*
* @param: list string with all the elemets that have to be sent
*
******************************************************************************/
void createAudioListMSG(char * list){
	unsigned int i = 0;

	for (i = 0; i < strlen(list); i++) {
		if (list[i] == '\n') {
			list[i] = ' ';
		}
	}
}//func

void readDirectoryUserConnected(int socket){

  char* buffer;
	char* aux;
  char** files;
	int type = 4;
	int c;

	sendSocketMSG(socket, NULL, 4);

	files = malloc(sizeof(char*));
	buffer = (char*)malloc(sizeof(char));
	aux = malloc(sizeof(char));

	receiveSocketMSG(socket, &type, &(buffer));

	if(strcmp(buffer, H_AUDIOKO) == 0){
		write(1, AUDIO_NO_LIST_TITLE, strlen(AUDIO_NO_LIST_TITLE));
	}	//if
	else{

		for (c = 0, aux = strtok (buffer, " ");
						aux != NULL; aux = strtok (NULL, " "), c++){

			files = realloc (files, sizeof (char *) * (c + 2));
			files[c] = aux;
		}

		showAudioFiles(files, c);
	}	//else
  free(files);
	free(buffer);
	free(aux);
}//func

/******************************************************************************
* <Description>
* showAudioFiles writes on screen all the audio files found in a given string
* in the desired manner
*
* @author: Alexander Roca <alexander.roca@students.salle.url.edu>
* @version: 1.5
*
* @param: files, char** with all the diferent strings that have to be displayed
* @param: num_files, number of different files to be displayed
*
******************************************************************************/
void showAudioFiles(char** files, int num_files){

	int i;

	write(1, AUDIO_LIST_TITLE, strlen(AUDIO_LIST_TITLE));

	for(i = 0; i < num_files; i++){
		write(1, files[i], strlen(files[i]));
		write(1, "\n", 1);
	}//for

	write(1, "\n", 1);
}//func

int initializationPipes(int fd[2]){

  if (pipe(fd)==-1){
      //Pipe failed
      close(fd[READ_PIPE]);
      close(fd[WRITE_PIPE]);
      return 1;
  }   //if

  return 0;
}//func

void showPorts(int ports[], int num_ports){

  char* buffer;
  int i;

  buffer = malloc(sizeof(char));

  sprintf(buffer, CONNECTIONS_AVAILABLE, num_ports);
  write(1, buffer, strlen(buffer));

	if(num_ports > 0)
	  for(i = 0; i < num_ports; i++){
			sprintf(buffer, "%d\n", ports[i]);
	    write(1, buffer, sizeof(int));
		}	//for

	write(1, "\n", strlen("\n"));

  free(buffer);
}//func

/******************************************************************************
* <Description>
* setupCI initializes all the parameters of the CI type to a value to avoid having
* uninitialized values
*
* @author: Victor Blasco <victor.blasco@students.salle.url.edu>
* @version: 1.6
*
* @param: cd source from which the values of ci will be initialized
* @param: ci strucutre which will be initialized
*
******************************************************************************/
void setupCI(configurationData cd, connectionInfo * ci){

	ci->socket = -1;
	ci->port = cd.port;

	ci->userName = realloc(ci->userName, sizeof(char) * (strlen(cd.userName) + 1));
	ci->audioDirectory = realloc(ci->audioDirectory, sizeof(char) * (strlen(cd.audioDirectory) + 1));
	ci->ip = realloc(ci->ip, sizeof(char) * (strlen(cd.ip) + 1));

	strcpy(ci->userName, cd.userName);
	strcpy(ci->audioDirectory, cd.audioDirectory);
	strcpy(ci->ip, cd.ip);

}//func

/******************************************************************************
* <Description>
* initString sets all the values of a string to '\0'
*
* @author: Victor Blasco <victor.blasco@students.salle.url.edu>
* @version: 1.6
*
* @param: string, char* that has to be initialized
* @param: size, length of the string
*
******************************************************************************/
void initString(char * string, int size){
	int i;

	for (i = 0; i < size; i++) {
		string[i] = '\0';
	}
}
