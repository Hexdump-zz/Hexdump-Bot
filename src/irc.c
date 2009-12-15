#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/event.h>
#include "irc.h"
#include "util.h"
#include "inqueue.h"
#include "threading.h"

void
irc_parse(char *_data, ssize_t data_size)
{
	char *data = _data;
	char *tok[4];
	char *sender;
	int cmd;

	for (int i = 0; i < 3 && (tok[i] = strsep(&data, " ")) != NULL ; i++);

	tok[3] = strsep(&data, "\n");

	if (tok[3] == NULL)
		return;

	tok[3][strlen(tok[3])-1] = '\0';

	if (!memcmp(tok[1], "PRIVMSG", 7))
		cmd = IRC_PRIVMSG;
	else if (!memcmp(tok[1], "NOTICE", 6))
		cmd = IRC_NOTICE;
	else
		return;

	sender = strsep(&tok[0], "!");

	if (cmd == IRC_PRIVMSG || cmd == IRC_NOTICE) {
		inqueue_lock();
		inqueue_new(sender+1, tok[2], tok[3], cmd, strlen(sender), strlen(tok[2]) + 1, strlen(tok[3]) + 1);
		inqueue_unlock();
	}

	return;
}
