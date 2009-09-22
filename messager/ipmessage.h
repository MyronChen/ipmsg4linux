#ifndef IP_MESSAGE_H
#define IP_MESSAGE_H

#include <stdio.h>
#include "gtk/gtk.h"
#include "sendfile.h"
#include "const.h"

extern char ATTACH_SEP1;
extern char ATTACH_SEP2;

typedef struct IP_MESSAGE{
	char version[256];
	long no;
	int command;
	char user[256];
	char host[256];
	char extraMsg[1024];
	char remoteIp[256];
	int port;
	SendFile *files[FILE_NUM];/* attachment files */
}IPMessage;

void ipmPacket(IPMessage* pMsg, char btMsg[], int* size);

IPMessage* ipmNewFromBytes(char* buffer, int bufSize);

/**
 * get the default ipmessage
 */
IPMessage* getDefIPMessage(int command,char extraMsg[]);
/**
 * get the acknowledge message
 */
IPMessage* getACKMsg(long packageNo, char remoteIp[256], int remotePort);

/**
 * get a common text message
 */
IPMessage* getTextMsg(char message[1024],char remoteIp[256],int remotePort);

IPMessage* getTextMsgWithAttachment(char message[1024],SendFile *files[FILE_NUM], char remoteIp[256], int remotePort);

/**
 * get a common text message with no check
 */
IPMessage* getNoCheckTextMsg(char message[1024],char remoteIp[256],int remotePort);

/**
 * get the request download file msg
 */
IPMessage* getGetFileDataMsg(long msgNo,long fileNo,int pointer);

/**
 * Cancel download file
 */
IPMessage* getCancelFileMsg(long msgNo);

/**
 * Get Response msg for user enter
 */
IPMessage* getAnsEnterMsg(char remoteIp[256],int remotePort);

/**
 * Get Entry Message
 */
IPMessage* getEntryMsg();

/**
 * Get Exit Message
 */
IPMessage* getExitMsg();


int udpSocketInit();
void udpSocketDestroy();
void userListDestroy();


extern int udpSocketDescriptor;
extern char localhost[256];
extern GList *userList;
extern GList *receiveFileList;
extern GList *sendFileList;

extern int isTextModel;

#endif

