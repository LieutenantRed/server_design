#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "orcish.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

#define handle_error(msg) \
    do { fprintf(stderr, msg); exit(EXIT_FAILURE); } while (0)

void* tcp_routine(void* arg);
//void* tcp_fullc(void* arg);


#endif