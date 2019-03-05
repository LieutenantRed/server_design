#include "udp_server.h"

void* msg_udp_routine(void* arg) {
	struct udpthread_routine_info info;
	memset(&info, 0, sizeof(struct udpthread_routine_info));

	memcpy(&info, arg, sizeof(info));

	struct msg msg_rec;
	memset(&msg_rec, 0, sizeof(msg_rec));

	while (1) {
		int len = msgrcv(info.msgid, &msg_rec, sizeof(msg_rec.from) + BUF_SIZE, UDP_REQUEST, 0);
		into_orcish(msg_rec.msg);
		sendto(info.sockd, msg_rec.msg, len, 0, (struct sockaddr*)&msg_rec.from, sizeof(msg_rec.from));
	}
	return 0;
}


void* udp_routine(void* arg) {
	int sockd = *(int*)arg;
	struct sockaddr_in peer_addr;
	char buf[BUF_SIZE];

	while (1) {
		memset(&buf, 0, BUF_SIZE * sizeof(char));
		socklen_t len = sizeof(peer_addr);
		int l = recvfrom(sockd, buf, BUF_SIZE, 0, (struct sockaddr*)&peer_addr, &len);
		if (strcmp(buf, "exit\n") == 0) {
			break;
		}
		into_orcish(buf);
		if (sendto(sockd, buf, BUF_SIZE, 0, (struct sockaddr*)&peer_addr, len) != l)
			fprintf(stderr, "error sending mess");
	}
	return NULL;	
}