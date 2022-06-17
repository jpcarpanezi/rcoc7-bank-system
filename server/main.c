#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include "errnoname.h"
#include "sockets.h"
#include "account.h"

typedef response (*func)(void *info);
typedef struct method
{
    char name[100];
    size_t structSize;
    func function;
} method;

int main()
{
    struct method methods[] = {
        {.name = "Register", .structSize = sizeof(new_account), .function = create_account}
    };
    int num_of_methods = sizeof(methods) / sizeof(method);

    printf("Starting\n");

    int sockid = create_socket();
    set_socket_for_reuse(sockid);
    printf("Created socket\n");

    bind_port(sockid);
    printf("Binded port\n");

    listen_socket(sockid);
    printf("Listening for connections on port %i\n", SERVER_PORT);

    while (1)
    {
        printf("Starting accept\n");
        fflush(stdout);

        int c = sizeof(struct sockaddr_in);
        struct sockaddr client;

        int accept_socket_id = accept(sockid, (struct sockaddr *)&client, (socklen_t *)&c);
        if (accept_socket_id < 0)
        {
            int err = errno;
            printf("Failed to accept connection with error %s (%i)\n", errnoname(err), err);
        }

        char client_ip[INET6_ADDRSTRLEN];
        void *addr = get_in_addr((struct sockaddr *)&client);
        inet_ntop(client.sa_family, addr, client_ip, INET6_ADDRSTRLEN);
        printf("Connection received from %s\n", client_ip);

        char method[100];
        receive_message(accept_socket_id, method, sizeof(method));
        printf("Received method %s\n", method);

        fflush(stdout);

        int method_found = 0;
        for (int i = 0; i < num_of_methods; i++)
        {
            if (strcmp(method, methods[i].name) == 0)
            {
                method_found = 1;

                void *info = malloc(methods[i].structSize);
                bzero(info, methods[i].structSize);

                receive_message(accept_socket_id, info, methods[i].structSize);

                struct response res = methods[i].function(info);

                if (res.response_str != NULL)
                {
                    send_message(accept_socket_id, res.response_str, res.response_size);
                }

                free(info);
                break;
            }
        }

        if (method_found == 0)
        {
            printf("Invalid method submitted\n");
            // TODO: SEND INVALID MESSAGE
        }

        close(accept_socket_id);

        printf("Closed connection\n\n");
    }
    return 0;
}
