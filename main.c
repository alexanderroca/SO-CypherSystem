/**
* SO-CypherSystem
*
* Authors:
* Alexander Roca Myroshnichenko
* <alexander.roca@students.salle.url.edu>
*
* Victor Blasco Ortuño
* <victor.blasco@students.salle.url.edu>
**/
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "io.h"
#include "tcpServer.h"
<<<<<<< HEAD

int main(int argc, char* argv[]) {
    configurationData cd;
    //char buffer[255];
=======
#include "command.h"//TO DEBUG

int main(int argc, char* argv[]) {

	write(1, "Trying to connect", strlen("Trying to connect"));
	connectToPort(8850, "127.0.0.1");
/*
	configurationData cd;
	char * user_input;
	//char buffer[255];
>>>>>>> aa27be5e697a01d63ad3d9bfa5b026200e7fc99c

    if (argc == 2) {

        if (readConfigurationFile(argv[1], &cd) < 0) {
            write(1, FILE_NOT_FOUND_ERR, strlen(FILE_NOT_FOUND_ERR));
            return EXIT_FAILURE;
        } //if

    } //if
    else {
        write(1, ERR_ARGUMENTS, strlen(ERR_ARGUMENTS));
        return EXIT_FAILURE;
    } //else

    write(1, STARTUP, strlen(STARTUP));

    cd.userName[strlen(cd.userName) - 1] = '\0';

    printf("entering server client \n"); //KILL ME

    serverClient(cd);

<<<<<<< HEAD
    return EXIT_SUCCESS;
=======
		user_input = readUntil(0, '\n');

		if (strlen(user_input)) {
			if (checkCommand(user_input, cd)){
				break;
			}//if
		}else{

			write(1, ERR_UNKNOWNCMD, strlen(ERR_UNKNOWNCMD));
		}//else
	}//while

	serverClient(cd);

	return EXIT_SUCCESS;*/
>>>>>>> aa27be5e697a01d63ad3d9bfa5b026200e7fc99c
}
