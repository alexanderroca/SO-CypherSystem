#ifndef _IO_H
#define  _IO_H

#include "DataStruct.h"
//#include "command.h"

//checks a file of a given file descriptor for the end of file character
int checkEOF(int fd);
//reads from a given file descriptor until it finds a given stopping character
char* readUntil(int fd, char end);
//reads the configuration.txt file and stores the data read accordingly
int readConfigurationFile(char* path, configurationData* cd);
//does the initial check for the users command input before calling command
//specific checking functions
int checkCommand(char * user_input, configurationData cd);
//DEPRECATED: Turns all the characters from a string to upper case
void stringToUpper(char * string);

/******************************************************************************
* All check functions work using an string array which contains the users input
* separated in words and they check the words according to the parameter they
* are trying to execute.
******************************************************************************/
void checkCMDConnect(char **ptr, int c);
void checkCMDSay(char **ptr, int c);
void checkCMDBroadcast(char **ptr, int c);
void checkCMDDownload(char **ptr, int c);
void checkCMDShow(char **ptr, int c, configurationData cd);

//makes the checks to see if the exit command has been correctly entered
int checkCMDExit(int c);

#endif
