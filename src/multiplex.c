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
#include "udp_server.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

#define CLIENTS 10

int main(int argc, char** argv) {
	struct sockaddr_in host_addr;
	memset(&host_addr, 0, sizeof(host_addr));
	host_addr.sin_family = AF_INET;
	int udp_port;

	/*recieve options for server*/
	int opt;
	while ((opt = getopt(argc, argv, "a:p:")) != -1) {
		switch(opt) {
		case 'a':
			if (inet_aton(optarg, &host_addr.sin_addr) == 0) 
				handle_error("invalid address");
			break;
		case 'p':			
			host_addr.sin_port = htons(atoi(optarg)); //current port tcp
			udp_port = atoi(optarg); // next port udp
			++udp_port;
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

	/*binding*/
	int tcp_listen = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_listen == -1)
		handle_error("invalid socket");

	if (bind(tcp_listen, (struct sockaddr*)&host_addr, sizeof(host_addr)) == -1)
		handle_error("invalid address"); 

	fprintf(stderr, "tcp listen server on: %s:%d\n", inet_ntoa(host_addr.sin_addr), ntohs(host_addr.sin_port));
	host_addr.sin_port = htons(udp_port);

	int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_sock == -1)
		handle_error("invalid socket");

	if (bind(udp_sock, (struct sockaddr*)&host_addr, sizeof(host_addr)) == -1)
		handle_error("invalid address"); 

	fprintf(stderr, "udp server on: %s:%d\n", inet_ntoa(host_addr.sin_addr), ntohs(host_addr.sin_port));

	/*select*/
	fd_set set;
	FD_ZERO(&set);
	FD_SET(tcp_listen, &set);
	FD_SET(udp_sock, &set);
	fd_set track;
	memcpy(&track, &set, sizeof(fd_set));

	int result;
	pthread_t thread;
	int tmp;
	listen(tcp_listen, CLIENTS);
	while (1) {
		select(FD_SETSIZE, &set, NULL, NULL, NULL);
		if (FD_ISSET(udp_sock, &set)) {
			pthread_create(&thread, NULL, &udp_routine, &udp_sock);
		}
		if (FD_ISSET(tcp_listen, &set)) {
			tmp = accept(tcp_listen, NULL, NULL);
			pthread_create(&thread, NULL, &tcp_routine, &tmp);
		}
		memcpy(&set, &track, sizeof(fd_set));
	}

	return 0;
}
