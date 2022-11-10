#ifndef _UNPACK_H_
#define _UNPACK_H_

// #include <msgpack.h>
#include <string.h>

typedef struct {
    char* serve; //@note for daemon connection
    char* forward; //@note for client proxy connetction
} daemon_id;

daemon_id* unpack_daemon_id(char const* buf, size_t len);

#endif /* _UNPACK_H_ */
