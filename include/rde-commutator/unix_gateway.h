#ifndef _UNIX_GATEWAY_
#define _UNIX_GATEWAY_

int serve_daemon_gateway(int, const char*);
int forward_daemon_message(int tcp_connection, const char*, char*);

#endif /* _UNIX_GATEWAY_ */