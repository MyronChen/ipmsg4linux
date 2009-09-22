#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "sendfile.h"
#include "filelist.h"
#include "util.h"
#include "const.h"

/**
 * get sendfile by message no and attachment no
 */
SendFile* fileListGetSendFileByNo(GList* fileList,long msgNo,long no){
	GList* it;
	for(it= fileList;it != NULL;it = it->next){
		SendFile *sendfile = it->data;
		if(sendfile->msgNo == msgNo && sendfile->no == no){
			return sendfile;
		}
	}
	return NULL;	
}

/**
 * get sendfiles by spec args
 */
GList* fileListGetSendFiles(GList* fileList, char param[256]){
	GList* sendfiles = NULL;
	if(param == NULL){
		return sendfiles;
	}

	if(strcasecmp(param, "all")==0){
		GList* it;
		for(it = fileList;it != NULL;it = it->next){
			SendFile *sendfile = it->data;
			sendfiles = g_list_append(sendfiles, sendfile);
		}
		return sendfiles;
	}

	int i = index(param,(int) ':');

	//param's format is packageNo:fileNo .
	if(i != -1){
		char *strMsgNo = strtok(param, ":");
		char *strFileNo = strtok(NULL, ":");
		long msgNo = atoll(strMsgNo);
		long fileNo = atoll(strFileNo);
		GList* it;
		for(it = fileList;it!=NULL;it = it->next){
			SendFile* sendfile = it->data;
			if(sendfile->msgNo == msgNo && sendfile->no==fileNo){
				sendfiles = g_list_append(sendfiles, sendfile);
			}
		}
	}else{
		long msgNo= atoll(param);
		GList* it;
		for(it=fileList;it!=NULL;it = it->next){
			SendFile* sendfile = it->data;
			if(sendfile->msgNo == msgNo){
				sendfiles =	g_list_append(sendfiles, sendfile);
			}
		}
	}
	return sendfiles;

}
/**
 * if msgNo is -1 ,release all attachments
 */
void fileListReleaseAttachment(GList *fileList,long msgNo){
	GList *it;
	for(it=fileList;it!=NULL;it=it->next){
		SendFile *sendfile = it->data;
		if(RELEASE_ALL==-1){
			it = g_list_remove(it,sendfile);
			FREE(sendfile);
		}else if(msgNo == sendfile->msgNo){
			it = g_list_remove(it,sendfile);
			FREE(sendfile);
		}	
	}
}

void fileListShow(GList *fileList){
	printf("%-14s|%6s| %s\n", "pkgNo:fileNo", "accept", "filename");
	GList *it;
	for(it=fileList;it!=NULL;it=it->next){
		SendFile *sendFile = it->data;
		char buffer[256];
		sendFileToString(sendFile, buffer);
		printf("%s\n", buffer);
	}
}


