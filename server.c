#include "server_functions.c"
#include "log_s.c"
#define LOGPORT 9998

int echoResult_tcp(char buf[256], int sockfd, struct sockaddr_in response) {
	int sockfd_log;
	struct sockaddr_in log_addr;
	setupLogServer(&sockfd_log, &log_addr, LOGPORT);
	char loginfo[256] = {0};

	printf("Received via TCP: %s", buf);
	while (1) {
		bzero(loginfo, 256);
		strcpy(loginfo, "\"");
		strncpy(loginfo, buf, strlen(buf) - 1);
		strcat(loginfo, "\" recieved from ");
		strcat(loginfo, inet_ntoa(response.sin_addr));
		printf("Sending message back and logging...\n");
		if (sendto(sockfd_log, loginfo, strlen(loginfo), 0, (struct sockaddr*)&log_addr, sizeof(struct sockaddr_in)) < 0)
			error("ERROR sendto");
		if (send(sockfd, buf, strlen(buf), 0) < 0)
			error("ERROR send");
		bzero(buf, 256);
		int response = recv(sockfd, buf, 256, 0);
		if (response < 0)
			error("ERROR recv");
		else if (response == 0)
			break;
		printf("Received via TCP: %s", buf);
	}
	close(sockfd_log);
	return 0;
}

int echoResult_udp(char buf[256], int sockfd, struct sockaddr_in response) {
	int sockfd_log;
	struct sockaddr_in log_addr;
	setupLogServer(&sockfd_log, &log_addr, LOGPORT);

	char loginfo[256] = {0};
	socklen_t clilen = sizeof(struct sockaddr_in);

	printf("Received via UDP: %s", buf);
	while (1) {
		bzero(loginfo, 256);
		strcpy(loginfo, "\"");
		strncpy(loginfo, buf, strlen(buf) - 1);
		strcat(loginfo, "\" recieved from ");
		strcat(loginfo, inet_ntoa(response.sin_addr));
		printf("Sending message back and logging...\n");
		if (sendto(sockfd_log, loginfo, strlen(loginfo), 0, (struct sockaddr*)&log_addr, clilen) < 0)
			error("ERROR sendto");
		if (sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&response, clilen) < 0)
			error("ERROR sendto");
		bzero(buf, 256);
		if (recvfrom(sockfd, buf, 256, 0, (struct sockaddr*)&response, &clilen) < 0)
			error("ERROR recvfrom");
		printf("Recieved via UDP: %s", buf);
	}
	close(sockfd_log);
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 4) {
		error("Please provide 3 ports to monitor like so: ./server port1 port2 port3 (eg. ./server 1000, 1001, 1002)");
		return 1;
	}
	if (fork() == 0)
		startLogServer(LOGPORT);
	else if (fork() == 0)
		startServer(atoi(argv[1]), echoResult_tcp, echoResult_udp);
	else if (fork() == 0)
		startServer(atoi(argv[2]), echoResult_tcp, echoResult_udp);
	else
		startServer(atoi(argv[3]), echoResult_tcp, echoResult_udp);
    return 0; 
}
