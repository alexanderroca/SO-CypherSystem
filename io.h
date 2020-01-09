#ifndef _IO_H
#define  _IO_H

#include "DataStruct.h"
#include "command.h"

#define MD5SUM "md5sum"
#define MD5SUM_FAILED "md5sum command error\n"
#define MD5SUM_COMMAND "md5sum %s"

//
void itoa(int n, char* s);
//
void reverse(char* s);
//
void clearBuffer(char* buffer);
//checks a file of a given file descriptor for the end of file character
int checkEOF(int fd);
//reads from a given file descriptor until it finds a given stopping character
char* readUntil(int fd, char end);

char* readUntilNumBytes(int fd, int num);
//reads the configuration.txt file and stores the data read accordingly
int readConfigurationFile(char* path, configurationData* cd);

int readAudioFile(char* path, int socket);

int getAudioFile(char* fileName, char* directoryUserConnected, int socket, char* usernameConnected);

//general function to send messages via sockets
int sendSocketMSG(int sockfd, char * data, int type);

//function to recieve data from sockets
char * receiveSocketMSG(int sockfd);

connectedInfo checkUserConnnected(char* userName, connectedList connected_list);

void replyDirectoryUserConnected(char* directory_name, int socket);

void readDirectoryUserConnected(int socket);

int initializationPipes(int fd[2]);

void showPorts(int ports[], int num_ports);

//does the initial check for the users command input before calling command
//specific checking functions
int checkCommand(char * user_input, configurationData cd, connectedList * cl);

//DEPRECATED: Turns all the characters from a string to upper case
void stringToUpper(char * string);

/******************************************************************************
* All check functions work using an string array which contains the users input
* separated in words and they check the words according to the parameter they
* are trying to execute.
******************************************************************************/
void checkCMDConnect(char **ptr, int c, connectedList * cl);
void checkCMDSay(char **ptr, int c, connectedList * cl, configurationData cd);
void checkCMDBroadcast(char **ptr, int c, connectedList * cl, configurationData cd);
void checkCMDDownload(char **ptr, int c);
void checkCMDShow(char **ptr, int c, configurationData cd);

//makes the checks to see if the exit command has been correctly entered
int checkCMDExit(int c);

#endif
