#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "util.h"
#include "const.h"

int getLocalIp(char *ip){
	int sockfd;
	if(-1==(sockfd=socket(PF_INET,SOCK_STREAM,0))){
		ERROR("socket");
		return -1;
	}

	struct ifreq req;
	struct sockaddr_in *host;
	bzero(&req,sizeof(struct ifreq));
	strcpy(req.ifr_name,"eth0");
	ioctl(sockfd,SIOCGIFADDR,&req);
	host = (struct sockaddr_in*)&req.ifr_addr;
	strcpy(ip,inet_ntoa(host->sin_addr));
	close(sockfd);
	return 1;
}
/**
 * message not is end with \0
 *
 */
int udpSend(int socketDescriptor, char ip[256],int port, char message[MSG_SIZE],int msgSize){
	struct sockaddr_in address;

	//Initialize sokcet address structure for Internet Protocols
	bzero(&address, sizeof(address)); 
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip);
	address.sin_port = htons(port);

	if(strcmp(ip, "255.255.255.255")==0){
		int broadcast = 1;
		if(setsockopt(socketDescriptor,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof(broadcast))==-1){
			ERROR("setsockopt failure!");
			return -1;
		}
	}

	sendto(socketDescriptor, message, msgSize,
				0, (struct sockaddr *)&address, sizeof(address));
	
	if(strcmp(ip, "255.255.255.255")==0){
		int broadcast = 0;
		if(setsockopt(socketDescriptor,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof(broadcast))==-1){
			ERROR("setsockopt failure!");
			return -1;
		}
	}
	
	return 0;
}



char* strTrim(char *str)
{
	char *tail, *head;
	for(tail = str + strlen(str)-1;tail >=str;tail--){
		if(!ISSAPCE(*tail)){
			break;
		}
	}
	tail[1]=0;
	for(head = str; head <=tail;head++){
		if(!ISSAPCE(*head)){
			break;
		}
	}
	if(head != str){
		memcpy(str,head,(tail-head+2)*sizeof(char));
	}

	return str;
}



int startsWith(char *str, char *prefix){
	return !strncasecmp(str,prefix, strlen(prefix));
}

int argsParse(char *cmdstring, char *pattern,int limit, char arr[][1024])
{
	int index = 0;
	char *p = NULL;
	char *last = NULL;
	if(limit == 1){
		strcpy(arr[index++],cmdstring);
	}else{
		for(index;index < limit-1;){
			if(index == 0){
				p = strtok_r(cmdstring,pattern,&last);
			}else{
				p = strtok_r(NULL,pattern,&last);
			}
			if(p == NULL){
				break;
			}
			strcpy(arr[index++],p);
		}
		if(last != NULL && strcmp(last, "")!= 0){
			strcpy(arr[index++],last);
		}
	}

	return index;
}

int exists(char* filename){
	int file = open(filename, O_RDONLY);
	if(file==-1){
		return 0;
	}else{
		close(file);
	}
	return 1;	
}
