#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/select.h>
 struct login{
	char name[1000];
	struct sockaddr_in socketaddr;
	int log;
	char roomIn[1024];
 };
struct members{
	char name[1024];
	struct sockaddr_in addr;
	int used;
};
struct room{
	char name[1000];
	struct members member[100];
	int alive;
};
int main(int argc, char* argv[]){
	struct sockaddr_in serveraddr;
	struct sockaddr_in cliaddr;
	int sockfd;
	int i, j;
	struct login list[1000];
	struct room chatroom[40];
	int ptroom =0;
	int ptr=0;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_port = htons(strtol(argv[1], NULL, 10));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	bind(sockfd, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
	
	int n;
	socklen_t length;
	char msgg[2048];
	char msg[10240];
	while(1){
		length = sizeof(cliaddr);
		memset(msg, 0, sizeof(msg));
		memset(msgg, 0, sizeof(msgg));
		n = recvfrom(sockfd, msgg, 2048, 0, (struct sockaddr* )&cliaddr, &length);
		
		strcpy(msg, "GET\n");
		sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
		
		memset(msg, 0, sizeof(msg));
		
		char msgCpy[2048];
		strcpy(msgCpy, msgg);
		char *s = strtok(msgg, " ");
		char tmp[2048];
		strcpy(tmp, s);
		if(!strcmp(tmp, "NA")){
			s = strtok(NULL, " ");
			char name[2048];
			char password[2048];
			char fileName[2048];
			strcpy(name, s);
			strcpy(fileName,name);
			strcat(fileName, ".txt");
			s = strtok(NULL, " ");
			strcpy(password, s);
			FILE * fp = fopen(fileName,"w");
			fputs(name, fp);
			fputs("\n", fp);
			fputs(password, fp);
			strcpy(fileName, name);
			strcat(fileName, "-friend.txt");
			FILE * ff = fopen(fileName,"w");
			strcpy(fileName, name);
			strcat(fileName, "-blacklist.txt");
			FILE* fb = fopen(fileName, "w");
			strcpy(fileName, name);
			strcat(fileName, "-message.txt");
			FILE* fm = fopen(fileName, "w");
			fclose(fp);
			fclose(ff);
			fclose(fb);
			fclose(fm);
			printf("%s is created\n", name);
			
			strcpy(msg, "Account create successfully\n");
			sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
			continue;
			
		}
		else if(!strcmp(tmp, "DA")){
			s = strtok(NULL, " ");
			char name[2048];
			char password[2048];
			char fileName[2048];
			char buf[2048];
			char msg[1024];
			strcpy(name, s);
			strcpy(fileName,name);
			strcat(fileName, ".txt");
			s = strtok(NULL, " ");
			strcpy(password, s);
			FILE * fp = fopen(fileName,"r");
			if(fp == NULL){
				strcpy(msg, "File Not existing.\n");
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				continue;
			}
			fgets(buf, 2048, fp);
			fgets(buf, 2048, fp);
			if(!strcmp(password, buf)){
				fclose(fp);
				remove(fileName);
				printf("Remove %s\n", name);
				strcpy(fileName, name);
				strcat(fileName, "-friend.txt");
				remove(fileName);
				printf("Remove %s friend list\n", name);
				strcpy(fileName, name);
				strcat(fileName, "-blacklist.txt");
				remove(fileName);
				printf("Remove %s black list\n", name);
				strcat(fileName, "-message.txt");
				remove(fileName);
				printf("Remove %s message\n", name);
				strcpy(msg, "Account delete successfully\n");
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				printf("%s is deleted\n", name);
				continue;
				
			}
			else{
				fclose(fp);
				strcpy(msg, "No permission.\n");
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				continue;
			}
		}
		else if(!strcmp(tmp, "I")){
			s = strtok(NULL, " ");
			char name[2048];
			char password[2048];
			char fileName[2048];
			char buf[2048];
			strcpy(name, s);
			strcpy(fileName,name);
			strcat(fileName, ".txt");
			s = strtok(NULL, " ");
			strcpy(password, s);
			FILE* fp = fopen(fileName, "r");
			if(fp == NULL){
				strcpy(msg, "File Not existing.\n");
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				continue;
			}
			fgets(buf, 2048, fp);
			fgets(buf, 2048, fp);
			if(!strcmp(password, buf)){
				for(i=0;i<ptr;i++){
					if(!strcmp(list[i].name,name)){
						list[i].log = 1;
						list[i].socketaddr = cliaddr;
						printf("2 %s login\n", list[i].name);
						goto S1;
					}
				}
				strcpy(list[ptr].name,name);
				list[ptr].log = 1;
				list[ptr].socketaddr = cliaddr;
				printf("%s login ip %s port %d\n", list[ptr].name, inet_ntoa(list[ptr].socketaddr.sin_addr), (int)htons(list[ptr].socketaddr.sin_port));
				
				ptr++;
				S1:;
				fclose(fp);
				strcpy(fileName, name);
				strcat(fileName, "-message.txt");
				fp = fopen(fileName, "r");
				while(fgets(msg, 1000, fp)!=NULL){
					sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				}
				fclose(fp);
				remove(fileName);
				fp = fopen(fileName, "w");
				fclose(fp);
				strcpy(msg, "Login\n");
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				sprintf(msg, "Room List:\n");
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				for(i=0;i<40;i++){
					if(chatroom[i].alive){
						sprintf(msg, "%s\n", chatroom[i].name);
						sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
					}
				}
				continue;
			}
			else{
				fclose(fp);
				strcpy(msg, "No permission.\n");
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				continue;
			}
		}
		else if(!strcmp(tmp, "O")){
			s = strtok(NULL, " ");
			char name[2048];
			char password[2048];
			char fileName[2048];
			char buf[2048];
			strcpy(name, s);
			strcpy(fileName,name);
			strcat(fileName, ".txt");
			s = strtok(NULL, " ");
			strcpy(password, s);
			FILE* fp = fopen(fileName, "r");
			if(fp == NULL){
				strcpy(msg, "File Not existing.\n");
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				continue;
			}
			fgets(buf, 2048, fp);
			fgets(buf, 2048, fp);
			char roomName[1024];
			if(!strcmp(password, buf)){
				fclose(fp);
				for(i=0;i<ptr;i++){
					if(!strcmp(inet_ntoa(list[i].socketaddr.sin_addr),inet_ntoa(cliaddr.sin_addr)) && htons(list[i].socketaddr.sin_port) == htons(cliaddr.sin_port) && list[i].log == 1){
						memset(list[i].roomIn, 0, sizeof(list[i].roomIn));
						list[i].log = 0;
					}
				}
				strcpy(msg, "Logout\n");
				printf("%s logout.\n", name);
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				continue;
			}
			else{
				fclose(fp);
				strcpy(msg, "No permission.\n");
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				continue;
			}
		}
		else if(!strcmp(tmp, "NF")){
			s = strtok(NULL, " ");
			char nowUser[1024];
			for(i=0;i<ptr;i++){
				if(!strcmp(inet_ntoa(list[i].socketaddr.sin_addr),inet_ntoa(cliaddr.sin_addr)) && htons(list[i].socketaddr.sin_port) == htons(cliaddr.sin_port) && list[i].log == 1){
					strcpy(nowUser, list[i].name);
					break;
				}
			}
			char fileName[1024];
			strcpy(fileName, s);
			strcat(fileName, "-friend.txt");
			FILE* fp = fopen(fileName, "a");
			if(fp == NULL){sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				sprintf(msg, "%s is not exist\n", s);
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				continue;
			}
			char addFriend[1024];
			printf("%s\n", nowUser);
			strcpy(addFriend, nowUser);
			fputs(addFriend, fp);
			fputs("\n", fp);
			fclose(fp);
			strcpy(fileName, nowUser);
			strcat(fileName, "-friend.txt");
			fp = fopen(fileName, "a");
			strcpy(addFriend, s);
			fputs(addFriend, fp);
			fputs("\n", fp);
			fclose(fp);
			printf("%s and %s become friends\n", nowUser, addFriend); 
			sprintf(msg, "Add %s as friend successfully\n", addFriend);
			sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
			continue;
		}
		else if(!strcmp(tmp, "DF")){
			s = strtok(NULL, " ");
			char nowUser[1024];
			for(i=0;i<ptr;i++){
				if(!strcmp(inet_ntoa(list[i].socketaddr.sin_addr),inet_ntoa(cliaddr.sin_addr)) && htons(list[i].socketaddr.sin_port) == htons(cliaddr.sin_port) && list[i].log == 1){
					strcpy(nowUser, list[i].name);
					break;
				}
			}
			char deleteFriend[1024];
			strcpy(deleteFriend, s);
			char fileName[1024];
			strcpy(fileName, nowUser);
			strcat(fileName, "-friend.txt");
			FILE* fp = fopen(fileName, "r");
			printf("delete %s\n", fileName);
			char tmp2[40][4096];
			int ptmp=0;
			char tmp1[1024];
			while(fgets(tmp1, 1024, fp)!=NULL){
				if(tmp1[strlen(tmp1)-1] == '\n')
					tmp1[strlen(tmp1)-1] = '\0';
				if(!strcmp(tmp1, ""))continue;
				if(!strcmp(tmp1, deleteFriend)){
					continue;
				}
				strcpy(tmp2[ptmp++], tmp1);
			}
			fclose(fp);                               
			remove(fileName);
			fp = fopen(fileName, "w");
			for(i=0;i<ptmp;i++){
				fputs(tmp2[i], fp);
				fputs("\n", fp);
			}
			fclose(fp);
			printf("%s remove %s from friend list\n", nowUser, deleteFriend);
			sprintf(msg, "Delete friend %s\n", deleteFriend);
			sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
			continue;
		}
		else if(!strcmp(tmp, "NB")){
			s = strtok(NULL, " ");
			char nowUser[1024];
			for(i=0;i<ptr;i++){
				if(!strcmp(inet_ntoa(list[i].socketaddr.sin_addr),inet_ntoa(cliaddr.sin_addr)) && htons(list[i].socketaddr.sin_port) == htons(cliaddr.sin_port) && list[i].log == 1){
					strcpy(nowUser, list[i].name);
					break;
				}
			}
			char fileName[1024];
			strcpy(fileName, nowUser);
			strcat(fileName, "-blacklist.txt");
			FILE* fp = fopen(fileName, "a");
			char addBlack[1024];
			strcpy(addBlack, s);
			fputs(addBlack, fp);
			fputs("\n", fp);
			fclose(fp);
			printf("%s adds %s to blacklist\n", nowUser, addBlack); 
			sprintf(msg, "Add %s to blacklist successfully\n", addBlack);
			sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
			continue;
		}
		else if(!strcmp(tmp, "DB")){
			s = strtok(NULL, " ");
			char nowUser[1024];
			for(i=0;i<ptr;i++){
				if(!strcmp(inet_ntoa(list[i].socketaddr.sin_addr),inet_ntoa(cliaddr.sin_addr)) && htons(list[i].socketaddr.sin_port) == htons(cliaddr.sin_port) && list[i].log == 1){
					strcpy(nowUser, list[i].name);
					break;
				}
			}
			char deleteBlack[1024];
			strcpy(deleteBlack, s);
			char fileName[1024];
			strcpy(fileName, nowUser);
			strcat(fileName, "-blacklist.txt");
			FILE* fp = fopen(fileName, "r");
			char tmp2[40][4096];
			int ptmp=0;
			char tmp1[1024];
			while(fgets(tmp1, 1024, fp)!=NULL){
				tmp1[strlen(tmp1)-1] = '\0';
				if(!strcmp(tmp1, ""))continue;
				if(!strcmp(tmp1, deleteBlack)){
					continue;
				}
				strcpy(tmp2[ptmp++], tmp1);
			}
			fclose(fp);                               
			remove(fileName);
			fp = fopen(fileName, "w");
			for(i=0;i<ptmp;i++){
				fputs(tmp2[i], fp);
				fputs("\n", fp);
			}
			fclose(fp);
			printf("%s removed %s from black list\n", nowUser, deleteBlack);
			sprintf(msg, "Delete %s from blacklist\n", deleteBlack);
			sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
			continue;
		}
		else if(!strcmp(tmp, "J")){
			s = strtok(NULL, " ");
			char nowUser[1024];
			int userId;
			for(i=0;i<ptr;i++){
				if(!strcmp(inet_ntoa(list[i].socketaddr.sin_addr),inet_ntoa(cliaddr.sin_addr)) && htons(list[i].socketaddr.sin_port) == htons(cliaddr.sin_port) && list[i].log == 1){
					strcpy(nowUser, list[i].name);
					userId = i;
					break;
				}
			}
			int roomNum;
			for(i=0;i<40;i++){
					if(!strcmp(chatroom[i].name, s)){
						roomNum = i;
						goto S4;
					}
			}
			strcpy(msg, "Can't find the room\n");
			sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
			continue;
			S4:;
			for(i=0;i<100;i++){
				if(chatroom[roomNum].member[i].used){
					sprintf(msg, "%s [%s] [%d]\n", chatroom[roomNum].member[i].name,  inet_ntoa(chatroom[roomNum].member[i].addr.sin_addr), htons(chatroom[roomNum].member[i].addr.sin_port));
					sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				}
			}
			for(j=0;j<100;j++){
				if(!chatroom[roomNum].member[j].used){
					strcpy(chatroom[roomNum].member[j].name,nowUser);
					chatroom[roomNum].member[j].used = 1;
					chatroom[roomNum].member[j].addr = cliaddr;
					strcpy(list[userId].roomIn, s);
					break;
				}
			}
			printf("%s joins [%s]\n", nowUser, chatroom[roomNum].name);
			sprintf(msg, "%s joins the room.[%s] [%d]\n", nowUser, inet_ntoa(cliaddr.sin_addr), htons(cliaddr.sin_port));
			for(j=0;j<100;j++){
				if(chatroom[roomNum].member[j].used){
					//printf("Send to %s\n", chatroom[roomNum].member[j].name);
					sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&chatroom[roomNum].member[j].addr, sizeof(chatroom[roomNum].member[j].addr));
				}
			}
			
			continue;
			
		}
		else if(!strcmp(tmp, "L")){
			char nowUser[1024];
			char roomName[1024];
			for(i=0;i<ptr;i++){
				if(!strcmp(inet_ntoa(list[i].socketaddr.sin_addr),inet_ntoa(cliaddr.sin_addr)) && htons(list[i].socketaddr.sin_port) == htons(cliaddr.sin_port) && list[i].log == 1){
					strcpy(nowUser, list[i].name);
					strcpy(roomName, list[i].roomIn);
					memset(list[i].roomIn, 0, sizeof(list[i].roomIn));
					break;
				}
			}
			int roomNum;
			for(i=0;i<40;i++){
				if(!strcmp(chatroom[i].name, roomName)){
					roomNum = i;
					break;
				}
			}
			
			
			
			for(i =0;i<100;i++){
				if(!strcmp(chatroom[roomNum].member[i].name, nowUser)){
					chatroom[roomNum].member[i].used = 0;
					break;
				}
			}
			int flag = 0;

			printf("%s leaves the room\n", nowUser);
			sprintf(msg, "%s leaves the room.\n", nowUser);
			for(j=0;j<100;j++){
				if(chatroom[roomNum].member[j].used){
					flag = 1;
					sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&chatroom[roomNum].member[j].addr, sizeof(chatroom[roomNum].member[j].addr));
				}
			}
			if(!flag){
				chatroom[roomNum].alive = 0;
			}
			sprintf(msg, "Room List:\n");
			sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
			for(i=0;i<40;i++){
				if(chatroom[i].alive){
					sprintf(msg, "%s\n", chatroom[i].name);
					sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				}
			}
			continue;

		}
		else if(!strcmp(tmp, "NR")){
			s = strtok(NULL, " ");
			char nowUser[1024];
			int userId;
			for(i=0;i<ptr;i++){
				if(!strcmp(inet_ntoa(list[i].socketaddr.sin_addr),inet_ntoa(cliaddr.sin_addr)) && htons(list[i].socketaddr.sin_port) == htons(cliaddr.sin_port) && list[i].log == 1){
					strcpy(nowUser, list[i].name);
					userId = i;
					break;
				}
			}
			int roomNum;
			for(i=0;i<40;i++){
				if(!chatroom[i].alive){
					chatroom[i].alive = 1;
					strcpy(chatroom[i].name, s);
					strcpy(chatroom[i].member[0].name, nowUser);
					chatroom[i].member[0].addr = cliaddr;
					chatroom[i].member[0].used = 1;
					strcpy(list[userId].roomIn, s);
					roomNum = i;
					break;
				}
			}
			printf("%s created [%s]\n", nowUser, chatroom[roomNum].name);
			strcpy(msg, "Create a room\n");
			sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
		}
		else if(!strcmp(tmp, "S")){
			s = strtok(NULL, " ");
			char nowUser[1024];
			char roomName[1024];
			for(i=0;i<ptr;i++){
				if(!strcmp(inet_ntoa(list[i].socketaddr.sin_addr),inet_ntoa(cliaddr.sin_addr)) && htons(list[i].socketaddr.sin_port) == htons(cliaddr.sin_port) && list[i].log == 1){
					strcpy(nowUser, list[i].name);
					strcpy(roomName, list[i].roomIn);
					break;
				}
			}
			int roomNum;
			for(i=0;i<40;i++){
				if(!strcmp(chatroom[i].name, roomName)){
					roomNum = i;
					break;
				}
			}
			printf("%s says: %s\n in [%s]", nowUser, s, chatroom[roomNum].name);
			sprintf(msg, "%s says: %s [in room]\n", nowUser, s);
			for(i=0;i<100;i++){
				if(chatroom[roomNum].member[i].used){
					//printf("send to %s\n", chatroom[roomNum].member[i].name);
					sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&chatroom[roomNum].member[i].addr, sizeof(chatroom[roomNum].member[i].addr));
				}
			}
		}
		else if(!strcmp(tmp, "PM")){
			s = strtok(NULL, " ");
			char nowUser[1024];
			for(i=0;i<ptr;i++){
				if(!strcmp(inet_ntoa(list[i].socketaddr.sin_addr),inet_ntoa(cliaddr.sin_addr)) && htons(list[i].socketaddr.sin_port) == htons(cliaddr.sin_port) && list[i].log == 1){
					strcpy(nowUser, list[i].name);
					break;
				}
			}
			char dest[1024];
			strcpy(dest, s);
			s = strtok(NULL, " ");
			
			char fileName[1024];
			strcpy(fileName,dest);
			strcat(fileName, "-friend.txt");
			FILE* fp = fopen(fileName, "r");
			printf("%s\n", fileName);
			int flag = 0;
			char tmp1[1024];
			while(fgets(tmp1, 1024, fp)!=NULL){
				if(tmp1[strlen(tmp1)-1] == '\n')
					tmp1[strlen(tmp1)-1] = '\0';
				if(!strcmp(tmp1, ""))continue;
				if(!strcmp(tmp1, nowUser)){
					flag = 1;
					break;
				}
			}
			fclose(fp);
			if(!flag){
				sprintf(msg, "You are not in his/her friendlist\n");
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				continue;
			}
			sprintf(msg, "%s says: %s\n", nowUser, s);
			for(i=0;i<ptr;i++){
				if(!strcmp(list[i].name, dest) && list[i].log == 1){
					sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&list[i].socketaddr, sizeof(list[i].socketaddr));
					sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
					goto S5;
				}
			}
			char tmp2[1024];
			strcpy(tmp2, dest);
			strcat(tmp2, "-message.txt");
			fp = fopen(tmp2, "a");
			fputs(msg, fp);
			fclose(fp);
			sprintf(msg, "User will see the message when he/she login\n");
			sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
			S5:;
		}
		else if(!strcmp(tmp, "Q")){
			s = strtok(NULL, " ");
			char nowUser[1024];
			for(i=0;i<ptr;i++){
				if(!strcmp(inet_ntoa(list[i].socketaddr.sin_addr),inet_ntoa(cliaddr.sin_addr)) && htons(list[i].socketaddr.sin_port) == htons(cliaddr.sin_port) && list[i].log == 1){
					strcpy(nowUser, list[i].name);
					break;
				}
			}
			char fileName[1024];
			strcpy(fileName, s);
			strcat(fileName, "-blacklist.txt");
			FILE *fp = fopen(fileName, "r");
			if(fp == NULL){
				sprintf(msg, "%s isn't exist\n", s);
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				goto S3;
			}
			int flag = 0;
			
			char tmp1[1024];
			while(fgets(tmp1, 1024, fp)!=NULL){
				if(tmp1[strlen(tmp1)-1] == '\n')
					tmp1[strlen(tmp1)-1] = '\0';
				if(!strcmp(tmp1, ""))continue;
				if(!strcmp(tmp1, nowUser)){
					flag = 1;
					break;
				}
			}
			fclose(fp);
			if(flag){
				sprintf(msg, "%s is offline.\n", s);
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				goto S3;
			}
			for(i=0;i<ptr;i++){
				if(!strcmp(s, list[i].name) && list[i].log == 1){
					sprintf(msg, "%s is online.\n", s);
					sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
					goto S3;
				}
			}
			sprintf(msg, "%s is offline.\n", s);
			sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
			goto S3;
			S3:;
		}
		else if(!strcmp(tmp, "SF")){
			char nowUser[1024];
			for(i=0;i<ptr;i++){
				if(!strcmp(inet_ntoa(list[i].socketaddr.sin_addr),inet_ntoa(cliaddr.sin_addr)) && htons(list[i].socketaddr.sin_port) == htons(cliaddr.sin_port) && list[i].log == 1){
					strcpy(nowUser, list[i].name);
					break;
				}
			}
			char fileName[1024];
			strcpy(fileName, nowUser);
			strcat(fileName, "-friend.txt");
			printf("Search %s\n", fileName);
			FILE* fp = fopen(fileName, "r");
			char tmp2[40][4096];
			int ptmp=0;
			char tmp1[1024];
			while(fgets(tmp1, 1024, fp)!=NULL){
				if(tmp1[strlen(tmp1)-1] == '\n')
					tmp1[strlen(tmp1)-1] = '\0';
				if(!strcmp(tmp1, ""))continue;
				strcpy(tmp2[ptmp++], tmp1);
			}
			fclose(fp);                               
			for(i=0;i<ptmp;i++){
				sprintf(msg, "%s\n",tmp2[i]);
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
			}
			continue;
		}
		else if(!strcmp(tmp, "SB")){
			char nowUser[1024];
			for(i=0;i<ptr;i++){
				if(!strcmp(inet_ntoa(list[i].socketaddr.sin_addr),inet_ntoa(cliaddr.sin_addr)) && htons(list[i].socketaddr.sin_port) == htons(cliaddr.sin_port) && list[i].log == 1){
					strcpy(nowUser, list[i].name);
					break;
				}
			}
			char fileName[1024];
			strcpy(fileName, nowUser);
			strcat(fileName, "-blacklist.txt");
			FILE* fp = fopen(fileName, "r");
			char tmp2[40][4096];
			int ptmp=0;
			char tmp1[1024];
			while(fgets(tmp1, 1024, fp)!=NULL){
				if(tmp1[strlen(tmp1)-1] == '\n')
					tmp1[strlen(tmp1)-1] = '\0';
				if(!strcmp(tmp1, ""))continue;
				strcpy(tmp2[ptmp++], tmp1);
			}
			fclose(fp);                               
			for(i=0;i<ptmp;i++){
				sprintf(msg, "%s\n",tmp2[i]);
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
			}
			continue;
		}
		else if(!strcmp(tmp, "ME")){
			char nowUser[1024];
			for(i=0;i<ptr;i++){
				printf("%s ip %s port %d\n", list[i].name, inet_ntoa(list[i].socketaddr.sin_addr), (int)htons(list[i].socketaddr.sin_port));
				if(!strcmp(inet_ntoa(list[i].socketaddr.sin_addr),inet_ntoa(cliaddr.sin_addr)) && htons(list[i].socketaddr.sin_port) == htons(cliaddr.sin_port) && list[i].log == 1){
					strcpy(nowUser, list[i].name);
					break;
				}
			}
			sprintf(msg, "%s\n", nowUser);
			sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
		}
		else if(!strcmp(tmp, "SR")){
			sprintf(msg, "Room List:\n");
			sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
			for(i=0;i<40;i++){
				if(chatroom[i].alive){
					sprintf(msg, "%s\n", chatroom[i].name);
					sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				}
			}
		}
		else if(!strcmp(tmp, "H")){
			sprintf(msg,"Create new account\n[NA] [User ID] [Password], ex: NA np2014 npnp2014\nDelete existing account\n[DA] [User ID] [Password]\nLog in\n[I] [User ID] [Password]\nLog out\n[O] [User ID] [Password]\nJoin a room\n[J] [Room ID]\nLeave the room\n[L]\nCreate a new room\n[NR] [Room ID]\nSend message in the room\n[S] [Message]\nSend private message to a friend\n[PM] [Friend ID] [Message]\nAdd an ID to friendlist\n[NF] [User ID]\nDelete an ID from friendlist\n[DF] [User ID]\nAdd an ID to blacklist\n[NB] [User ID]\nDelete an ID from blacklist\n[DB] [User ID]\nQuery user status\n[Q] [User ID]\nShow Friend list\n[SF]\nShow Black list\n[SB]\nShow room list\n[SR]\n");
			sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
		}
	}	
	
	
	
	
}
