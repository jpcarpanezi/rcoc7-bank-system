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
#define LOGIN_METHOD "SignIn"
#define ACCOUNT_INFO_METHOD "Info"
#define LIST_ACCOUNTS_METHOD "List"
#define DEPOSIT_METHOD "Deposit"
#define WITHDRAW_METHOD "Withdraw"
#define TRANSFER_METHOD "Transfer"

#define PAGE_SIZE 10

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

typedef struct transfer
{
    double value;
    char token[37];
    char destination_account_pix[37];
} transfer;

typedef struct transfer_response
{
    char response[100];
    int success;
} transfer_response;

char acc_token[37];
char secondary_acc_pix[37];

char *gerar_cpf()
{
    char cpfs[][12] = {
        "11111111111"
    };

    size_t size = sizeof(cpfs) / sizeof(cpfs[0]);

    int rdm = rand();
    int index = rdm % (size);

    char *ret = malloc(sizeof(char) * 12);
    strcpy(ret, cpfs[index]);
    return ret;
}
typedef struct login
{
    char cpf[12];
    char password[50];
} login;

typedef struct login_response
{
    char token[37];
    char response[100];
    int success;
} login_response;

typedef struct account_info
{
    char token[37];
} account_info;

typedef struct account_info_response
{
    char name[100];
    char pix[37];
    char balance[50];
    char response[100];
    int success;
} account_info_response;

typedef struct list_account
{
    unsigned int page;
} list_account;

typedef struct list_account_response
{
    unsigned int page_index;
    unsigned int page_size;
    unsigned int current_page_size;
    unsigned int total_count;
    char accounts[PAGE_SIZE][37];
} list_account_response;

void create_account(int sock_id, int secondary)
{
    struct new_account acc;
    bzero(&(acc), sizeof(acc));
    strcpy(acc.cpf, gerar_cpf());
    printf("CPF: %s\n", acc.cpf);
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

    if (response->success)
    {
        if (secondary)
        {
            strcpy(secondary_acc_pix, response->pix);
        }
        else
        {
            strcpy(acc_token, response->token);
        }
    }
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
    withdraw.value = 1.00;

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

void make_transfer(int sock_id)
{
    struct transfer transfer;
    bzero(&(transfer), sizeof(transfer));
    strcpy(transfer.token, acc_token);
    strcpy(transfer.destination_account_pix, secondary_acc_pix);
    transfer.value = 1.25;

    size_t size = METHOD_SIZE + sizeof(transfer);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, TRANSFER_METHOD);

    memcpy(data, method, METHOD_SIZE);
    memcpy(data + METHOD_SIZE, &(transfer), sizeof(transfer));

    send_message(sock_id, data, size);

    printf("Sent message\n");

    struct transfer_response *response = malloc(sizeof(struct transfer_response));
    bzero(response, sizeof(struct transfer_response));
    receive_message(sock_id, response, sizeof(struct transfer_response));

    printf("Success: %d\nResponse: %s\n\n", response->success, response->response);
}

void sign_in(int sock_id)
{
    struct login login;
    bzero(&(login), sizeof(login));
    strcpy(login.cpf, "11111111111");
    strcpy(login.password, "12345");

    size_t size = METHOD_SIZE + sizeof(login);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, LOGIN_METHOD);

    memcpy(data, method, METHOD_SIZE);
    memcpy(data + METHOD_SIZE, &(login), sizeof(login));

    send_message(sock_id, data, size);

    printf("Sent message\n");

    struct login_response *response = malloc(sizeof(struct login_response));
    bzero(response, sizeof(struct login_response));
    receive_message(sock_id, response, sizeof(struct login_response));

    printf("Response: %s\n", response->response);
    printf("Success: %i\n", response->success);
    printf("Token: %s\n\n", response->token);

    if (response->success)
    {
        strcpy(acc_token, response->token);
    }
}

void check_info(int sock_id)
{
    struct account_info info;
    bzero(&(info), sizeof(info));
    strcpy(info.token, acc_token);

    size_t size = METHOD_SIZE + sizeof(info);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, ACCOUNT_INFO_METHOD);

    memcpy(data, method, METHOD_SIZE);
    memcpy(data + METHOD_SIZE, &(info), sizeof(info));

    send_message(sock_id, data, size);

    printf("Sent message\n");

    struct account_info_response *response = malloc(sizeof(struct account_info_response));
    bzero(response, sizeof(struct account_info_response));
    receive_message(sock_id, response, sizeof(struct account_info_response));

    printf("Response: %s\n", response->response);
    printf("Success: %i\n", response->success);
    printf("Name: %s\n", response->name);
    printf("Pix: %s\n", response->pix);
    printf("Balance: %s\n\n", response->balance);
}

void list_accounts(int sock_id, unsigned int page)
{
    struct list_account info;
    bzero(&(info), sizeof(info));
    info.page = page;

    size_t size = METHOD_SIZE + sizeof(info);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, LIST_ACCOUNTS_METHOD);

    memcpy(data, method, METHOD_SIZE);
    memcpy(data + METHOD_SIZE, &(info), sizeof(info));

    send_message(sock_id, data, size);

    printf("Sent message\n");

    struct list_account_response *response = malloc(sizeof(struct list_account_response));
    bzero(response, sizeof(struct list_account_response));
    receive_message(sock_id, response, sizeof(struct list_account_response));

    printf("Page index: %u\n", response->page_index);
    printf("Page size: %u\n", response->page_size);
    printf("Current page size: %u\n", response->current_page_size);
    printf("Total count: %u\n", response->total_count);

    for (int i = 0; i < response->current_page_size; i++)
    {
        printf("%i: %s\n", i, response->accounts[i]);
    }

    printf("\n");
}

int main()
{
    srand(time(NULL));

    int sock_id = create_socket();
    connect_socket(sock_id);
    create_account(sock_id, 0);

    sock_id = create_socket();
    connect_socket(sock_id);
    create_account(sock_id, 1);

    // sock_id = create_socket();
    // connect_socket(sock_id);
    // sign_in(sock_id);

    sock_id = create_socket();
    connect_socket(sock_id);
    check_info(sock_id);

    sock_id = create_socket();
    connect_socket(sock_id);
    make_deposit(sock_id);

    sock_id = create_socket();
    connect_socket(sock_id);
    check_info(sock_id);

    sock_id = create_socket();
    connect_socket(sock_id);
    make_withdraw(sock_id);

    sock_id = create_socket();
    connect_socket(sock_id);
    check_info(sock_id);

    sock_id = create_socket();
    connect_socket(sock_id);
    make_transfer(sock_id);

    sock_id = create_socket();
    connect_socket(sock_id);
    check_info(sock_id);

    sock_id = create_socket();
    connect_socket(sock_id);
    list_accounts(sock_id, 0);
    return 0;
}