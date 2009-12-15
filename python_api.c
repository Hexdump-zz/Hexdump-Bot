#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <Python.h>
#include "irc_bindings.h"
#include "inqueue.h"
#include "sendqueue.h"
#include "threading.h"

PyObject *py_module;

static PyObject
*api_privmsg(PyObject *self, PyObject *args)
{
	PyObject *res = NULL;
	char *irc_buf;
	struct sendqueue_s *sndque;

	ssize_t irc_buf_size;

	printf("Sending MSG!\n");

	if (PyArg_ParseTuple(args, "s#:Send", &irc_buf, &irc_buf_size)) {
		printf("BUF: %s\n", irc_buf);
		/*
		sndque = sendqueue_new(512);
		strcpy(sndque->data, "PRIVMSG #Hexdump :TJOBRE!\r\n");
		*/
		printf("SENDQUEUE->data = %s\n", sndque->data);
		printf("HERE! RAWR!\n");
		res = Py_None;
	}
	printf("RAWR\n");
	return res;
}

static PyObject
*api_bind_cmd(PyObject *self, PyObject *args)
{
	PyObject *cb;
	PyObject *res = NULL;
	ssize_t cmd_size;
	char *cmd;

	if (PyArg_ParseTuple(args, "Os#:BindCommand", &cb, &cmd, &cmd_size)) {
		if (!PyCallable_Check(cb)) {
			PyErr_SetString(PyExc_TypeError, "Python: The function is not callable.");
			return NULL;
		}

		Py_XINCREF(cb);
		irc_bind_new(cmd, cmd_size, cb);
		res = Py_None;
	}

	return res;
}

static void
notify_callback(struct inqueue_s *inque)
{
	PyObject *arglist;
	struct irc_bind_s *irc_bind;
	if ((irc_bind = irc_bind_get(inque->data, inque->data_size)) != NULL) {
		printf("Found Binding.\n");
		if (!PyCallable_Check(irc_bind->py_callback)) {
			printf("Cannot call function.\n");
			return;
		}
		arglist = Py_BuildValue("sss", inque->sender, inque->reciever, inque->data);
		PyObject_CallObject(irc_bind->py_callback, arglist);
		Py_XDECREF(arglist);
	}

	return;
}

static PyMethodDef SopbotMethods[] = {
	{"BindCommand", api_bind_cmd, METH_VARARGS, "Binds a callback to a specific command."},
	{"Send", api_privmsg, METH_VARARGS, "Sends a command to irc."},
	{NULL, NULL, 0, NULL}
};

void
*api_worker(void *arg)
{

	struct irc_bind_s *binding;
	setenv("PYTHONPATH", "./", 1);
	Py_Initialize();
	Py_InitModule("Sopbot", SopbotMethods);

	PyObject *py_modname = PyString_FromString("bindings");
	PyObject *py_func;
	py_module = PyImport_Import(py_modname);
	PyErr_Print();
	Py_XDECREF(py_modname);
	Py_INCREF(py_module);

	py_func = PyObject_GetAttrString(py_module, "RegisterBindings");
	PyObject_CallObject(py_func, NULL);
	PyErr_Print();

	printf("Running API-Loop.\n");

	if ((binding = irc_bind_get("!help", 5)) != NULL) {
		printf("SUCCESS!\n");
	} else {
		printf("FAIL\n");
	}

	for (;;) {
		if (!pthread_mutex_trylock(&inqueue_mutex)) {
			if (INQUEUE_HEAD != NULL) {
				printf("Recieved data from %s to %s: %s\n", INQUEUE_HEAD->sender, INQUEUE_HEAD->reciever, INQUEUE_HEAD->data);
				notify_callback(INQUEUE_HEAD);
				inqueue_free(INQUEUE_HEAD);
			}
			inqueue_unlock();
		}

		usleep(10);
	}
}
