#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "include/handler.h"
#include "include/server.h"


struct Server server_Constructor(int domain, int port, int service, int protocol, int backlog, __u_long interface, void (*launch)(struct Server *server), void *app_state) {
    struct Server server;

    memset(server.handlers, 0, sizeof(server.handlers));

    server.domain = domain;
    server.service = service;
    server.port = port;
    server.protocol = protocol;
    server.backlog = backlog;
    server.handler_count = 0;
    server.app_state = app_state;

    server.address.sin_family = domain;
    server.address.sin_port = htons(port);
    server.address.sin_addr.s_addr = htonl(interface);

    server.socket = socket(domain, service, protocol);
    if (server.socket < 0) {
        perror("Failed to initialize/connect to socket...\n");
        exit(EXIT_FAILURE);
    }

    if (bind(server.socket, (struct sockaddr*)&server.address, sizeof(server.address)) < 0) {
        perror("Failed to bind socket...\n");
        exit(EXIT_FAILURE);
    }

    if (listen(server.socket, server.backlog) < 0) {
        perror("Failed to start listening...\n");
        exit(EXIT_FAILURE);
    }

    server.launch = launch;
    return server;
}

void launch(struct Server *server) {
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("WAITING FOR REQUEST\n");
        fflush(stdout);

        socklen_t addrlen = sizeof(server->address);
        int client_fd = accept(server->socket,
            (struct sockaddr *)&server->address,
            &addrlen);

        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read <= 0) {
            close(client_fd);
            continue;
        }

        buffer[bytes_read] = '\0';

        char *body = strstr(buffer, "\r\n\r\n");
        if (!body) {
            close(client_fd);
            continue;
        }
        body += 4;

        struct json_object *json = NULL;
        if (strstr(buffer, "Content-Type: application/json")) {
            json = json_tokener_parse(body);
        }

        RequestContext ctx = {0};
        ctx.client_fd = client_fd;
        ctx.app = server->app_state;
        ctx.obj = json;

        char method[8];
        char path[1024];

        if (sscanf(buffer, "%7s %1023s", method, path) != 2) {
            if (json) json_object_put(json);
            close(client_fd);
            continue;
        }

        char *q = strchr(path, '?');
        if (q) *q = '\0';


        int handled = 0;
        for (int i = 0; i < server->handler_count; i++) {
            if (strcmp(path, server->handlers[i].route) == 0) {
                server->handlers[i].handler(&ctx);
                handled = 1;
                break;
            }
        }

        if (!handled) {
            const char *resp =
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Type: text/plain\r\n\r\n"
                "404 Not Found\n";
            write(client_fd, resp, strlen(resp));
        }

        close(client_fd);
    }
}


void server_add_handler(struct Server *server, const char *route, void (*handler)(RequestContext *)) {
    if (server->handler_count >= MAX_HANDLERS) {
        printf("cannot add more handlers");
        return;
    }

    strcpy(server->handlers[server->handler_count].route, route);
    server->handlers[server->handler_count].handler = handler;
    server->handler_count++;
}