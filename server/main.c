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
    char name[100];     // Nome do método
    size_t struct_size; // Tamanho esperado da struct com os parâmetros
    func function;      // Ponteiro para função a ser chamada
} method;

int main()
{
    // Utiliza o tempo atual para como semente para o gerador de números aleatórios
    srand(time(NULL));

    // Métodos disponíveis para serem utilizados pelo cliente
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

        // Aceita conexão de entrada
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

        // Alocação de espaço para recebimento da mensagem
        void *data = malloc(MAX_STREAM_SIZE);
        bzero(data, MAX_STREAM_SIZE);

        // Recebimento da mensagem com o método desejado e os parâmetros do método
        receive_message(accept_socket_id, data, MAX_STREAM_SIZE);

        // Alocação de espaço para copiar o nome do método
        char *method = malloc(METHOD_SIZE);
        bzero(method, METHOD_SIZE);

        // Cópia da variável data para a variável method com tamanho METHOD_SIZE
        memcpy(method, data, METHOD_SIZE);

        printf("Received method %s\n", method);

        fflush(stdout);

        // Busca se o método informado está disponível
        int method_found = 0;
        for (int i = 0; i < num_of_methods; i++)
        {
            if (strcmp(method, methods[i].name) == 0)
            {
                method_found = 1;

                // Alocação de espaço para copiar os parâmetros do método
                void *info = malloc(methods[i].struct_size);
                bzero(info, methods[i].struct_size);

                // Cópia da variável data para a variável info, iniciando em data + METHOD_SIZE
                memcpy(info, data + METHOD_SIZE, methods[i].struct_size);

                // Chamada do método e atribuição da struct de resposta
                struct response res = methods[i].function(info);

                if (res.response_str != NULL)
                {
                    // Envia a resposta do método
                    send_message(accept_socket_id, res.response_str, res.response_size);

                    // Libera a memória utilizada pela struct de resposta do método
                    free(res.response_str);
                }

                // Libera a memória utilizada pelos parâmetros do método
                free(info);
                break;
            }
        }

        // Libera a memória utilizada pelo nome do método e a mensagem recebida
        free(data);
        free(method);

        if (method_found == 0)
        {
            printf("Invalid method submitted\n");
        }

        // Fecha a conexão
        close(accept_socket_id);

        printf("Closed connection\n\n");
    }
    return 0;
}
