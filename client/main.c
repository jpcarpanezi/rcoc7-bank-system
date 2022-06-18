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

#define REGISTER_METHOD "Register"

typedef struct new_account
{
    char name[100];
    char cpf[12];
    char senha[50];
} new_account;

typedef struct new_account_response
{
    char pix[37];
    char token[37];
    char response[100];
    int success;
} new_account_response;

void create_account(int sock_id)
{
    struct new_account acc;
    bzero(&(acc), sizeof(acc));
    strcpy(acc.cpf, "123456");
    strcpy(acc.name, "MEU NOMBRE");
    strcpy(acc.senha, "12345");

    send_message(sock_id, REGISTER_METHOD, sizeof(REGISTER_METHOD));

    char ok_msg[100];
    receive_message(sock_id, ok_msg, sizeof(ok_msg));

    if (strcmp(ok_msg, "OK") != 0)
    {
        printf("Expected message OK, received: %s\n", ok_msg);
    }

    send_message(sock_id, &acc, sizeof(acc));

    printf("Sent message\n");

    struct new_account_response response;
    bzero(&(response), sizeof(response));
    receive_message(sock_id, &response, sizeof(response));

    printf("Received message\n");

    printf("Response: %s\n", response.response);
    printf("Success: %i\n", response.success);
    printf("Pix: %s\n", response.pix);
    printf("Token: %s\n", response.token);
}

int main()
{
    int sock_id = create_socket();
    connect_socket(sock_id);

    create_account(sock_id);

    sock_id = create_socket();
    connect_socket(sock_id);

    create_account(sock_id);
    return 0;
}