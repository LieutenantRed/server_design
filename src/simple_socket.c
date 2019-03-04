#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "orcish.h"
#include "tcp_server.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

#define CLIENTS 200

int main(int argc, char** argv) {
	struct sockaddr_in host_addr;
	memset(&host_addr, 0, sizeof(host_addr));
	host_addr.sin_family = AF_INET;

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

	int sockd;
	sockd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockd == -1)
		handle_error("invalid socket");

	if (bind(sockd, (struct sockaddr*)&host_addr, sizeof(host_addr)) == -1)
		handle_error("invalid address"); 

	fprintf(stderr, "server on: %s:%d\n", inet_ntoa(host_addr.sin_addr), ntohs(host_addr.sin_port));

	if (listen(sockd, CLIENTS) == -1) 
		handle_error("listen error");

	pthread_t thread;
	
	while (1) {
		int connection = accept(sockd, NULL, NULL);
		if (connection == -1)
			handle_error("connection failure");
		thread = 0;
		if (pthread_create(&thread, NULL, &tcp_routine, &connection) != 0)
			handle_error("invalid thread");
		else 
			pthread_detach(thread);
	}

	return 0;
}