#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "rde-commutator/err.h"
#include "rde-commutator/config.h"
#include "path-join/path-join.h"

void serve_daemon_gateway(int tcp_connection, const char* id) {
    char* sock_path = path_join(DAEMON_SOCK_LOCATION, id);
    
    struct sockaddr_un sockaddr;
    memset(&sockaddr, 0, sizeof(struct sockaddr_un));

    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        printerr("unix->tcp create");
        return;
    }
    
    sockaddr.sun_family = AF_UNIX;   
    strcpy(sockaddr.sun_path, sock_path); 
    unlink(sock_path);

    if(bind(sock, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) < 0) {
        printerr("unix->tcp bind");
        close(sock);
        return;
    }

    if (listen(sock, UNIX_BACKLOG) < 0) {
        printerr("unix->tcp listen");
        close(sock);
        return;
    }

    while (true) {
        char in[MAX_MSG_SIZE], out[MAX_MSG_SIZE];
        struct sockaddr_un connaddr;
        socklen_t cl_addr_size = sizeof(connaddr);
        int conn = accept(sock, (struct sockaddr *) &connaddr, &cl_addr_size);
        if (conn == -1) {
            printerr("unix->tcp accept");
            close(conn);
            break;
        }
        // MARK: - unix read
        if ((recv(conn, in, sizeof(in), 0)) < 0) {
            printerr("unix->tcp recv");
            close(conn);
            continue;
        }
        // MARK: - tcp write
        if (send(tcp_connection, in, strlen(in), 0) < 0) {
            printerr("unix->tcp send tcp");
            close(conn);
            break; //@note most likely daemon connection is closed
        }
        // MARK: - tcp read
        if ((recv(tcp_connection, out, sizeof(out), 0) < 0)) {
            printerr("unix->tcp recv tcp");
            close(conn);
            break; //@note most likely daemon connection is closed
        }
        // MARK: - unix write
        if((send(conn, out, strlen(out), 0)) < 0){
            printerr("unix-tcp send");
            close(conn);
            continue;
        }
        close(conn);
    }
    close(sock);
    return;
}

void forward_daemon_message(int tcp_connection, const char * id, char * msg) {
    char* client_sock_path = path_join(CLIENT_SOCK_LOCATION, id);
    char* daemon_sock_path = path_join(DAEMON_SOCK_LOCATION, id);

    char out[MAX_MSG_SIZE];

    int sock;
    struct sockaddr_un server_sockaddr, client_sockaddr;
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        printerr("tcp->unix create");
        return;
    }

    client_sockaddr.sun_family = AF_UNIX;   
    strcpy(client_sockaddr.sun_path, client_sock_path); 
    socklen_t len = sizeof(client_sockaddr);
    
    unlink(client_sock_path);

    if(bind(sock, (struct sockaddr *) &client_sockaddr, len) < 0) {
        printerr("tcp->unxi bind");
        close(sock);
        return;
    }

    server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, daemon_sock_path);
    if(connect(sock, (struct sockaddr *) &server_sockaddr, len)){
        printerr("tcp->unix connect");
        close(sock);
        return;
    }
          
    if(send(sock, msg, strlen(msg), 0)){
        printerr("tcp->unix send unix");
        close(sock);
        return;
    }

    if(recv(sock, out, sizeof(out), 0)){
        printerr("tcp->unix recv unix");
        close(sock);
        return;
    }

    if (send(tcp_connection, out, strlen(out), 0) < 0) {
        printerr("tcp->unix send");
    }
    close(sock);
    return;
}
