#include "udp_server.h"

void* udp_routine(void* arg) {
	struct thread_routine_info info;
	memset(&info, 0, sizeof(struct thread_routine_info));

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