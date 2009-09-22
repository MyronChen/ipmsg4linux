#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>

#include "user.h"
#include "const.h"
#include "ipmessage.h"
#include "util.h"

UserInfo* userListFindByProps(GList *list, char userOrHost[256]){
	GList* it = list;
	for(it;it != NULL; it = it->next){
		UserInfo *user = it->data;

		if(strcasecmp(user->userName,userOrHost)==0
			|| strcasecmp(user->host,userOrHost)==0
			|| strcasecmp(user->ip, userOrHost)==0){
			return user;
		}
	}
	return NULL;
}


UserInfo* userListFindByLike(GList *list, char userOrHost[256]){
	GList* it = list;
	for(it;it != NULL; it = it->next){
		UserInfo *user = it->data;

		if(startsWith(user->userName,userOrHost)
			|| startsWith(user->host,userOrHost)
			|| startsWith(user->ip, userOrHost)){
			return user;
		}
	}
	return NULL;
}

void userListDestroy(){
	GList* it = userList;
	for(it;it != NULL; it = it->next){
		UserInfo *user = it->data;
		g_free(user);
	}
	g_list_free(userList);
	userList = NULL;
	//printf("########### userList=%d\n", userList);		
}
