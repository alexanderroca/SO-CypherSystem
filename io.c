#include "io.h"

void fixStrings(configurationData * cd){
	cd->userName[strlen(cd->userName) - 1] = '\0';
	cd->audioDirectory[strlen(cd->audioDirectory) - 1] = '\0';
	cd->ip[strlen(cd->ip) - 1] = '\0';
}//func

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
* they are written (the firs character will be the last, the second one wiil
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
}//func+

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
char* executeMD5sum(char* file_name){

	char* md5sum;
	pid_t pid;

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

			md5sum = strtok(buffer, " ");

			close(fd[READ_PIPE]);
		} //else
	} //else

	return md5sum;
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
	char* md5sum_file;
	int status = 0;

	struct stat st;

	if(fd < 0){
		//send KO
		write(socket, H_FILEKO, strlen(H_FILEKO));
		status = 1;
	}	//if
	else{
		//send OK
		write(socket, H_FILEOK, strlen(H_FILEOK));

		int i, trunk_st_size;
		int num_bytes;

		stat(path, &st);

		trunk_st_size = (int) st.st_size / 255;

		i = 0;

		while(i < (trunk_st_size * 255)){
			bzero(buff, 255);
			num_bytes = read(fd, buff, 255);
			write(socket, buff, num_bytes);
			printf("num_bytes: %d - i: %d - trunk_size: %d\n", num_bytes, i, trunk_st_size * 255);
			i += 255;
		}	//while

		if(i < st.st_size){
			bzero(buff, 255);
			i = st.st_size - (trunk_st_size * 255);
			printf("I = %d\n", i);
			num_bytes = read(fd, buff, i);
			write(socket, buff, i);
		}	//if

		write(socket, H_EOF, strlen(H_EOF));
		printf("EXIT\n");//KILL ME

		md5sum_file = executeMD5sum(path);
		printf("md5sum_file: %s\n", md5sum_file);//KILL ME

		if(md5sum_file != NULL)
			write(socket, md5sum_file, 32);
		else
			write(1, MD5SUM_FAILED, strlen(MD5SUM_FAILED));
	}	//else

	close(fd);
	return status;
}//func

int getAudioFile(char* fileName, char* directoryUserConnected, int socket, char* usernameConnected){

	int status = 0;
	char buffer[255];
	int num_bytes;
	char* md5sum_file;
	char* path;

	path = (char*)malloc(sizeof(char) * (strlen(directoryUserConnected) + strlen(fileName) + 2));
	sprintf(path, "%s/%s", directoryUserConnected, fileName);
	//path = strcat(directoryUserConnected, "/");
	//path = strcat(path, fileName);

	//Check if file exists
	char* response = get_message(socket, ']');

	if(strcmp(H_FILEKO, response) == 0)
			status = 1;
	else{
		int fd = open(path, O_CREAT|O_WRONLY,0644);

		if(fd < 0){
			printf("Fd: %d\n", fd);
			write(1, ERROR_OPENING_FILE, strlen(ERROR_OPENING_FILE));
		}	//if
		else{

			printf("pre while 1\n");
			while(1){
				bzero(buffer, 255);
				num_bytes = read(socket, buffer, 255);
				printf("num_bytes == %d buffer == %s\n", num_bytes, buffer);
				if(strcmp(buffer, H_EOF) == 0){
						printf("EOF? -> %s\n", buffer);
						break;
				}
				write(fd, buffer, num_bytes);
			}	//while

			bzero(buffer, 255);
			read(socket, buffer, 255);

			printf("Path: %s\n", path);//KILL ME

			md5sum_file = executeMD5sum(path);

			printf("Buffer: %s\n", buffer);//KILL ME
			printf("md5sum_file: %s\n", md5sum_file);//KILL ME

			if(strcmp(buffer, md5sum_file) == 0){
				sprintf(buffer, FILE_TRANSFER_OK, usernameConnected, fileName);
				write(1, buffer, strlen(buffer));
			}	//if
			else
				write(1, FILE_TRANSFER_KO, strlen(FILE_TRANSFER_KO));

		}	//else

		close(fd);
	}	//else

	free(path);

	return status;
}//func

int sendSocketMSG(int sockfd, char * data, int type){
	char* message;
	char c_length[5];
	int length = 0;

	printf("data == %s\n", data);//KILL ME

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
			break;
		case 2:
			printf("in case 2\n");//KILL ME
			length = strlen(data);
			itoa(length, c_length);
			printf("size len %d\n", length);//KILL ME
			printf("size strlen %ld\n", strlen(H_MSG));//KILL ME
			printf("size %ld\n", length + strlen(H_MSG) + 10);//KILL ME
			message = (char*)malloc(sizeof(char) * (length + strlen(H_MSG) + 10));
			printf("post malloc\n");

			sprintf(message, PROTOCOL_MESSAGE, MT_SAY, H_MSG, c_length, data);
			printf("post sprintf\n");
			write(sockfd, message, strlen(message));
			printf("post write\n");
			free(message);
			break;
		case 3:
			printf("in case 3\n");//KILL ME
			length = strlen(data);
			itoa(length, c_length);
			message = (char*)malloc(sizeof(char) * (length + strlen(H_BROAD) + 10));

			sprintf(message, PROTOCOL_MESSAGE, MT_BROADCAST, H_BROAD, c_length, data);
			write(sockfd, message, strlen(message));
			free(message);
			break;
		case 4:
			//SHOW_AUDIOS
			printf("in case 4\n");//KILL ME
			printf("data post case 4 == %s\n", data);//KILL ME
			itoa(length, c_length);

			if (data == NULL) {

				printf("in audio list request send\n");//KILL ME
				message = (char*)malloc(sizeof(char) * (length + strlen(H_SHOWAUDIO) + 10));

				sprintf(message, PROTOCOL_MESSAGE, MT_SHOWAUDIO, H_SHOWAUDIO, c_length, " ");
				printf("message sent == %s\n", message);//KILL ME
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
			printf("in sendSocketMSG download audios\n");//KILL ME
			length = strlen(data);
			itoa(length, c_length);
			message = (char*)malloc(sizeof(char) * (length + strlen(H_AUDIOREQ) + 10));

			sprintf(message, PROTOCOL_MESSAGE, MT_DOWNAUDIO, H_AUDIOREQ, c_length, data);
			printf("message sent == %s\n", message);//KILL ME
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

int receiveSocketMSG(int sockfd, int * type, char ** data){
	char * buffer, * aux;
	int c, num_camps, type_int, length;
	char **ptr; //ptr[0] = Type ptr[1] = header ptr[2] = length
	*data = malloc(sizeof(char));

	length = 0;
	buffer = readUntil(sockfd, '\n');
	printf("message received-%s\n", buffer);
	ptr = (char**)malloc(sizeof(char*));

	for (c = 0, aux = strtok (buffer, " ");
					aux != NULL; aux = strtok (NULL, " "), c++){

		ptr = realloc (ptr, sizeof (char *) * (c + 2));
		ptr[c] = aux;
	}

	length = atoi(ptr[2]);
	num_camps = c;
	type_int = atoi(ptr[0]);
	*type = type_int;

	printf("length == %d\n", length);//KILL ME
	switch (type_int) {

		case 0://RECEIVE CD
		case 2://RECEIVE MSG
		case 3://RECEIVE BROADCAST
			//separem el missatge sencer en les diferents parts separades per " "
			*data = realloc(*data, sizeof(char) * (length + 1));

			//guardem totes la info rebuda en un sol string on nomes hi ha dades utils
			strcpy(*data, ptr[3]);
			for (c = 4; c < num_camps; c++) {
				sprintf(*data, "%s %s", *data, ptr[c]);
			}//for

			printf("strlen post copy == %ld\n", strlen(*data));//KILL ME
			printf("data in receiveSocketMSG == %s\n", *data);//KILL ME

			break;
		case 1:
		//CONNECT
			break;
		case 4:
		//SHOW AUDIOS
			if (strcmp(ptr[1], H_SHOWAUDIO) == 0) {

				printf("in RCV show audio\n");//KILL ME
				*data = realloc(*data, sizeof(char) * (strlen(FILL_SHOWAUDIO) + 1));
				strcpy(*data, FILL_SHOWAUDIO);
			}else{

				printf("in RCV list audio\n");//KILL ME
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
			}

			printf("data in RCV == %s\n", *data);//KILL ME
			break;
		case 5:
		//DOWNLOAD AUDIOS
			printf("in receeive audio download\n");//KILL ME
			if (strcmp(ptr[1], H_AUDIOREQ) == 0) {
				*data = realloc(*data, sizeof(char) * strlen(ptr[3]));
				strcpy(*data, ptr[3]);

			}else{
				if (strcmp(ptr[1], H_AUDIORES) == 0) {
					length = atoi(ptr[2]);
					*data = realloc(*data, sizeof(char) * length);
					strcpy(*data, ptr[3]);

				}else{
					if (strcmp(ptr[1], H_EOF) == 0) {
						*data = realloc(*data, sizeof(char) * 32);
						strcpy(*data, ptr[3]);
					}else{
						//cas que no existeixi el audio file demanat
						write(1, ERR_UNKNOWNFILE, strlen(ERR_UNKNOWNFILE));
					}//else
				}//else
			}//else

			break;
		case 6:
		//EXIT
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

	return 1;
}//func

/*
int sendServerCheck(int sockfd, int type, char * data, int ok){
	char * message;
	int length;

	length = strlen(data);

	switch (type) {
		case 1:
			if (ok) {
				message = (char*)malloc(sizeof(char) * (strlen(H_CONOK) + length + 5));
				sprintf(message, PROTOCOL_MESSAGE, MT_CONNECTION, H_CONOK, length, data);

			}else{
				message = (char*)malloc(sizeof(char) * (strlen(H_CONKO) + length + 5));
				sprintf(message, PROTOCOL_MESSAGE, MT_CONNECTION, H_CONKO, length, data);

			}

			write(sockfd, message, strlen(message));
			free(message);

		break;
		case 2://Only possible response MSGOK?
		case 3://Same reply for say & broadcast
			message = (char*)malloc(sizeof(char) * (strlen(H_MSGOK) + 5));
			sprintf(message, PROTOCOL_MESSAGE, MT_SAY, H_MSGOK, 0, NULL);

			write(sockfd, message, strlen(message));
			free(message);

		case 5://only MD5SUM reply
			message = (char*)malloc(sizeof(char) * (strlen(H_AUDIOKO) + 5));
			sprintf(message, PROTOCOL_MESSAGE, MT_DOWNAUDIO, H_AUDIOKO, 0, NULL);

			write(sockfd, message, strlen(message));
			free(message);

		break;
		case 6:
			if (ok) {
				message = (char*)malloc(sizeof(char) * (strlen(H_CONOK) + 5));
				sprintf(message, PROTOCOL_MESSAGE, MT_EXIT, H_CONOK, 0, NULL);

			}else{
				message = (char*)malloc(sizeof(char) * (strlen(H_CONKO) + 5));
				sprintf(message, PROTOCOL_MESSAGE, MT_EXIT, H_CONKO, 0, NULL);
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

}

void receiveServerCheck(int sockfd, char * data){
	char * buffer, * aux;
	int c, type, length;
	char **ptr; //ptr[0] = Type ptr[1] = header ptr[2] = length

	buffer = readUntil(sockfd, '\n');
	printf("serverCheck received-%s\n", buffer);//KILL ME
	ptr = (char**)malloc(sizeof(char*));

	for (c = 0, aux = strtok (buffer, " ");
					aux != NULL; aux = strtok (NULL, " "), c++){

		ptr = realloc (ptr, sizeof (char *) * (c + 2));
		ptr[c] = aux;
	}//for

	type = atoi(ptr[0]);
	length = atoi(ptr[2]);

	switch (type) {
		case 1:
			if (strcmp(ptr[1], H_CONOK) == 0) {
				data = realloc(data, sizeof(char) * (length + 5));

			}else{
				data = realloc(data, strlen(ptr[1]));
				strcpy(data, ptr[2]);

			}

		break;
		case 2:
		case 3:
		case 6:
			data = realloc(data, sizeof(char) * strlen(ptr[1]));
			strcpy(data, ptr[2]);

		break;
		default:
			write(1, "Error Default criteria met in receiveServerCheck\n",
				strlen("Error Default criteria met in receiveServerCheck\n"));

		break;
	}//switch
}//func

void sendMD5Check(int sockfd, int ok){
	char * message;

	if (ok) {
		message = (char*)malloc(sizeof(char) * (strlen(H_MD5OK) + 5));
		sprintf(message, PROTOCOL_MESSAGE, MT_DOWNAUDIO, H_MD5OK, 0, NULL);

	}else{
		message = (char*)malloc(sizeof(char) * (strlen(H_MD5KO) + 5));
		sprintf(message, PROTOCOL_MESSAGE, MT_DOWNAUDIO, H_MD5KO, 0, NULL);

	}

	write(sockfd, message, strlen(message));
	free(message);
}
*/

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

	printf("Inside checkCommand\n");

	char *aux, **ptr;
	int c, exit;

	exit = 0;
	ptr = malloc (sizeof (char *));
	assert (ptr != NULL);

	//Llegim l'input de l'usuari i el guardem paraula a paraula a ptr
	for (c = 0, aux = strtok (user_input, " ");
					aux != NULL; aux = strtok (NULL, " "), c++){

		ptr = realloc (ptr, sizeof (char *) * (c + 2));
		ptr[c] = aux;
	}

	//mirem la primera paraula llegida i actuem en consequencia
	if (strcasecmp(ptr[0], CMD_CONNECT) == 0) {

		checkCMDConnect(ptr, c, info_client);
	}else{//if connect
		if (strcasecmp(ptr[0], CMD_SAY) == 0) {

			checkCMDSay(ptr, c, info_client);
			printf("post checkCMDSay\n");
		}else{//if say
			if (strcasecmp(ptr[0], CMD_BROADCAST) == 0) {

				checkCMDBroadcast(ptr, c, info_client);
			}else{//if broadcast
				if (strcasecmp(ptr[0], CMD_DOWNLOAD) == 0) {

					checkCMDDownload(ptr, c, info_client);
				}else{//if download
					if (strcasecmp(ptr[0], CMD_EXIT) == 0) {

						exit = checkCMDExit(c);
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

	printf("exiting checkCommand\n");//KILL ME
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
	char buffer[100] = " ";//KILL ME
	if (c != 2) {

		if (c < 2) {

			write(1, ERR_2FEWARGS, strlen(ERR_2FEWARGS));
		}else{

			write(1, ERR_2MANYARGS, strlen(ERR_2MANYARGS));
		}
	}else{

		if (atoi(ptr[1]) != 0) {
			sprintf(buffer, "connecting to %d\n", atoi(ptr[1]));//KILL ME
			write(1, buffer, strlen(buffer));//KILL ME
			if(alreadyConnected(info_client, ptr[1]))
				connectToPort(atoi(ptr[1]), "127.0.0.1", info_client);
			else
				write(1, ALREADY_CONNECTED, strlen(ALREADY_CONNECTED));
		}else{
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
}

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
	char buffer_aux[100] = " ";//KILL ME

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

	printf("strlen message == %ld\n", strlen(message));

	if (strlen(message) > 80) {
		write(1, "Please send shorter messages.\n",
				strlen("Please send shorter messages.\n"));
		ok = 0;
	}//if

	if (ok) {

		sprintf(buffer_aux, "sending %s to user %s\n", message, user);//KILL ME
		write(1, buffer_aux, strlen(buffer_aux));//KILL ME
		say(user, message, info_client);
	}//if

	free(message);
	free(user);
	printf("post say\n");//KILL ME
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

		broadcast(message, info_client);
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
	char buffer_aux[100] = " ";//KILL ME

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
		sprintf(buffer_aux, "downloading audio file %s from user %s\n", audio_file, user);//KILL ME
		write(1, buffer_aux, strlen(buffer_aux));//KILL ME
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
int checkCMDExit(int c){
	int exit = 0;

	if (c > 1) {

		write(1, ERR_2MANYARGS, strlen(ERR_2MANYARGS));
	}else{

		exit = 1;
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
	char buffer_aux[100] = " ";//KILL ME

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

int checkUserConnnected(char* userName, LlistaBid list){
	connectionInfo ci;
	int i, socket;
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

			printf("num_bytes = %d - buffer: %s\n", num_bytes, buffer);//KILL ME
			sendSocketMSG(socket, buffer, 4);

      close(fd[READ_PIPE]);
    } //else
  } //else
}//func

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
  char** files = malloc(sizeof(char*));
	int type;
	int c;


	sendSocketMSG(socket, NULL, 4);

	buffer = (char*)malloc(sizeof(char));
	receiveSocketMSG(socket, &type, &(buffer));
	printf("buffer audio list == %s\n", buffer);//KILL ME

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
}//func

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

void initString(char * string, int size){
	int i;

	for (i = 0; i < size; i++) {
		string[i] = '\0';
	}
}
