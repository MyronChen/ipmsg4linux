#ifndef _FILE_SENDER_
#define _FILE_SENDER_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void fileSendRun();

int fileSendThreadStart();

pthread_t fileSendThread;

#endif
