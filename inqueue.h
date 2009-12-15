#ifndef _INQUEUE_H_
#define _INQUEUE_H_

struct inqueue_s {
	char *sender;
	char *reciever;
	char *data;

	ssize_t data_size;

	int cmd;

	struct inqueue_s *inque_next;
	struct inqueue_s *inque_prev;
};

struct inqueue_s *INQUEUE_LIST;
struct inqueue_s *INQUEUE_HEAD;

void *inqueue_new(char *sender, char *reciever, char *data, int cmd, ssize_t sender_size, ssize_t reciever_size, ssize_t data_size);
void inqueue_free(struct inqueue_s *inque);
void inqueue_lock();
void inqueue_unlock();

#endif /* _INQUEUE_H_ */
