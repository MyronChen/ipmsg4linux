#ifndef _CONSOLE_
#define _CONSOLE_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#define FUNCS 15 

typedef void (*METHOD)(char *cmd);

typedef struct FUNCTION{
	char name[256];
	char alias[16];
	METHOD method;
	char remark[256];
	char usage[1024];
}Function;



void consoleRun();

int consoleThreadStart();

Function* getFunctionByName(char name[256]);

Function functions[FUNCS];

pthread_t consoleThread;

#endif
