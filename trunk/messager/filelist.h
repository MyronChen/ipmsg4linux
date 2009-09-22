#ifndef _FILE_LIST_
#define _FILE_LIST_

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include "sendfile.h"

#define RELEASE_ALL -1

/**
 * get sendfile by message no and attachment no
 */
SendFile* fileListGetSendFileByNo(GList* fileList,long msgNo,long no);

/**
 * get sendfiles by spec args
 */
GList* fileListGetSendFiles(GList* fileList, char param[256]);

/**
 * if msgNo is -1 ,release all attachments
 */
void fileListReleaseAttachment(GList *fileList,long msgNo);

void fileListShow(GList *fileList);

#endif
