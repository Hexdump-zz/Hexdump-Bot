#ifndef _BINDINGS_H_
#define _BINDINGS_H_

struct irc_bind_s {
	char *cmd;
	size_t cmd_size;
	PyObject *py_callback;

	struct irc_bind_s *bind_next;
	struct irc_bind_s *bind_prev;
};

struct irc_bind_s *BIND_LIST;
struct irc_bind_s *BIND_HEAD;

void *irc_bind_new(char *cmd, size_t cmd_size, PyObject *py_callback);
void irc_bind_free(struct irc_bind_s *binding);
void *irc_bind_get(char *cmd, size_t cmd_size);

#endif /* _BINDINGS_H_ */
