#ifndef _IRC_H_
#define _IRC_H_

#define IRC_PRIVMSG 0x01
#define IRC_NOTICE 0x02

void irc_parse(char *data, ssize_t data_size);

#endif /* _IRC_H_ */
