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
#include "transactions.h"
#include <time.h>

typedef response (*func)(void *info);
typedef struct method
{
    char name[100];
    size_t struct_size;
    func function;
} method;

int main()
{
    srand(time(NULL));

    struct method methods[] = {
        {.name = "Register", .struct_size = sizeof(new_account), .function = create_account},
        {.name = "SignIn", .struct_size = sizeof(login), .function = sign_in},
        {.name = "Info", .struct_size = sizeof(account_info), .function = check_info},
        {.name = "List", .struct_size = sizeof(list_account), .function = list_accounts},
        {.name = "ListBankStatement", .struct_size = sizeof(list_bank_statement), .function = get_bank_statement},
        {.name = "Deposit", .struct_size = sizeof(deposit), .function = make_deposit},
        {.name = "Withdraw", .struct_size = sizeof(withdraw), .function = make_withdraw},
        {.name = "Transfer", .struct_size = sizeof(transfer), .function = make_transfer},
    };
    int num_of_methods = sizeof(methods) / sizeof(method);

    printf("Starting\n");

    int sock_id = create_socket();
    set_socket_for_reuse(sock_id);
    printf("Created socket\n");

    bind_port(sock_id);
    printf("Binded port\n");

    listen_socket(sock_id);
    printf("Listening for connections on port %i\n", SERVER_PORT);

    while (1)
    {
        printf("Starting accept %i\n", rand());
        fflush(stdout);

        int c = sizeof(struct sockaddr_in);
        struct sockaddr client;

        int accept_socket_id = accept(sock_id, (struct sockaddr *)&client, (socklen_t *)&c);
        if (accept_socket_id < 0)
        {
            int err = errno;
            printf("Failed to accept connection with error %s (%i)\n", errnoname(err), err);
        }

        char client_ip[INET6_ADDRSTRLEN];
        void *addr = get_in_addr((struct sockaddr *)&client);
        inet_ntop(client.sa_family, addr, client_ip, INET6_ADDRSTRLEN);
        printf("Connection received from %s\n", client_ip);

        void *data = malloc(MAX_STREAM_SIZE);
        bzero(data, MAX_STREAM_SIZE);
        receive_message(accept_socket_id, data, MAX_STREAM_SIZE);

        char *method = malloc(METHOD_SIZE);
        bzero(method, METHOD_SIZE);
        memcpy(method, data, METHOD_SIZE);

        printf("Received method %s\n", method);

        fflush(stdout);

        int method_found = 0;
        for (int i = 0; i < num_of_methods; i++)
        {
            if (strcmp(method, methods[i].name) == 0)
            {
                method_found = 1;

                void *info = malloc(methods[i].struct_size);
                bzero(info, methods[i].struct_size);
                memcpy(info, data + METHOD_SIZE, methods[i].struct_size);

                struct response res = methods[i].function(info);

                if (res.response_str != NULL)
                {
                    send_message(accept_socket_id, res.response_str, res.response_size);
                    free(res.response_str);
                }

                free(info);
                break;
            }
        }

        free(data);
        free(method);

        if (method_found == 0)
        {
            printf("Invalid method submitted\n");
        }

        close(accept_socket_id);

        printf("Closed connection\n\n");
    }
    return 0;
}
