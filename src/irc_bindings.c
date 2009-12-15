#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/event.h>
#include <Python.h>
#include "irc_bindings.h"
#include "util.h"

void
*irc_bind_new(char *cmd, size_t cmd_size, PyObject *py_callback)
{
	struct irc_bind_s *binding = malloc(sizeof(struct irc_bind_s));
	assert(binding);
	printf("%s; CMD: %s: CMD_SIZE: %d\n", __FILE__, cmd, cmd_size);
	
	binding->cmd = malloc(cmd_size);
	strcpy(binding->cmd, cmd);
	binding->cmd_size = cmd_size;
	binding->py_callback = py_callback;

	if (BIND_LIST == NULL) {
		BIND_LIST = binding;
		BIND_HEAD = binding;
	} else {
		BIND_HEAD->bind_next = binding;
		binding->bind_prev = BIND_HEAD;
		BIND_HEAD = binding;
	}

	return binding;
}

void
irc_bind_free(struct irc_bind_s *binding)
{
	assert(binding);
	
	if (binding == BIND_LIST)
		BIND_LIST = binding->bind_next;

	if (binding == BIND_HEAD)
		BIND_HEAD = binding->bind_prev;

	if (binding->bind_next != NULL)
		binding->bind_next->bind_prev = binding->bind_prev;

	if (binding->bind_prev != NULL)
		binding->bind_prev->bind_next = binding->bind_next;

	assert(binding->cmd);
	free(binding->cmd);

	Py_XDECREF(binding->py_callback);

	free(binding);

	return;
}

void
*irc_bind_get(char *cmd, size_t cmd_size)
{
	printf("Checking if %s matches any of my binds.\n", cmd+1);
	for (struct irc_bind_s *binding = BIND_LIST; binding != NULL; binding = binding->bind_next) {
		printf("Bind CMD: %s Len %d\n", binding->cmd, binding->cmd_size);
		if (!strncmp(binding->cmd, cmd+1, binding->cmd_size))
			return binding;
	}

	return NULL;
}
