#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include "ipmessage.h"
#include "msgtype.h"
#include "util.h"
#include "const.h"

#define DELIM ((char*)":")

char ATTACH_SEP1 = '\0';
char ATTACH_SEP2 = (char)7;

char version[256] = "1";
char localhost[256] = {0};
int isTextModel=1;
int udpSocketDescriptor;
GList *userList = NULL;
GList *receiveFileList = NULL;
GList *sendFileList = NULL;

int udpSocketInit(){

	getLocalIp(localhost);

	struct sockaddr_in sin;
	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(DEF_PORT);
	udpSocketDescriptor = socket(AF_INET,SOCK_DGRAM,0);
	if(udpSocketDescriptor == -1){
		ERROR("init socket failure..");
		return -1;
	}
	if(bind(udpSocketDescriptor,(struct sockaddr*)&sin,sizeof(sin))
			== -1){
		ERROR("bind failure, port is used!");	
		return -2;
	}
	return 0;
}

void udpSocketDestroy(){
	if(close(udpSocketDescriptor)==-1){
		ERROR("close socket failure!");
	}
}


IPMessage* ipmNew(char version[256],long no,char user[256], char host[256], int command, char extraMsg[1024]){
	IPMessage* pMsg = malloc(sizeof(IPMessage));
	memset(pMsg, 0, sizeof(IPMessage));

	strcpy(pMsg->version,version);
	pMsg->no = no;
	if(user != NULL){
		strcpy(pMsg->user, user);
	}
	if(host != NULL){
		strcpy(pMsg->host, host);
	}

	pMsg->command = command;
	if(extraMsg != NULL){
		strcpy(pMsg->extraMsg, extraMsg);
	}
	pMsg->port = DEF_PORT;

	return pMsg;
}

void ipmPacket(IPMessage* pMsg, char btMsg[],int* size){
	memset(btMsg,0,MSG_SIZE);
	sprintf(btMsg,"%s:%ld:%s:%s:%d:%s",pMsg->version,pMsg->no,pMsg->user,pMsg->host,pMsg->command,pMsg->extraMsg);
	int i;
	int index = 0;

	for(i=0;i<FILE_NUM;i++){
		if(pMsg->files[i]==NULL){
			break;
		}
		if(i==0){
			index = strlen(btMsg);
			strcat(btMsg,"|");
		}else{
			strcat(btMsg,"\007");
		}	
		char buffer[512] = {0};
		sendFilePacked(pMsg->files[i],buffer);
		strcat(btMsg,buffer);
		if(strlen(btMsg) >= MSG_SIZE){
			break;
		}
	}
	*size = strlen(btMsg);
	if(index != 0){
		btMsg[index]='\0';
	}
}

/**
 * return index  of c in arr
 */
int indexOf(char* arr,int arrSize,char c){
	int i=0;
	for(i=0;i< arrSize;i++){
		if(arr[i] == c){
			break;
		}
	}
	return i;
}


IPMessage* ipmNewFromBytes(char *buf, int bufSize){
	if(buf[bufSize-1]==0){
		while(buf[bufSize-1]==0){
			bufSize--;
		}
	}

	IPMessage* pMsg = malloc(sizeof(IPMessage));
	memset(pMsg, 0, sizeof(IPMessage));

/*
	int i= indexOf(buf, bufSize,(char)0);

	char baseMsg[MSG_SIZE];
	memcpy(baseMsg,buf,i);
	baseMsg[i] = 0;
*/
	char *attachment = strchr(buf, (int)'\0')+1;
	
	//version..
	char *p = strtok(buf, DELIM);
	if(p != NULL){
		strcpy(pMsg->version, p);
	}
	//no
	p = strtok(NULL,DELIM);
	if(p != NULL){
		pMsg->no = atoll(p);
	}
	//user
	p = strtok(NULL,DELIM);
	if(p!=NULL){
		strcpy(pMsg->user,p);
	}
	//host
	p = strtok(NULL,DELIM);
	if(p!=NULL){
		strcpy(pMsg->host, p);
	}
	//command
	p = strtok(NULL,DELIM);
	if(p!=NULL){
		pMsg->command = atol(p);
	}
	//extraMsg
	p = strtok(NULL, DELIM);
	if(p!=NULL){
		strcpy(pMsg->extraMsg,p);
	}

	if(strcmp(attachment, "")!=0){
		int i;
		for(i=0;i<FILE_NUM;i++){
			if(i==0){
				p = strtok(attachment,"\007");
			}else{
				p = strtok(NULL,"\007");
			}
			if(p== NULL){
				break;
			}
			pMsg->files[i] = sendFileNewFromBytes(p);
			pMsg->files[i]->msgNo = pMsg->no;
		}	
	}


	return pMsg;
}

/**
 * get the packet number
 */
long getPacketNumber(){
	time_t now = time(NULL);
	return now;
}

/**
 * get the default ipmessage
 */
IPMessage* getDefIPMessage(int command,char extraMsg[]){
	char selfName[256];
    getlogin_r(selfName,256);
	char localhost[256];
	gethostname(localhost, 256);
	return ipmNew(version, getPacketNumber(),selfName,localhost, command,extraMsg);
}

/**
 * get the acknowledge message
 */
IPMessage* getACKMsg(long packageNo, char remoteIp[256], int remotePort){
	char buffer[32];
	sprintf(buffer,"%ld", packageNo);
	IPMessage* pMsg = getDefIPMessage(IPMSG_RECVMSG ,buffer);
   	strcpy(pMsg->remoteIp,remoteIp);
	pMsg->port = remotePort;
	return pMsg;	
}

/**
 * get a common text message
 */
IPMessage* getTextMsg(char message[1024],char remoteIp[256],int remotePort){
	IPMessage* pMsg = getDefIPMessage(IPMSG_SENDMSG|IPMSG_SENDCHECKOPT,message);
	strcpy(pMsg->remoteIp,remoteIp);
	pMsg->port = remotePort;
	return pMsg;
}

IPMessage* getTextMsgWithAttachment(char message[1024],SendFile *files[FILE_NUM], char remoteIp[256], int remotePort){
	IPMessage *pMsg = getDefIPMessage(IPMSG_SENDMSG |IPMSG_SENDCHECKOPT |IPMSG_FILEATTACHOPT,message);
	int i;
	for(i=0;i<FILE_NUM;i++){
		pMsg->files[i] = files[i];
	}
	strcpy(pMsg->remoteIp,remoteIp);
	pMsg->port = remotePort;

	return pMsg;
}
/**
 * get a common text message with no check
 */
IPMessage* getNoCheckTextMsg(char message[1024],char remoteIp[256],int remotePort){
	IPMessage* pMsg = getDefIPMessage(IPMSG_SENDMSG,message);
	strcpy(pMsg->remoteIp,remoteIp);
	pMsg->port = remotePort;
	return pMsg;
}

/**
 * get the request download file msg
 */
IPMessage* getGetFileDataMsg(long msgNo,long fileNo,int pointer){

	char buffer[128];
	sprintf(buffer,"%x:%x:%x",msgNo,fileNo,pointer);
	return getDefIPMessage(IPMSG_GETFILEDATA, buffer);
}

/**
 * Cancel download file
 */
IPMessage* getCancelFileMsg(long msgNo){
	char buffer[32];
	sprintf(buffer,"%ld",msgNo);
	return getDefIPMessage(IPMSG_RELEASEFILES,buffer);
}

/**
 * Get Response msg for user enter
 */
IPMessage* getAnsEnterMsg(char remoteIp[256],int remotePort){
	IPMessage* pMsg = getDefIPMessage(IPMSG_ANSENTRY,NULL);
	strcpy(pMsg->remoteIp,remoteIp);
	pMsg->port = remotePort;
	return pMsg;
}


/**
 * Get Entry Message
 */
IPMessage* getEntryMsg(){
	IPMessage* pMsg = getDefIPMessage(IPMSG_BR_ENTRY,NULL);
	strcpy(pMsg->remoteIp,"255.255.255.255");
	pMsg->port = 2425;

	return pMsg;
}

/**
 * Get Exit Message
 */
IPMessage* getExitMsg(){
	IPMessage* pMsg = getDefIPMessage(IPMSG_BR_EXIT,NULL);
	strcpy(pMsg->remoteIp,"255.255.255.255");
	pMsg->port = 2425;

	return pMsg;
}

/**
int main(void){
	
	IPMessage * pMsg = getEntryMsg();

	INFO("SDFSDLFJ");
	ERROR("this line has a error !");
	INFO("===================================================");
	sendMsg(pMsg);
	FREE(pMsg);
	sleep(3);
	pMsg = getTextMsg("I am liuxiaojie ....!", "192.168.1.101",2425);
	sendMsg(pMsg);
	FREE(pMsg);

	sleep(3);

	pMsg = getExitMsg();
	sendMsg(pMsg);
	FREE(pMsg);

	return 0;
}
*/
