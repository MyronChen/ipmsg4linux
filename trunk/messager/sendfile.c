#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "sendfile.h"


/**
 *return 
 */
SendFile* sendFileNew(int no, char fileName[256]){
	SendFile *sendFile =malloc(sizeof(SendFile));
	memset(sendFile, 0 , sizeof(SendFile));
	sendFile->no = no;
	strcpy(sendFile->fileName,fileName);
	char dir[256];
	getcwd(dir, 256);
	sprintf(sendFile->fileFullName,"%s/%s",dir,fileName);
	
	struct stat info;
	if(stat(sendFile->fileFullName,&info) != -1){
		sendFile->size = info.st_size;
		sendFile->lastModifyTime = info.st_mtime;
	}else{
		free(sendFile);
		if(errno == ENOENT){/* file not exists!*/
			sendFile = (SendFile*)-1;
		}else{
			sendFile = (SendFile*)-2;
		}
	}
	return sendFile;
}

SendFile* sendFileNewFromBytes(char buffer[512]){
	SendFile *sendFile = malloc(sizeof(SendFile));
	memset(sendFile, 0,sizeof(SendFile));

	sscanf(buffer,"%d:%[^:]:%x:%x:%s",&sendFile->no,sendFile->fileName,&sendFile->size,&sendFile->lastModifyTime,sendFile->properties);

	return sendFile;
}

void sendFilePacked(SendFile *sendFile,char buffer[512]){
	sprintf(buffer,"%d:%s:%x:%x:%s",sendFile->no,sendFile->fileName,sendFile->size,sendFile->lastModifyTime,sendFile->properties);
}

void sendFileToString(SendFile *sendFile, char buffer[256]){
	sprintf(buffer, "%-8d:%-2d | [%c]  | %-s ", sendFile->msgNo,sendFile->no,sendFile->isReceived?'Y':'N', sendFile->fileName);
}
