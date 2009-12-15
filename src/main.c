#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/event.h>
#include <arpa/inet.h>
#include "network.h"
#include "irc_struct.h"
#include "threading.h"
#include "irc.h"
#include "python_api.h"

void
app_exit()
{
	exit(EXIT_SUCCESS);
}

int
main()
{
	signal(SIGINT, app_exit);

	kq = kqueue();
	struct irc_s *irc = irc_new(inet_addr("91.90.27.237"), 6667);
	irc_str = irc;
	
	if (irc_connect(irc) < 0) {
		exit(EXIT_FAILURE);
	}


	pthread_mutex_init(&sendqueue_mutex, NULL);
	pthread_mutex_init(&inqueue_mutex, NULL);
	pthread_create(&py_thread, NULL, api_worker, NULL);

	irc_loop();
}


