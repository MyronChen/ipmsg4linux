#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <assert.h>

#include "functions.h"
#include "console.h"
#include "util.h"
#include "const.h"

Function functions[FUNCS];
pthread_t consoleThread;

void function_init(Function *func, char name[256],char alias[16],METHOD method, char remark[256],char usage[1024])
{
 	strcpy(func->name,name);
	strcpy(func->alias, alias);
	func->method = method;
	strcpy(func->remark, remark);
	strcpy(func->usage, usage);
}

int consoleIndexOf(char *str, char c){
	int i;
	for(i=0;str[i]!=0;i++){
		if(str[i]==c){
			return i;
		}
	}
	return -1;
}


char* getCmd(char *cmdline){
	if(cmdline[0]=='!'){
		return "!";
	}

	int i=consoleIndexOf(cmdline, ' ');
	if(i>256){
		i= 256;
	}

	char cmd[256]={0};

	if(i==-1){
		return cmdline;
	}else{
		strncpy(cmd,cmdline, i);
		return cmd;
	}
}


void consoleInit(){
	int index = 0;
	function_init(&functions[index++],"list","ls", funcListUser,"list all users","list");
	function_init(&functions[index++],"refresh","rf", funcRefreshUser, "refresh user list", "refresh");
	function_init(&functions[index++],"send", "send", funcSend, "send a message to a user","send user(or host,ip) message");
	function_init(&functions[index++], "sendall","sa", funcSendAll, "send a message to all user", "sa message");

	function_init(&functions[index++], "sendfile", "sf", funcSendFile, "send a file to other user", "sf user(or host,ip) filename");
	function_init(&functions[index++], "listfile", "lf", funcListFile, "list the sent or receive files", "lf");
	function_init(&functions[index++], "accept", "ac", funcAcceptFile, "accept files", "accept file name(or msgNo:fileNo)");
	function_init(&functions[index++], "cancel", "ca", funcCancelFile, "cancel files", "cancel file name(or MsgNo ,or cancel all" );

	function_init(&functions[index++], "pwd", "pwd", funcPwd, "show current directory", "pwd");
	function_init(&functions[index++], "cd", "cd", funcCd, "change current directory", "cd dir");
	function_init(&functions[index++], "dir", "dir", funcDir, "list directory contents", "dir");
	function_init(&functions[index++], "!", "!", funcShell, "execute a system command", "!command args...");
		
	function_init(&functions[index++],"exit","quit", funcExit, "exit this program", "exit");
	function_init(&functions[index++],"help", "?", funcHelp, "display this help", "help");
	
	//if(Functions[FUNCS-1].name == "") must  FUNCS++
	assert(strcmp(functions[FUNCS-1].name,"")==0);

	function_init(&functions[FUNCS-1], "unknowCmd", "uc", funcUnknowCmd, "unknow command!", "");
}

void consoleRun(){
	while(1){
		char buffer[1024];
		printf(">");
		fgets(buffer,1024, stdin);
		char *input = strTrim(buffer);
		//scanf("%s",input);//cannot accept space
		//printf("input is :'%s'\n", input);

		if(strlen(input)>0){
			char *cmd = getCmd(input);
			Function* function = getFunctionByName(cmd);
			if(function->method != NULL){
				(*(function->method))(input);
			}
		}
	}
}

int consoleThreadStart(){
	int ret = pthread_create(&consoleThread,NULL,(void*)consoleRun,NULL);
	if(ret){
		ERROR("pthead_create failure!");
		return ret;
	}

	consoleInit();
	return ret;
}

Function* getFunctionByName(char name[256]){
	int i;
	for(i=0;i < FUNCS-1;i++){
		if(strcasecmp(functions[i].name, name)==0 
			|| strcasecmp(functions[i].alias, name)==0){
			return &functions[i];
		}	
	}	

	return &functions[FUNCS-1];
}

