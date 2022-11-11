#include <stdio.h>
#include <stdlib.h>
#include "parson/parson.h"
#include "rde-commutator/unpack.h"

const char* key_daemon_id = "did";
const char* key_forward_daemon_id = "fdid";

daemon_id* unpack_daemon_id(char const* buf, size_t len) {
    daemon_id* did = &(daemon_id){NULL, NULL};
    JSON_Value *root = json_parse_string(buf);
    size_t i;
    if (json_value_get_type(root) != JSONObject) goto DEFER;
    const JSON_Object *obj = json_value_get_object(root);

    const char *val_did = json_object_get_string(obj, key_daemon_id);
    if (val_did != NULL) {
        (*did).serve = malloc(strlen(val_did));
        strcpy((*did).serve, val_did);
        goto DEFER;
    }
    const char *val_fdid = json_object_get_string(obj, key_forward_daemon_id);
    if (val_fdid != NULL){
        (*did).forward = malloc(strlen(val_fdid));
        strcpy((*did).forward, val_fdid);
    }
DEFER:
    json_value_free(root);
    return did;
}
