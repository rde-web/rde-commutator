#ifndef _UNIX_GATEWAY_
#define _UNIX_GATEWAY_

int serve_daemon_gateway(int tcp_connection);
int forward_daemon_message(int tcp_connection);

#endif /* _UNIX_GATEWAY_ */