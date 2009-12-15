#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/event.h>
#include "network.h"
#include "util.h"

void
kqueue_apply(struct kevent *kev)
{
	int n;
	
	if (n = kevent(kq, kev, 1, NULL, 0, NULL) < 0)
		PRINT_ERR(__FUNCTION__, __FILE__, __LINE__, errno);
}

/**
 * Note: dest must be unallocated.
 */
void
*malloc_copy(void *src, ssize_t src_size)
{
	void *dest;
	dest = malloc(src_size);
	assert(dest);
	bcopy(src, dest, src_size);

	return dest;
}
