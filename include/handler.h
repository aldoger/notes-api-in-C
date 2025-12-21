#ifndef handler_h
#define handler_h

#include "context.h"

typedef void (*HandlerFn)(RequestContext *ctx);

struct Handler {
    char route[64];
    HandlerFn handler;
};

#endif