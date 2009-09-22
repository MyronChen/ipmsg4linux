#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <unistd.h>

#include "functions.h"
#include "console.h"
#include "ipmessage.h"
#include "user.h"
#include "messager.h"
#include "util.h"
#include "const.h"

void funcUnknowCmd(char cmdline[1024]){
	printf("%s\n", functions[FUNCS-1].remark);
}

void errPrompt(char *funcName){
	Function *function = getFunctionByName(funcName);
	if(function != NULL){
		printf("command invalid! usage:%s\n", function->usage);
	}
}

void funcListUser(char *cmd){
	GList* it = userList;
	for(;it != NULL;it = it->next){
		UserInfo *user = it->data;
		printf("%s@%s[%s]\n", user->userName,user->host,user->ip);
	}
}

void funcRefreshUser(char *cmd){
	userEntry();	
}

void funcSend(char *cmd){
	char args[3][1024];
	int size = argsParse(cmd, " ", 3, args);
	if(size != 3){
		errPrompt(args[0]);
	}else{
		int ret = sendMsg(args[1],args[2]);
		if(ret == -1){
			printf("user or host [%s] not exist!\n",args[1]);
		}

	}
}

void funcSendAll(char *cmd){
	char args[2][1024];
	int size = argsParse(cmd, " ", 2,args);
	if(size != 2){
		errPrompt(args[0]);
	}else{
		sendMsg2All(args[1]);
	}
}

void funcSendFile(char *cmd){
	char args[3][1024];
	int size = argsParse(cmd, " ", 3, args);
	if(size != 3){
		errPrompt(args[0]);
	}else{
		char files[FILE_NUM][256];
		strcpy(files[0],args[2]);
		int ret = sendfile(args[1],files);
		switch(ret){
			case -1:
				printf("user or host [%s] not exist!\n", args[1]);
				break;
			case -2:
				printf("file [%s] not exist!\n", args[2]);
				break;
		}
	}
}

void funcListFile(char *cmd){
	listfile();
}

void funcAcceptFile(char *cmd){
	char args[3][1024];
	int size = argsParse(cmd, " ", 3,args);
	if(size <2){
		errPrompt(args[0]);
	}else{
		int overwrite = 0;
		if(size ==3 && strcasecmp(args[2],"/o")==0){
			overwrite = 1;
		}
		acceptfile(args[1], overwrite);
	}
}

void funcCancelFile(char *cmd){
	char args[2][1024];
	int size =argsParse(cmd, " ", 2, args);
	if(size != 2){
		errPrompt(args[0]);
	}
	cancelfile(args[1]);
}

void funcPwd(char *cmd){
	char dir[256];
	getcwd(dir, 256);
	printf("%s\n", dir); 	
}

void funcCd(char *cmd){
	char args[2][1024];
	int size = argsParse(cmd, " ",2,args);
	if(size < 2){
		errPrompt(args[0]);
	}else{
		chdir(args[1]);
		char dir[256];
		getcwd(dir, 256);
		printf("current directory change to: '%s'\n", dir); 	
		
	}
}

void funcDir(char *cmd){
	system("ls");
}

/* execute system command!*/
void funcShell(char *cmd){
	cmd++;
	system(cmd);
}


void funcExit(char *cmd){
	userExit();
	printf("System Exit Now!\n");
	exit(0);
}

#define FORMAT "%-10s \t %-5s \t %-10s \n"

void funcHelp(char *cmd){
	printf("========= you can use alias too!...eg: list use ls to replace!\n");
	printf(FORMAT, "func", "alias", "usage");
	int i;
	for(i=0;i < FUNCS-1;i++){
		Function func = functions[i];
		if(func.method != NULL){
			printf(FORMAT, func.name, func.alias, func.usage);
		}
	}	
}

