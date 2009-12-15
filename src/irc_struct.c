#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/event.h>
#include "irc_struct.h"

void
*irc_new(in_addr_t addr, uint16_t port)
{
	struct irc_s *irc = malloc(sizeof(struct irc_s));
	assert(irc);

	irc->addr = addr;
	irc->port = port;

	irc->readkev = malloc(sizeof(struct kevent));
	assert(irc->readkev);
	irc->writekev = malloc(sizeof(struct kevent));
	assert(irc->writekev);

	return irc;
}
