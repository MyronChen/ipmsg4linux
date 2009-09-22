
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "filesender.h"
#include "ipmessage.h"
#include "util.h"
#include "const.h"
#include "sendfile.h"
#include "filelist.h"

pthread_t fileSendThread;

void fileSendRun(){
	printf("File Send thread start...\n");
	int fileSendSocket = socket(AF_INET, SOCK_STREAM,0);
	if(fileSendSocket == -1){
		ERROR("init file Send Socket..");
		return;
	}
	struct sockaddr_in sin;
	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(DEF_PORT);
	if(bind(fileSendSocket,(struct sockaddr*)&sin,sizeof(sin))==-1){
		ERROR("bind file send socket!");
		return;
	}

	if(listen(fileSendSocket,25)==-1){
		ERROR("listen file send socket!");
		return;
	}

	while(1){
		struct sockaddr cin;
		char buffer[MSG_SIZE];

		int cin_size = sizeof(struct sockaddr);
		int clientSocket =accept(fileSendSocket,(struct sockaddr*)&cin,&cin_size);
	   if(clientSocket == -1){
	   		ERROR("accept error!");
	 		continue;
	   }
	   int rec_size = recv(clientSocket,buffer,MSG_SIZE,0);
	   if(rec_size==-1){
			ERROR("recv error!");
	 		continue;
	   }
	   IPMessage* pMsg = ipmNewFromBytes(buffer, rec_size);
	   char * p = strtok(pMsg->extraMsg, ":");
	   if(p==NULL){
			printf("error request!");
			continue;
	   }
	   int msgNo=0;
	   sscanf(p, "%x",&msgNo);
	   p = strtok(NULL, ":");
	   long no = 0;
	   if(p!=NULL){
		sscanf(p,"%x",&no);
	   }
	   p = strtok(NULL, ":");
	   long pointer = 0;
	   if(p!=NULL){
		   sscanf(p,"%x",&pointer);
	   }

	   SendFile* requestFile = fileListGetSendFileByNo(sendFileList, msgNo, no);
	   if(requestFile == NULL){
			printf("request file[%d:%d] not exists!",msgNo,no);
	   }else{
			FILE* file = fopen(requestFile->fileFullName, "r");
			if(file != NULL){
				char read_buf[1024];
				if(pointer != 0){
					fseek(file, pointer, SEEK_SET);
				}
				while(!feof(file)){
					int read_size = fread(read_buf, sizeof(char),1024,file);
					if(read_size != 0){
						if(send(clientSocket, read_buf, read_size, 0)==-1){
							ERROR("send data!");
							break;
						}
					}
				}
				requestFile->isReceived = 1;
				fclose(file);
			}else{
				printf("request file[%s] not exist!", requestFile->fileFullName);
			}
	   }

	   FREE(pMsg);
	   close(clientSocket);	   
	}
		
}

int fileSendThreadStart(){
	int ret = pthread_create(&fileSendThread, NULL, (void*)fileSendRun,NULL);
	if(ret){
		ERROR("file send thread craete failure!");
		return ret;
	}

	return ret;
}
