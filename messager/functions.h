#ifndef _FUNCTIONS_
#define _FUNCTIONS_

#include <stdlib.h>
#include <stdio.h>

void funcUnknowCmd(char *cmd);

void funcListUser(char *cmd);

void funcRefreshUser(char *cmd);

void funcSend(char *cmd);

void funcSendAll(char *cmd);

void funcSendFile(char *cmd);

void funcListFile(char *cmd);

void funcAcceptFile(char *cmd);

void funcCancelFile(char *cmd);

void funcPwd(char *cmd);

void funcCd(char *cmd);

void funcDir(char *cmd);

void funcShell(char *cmd);

void funcExit(char *cmd);

void funcHelp(char *cmd);

#endif
