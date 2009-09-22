#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "messager.h"
#include "util.h"
#include "ipmessage.h"
#include "receiver.h"
#include "msgtype.h"
#include "user.h"
#include "const.h"
#include "console.h"
#include "filelist.h"
#include "filesender.h"
#include "filereceiver.h"

int msgSendOk;

AddOrRemoveUser pAddUser= NULL;
AddOrRemoveUser pRemoveUser = NULL;

void messageSendSuccess(int success){
	msgSendOk = success;
}

void messagerInit(){
	
}

int sendMessage(IPMessage* pMsg){
	char btMsg[MSG_SIZE];
	int msgSize = 0;

	ipmPacket(pMsg,btMsg, &msgSize);
	
	int mustAck = (pMsg->command & IPMSG_SENDCHECKOPT)!=0;
	int times = 1;
	if(mustAck){
		times = 5;
	}

	msgSendOk = 0;
	int i;
	for(i=0;i<times;i++){
		if(msgSendOk){
			break;
		}
		udpSend(udpSocketDescriptor,pMsg->remoteIp,pMsg->port,btMsg,msgSize);
		sleep(2);
	}
	return (!mustAck||msgSendOk);
}

void userEntry(){
	userListDestroy();
	IPMessage* pMsg = getEntryMsg();
	sendMessage(pMsg);
	FREE(pMsg);
}

void userExit(){
	IPMessage* pMsg = getExitMsg();
	sendMessage(pMsg);
	FREE(pMsg);
	udpSocketDestroy();
	userListDestroy();
	sendfileListRelease();
	receivefileListRelease();
}

void sendfileListRelease(){
	fileListReleaseAttachment(sendFileList,RELEASE_ALL);
}

void receivefileListRelease(){
	fileListReleaseAttachment(receiveFileList,RELEASE_ALL);
}

/**
 * return -1 if userOrHost not found
 */
int sendMsg(char *userOrHost,char *msg){
	UserInfo *user = userListFindByLike(userList,userOrHost);
	if(user == NULL){
		return -1;
	}
	IPMessage *pMsg = getTextMsg(msg,user->ip,user->port);
	sendMessage(pMsg);
	FREE(pMsg);
	return 0;
}

void sendMsg2All(char *msg){
	IPMessage* pMsg = getNoCheckTextMsg(msg,"255.255.255.255",DEF_PORT);
	sendMessage(pMsg);
	FREE(pMsg);
}

/**
 * the max number file is FILE_NUM
 */
int sendfile(char *userOrHost,char files[FILE_NUM][256]){
	UserInfo *user = userListFindByLike(userList,userOrHost);			
	if(user == NULL){
		return -1;
	}
	SendFile *sendfiles[FILE_NUM];

	char msg[1024]={0};
	strcat(msg, "please accept files");
	int i;
	for(i=0;i<FILE_NUM;i++){
		if(strcmp(files[i], "")!=0){
			SendFile *tempFile = sendFileNew(i,files[i]);
			if(((int)tempFile) == -1||((int)tempFile) == -2){
				return -2;
			}else{
				sendfiles[i] = tempFile;
			}
		}else{
			break;
		}
	}
	IPMessage *pMsg = getTextMsgWithAttachment(msg,sendfiles,user->ip,user->port);
	for(i=0;i<FILE_NUM;i++){
		if(sendfiles[i]!=NULL){
			sendfiles[i]->msgNo = pMsg->no;
			sendFileList = g_list_append(sendFileList, sendfiles[i]);				
		}else{
			break;
		}
	}

	sendMessage(pMsg);
	FREE(pMsg);

	return 0;	
}

void listfile(){
	printf("======================   sent files   ======================\n");
	fileListShow(sendFileList);
	printf("====================== received files ======================\n");
	fileListShow(receiveFileList);
}

void acceptfile(char *fileOrNo, int overwrite){
	GList *acceptFiles = fileListGetSendFiles(receiveFileList, fileOrNo);
	if(acceptFiles == NULL){
		printf("can't find file[%s]", fileOrNo);
	}else{
		GList* it;
		for(it=acceptFiles;it != NULL;it=it->next){
			SendFile* sendFile = it->data;
			pthread_t recvThread;
			void* args[2] = {sendFile,&overwrite};
			int ret = pthread_create(&recvThread,NULL,(void*)receiveFile,(void*)args);
			if(ret){
				printf("start thread to receive file[%s] failure!\n",sendFile->fileName);
				continue;
			}
		}
		g_list_free(acceptFiles);
	}
}

void cancelfile(char *fileOrNo){
	GList *cancelFiles = fileListGetSendFiles(receiveFileList, fileOrNo);
	if(cancelFiles == NULL){
		printf("can't find file[%s]", fileOrNo);
	}else{
		GList* it;
		for(it=cancelFiles;it != NULL;it=it->next){
			SendFile* cancelFile = it->data;
			receiveFileList = g_list_remove(receiveFileList, cancelFile);
			IPMessage* pMsg = getCancelFileMsg(cancelFile->msgNo);
			sendMessage(pMsg);
			FREE(pMsg);
			free(cancelFile);
		}
		g_list_free(cancelFiles);
	}

}
void addUser(IPMessage* pMsg){
	UserInfo *user =g_new(UserInfo,1);

	strcpy(user->userName, pMsg->user);
	strcpy(user->host, pMsg->host);
	strcpy(user->ip, pMsg->remoteIp);
	user->port = pMsg->port;

	if(userListFindByProps(userList,user->ip)==NULL){
		printf("user %s login !\n", pMsg->remoteIp);	
		userList = g_list_append(userList,user);
		if(!isTextModel){
			pAddUser(user);
		}
	}
}

void removeUser(IPMessage* pMsg){
	printf("user %s logout!\n", pMsg->remoteIp);
	UserInfo *user = userListFindByProps(userList, pMsg->remoteIp);
	if(user != NULL){
		userList = g_list_remove(userList, user);
		if(!isTextModel){
			pRemoveUser(user);
		}
		g_free(user);
	}
}

int main(void){
	int sret = udpSocketInit();
	if(sret!=0){
		return sret;
	}
	
	receiverThreadStart();
	consoleThreadStart();	
	fileSendThreadStart();

	userEntry();

	pthread_join(consoleThread, NULL);
	return 0;
}
