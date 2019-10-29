#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "io.h"

int main(int argc, char* argv[]) {
	configurationData cd;

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

	return EXIT_SUCCESS;
}
