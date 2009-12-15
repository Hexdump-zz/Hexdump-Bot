#ifndef _NETWORK_H_
#define _NETWORK_H_

int kq;

struct irc_s;

struct irc_s *irc_str;

int irc_connect(struct irc_s *irc);
void irc_loop(void);
void irc_read(struct kevent *kev);
void irc_write(struct kevent *kev);

#endif
