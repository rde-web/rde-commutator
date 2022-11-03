#include <unistd.h>
#include <signal.h>
#include <mm_malloc.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "rde-commutator/err.h"
#include "rde-commutator/unpack.h"
#include "rde-commutator/tcp_gateway.h"
#include "rde-commutator/unix_gateway.h"

volatile int listener;

const char remote_register_ok = 0;
const char remote_register_fail = 1;
const unsigned short max_msg_size = 256;
const unsigned int max_cons = 10;

void onInterrupt(int arg) {
    close(listener);
}

void* handle(void* arg) {
    int conn = *((int *)arg);
    char msg[max_msg_size];
    memset(msg, '\0', max_msg_size);

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
        printf("no daemon id given");
        goto DEFER;
    }

    if (did->forward != NULL) {
        // forward_daemon_message(conn);
    }
    
    if (did->serve != NULL) {
        serve_daemon_gateway(conn);
    }

DEFER:
    close(conn);
    printf("will exit now\n");
    pthread_exit(NULL);
    return (void*)0;
}

int serve_tcp_gateway(const char * addr, const unsigned short port) {
    signal(SIGINT, onInterrupt);
    struct sockaddr_in server_addr;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        printerr("create socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(addr);
    
    if (
        bind(
            listener,
            (struct sockaddr*)&server_addr,
            sizeof(server_addr)
        ) < 0
    ) {
        printerr("bind");
        return -1;
    }
    
    if(listen(listener, 1) < 0){
        printerr("listen");
        return -1;
    }

    unsigned short cons = 0;

    while(true) {
        struct sockaddr_in client_addr;
        int client_size = sizeof(client_addr);
        int conn = accept(listener, (struct sockaddr*)&client_addr, (socklen_t *)&client_size);
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
    close(listener);
    return -1;
}
