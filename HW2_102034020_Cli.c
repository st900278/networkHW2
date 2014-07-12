#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/select.h>
#include<netinet/in.h>
#include<time.h>
#include<arpa/inet.h>
#define STDIN 0
int main(int argc, char* argv[]){
	struct sockaddr_in dest;
	int destfd, n, nready;
	destfd = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&dest, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(strtol(argv[2], NULL, 10));
	dest.sin_addr.s_addr = inet_addr(argv[1]);
	char msg[1000000];
	fd_set rset, allset;
	FD_ZERO(&allset);
	FD_SET(STDIN, &allset);
	FD_SET(destfd, &allset);
	int maxfd = STDIN > destfd ? STDIN : destfd;
	int flag = 0;
	while(1){
		memset(msg, 0, sizeof(msg));
		rset = allset;
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		if(FD_ISSET(destfd,  &rset)){
			n = recvfrom(destfd, msg, 1000000, 0, NULL, NULL);
			msg[n] = '\0';
			msg[n-1] = '\0';
			puts(msg);
			
		}
		if(FD_ISSET(STDIN, &rset)){
			gets(msg);	
			sendto(destfd, msg, strlen(msg), 0, (struct sockaddr*)&dest, sizeof(dest));
			while(1){
				clock_t start_time = clock();
				clock_t end_time = 3 * 1000 + start_time;
				while(clock() < end_time ){
					if(recvfrom(destfd, msg, 100000, 0, NULL, NULL))
						goto S1;
				}
				sendto(destfd, msg, strlen(msg), 0, (struct sockaddr*)&dest, sizeof(dest));
			}
			S1:;
		}
		
		
	}
	
	
	
	
}
