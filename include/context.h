#ifndef context_h
#define context_h

#include "app_state.h"
#include <json-c/json.h>

typedef struct {
    int client_fd;

    struct json_object *obj;
    struct AppState *app;
} RequestContext;

#endif