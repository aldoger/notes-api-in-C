#ifndef context_h
#define context_h

#include "app_state.h"

typedef struct {
    int client_fd;

    char method[8];
    char path[256];

    struct AppState *app;
} RequestContext;


#endif