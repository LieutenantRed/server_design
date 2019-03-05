#ifndef __UDP_SERVER_H__
#define __UDP_SERVER_H__

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include "orcish.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

#define handle_error(msg) \
    do { fprintf(stderr, msg); exit(EXIT_FAILURE); } while (0)

#define UDP_REQUEST 444

struct thread_routine_info {
	int sockd;
	int msgid;
};

struct msg {
	long type;
	struct sockaddr_in from;
	char msg[BUF_SIZE]; 	
};

void* udp_routine(void* arg);

#endif