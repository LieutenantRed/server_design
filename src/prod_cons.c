#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "orcish.h"
#include "udp_server.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

#define CLIENTS 10

int main(int argc, char** argv) {
	struct sockaddr_in host_addr;
	memset(&host_addr, 0, sizeof(host_addr));
	host_addr.sin_family = AF_INET;

	/*recieve options for server*/
	int opt;
	while ((opt = getopt(argc, argv, "a:p:")) != -1) {
		switch(opt) {
		case 'a':
			if (inet_aton(optarg, &host_addr.sin_addr) == 0) 
				handle_error("invalid address");
			break;
		case 'p':
			host_addr.sin_port = htons(atoi(optarg));
			break;
		default:
			printf("Usage: %s -a [ip address] -p [port]\n", argv[0]);
			exit(EXIT_FAILURE);
		}	
	}

	if ((host_addr.sin_port == 0) || (host_addr.sin_addr.s_addr == 0)) {
		printf("Usage: %s -a [ip address] -p [port]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*create socket*/
	int sockd;
	sockd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockd == -1)
		handle_error("invalid socket");

	if (bind(sockd, (struct sockaddr*)&host_addr, sizeof(host_addr)) == -1)
		handle_error("invalid address"); 

	fprintf(stderr, "server on: %s:%d\n", inet_ntoa(host_addr.sin_addr), ntohs(host_addr.sin_port));

	/*Create IPC*/
	int msgid = msgget(IPC_PRIVATE, 0666);
	if (msgid == -1)
		handle_error("cannot create msgque");
	

	struct thread_routine_info info;
	info.sockd = sockd;
	info.msgid = msgid;

	pthread_t thread;
	for (int i = 0; i < CLIENTS; ++i) {
		if (pthread_create(&thread, NULL, &udp_routine, &info) != 0)
			handle_error("invalid thread");
	}

	struct msg msg_snd;
	memset(&msg_snd, 0, sizeof(msg_snd));
	int len = sizeof(msg_snd.from);
	while(1) {
		msg_snd.type = UDP_REQUEST;
		recvfrom(sockd, msg_snd.msg, BUF_SIZE, 0, (struct sockaddr*)&(msg_snd.from), (socklen_t*)&len);
		msgsnd(msgid, &msg_snd, BUF_SIZE + sizeof(msg_snd.from), 0);
		memset(&msg_snd, 0, sizeof(msg_snd));
	}

	return 0;
}