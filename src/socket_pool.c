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
	sockd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockd == -1)
		handle_error("invalid socket");

	if (bind(sockd, (struct sockaddr*)&host_addr, sizeof(host_addr)) == -1)
		handle_error("invalid address"); 

	fprintf(stderr, "server on: %s:%d\n", inet_ntoa(host_addr.sin_addr), ntohs(host_addr.sin_port));

	/*create IPC to watch over threads*/
	int cell[CLIENTS];
	struct thread_routine_info info;
	info.memptr = (void*)cell;
	pthread_t thread;

	for (int i = 0; i < CLIENTS; ++i) {
		cell[i] = 1;
		info.offset = i;
		if (pthread_create(&thread, NULL, &handle_mem_cell, &info) != 0)
			handle_error("invalid thread");
		while (cell[i] != 0) {}
	}

	if (listen(sockd, CLIENTS) == -1) 
		handle_error("listen error");

	while (1) {
		int connection = accept(sockd, 0, 0);
		if (connection == -1)
			handle_error("connection failure");
		for (int i = 0; i < CLIENTS; ++i) {
			if (cell[i] == 0) {
				cell[i] = connection;
				break;
			}
		}
	}

	return 0;
}