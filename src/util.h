#ifndef _UTIL_H_
#define _UTIL_H_

#define PRINT_ERR(func, file, line, errno) (\
	printf("%s.%d in %s(): %s\n", file, line, func, strerror(errno)))

#define DEBUG(file, line) (\
	printf("%s.%d\n", file, line))

void kqueue_apply(struct kevent *kev);
void *malloc_copy(void *src, ssize_t src_size);

#endif
