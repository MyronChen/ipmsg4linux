#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>

#include "receiver.h"
#include "ipmessage.h"
#include "msgtype.h"
#include "messager.h"
#include "util.h"
#include "const.h"
#include "filelist.h"

void sendAckMsg(IPMessage* pMsg){
	long opt = pMsg->command & IPMSG_OPTMASK;
	if((opt & IPMSG_SENDCHECKOPT)!=0){
		IPMessage* pAckMsg = getACKMsg(pMsg->no,pMsg->remoteIp,pMsg->port);
		sendMessage(pAckMsg);
		FREE(pAckMsg);
	}
}

void showMessage(IPMessage* pMsg){
	char buffer[4096];
	sprintf(buffer,"==============[ %s ]================\n"
			">:%s\n",pMsg->remoteIp, pMsg->extraMsg);
	int i=0;
	
	if(pMsg->files[0] != NULL){
		strcat(buffer, pMsg->remoteIp);
		strcat(buffer, " send some files:\n"); 	
	}

	for(i;i<FILE_NUM;i++){
		if(pMsg->files[i] == NULL){
			break;
		}
		strcat(buffer, "\t");
		char strFile[512];
		sprintf(strFile, "%-d:%-4d | %-20s\n",pMsg->files[i]->msgNo,pMsg->files[i]->no,pMsg->files[i]->fileName);
		strcat(buffer, strFile);
	}
	printf(buffer);

	printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
}

/**
 * add the attachment files
 */
void addAttachFile(IPMessage* pMsg){
	int i;
	for(i=0;i<FILE_NUM;i++){
		if(pMsg->files[i]==NULL){
			break;
		}
		receiveFileList = g_list_append(receiveFileList, pMsg->files[i]);
	}
}

/**
 * message receive thread..
 */
void receiverRun(void){
	while(1){
		IPMessage* pMsg = receiverAccept();
		
		if((int)pMsg == -1){/* error ..*/
			break;
		}

		int cmdSwitch = pMsg->command & IPMSG_COMMASK;
		switch(cmdSwitch){
			case IPMSG_NOOPERATION://no operation.
				break;
			case IPMSG_BR_ENTRY:
				addUser(pMsg);

				sendAckMsg(pMsg);
				/**
				 * If is sent from itself
				 */
				if(strcmp(localhost, pMsg->remoteIp)==0){
					break;
				}
				//send answer messager
				IPMessage* pAnsMsg = getAnsEnterMsg(pMsg->remoteIp,pMsg->port);
				sendMessage(pAnsMsg);
				break;
			case IPMSG_BR_EXIT:
				removeUser(pMsg);
				break;
			case IPMSG_ANSENTRY:
				addUser(pMsg);
				sendAckMsg(pMsg);
				break;
			case IPMSG_SENDMSG:
				if(strcmp(localhost, pMsg->remoteIp)==0){
					break;
				}

				sendAckMsg(pMsg);
				showMessage(pMsg);
				addAttachFile(pMsg);
				addUser(pMsg);
			case IPMSG_RECVMSG:
				messageSendSuccess(1);
				break;
			case IPMSG_GETFILEDATA:
				//process in filesendthread.c
				break;
			case IPMSG_RELEASEFILES:
				{
					long msgNo = atoll(pMsg->extraMsg);
					fileListReleaseAttachment(sendFileList, msgNo);
				}
				break;
			default:
				{
				int size = 0;
				char buffer[MSG_SIZE];
				ipmPacket(pMsg,buffer,&size);
				char msg[MSG_SIZE + 32];
				sprintf(msg,"Unprocessed MSG:%s\n", buffer);
				}
		}//end of switch(cmdSwitch)
		FREE(pMsg);
	}	
}

int receiverThreadStart(){
	pthread_t recThread;
	int ret = pthread_create(&recThread,NULL,(void*)receiverRun,NULL);
	if(ret){
		ERROR("pthread_create failure!");
	}
	return ret;
	//pthread_join(recThread,NULL);
}

IPMessage* receiverAccept(){
	IPMessage* pMsg;
	int run = 1;
	while(run){
		struct sockaddr_in sin;
		int sin_len = sizeof(sin);
		char buffer[MSG_SIZE] = {0};
		//memset(buffer, 0, MSG_SIZE);

		int recvSize =  0;
		recvSize = recvfrom(udpSocketDescriptor,
				buffer,MSG_SIZE,0,
				(struct sockaddr*)&sin,&sin_len);
		//INFO(buffer);

		if(recvSize==-1){
			switch(errno){
				case EBADF:
					ERROR("EBADF, bad socket descriptor.");
					run = 0;
					pMsg = (IPMessage*)-1;
					break;
				case ECONNRESET:
					ERROR("ECONNRESET");
					break;
				case ETIMEDOUT:
					ERROR("ETIMEDOUT");
					break;
				default:
					{char msg[256];
					sprintf(msg, "Recefrom returned:%d", errno);
					ERROR(msg);
					}
			}
			continue;
		}else{
			pMsg = ipmNewFromBytes(buffer,recvSize);
			char* address = inet_ntoa(sin.sin_addr);
			strcpy(pMsg->remoteIp,address);
			pMsg->port = ntohs(sin.sin_port);
		   	int i;
			for(i=0;i< FILE_NUM;i++){
				if(pMsg->files[i] != NULL){
					strcpy(pMsg->files[i]->remoteIp,address);
					pMsg->files[i]->port = pMsg->port;
				}
			}	
			break;
		}
	}
	return pMsg;
}

