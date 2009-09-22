#ifndef UTIL_H
#define UTIL_H
#include "const.h"

int getLocalIp(char *ip);

int udpSend(int socketDescriptor, char ip[256],int port, char message[MSG_SIZE], int msgSize);

char* strTrim(char *str);


int startsWith(char *str, char *prefix);

/*parse console input args*/
int argsParse(char *cmdstring, char *parrten, int limit, char arr[][]);

/* if file is exists return 1,else return 0 */
int exists(char filename[256]);

#endif
