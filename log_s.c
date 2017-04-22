#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>
#include <arpa/inet.h>
#include <time.h>

int startLogServer(int portno){
	int sockfd;
	struct sockaddr_in serv_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) error("ERROR OPENING UDP SOCKET FOR LOG SERVER");
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if(bind(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	  error("ERROR BINDIN UDP FOR LOG SERVER");
	
	udp_loop(sockfd);
	
	close(sockfd);
}

void *SigCatcher(int n)
{
  wait3(NULL,WNOHANG,NULL);
}

void udp_loop(int udp_sockfd)
{
      int n;
      struct sockaddr_in from;
      socklen_t fromlen;
      char buf[1024];
      fromlen = sizeof(struct sockaddr_in);
      while(1)
      {
	   n = recvfrom(udp_sockfd,buf,1024,0,(struct sockaddr *)&from,&fromlen);
	   if(n<0) error("ERROR recvfrom");
	   write(1,buf,n);
	   writetofile(buf);
	   if(n<0) error("ERROR sendto");
		     signal(SIGCHLD,SigCatcher);
      }
}

void writetofile(char buf[1024])
{
	FILE *fw;
	fw = fopen("echo.log", "a");
	time_t ti = time(NULL);
	struct tm t = *localtime(&ti);
	fprintf(fw,"%d-%d-%d %d:%d:%d\t\"%s\n", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, buf);
	fclose(fw);
}

