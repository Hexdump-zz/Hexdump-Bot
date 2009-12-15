#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/event.h>
#include "sendqueue.h"
#include "threading.h"
#include "irc_struct.h"
#include "network.h"
#include "util.h"

void
*sendqueue_new(ssize_t data_len)
{
	struct sendqueue_s *sndque = malloc(sizeof(struct sendqueue_s));
	assert(sndque);

	sndque->data = malloc(data_len);
	assert(sndque->data);

	sndque->data_size = data_len;

	if (SNDQUE_LIST == NULL) {
		SNDQUE_LIST = sndque;
		SNDQUE_HEAD = sndque;
	} else {
		SNDQUE_HEAD->sndque_next = sndque;
		sndque->sndque_prev = SNDQUE_HEAD;
		SNDQUE_HEAD = sndque;
	}

	irc_str->writekev->flags |= EV_ENABLE;
	kqueue_apply(irc_str->writekev);

	return sndque;
}

void
sendqueue_free(struct sendqueue_s *sndque)
{
	assert(sndque);
	assert(sndque->data);

	if (sndque == SNDQUE_LIST) {
		SNDQUE_LIST = sndque->sndque_next;
	}

	if (sndque == SNDQUE_HEAD) {
		SNDQUE_HEAD = sndque->sndque_prev;
	}

	if (sndque->sndque_next != NULL)
		sndque->sndque_next->sndque_prev = sndque->sndque_prev;
	
	if (sndque->sndque_prev != NULL)
		sndque->sndque_prev->sndque_next = sndque->sndque_next;

	free(sndque->data);
	free(sndque);

	return;
}

void
sendqueue_lock()
{
	pthread_mutex_lock(&sendqueue_mutex);
	return;
}

void
sendqueue_unlock()
{
	pthread_mutex_unlock(&sendqueue_mutex);
	return;
}
