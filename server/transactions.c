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

struct response make_deposit(void *info_ptr)
{
    struct deposit *info = (struct deposit *)info_ptr;

    struct deposit_response *res = malloc(sizeof(struct deposit_response));
    bzero(res, sizeof(struct deposit_response));
    struct response final_res;

    final_res.response_str = res;
    final_res.response_size = sizeof(struct deposit_response);

    struct account *acc = find_account_by_token(info->token);
    if (acc == NULL)
    {
        res->success = 0;
        strcpy(res->response, "Conta não encontrada");
        return final_res;
    }

    int value = (int)(info->value * 100);

    if (value < 0)
    {
        res->success = 0;
        strcpy(res->response, "Valor inválido");
        return final_res;
    }

    acc->balance += value;

    double balance = ((double)acc->balance) / 100;
    char balance_str[50];
    snprintf(balance_str, sizeof(balance_str), "%.2f", balance);

    res->success = 1;

    char response[100] = "Depósito realizado com sucesso, balanço atual: R$ ";
    strcpy(res->response, strcat(response, balance_str));

    return final_res;
}

struct response make_withdraw(void *info_ptr)
{
    struct withdraw *info = (struct withdraw *)info_ptr;

    struct withdraw_response *res = malloc(sizeof(struct withdraw_response));
    bzero(res, sizeof(struct withdraw_response));
    struct response final_res;

    final_res.response_str = res;
    final_res.response_size = sizeof(struct withdraw_response);

    struct account *acc = find_account_by_token(info->token);
    if (acc == NULL)
    {
        res->success = 0;
        strcpy(res->response, "Conta não encontrada");
        return final_res;
    }

    int value = (int)(info->value * 100);

    if (value < 0)
    {
        res->success = 0;
        strcpy(res->response, "Valor inválido");
        return final_res;
    }

    if (value > acc->balance)
    {
        res->success = 0;
        strcpy(res->response, "Valor informado maior que o saldo disponível");
        return final_res;
    }

    acc->balance -= value;

    double balance = ((double)acc->balance) / 100;
    char balance_str[50];
    snprintf(balance_str, sizeof(balance_str), "%.2f", balance);

    res->success = 1;

    char response[100] = "Saque realizado com sucesso, balanço atual: R$ ";
    strcpy(res->response, strcat(response, balance_str));

    return final_res;
}

struct response make_transfer(void *info_ptr)
{
    struct transfer *info = (struct transfer *)info_ptr;

    struct transfer_response *res = malloc(sizeof(struct transfer_response));
    bzero(res, sizeof(struct transfer_response));
    struct response final_res;

    final_res.response_str = res;
    final_res.response_size = sizeof(struct transfer_response);

    struct account *acc = find_account_by_token(info->token);
    if (acc == NULL)
    {
        res->success = 0;
        strcpy(res->response, "Conta não encontrada");
        return final_res;
    }

    struct account *destination_acc = find_account_by_pix(info->destination_account_pix);
    if (destination_acc == NULL)
    {
        res->success = 0;
        strcpy(res->response, "Conta destino não encontrada");
        return final_res;
    }

    int value = (int)(info->value * 100);

    if (value < 0)
    {
        res->success = 0;
        strcpy(res->response, "Valor inválido");
        return final_res;
    }

    if (value > acc->balance)
    {
        res->success = 0;
        strcpy(res->response, "Valor informado maior que o saldo disponível");
        return final_res;
    }

    acc->balance -= value;
    destination_acc->balance += value;

    double balance = ((double)acc->balance) / 100;
    char balance_str[50];
    snprintf(balance_str, sizeof(balance_str), "%.2f", balance);

    res->success = 1;

    char response[100] = "Transferência realizada com sucesso, balanço atual: R$ ";
    strcpy(res->response, strcat(response, balance_str));

    return final_res;
}