#ifndef _IO_H
#define  _IO_H

#include "DataStruct.h"
#include "LlistaBid.h"
#include "command.h"

//
void fixStrings(configurationData * cd);
//
void charTransfer(char * result, char * origin, int length);
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

int sendServerCheck(int sockfd, int type, char * data, int length, int ok);

void receiveServerCheck(int sockfd, char * data);

void sendEOF(int sockfd, char * md5);

//function to recieve data from sockets
int receiveSocketMSG(int sockfd, int * type, char ** data);

int checkUserConnnected(char* userName, LlistaBid list);

void replyDirectoryUserConnected(char* directory_name, int socket);

void createAudioListMSG(char * list);

void readDirectoryUserConnected(int socket);

int initializationPipes(int fd[2]);

void showAudioFiles(char** files, int num_files);

void showPorts(int ports[], int num_ports);

//does the initial check for the users command input before calling command
//specific checking functions
int checkCommand(char * user_input, Info * info_client);

//DEPRECATED: Turns all the characters from a string to upper case
void stringToUpper(char * string);

/******************************************************************************
* All check functions work using an string array which contains the users input
* separated in words and they check the words according to the parameter they
* are trying to execute.
******************************************************************************/
void checkCMDConnect(char **ptr, int c, Info * info_client);
void checkCMDSay(char **ptr, int c, Info * info_client);
void checkCMDBroadcast(char **ptr, int c, Info * info_client);
void checkCMDDownload(char **ptr, int c, Info * info_client);
void checkCMDShow(char **ptr, int c, Info * info_client);

//makes the checks to see if the exit command has been correctly entered
int checkCMDExit(int c, Info * info_client);

void setupCI(configurationData cd, connectionInfo * ci);
void initString(char * string, int size);
char * get_message(int fd, char delimiter);
int alreadyConnected(Info * info_client, char * port);
#endif
