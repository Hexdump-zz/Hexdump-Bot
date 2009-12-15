#ifndef _IRC_STRUCT_H_
#define _IRC_STRUCT_H_

struct irc_s {
	in_addr_t addr;
	uint16_t port;

	int fd;

	struct kevent *readkev;
	struct kevent *writekev;
};

void *irc_new(in_addr_t addr, uint16_t port);

#endif
