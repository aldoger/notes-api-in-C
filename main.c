#include <unistd.h>   
#include <string.h>   
#include <stdio.h>
#include "include/server.h"

void hello_handler(RequestContext *ctx) {
    const char *body = "Hello from /hello\n";

    char response[256];
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %zu\r\n\r\n"
        "%s",
        strlen(body),
        body
    );

    write(ctx->client_fd, response, strlen(response));
}

void print_notes(RequestContext *ctx) {
    AppState *app = (AppState *)ctx->app;

    char body[1024];
    int offset = 0;

    offset += snprintf(body + offset, sizeof(body) - offset,
                       "Notes:\n");

    for (int i = 0; i < app->notes_count; i++) {
        offset += snprintf(body + offset, sizeof(body) - offset,
                           "- %s\n", app->notes[i]);
    }

    char response[2048];
    int resp_len = snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s",
        offset, body
    );

    write(ctx->client_fd, response, resp_len);
}


int main() {
    static AppState app = {
        .notes = {
            "First note",
            "Second note"
        },
        .notes_count = 2
    };

    struct Server server = server_Constructor(
        AF_INET, 8000, SOCK_STREAM, 0, 10, INADDR_ANY, launch, &app
    );

    server_add_handler(&server, "/hello", hello_handler);
    server_add_handler(&server, "/notes", print_notes);

    server.launch(&server);
}

