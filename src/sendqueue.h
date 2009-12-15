#ifndef _SENDQUEUE_STRUCT_H_
#define _SENDQUEUE_STRUCT_H_

#define SENDQUEUE_BUFSIZE 512

struct sendqueue_s {
	char *data;
	size_t data_size;

	struct sendqueue_s *sndque_next;
	struct sendqueue_s *sndque_prev;
};

struct sendqueue_s *SNDQUE_HEAD;
struct sendqueue_s *SNDQUE_LIST;

void *sendqueue_new(ssize_t data_len);
void sendqueue_free(struct sendqueue_s *sndque);
void sendqueue_lock();
void sendqueue_unlock();

#endif
