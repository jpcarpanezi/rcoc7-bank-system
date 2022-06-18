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
#include <time.h>

#define REGISTER_METHOD "Register"
#define DEPOSIT_METHOD "Deposit"
#define WITHDRAW_METHOD "Withdraw"

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

typedef struct deposit
{
    double value;
    char token[37];
} deposit;

typedef struct deposit_response
{
    char response[100];
    int success;
} deposit_response;

typedef struct withdraw
{
    double value;
    char token[37];
} withdraw;

typedef struct withdraw_response
{
    char response[100];
    int success;
} withdraw_response;

char acc_token[37];

char *gerar_cpf()
{
    char cpfs[][12] = {
        "11529137004",
        "94088831012",
        "14418777032",
        "77859229005",
        "23606689004",
        "27288851095",
        "98787447010",
        "85839138037",
        "13750786070",
        "79417837036",
        "60093889020",
        "61316773019",
        "95094217070",
        "30192886061",
        "10600044025",
        "27534520088",
        "62180757026",
        "86857732027",
        "66424106022",
        "52163983099",
        "86011783082",
        "66357725021",
        "29656868005",
    };

    size_t size = sizeof(cpfs) / sizeof(cpfs[0]);

    srand(time(NULL));
    int rdm = rand();
    int index = rdm % (size);

    char *ret = malloc(sizeof(char) * 12);
    strcpy(ret, cpfs[index]);
    return ret;
}

void create_account(int sock_id)
{
    struct new_account acc;
    bzero(&(acc), sizeof(acc));
    strcpy(acc.cpf, gerar_cpf());
    strcpy(acc.name, "MEU NOMBRE");
    strcpy(acc.password, "12345");

    size_t size = METHOD_SIZE + sizeof(acc);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, REGISTER_METHOD);
    
    memcpy(data, method, METHOD_SIZE);
    memcpy(data + METHOD_SIZE, &(acc), sizeof(acc));

    send_message(sock_id, data, size);

    printf("Sent message\n");

    struct new_account_response *response = malloc(sizeof(struct new_account_response));
    bzero(response, sizeof(struct new_account_response));
    receive_message(sock_id, response, sizeof(struct new_account_response));

    printf("Response: %s\n", response->response);
    printf("Success: %i\n", response->success);
    printf("Pix: %s\n", response->pix);
    printf("Token: %s\n\n", response->token);
}

void make_deposit(int sock_id)
{
    struct deposit deposit;
    bzero(&(deposit), sizeof(deposit));
    strcpy(deposit.token, acc_token);
    deposit.value = 10.51;

    size_t size = METHOD_SIZE + sizeof(deposit);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, DEPOSIT_METHOD);
    
    memcpy(data, method, METHOD_SIZE);
    memcpy(data + METHOD_SIZE, &(deposit), sizeof(deposit));

    send_message(sock_id, data, size);

    printf("Sent message\n");

    struct deposit_response *response = malloc(sizeof(struct deposit_response));
    bzero(response, sizeof(struct deposit_response));
    receive_message(sock_id, response, sizeof(struct deposit_response));

    printf("Success: %d\nResponse: %s\n\n", response->success, response->response);
}

void make_withdraw(int sock_id)
{
    struct withdraw withdraw;
    bzero(&(withdraw), sizeof(withdraw));
    strcpy(withdraw.token, acc_token);
    withdraw.value = 10.00;

    size_t size = METHOD_SIZE + sizeof(withdraw);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, WITHDRAW_METHOD);
    
    memcpy(data, method, METHOD_SIZE);
    memcpy(data + METHOD_SIZE, &(withdraw), sizeof(withdraw));

    send_message(sock_id, data, size);

    printf("Sent message\n");

    struct withdraw_response *response = malloc(sizeof(struct withdraw_response));
    bzero(response, sizeof(struct withdraw_response));
    receive_message(sock_id, response, sizeof(struct withdraw_response));

    printf("Success: %d\nResponse: %s\n\n", response->success, response->response);
}

int main()
{
    int sock_id = create_socket();
    connect_socket(sock_id);

    create_account(sock_id);


    sock_id = create_socket();
    connect_socket(sock_id);

    make_deposit(sock_id);

    sock_id = create_socket();
    connect_socket(sock_id);

    make_withdraw(sock_id);
    return 0;
}