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
#include "io.h"
#include "tcpServer.h"

int main(int argc, char* argv[]) {
    configurationData cd;
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

    signal(SIGINT, sig_handler);

    serverClient(cd);

    return EXIT_SUCCESS;
}
