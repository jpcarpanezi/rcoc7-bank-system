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
#define LOGIN_METHOD "SignIn"

typedef struct new_account
{
    char name[100];
    char cpf[12];
    char password[50];
} new_account;

typedef struct new_account_response
{
    char pix[37];
    char token[37];
    char response[100];
    int success;
} new_account_response;

typedef struct login {
    char cpf[12];
    char password[50];
} login;

typedef struct login_response {
    char token[37];
    char response[100];
    int success;
} login_response;

void create_account(int sock_id)
{
    struct new_account acc;
    bzero(&(acc), sizeof(acc));
    strcpy(acc.cpf, "39644355890");
    strcpy(acc.name, "MEU NOMBRE");
    strcpy(acc.password, "Senha@pwd20");

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

void sign_in(int sock_id) {
    struct login login;
    bzero(&(login), sizeof(login));
    strcpy(login.cpf, "39644355890");
    strcpy(login.password, "Senha@pwd20");

    printf("Sent message\n");

    send_message(sock_id, LOGIN_METHOD, sizeof(LOGIN_METHOD));
    send_message(sock_id, &login, sizeof(login));

    struct login_response response;
    bzero(&(response), sizeof(response));
    receive_message(sock_id, &response, sizeof(response));

    printf("Received message\n");

    printf("Response: %s\n", response.response);
    printf("Success: %i\n", response.success);
    printf("Token: %s\n", response.token);
}

int main()
{
    // int sock_id = create_socket();
    // connect_socket(sock_id);

    // create_account(sock_id);

    int sock_id = create_socket();
    connect_socket(sock_id);
    create_account(sock_id);
    close(sock_id);

    // printf("\n\n===============================================================\n\n");

    // int sock_id_2 = create_socket();
    // connect_socket(sock_id_2);
    // sign_in(sock_id_2);
    // close(sock_id_2);

    return 0;
}