#include "rde-commutator/err.h"


void printerr(const char * prefix) {
    printf("%s err: %s\n", prefix, strerror(errno));
}