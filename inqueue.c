#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/event.h>
#include "inqueue.h"
#include "util.h"
#include "threading.h"

void
*inqueue_new(char *sender, char *reciever, char *data, int cmd,
	ssize_t sender_size, ssize_t reciever_size, ssize_t data_size)
{

	struct inqueue_s *inque = malloc(sizeof(struct inqueue_s));
	assert(inque);

	inque->sender = malloc_copy(sender, sender_size);
	inque->reciever = malloc_copy(reciever, reciever_size);
	inque->data = malloc_copy(data, data_size);

	printf("SENDER: %s\n", inque->sender);

	inque->cmd = cmd;
	inque->data_size = data_size;

	/* Insert it into the list. */
	if (INQUEUE_LIST == NULL) {
		INQUEUE_LIST = inque;
		INQUEUE_HEAD = inque;
		inque->inque_next = NULL;
		inque->inque_prev = NULL;
	} else {
		INQUEUE_HEAD->inque_next = inque;
		inque->inque_prev = INQUEUE_HEAD;
		INQUEUE_HEAD = inque;
	}

	return inque;
}

void
inqueue_free(struct inqueue_s *inque)
{
	assert(inque);
	
	if (inque == INQUEUE_LIST) {
		INQUEUE_LIST = inque->inque_next;
	}

	if (inque == INQUEUE_HEAD)
		INQUEUE_HEAD = inque->inque_prev;

	if (inque->inque_next != NULL)
		inque->inque_next->inque_prev = inque->inque_prev;

	if (inque->inque_prev != NULL)
		inque->inque_prev->inque_next = inque->inque_next;

	assert(inque->sender);
	free(inque->sender);

	assert(inque->reciever);
	free(inque->reciever);

	assert(inque->data);
	free(inque->data);

	free(inque);

	return;
}

void
inqueue_lock()
{
	pthread_mutex_lock(&inqueue_mutex);
	return;
}

void
inqueue_unlock()
{
	pthread_mutex_unlock(&inqueue_mutex);
	return;
}
