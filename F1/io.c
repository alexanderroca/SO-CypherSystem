#include "io.h"

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
		printf("Username: %s\n", cd->userName);
		cd->audioDirectory = readUntil(fd, ENTER_SEP);
		printf("AudioDirectory: %s\n", cd->audioDirectory);
		cd->ip = readUntil(fd, ENTER_SEP);
		printf("IP: %s\n", cd->ip);
		//read(fd, &cd->port, sizeof(uint16_t));
		aux = readUntil(fd, ENTER_SEP);
		cd->port = atoi(aux);
		printf("Port: %d\n", cd->port);

		cd->connectionavailalble = malloc(sizeof(connectionAvailalble));

		while (1) {

			cd->connectionavailalble[i].toConnect = readUntil(fd, ENTER_SEP);
			printf("toConnect: %s\n", cd->connectionavailalble[i].toConnect);
			//read(fd, &cd->connectionavailalble[i].firstPort, sizeof(uint16_t));
			aux = readUntil(fd, ENTER_SEP);
			cd->connectionavailalble[i].firstPort = atoi(aux);
			printf("firstPort: %d\n", cd->connectionavailalble[i].firstPort);
			//read(fd, &cd->connectionavailalble[i].lastPort, sizeof(uint16_t));
			aux = readUntil(fd, ENTER_SEP);
			cd->connectionavailalble[i].lastPort = atoi(aux);
			printf("lastPort: %d\n", cd->connectionavailalble[i].lastPort);

			if (checkEOF(fd) == 1)
				break;

			i++;
			cd->connectionavailalble = (connectionAvailalble*)realloc(cd->connectionavailalble, (i + 1) * sizeof(connectionAvailalble));
			cd->numConnections++;
			printf("Numconnections: %d\n", cd->numConnections);
		} //while

		close(fd);
		return 0;
	} //else
}

int checkEOF(int fd) {
	char c;
	ssize_t n = read(fd, &c, sizeof(char));

	if (n == 0) return 1;

	lseek(fd, -1, SEEK_CUR);
	return 0;

}
