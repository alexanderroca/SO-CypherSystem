#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "io.h"

int main(int argc, char* argv[]) {
	configurationData cd;
	char * user_input;

	if (argc == 2) {

		if (readConfigurationFile(argv[1], &cd) < 0) {
			write(1, FILE_NOT_FOUND_ERR, strlen(FILE_NOT_FOUND_ERR));
			return EXIT_FAILURE;
		} //if

	} //if
	else {
		write(1, "ERROR, invalid arguments\n", strlen("ERROR, invalid arguments\n"));
		return EXIT_FAILURE;
	} //else

	while (1) {

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
