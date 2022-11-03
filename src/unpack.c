#include "rde-commutator/unpack.h"

const char* key_daemon_id = "daemon_id";
const char* key_forwar_daemon_id = "forward_daemon_id";

bool hasPrefix(const char *str, const char *pre) {
    return strncmp(pre, str, strlen(pre)) == 0;
}

daemon_id* unpack_daemon_id(char const* buf, size_t len) {
    daemon_id* did = &(daemon_id){NULL, NULL};
    msgpack_unpacked result;
    size_t off = 0;
    msgpack_unpack_return ret;
    msgpack_unpacked_init(&result);
    ret = msgpack_unpack_next(&result, buf, len, &off);
    while (ret == MSGPACK_UNPACK_SUCCESS) {
        msgpack_object obj = result.data;
        msgpack_object_kv* map = obj.via.map.ptr;

        const char * key = map->key.via.str.ptr;
        const char * val = map->val.via.str.ptr;

        if (hasPrefix(key, key_daemon_id)) {
            (*did).serve = val;
            break;
        }
        if (hasPrefix(key, key_forwar_daemon_id)){
            (*did).forward = val;
            break;
        }
        ret = msgpack_unpack_next(&result, buf, len, &off);
    }
    msgpack_unpacked_destroy(&result);

    if (ret == MSGPACK_UNPACK_PARSE_ERROR)
        return NULL;
    return did;
}