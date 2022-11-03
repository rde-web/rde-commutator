#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_PATH  "tpf_unix_sock.server"
#define DATA "Hello from server"

int serve_daemon_gateway(int tcp_connection) {
    int server_sock, client_sock, len, rc;
    int bytes_rec = 0;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;     
    char buf[256];
    int backlog = 10;
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(buf, 0, 256);                
    
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1) {
        printerr("unix create socket");
        return -1;
    }

    server_sockaddr.sun_family = AF_UNIX;   
    strcpy(server_sockaddr.sun_path, SOCK_PATH); 
    len = sizeof(server_sockaddr);

    unlink(SOCK_PATH);
    rc = bind(server_sock, (struct sockaddr *) &server_sockaddr, len);
    if (rc == -1) {
        printerr("unxi bind");
        close(server_sock);
        return -1;
    }

    rc = listen(server_sock, backlog);
    if (rc == -1){ 
        printerr("unix listen");
        close(server_sock);
        return -1;
    }

    client_sock = accept(server_sock, (struct sockaddr *) &client_sockaddr, &len);
    if (client_sock == -1) {
        printerr("unix accept");
        close(server_sock);
        close(client_sock);
        return -1;
    }

    if ((recv(client_sock, buf, sizeof(buf), 0)) < 0) {
        printerr("unix recieve");
        close(server_sock);
        close(client_sock);
        return -1;
    }

    rc = send(client_sock, buf, strlen(buf), 0);
    if (rc == -1) {
        printf("SEND ERROR: %d", sock_errno());
        printerr("unix send");
        close(server_sock);
        close(client_sock);
        printerr("unix send");
    }

    close(server_sock);
    close(client_sock);
    
    return 0;
}

int forward_daemon_message(int tcp_connection, const char * msg) {
    if (send(tcp_connection, msg, strlen(msg), 0) < 0) {
        printerr("tcp send");
        return;
    }
}
