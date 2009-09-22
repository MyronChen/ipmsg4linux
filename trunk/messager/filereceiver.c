#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "filereceiver.h"
#include "sendfile.h"
#include "util.h"
#include "ipmessage.h"

void receiveFile(void* args){
	SendFile* sendFile = (SendFile*)((void**)args)[0];
	int* overwrite = (int*)((void**)args)[1];
	
	IPMessage* pMsg = getGetFileDataMsg(sendFile->msgNo,sendFile->no,0);
	/* no /o option and file exists.*/
	if((!overwrite) && exists(sendFile->fileName)){
		printf("file[%s] exists,you can use /o option to overrite it .");
		return ;
	}

	int socket_descriptor = socket(AF_INET, SOCK_STREAM,0);
	if(socket_descriptor==-1){
		ERROR("create socket error!");
		return;
	}
	struct sockaddr_in sin;
	bzero(&sin,sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(sendFile->remoteIp);
	sin.sin_port = htons(sendFile->port);
	if(connect(socket_descriptor,(void*)&sin,sizeof(sin))==-1){
		char buffer[256];
		sprintf(buffer, "connect to %s failure!",sendFile->remoteIp);
		ERROR(buffer);
		goto end;
	}
	char buffer[MSG_SIZE];
	int size=0;
	ipmPacket(pMsg,buffer, &size);	
	send(socket_descriptor, buffer,size,0);

	char recv_buffer[1024];
	FILE *file = fopen(sendFile->fileName, "w");
	if(file == NULL){
		printf("open local file [%s] failure!\n",sendFile->fileName);
		goto end;
	}

	while(1){
		int rec_size = recv(socket_descriptor,recv_buffer,1024,0);
		if(rec_size == -1){
			ERROR("recv error!");
			goto end;
		}else if(rec_size == 0){
			break;
		}
		fwrite(recv_buffer,rec_size,1,file);
	}
	//INFO(recv_buffer);

end:
	if(file != NULL){
		fclose(file);
	}
	close(socket_descriptor);

}
