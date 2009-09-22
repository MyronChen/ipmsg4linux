#ifndef _SEND_FILE_
#define _SEND_FILE_
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct SENDFILE{
	long msgNo;
	int isReceived;

	int no;

	char fileName[256];
	char fileFullName[256];
	long size;
	long lastModifyTime;

	//extra properties
	char properties[256];

	/**
	 * if is accept file ,record the computer'ip of send file.
	 */
	char remoteIp[256];
	int port;
}SendFile;

SendFile* sendFileNew(int no, char fileName[256]);

void sendFilePacked(SendFile *sendFile,char buffer[1024]);
#endif
