#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "network.h"
#include "irc_struct.h"
#include "irc.h"
#include "util.h"
#include "sendqueue.h"
#include "inqueue.h"
#include "threading.h"

int
irc_connect(struct irc_s *irc)
{
	struct sockaddr_in sin;
	struct sendqueue_s *sndque = sendqueue_new(512);
	ssize_t n;

	memset(&sin, 0, sizeof(struct sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = irc->addr;
	sin.sin_port = htons(irc->port);

	if ((irc->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		PRINT_ERR(__FUNCTION__, __FILE__, __LINE__, errno);
		return -1;
	}

	if (connect(irc->fd, (struct sockaddr *)&sin, sizeof(struct sockaddr)) < 0) {
		PRINT_ERR(__FUNCTION__, __FILE__, __LINE__, errno);
		return -1;
	}

	n = snprintf(sndque->data, 512, 
		"USER kalle 8 * :RUTGER\r\n"
		"NICK bert\r\n");
	sndque->data_size = n;

	fcntl(irc->fd, F_SETFL, fcntl(irc->fd, F_GETFL, 0) | O_NONBLOCK);

	EV_SET(irc->readkev, irc->fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, irc);
	EV_SET(irc->writekev, irc->fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, irc);

	kqueue_apply(irc->readkev);
	kqueue_apply(irc->writekev);

	return 1;
}

void
irc_loop()
{
	struct kevent kev[2];
	int n;

	for (;;) {
		n = kevent(kq, NULL, 0, kev, 2, NULL);

		if (n < 0) {
			PRINT_ERR(__FUNCTION__, __FILE__, __LINE__, errno);
		} else if (n > 0) {
			for (int i = 0; i < n; i++) {
				if (kev[i].filter == EVFILT_READ) {
					irc_read(&kev[i]);
				} else if (kev[i].filter == EVFILT_WRITE) {
					if (!pthread_mutex_trylock(&sendqueue_mutex)) {
						irc_write(&kev[i]);
						sendqueue_unlock();
					} else {
						printf("Mutex is locked.\n");
					}
				}
			}
		}
	}
}

void
irc_read(struct kevent *kev)
{
	struct irc_s *irc = kev->udata;
	struct sendqueue_s *sndque;
	ssize_t n;

	char *buf = malloc(512);
	memset(buf, 0, 512);

	if ((n = read(irc->fd, buf, 512)) > 0) {
		inqueue_lock();
		for (char *line; (line = strsep(&buf, "\n")) != NULL ;) {
			if (line == NULL)
				break;

			if (!memcmp(line, "PING", 4)) {
				sendqueue_lock();
				sndque = sendqueue_new(strlen(line));
				memcpy(sndque->data, line, strlen(line));
				sndque->data[1] = 'O';
				sendqueue_unlock();
			} else
				irc_parse(line, n);
		}

		inqueue_unlock();
	}

	if (!pthread_mutex_trylock(&sendqueue_mutex)) {
		if (SNDQUE_LIST != NULL) {
			irc->writekev->flags |= EV_ENABLE;
			kqueue_apply(irc->writekev);
		}
		sendqueue_unlock();
	}

	free(buf);
	return;
}

void
irc_write(struct kevent *kev)
{
	if (SNDQUE_LIST == NULL) {
		return;
	}

	struct irc_s *irc = kev->udata;
	for (struct sendqueue_s *sndque = SNDQUE_LIST; sndque != NULL; sndque = SNDQUE_LIST) {
		if (write(irc->fd, sndque->data, sndque->data_size) > 0) {
			sendqueue_free(sndque);
		} else {
			if (errno != EAGAIN) {
				PRINT_ERR(__FUNCTION__, __FILE__, __LINE__, errno);
			}
			break;
		}
	}


	printf("Finished writing.\n");

	irc->writekev->flags = EV_DISABLE;
	kqueue_apply(irc->writekev);
}
