#include "io.h"

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
	char c = '\0';
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
}

/******************************************************************************
* <Description>
* readConfigurationFile reads the configuration.txt file based on the files'
* path and stores the information from the file in the configurationData data
* structure.
*
* @authors: Alexander Roca <alexander.roca@students.salle.url.edu>
*					  Victor Blasco <victor.blasco@students.salle.url.edu>
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
}

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

}

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
int  checkCommand(char * user_input, configurationData cd) {

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

		checkCMDConnect(ptr, c);
	}else{//if connect
		if (strcasecmp(ptr[0], CMD_SAY) == 0) {

			checkCMDSay(ptr, c);
		}else{//if say
			if (strcasecmp(ptr[0], CMD_BROADCAST) == 0) {

				checkCMDBroadcast(ptr, c);
			}else{//if broadcast
				if (strcasecmp(ptr[0], CMD_DOWNLOAD) == 0) {

					checkCMDDownload(ptr, c);
				}else{//if download
					if (strcasecmp(ptr[0], CMD_EXIT) == 0) {

						exit = checkCMDExit(c);
					}else{//if exit
						if (strcasecmp(ptr[0], CMD_SHOW) == 0) {

							checkCMDShow(ptr, c, cd);
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
void checkCMDConnect(char **ptr, int c) {
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
			//connectToPort(atoi(ptr[1]));//TODO: implement function
		}else{
			write(1, ERR_PORT, strlen(ERR_PORT));
		}//else
	}//else
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
void checkCMDSay(char **ptr, int c){

	char *user, *message;
	char * buffer;
	int i = 0;
	int ok = 1;
	char buffer_aux[100] = " ";//KILL ME

	buffer = (char*)malloc(sizeof(char));
	user = (char*)malloc(sizeof(char));

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
			user = (char*)realloc(user, sizeof(char) * (strlen(ptr[1])));
			strcpy(user, ptr[1]);

			//recorrem totes les paraules fins trobar la que inicia el text o ens quedem sense
			while (i < c && ptr[i][0] != '"') {

				user = (char*)realloc(user, sizeof(char) * (strlen(user) + strlen(ptr[i]) + 1));
				sprintf(user, "%s %s", user, ptr[i]);
				i++;
			}

			//comprovem que l'usuari haigi posat text (entre " ")
			if (i == c) {

				write(1, ERR_2FEWARGS, strlen(ERR_2FEWARGS));
				ok = 0;
			}else{

				message = ptr[i];
				i++;

				while (i < c) {

					//comprovem que no hi haigi mes arguments dels necesaris
					if(ok && message[strlen(message) - 1] == '"'){
						write(1, ERR_2MANYARGS, strlen(ERR_2MANYARGS));
						ok = 0;
					}

					buffer = (char*)realloc(buffer, sizeof(char) * (strlen(ptr[i]) + 1));
					sprintf(buffer, " %s", ptr[i]);
					strcat(message, buffer);
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

	if (ok) {

		sprintf(buffer_aux, "sending %s to user %s\n", message, user);//KILL ME
		write(1, buffer_aux, strlen(buffer_aux));//KILL ME
		//say(user, message);//TODO: implement function
	}//if

	free(buffer);
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
void checkCMDBroadcast(char **ptr, int c) {

	char *message, *buffer;
	int i, ok;
	char buffer_aux[100] = " ";//KILL ME

	ok = 1;
	buffer = (char*)malloc(sizeof(char));

	if (c < 2) {
		ok = 0;
		write(1, ERR_2FEWARGS, strlen(ERR_2FEWARGS));

	}else{

		if (ptr[1][0] == '"') {

			message = ptr[1];
			i = 2;
			while (i < c) {

				//comprovem que no hi haigi mes arguments dels necesaris
				if(message[strlen(message) - 1] == '"'){

					write(1, ERR_2MANYARGS, strlen(ERR_2MANYARGS));
					ok = 0;
				}

				buffer = (char*)realloc(buffer, sizeof(char) * (strlen(ptr[i]) + 1));
				sprintf(buffer, " %s", ptr[i]);
				strcat(message, buffer);
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
	}

	free(buffer);

	if (ok) {

		sprintf(buffer_aux, "broadcasting %s\n", message);//KILL ME
		write(1, buffer_aux, strlen(buffer_aux));
		//broadcast(message); //TODO: implement function
	}//if
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
void checkCMDDownload(char **ptr, int c) {

	char *user, *buffer, *audio_file;
	int i, ok;
	char buffer_aux[100] = " ";//KILL ME

	ok = 1;
	buffer = (char*)malloc(sizeof(char));
	if (c < 3) {

		ok = 0;
		write(1, ERR_2FEWARGS, strlen(ERR_2FEWARGS));
	}else{

		if (strstr(ptr[1], AUDIO_FILE) != NULL) {

			ok = 0;
			write(1, ERR_ORDER, strlen(ERR_ORDER));
		}else{

			user = ptr[1];
			i = 2;
			while (i < (c - 1)) {

				buffer = (char*)realloc(buffer, sizeof(char) * (strlen(ptr[i]) + 1));
				sprintf(buffer, " %s", ptr[i]);
				strcat(user, buffer);
				i++;
			}//while

			if (strstr(ptr[i], AUDIO_FILE) != NULL) {

				audio_file = ptr[i];
			}else{

				ok = 0;
				write(1, ERR_NOAUDIO, strlen(ERR_NOAUDIO));
				buffer = realloc(buffer, sizeof(char) * 64);
				sprintf(buffer, "Audio file termination must be %s\n", AUDIO_FILE);
				write(1, buffer, strlen(buffer));
			}//else
		}//esle
	}//else

	free(buffer);

	if(ok){
		sprintf(buffer_aux, "downloading audio file %s from user %s\n", audio_file, user);//KILL ME
		write(1, buffer_aux, strlen(buffer_aux));
		//download(user, audio_file); //TODO: implement function
	}
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
void checkCMDShow(char **ptr, int c, configurationData cd){

	char *user, *buffer;
	int i;
	char buffer_aux[100] = " ";//KILL ME

	buffer = (char*)malloc(sizeof(char));
	if (c < 2) {

		write(1, ERR_2FEWARGS, strlen(ERR_2FEWARGS));
	}else{

		if (strcasecmp(ptr[1], CMD_SHOW_CONNECTIONS) == 0) {

			if(c > 2){

				write(1, ERR_2MANYARGS, strlen(ERR_2MANYARGS));
			}else{

				sprintf(buffer_aux, "Showing connections of %d connections\n", cd.numConnections);//KILL ME
				write(1, buffer_aux, strlen(buffer_aux));//KILL ME
				//showConnections(cd.connectionAvailable, cd.numConnections); //TODO: implement function
			}//else
		}else{

			if (strcasecmp(ptr[1], CMD_SHOW_AUDIOS) == 0) {

				if (c < 3) {

					write(1, ERR_2FEWARGS, strlen(ERR_2FEWARGS));
				}else{

					user = ptr[2];
					i = 3;
					while(i < c) {

						buffer = (char*)realloc(buffer, sizeof(char) * (strlen(ptr[i]) + 1));
						sprintf(buffer, " %s", ptr[i]);
						strcat(user, buffer);
						i++;
					}//while

					sprintf(buffer_aux, "showing audios from user %s\n", user);//KILL ME
					write(1, buffer_aux, strlen(buffer_aux));//KILL ME
					//showAudios(user); //TODO: implement function
				}
			}else{

				write(1, ERR_UNKNOWNCMD, strlen(ERR_UNKNOWNCMD));
			}//else
		}//else
	}//else

	free(buffer);

}//func
