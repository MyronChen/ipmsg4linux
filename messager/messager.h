#ifndef MESSAGER_H
#define MESSAGER_H

#include "ipmessage.h"
#include "user.h"

void messagerInit();

void messageSendSuccess(int success);

int sendMessage(IPMessage* pMsg);

void userEntry();

void userExit();

int sendMsg(char *userOrHost,char *msg);

void sendMsg2All(char msg[]);

int sendfile(char *userOrHost, char files[FILE_NUM][256]);

void listfile();

void acceptfile(char *fileOrNo, int overwrite);

void cancelfile(char *fileOrNo);

void addUser(IPMessage* pMsg);

void removeUser(IPMessage* pMsg);

void sendfileListRelease();

void receivefileListRelease();

typedef void (*AddOrRemoveUser)(UserInfo* user);

extern AddOrRemoveUser pAddUser;
extern AddOrRemoveUser pRemoveUser;

#endif
