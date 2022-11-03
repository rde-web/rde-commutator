#include "rde-commutator/tcp_gateway.h"

int main(void) {
    return serve_tcp_gateway("127.0.0.1", 8080);
}
