/******************************************************************************
* SO-CypherSystem
*
* Authors:
* Alexander Roca Myroshnichenko
* <alexander.roca@students.salle.url.edu>
*
* Victor Blasco Ortuño
* <victor.blasco@students.salle.url.edu>
******************************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "io.h"

int main(int argc, char* argv[]) {
	configurationData cd;
	char * user_input;
	//char buffer[255];

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

	while (1) {

		write(1, "$", 1);
		write(1, cd.userName, strlen(cd.userName));
		write(1, ":", 1);

		user_input = readUntil(0, '\n');

		if (strlen(user_input)) {
			if (checkCommand(user_input, cd)){
				break;
			}//if
		}else{

			write(1, ERR_UNKNOWNCMD, strlen(ERR_UNKNOWNCMD));
		}//else
	}//while

	return EXIT_SUCCESS;
}
