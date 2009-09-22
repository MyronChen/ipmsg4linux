#ifndef USER_H
#define USER_H

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>

typedef struct USER_INFO
{
	char userName[256];
	char host[256];
	char ip[256];
	int port;
}UserInfo;

UserInfo* userListFindByProps(GList *list, char userOrHost[256]);

UserInfo* userListFindByLike(GList *list, char userOrHost[256]);

void userListDestroy();

#endif
