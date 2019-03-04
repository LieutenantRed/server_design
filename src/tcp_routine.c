#include "tcp_server.h"

void* tcp_routine(void* arg) {
	int sock = *((int*)arg);
	char buf[BUF_SIZE];
	
	while (1) {
		memset(&buf, 0, BUF_SIZE * sizeof(char));
		recv(sock, buf, BUF_SIZE, 0);
		if (strcmp(buf, "exit\n") == 0) {
			break;
		}
		into_orcish(buf);
		send(sock, buf, BUF_SIZE, 0);
	}
	close(sock);
	return 0;
}