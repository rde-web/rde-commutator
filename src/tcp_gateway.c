#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <mm_malloc.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "rde-commutator/err.h"
#include "rde-commutator/config.h"
#include "rde-commutator/unpack.h"
#include "rde-commutator/tcp_gateway.h"
#include "rde-commutator/unix_gateway.h"

volatile int sock;

void onInterrupt(int arg) {
    close(sock);
}

void* handle(void* arg) {
    int conn = *((int *)arg);
    char msg[MAX_MSG_SIZE];
    // memset(msg, '\0', max_msg_size);

    if (recv(conn, msg, sizeof(msg), 0) < 0){
        printerr("recieve");
        goto DEFER;
    }

    daemon_id* did = unpack_daemon_id(msg, strlen(msg));

    if (did == NULL){
        printerr("get_daemon_id");
        goto DEFER;
    }

    if (did->serve == NULL && did->forward == NULL) {
        printf("no daemon id given\n");
        goto DEFER;
    }

    if (did->forward != NULL) {
        forward_daemon_message(conn, did->forward, msg);
    }
    
    if (did->serve != NULL) {
        serve_daemon_gateway(conn, did->serve);
    }

DEFER:
    close(conn);
    pthread_exit(NULL);
    return (void*)0;
}

int serve_tcp_gateway(const char * addr, const unsigned short port) {
    signal(SIGINT, onInterrupt);
    struct sockaddr_in server_addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printerr("create socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(addr);
    
    if (
        bind(
            sock,
            (struct sockaddr*)&server_addr,
            sizeof(server_addr)
        ) < 0
    ) {
        printerr("bind");
        return -1;
    }
    
    if(listen(sock, TCP_BACKLOG) < 0){
        printerr("listen");
        return -1;
    }

    while(true) {
        struct sockaddr_in client_addr;
        socklen_t cl_size = sizeof(client_addr);
        int conn = accept(sock, (struct sockaddr*)&client_addr, &cl_size);
        if (conn < 0) {
            printerr("accept");
            break;
        }
        pthread_t tid;
        if ((pthread_create(&tid, NULL, handle, (void*)&conn)) < 0){
            printerr("err create thread");
            close(conn);
            break;
        };
    }
    close(sock);
    return -1;
}
