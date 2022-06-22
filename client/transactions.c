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
#include "sockets.h"
#include "methods.h"

#define PAGE_SIZE 10

/**
 * @brief Enum com os tipos de transações
 * 
 */
typedef enum
{
    Deposit,
    Withdraw,
    Transfer
} transaction_type;

/**
 * @brief Parâmetros para realização de um depósito
 * 
 */
typedef struct deposit
{
    double value;
    char token[37];
} deposit;

/**
 * @brief Parâmetros de resposta para realização de um depósito
 * 
 */
typedef struct deposit_response
{
    char response[100];
    int success;
} deposit_response;

/**
 * @brief Parâmetros para realização de um saque
 * 
 */
typedef struct withdraw
{
    double value;
    char token[37];
} withdraw;


/**
 * @brief Parâmetros de resposta para realização de um saque
 * 
 */
typedef struct withdraw_response
{
    char response[100];
    int success;
} withdraw_response;

/**
 * @brief Parâmetros para realização de uma transferência
 * 
 */
typedef struct transfer
{
    double value;
    char token[37];
    char destination_account_pix[37];
} transfer;

/**
 * @brief Parâmetros de resposta para realização de uma transferência
 * 
 */
typedef struct transfer_response
{
    char response[100];
    int success;
} transfer_response;

/**
 * @brief Lista encadeada com informações de extrato
 * 
 */
typedef struct bank_statement
{
    char destination_account_pix[37];
    char origin_account_pix[37];
    int value;
    transaction_type type;
    struct bank_statement *next;
} bank_statement;

/**
 * @brief Parâmetros para solicitar uma lista de extrato
 * 
 */
typedef struct list_bank_statement
{
    char token[37];
    unsigned int page;
} list_bank_statement;

/**
 * @brief Parâmetros de resposta para solicitar uma lista de extrato
 * 
 */
typedef struct list_bank_statement_response
{
    unsigned int page_index;
    unsigned int current_page_size;
    unsigned int total_count;
    char value[PAGE_SIZE][100];
} list_bank_statemente_response;

void make_deposit(char *account_token)
{
    struct deposit deposit;
    bzero(&(deposit), sizeof(deposit));
    strcpy(deposit.token, account_token);

    printf("Digite o valor que deseja depositar: ");
    scanf("%lf", &(deposit.value));

    size_t size = METHOD_SIZE + sizeof(deposit);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, DEPOSIT_METHOD);

    memcpy(data, method, METHOD_SIZE);
    memcpy(data + METHOD_SIZE, &(deposit), sizeof(deposit));

    int sock_id = create_socket();
    connect_socket(sock_id, server_ip);
    send_message(sock_id, data, size);

    struct deposit_response *response = malloc(sizeof(struct deposit_response));
    bzero(response, sizeof(struct deposit_response));
    receive_message(sock_id, response, sizeof(struct deposit_response));

    printf("%s\n", response->response);
}

void make_withdraw(char *account_token)
{
    struct withdraw withdraw;
    bzero(&(withdraw), sizeof(withdraw));
    strcpy(withdraw.token, account_token);

    printf("Digite o valor que deseja sacar: ");
    scanf("%lf", &(withdraw.value));

    size_t size = METHOD_SIZE + sizeof(withdraw);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, WITHDRAW_METHOD);

    memcpy(data, method, METHOD_SIZE);
    memcpy(data + METHOD_SIZE, &(withdraw), sizeof(withdraw));

    int sock_id = create_socket();
    connect_socket(sock_id, server_ip);
    send_message(sock_id, data, size);

    struct withdraw_response *response = malloc(sizeof(struct withdraw_response));
    bzero(response, sizeof(struct withdraw_response));
    receive_message(sock_id, response, sizeof(struct withdraw_response));

    printf("%s\n", response->response);
}

void make_transfer(char *account_token)
{
    struct transfer transfer;
    bzero(&(transfer), sizeof(transfer));
    strcpy(transfer.token, account_token);

    printf("Digite o valor que deseja transferir: ");
    scanf("%lf", &(transfer.value));

    printf("Insira a conta para qual deseja transferir: ");
    scanf("%s", transfer.destination_account_pix);

    size_t size = METHOD_SIZE + sizeof(transfer);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, TRANSFER_METHOD);

    memcpy(data, method, METHOD_SIZE);
    memcpy(data + METHOD_SIZE, &(transfer), sizeof(transfer));

    int sock_id = create_socket();
    connect_socket(sock_id, server_ip);
    send_message(sock_id, data, size);

    struct transfer_response *response = malloc(sizeof(struct transfer_response));
    bzero(response, sizeof(struct transfer_response));
    receive_message(sock_id, response, sizeof(struct transfer_response));

    printf("%s\n", response->response);
}

void get_bank_statement(char *account_token)
{
    struct list_bank_statement info;
    bzero(&(info), sizeof(info));
    strcpy(info.token, account_token);

    size_t size = METHOD_SIZE + sizeof(info);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, LIST_BANK_STATEMENT_METHOD);

    int page = 0;

    do
    {
        info.page = page;

        memcpy(data, method, METHOD_SIZE);
        memcpy(data + METHOD_SIZE, &(info), sizeof(info));

        int sock_id = create_socket();
        connect_socket(sock_id, server_ip);
        send_message(sock_id, data, size);

        struct list_bank_statement_response *response = malloc(sizeof(struct list_bank_statement_response));
        bzero(response, sizeof(struct list_bank_statement_response));
        receive_message(sock_id, response, sizeof(struct list_bank_statement_response));

        printf("Página atual: %u\n", response->page_index);
        printf("Transações na página atual: %u\n", response->current_page_size);
        printf("Total de transações: %u\n", response->total_count);

        if (response->current_page_size)
        {
            printf("Extrato:\n");
        }
        else
        {
            printf("Nenhuma transação na página atual\n");
        }

        for (int i = 0; i < response->current_page_size; i++)
        {
            printf("%i: %s\n", (i + 1), response->value[i]);
        }

        printf("\nDigite o número da página desejada ou digite -1 para voltar ao menu: ");
        scanf("%i", &(page));

        printf("\n");

        if (page < -1)
        {
            page = -1;
        }
    } while (page != -1);
}