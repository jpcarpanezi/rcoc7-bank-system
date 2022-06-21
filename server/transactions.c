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
#include <math.h>

#define PAGE_SIZE 10

typedef enum {
    Deposit,
    Withdraw,
    Transfer
} transaction_type;

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

typedef struct bank_statement {
    char destination_account_pix[37];
    char origin_account_pix[37];
    int value;
    transaction_type type;
    struct bank_statement *next;
} bank_statement;

typedef struct list_bank_statement {
    char token[37];
    unsigned int page;
} list_bank_statement;

typedef struct list_bank_statement_response {
    unsigned int page_index;
    unsigned int current_page_size;
    unsigned int total_count;
    char value[PAGE_SIZE][100];
} list_bank_statemente_response;

struct bank_statement *bank_statement_head = NULL;

void add_to_bank_statement(char destination_account_pix[], char origin_account_pix[], int value, transaction_type type) {
    struct bank_statement *new_bank_statement = malloc(sizeof(struct bank_statement));
    bzero(new_bank_statement, sizeof(struct bank_statement));

    new_bank_statement->value = value;
    new_bank_statement->type = type;
    strcpy(new_bank_statement->destination_account_pix, destination_account_pix);
    strcpy(new_bank_statement->origin_account_pix, origin_account_pix);

    if (bank_statement_head == NULL) {
        bank_statement_head = new_bank_statement;
    } else {
        struct bank_statement *last = bank_statement_head;
        
        while (last->next != NULL) {
            last = last->next;
        }

        last->next = new_bank_statement;
    }

    return;
}

struct response get_bank_statement(void *info_ptr) {
    struct list_bank_statement *list = (struct list_bank_statement *)info_ptr;

    struct list_bank_statement_response *res = malloc(sizeof(struct list_bank_statement_response));
    bzero(res, sizeof(struct list_bank_statement_response));
    struct response final_res;

    struct account *acc = find_account_by_token(list->token);

    final_res.response_str = res;
    final_res.response_size = sizeof(struct list_bank_statement_response);

    res->page_index = list->page;
    res->current_page_size = 0;

    struct bank_statement *bs = bank_statement_head;
    unsigned int i = 0;

    while (bs != NULL) {
        unsigned int current_page = (unsigned int)ceil(i / PAGE_SIZE);

        if (current_page == res->page_index && (strcmp(bs->destination_account_pix, acc->pix) == 0 || strcmp(bs->origin_account_pix, acc->pix) == 0)) {
            double value = 0.0;
            switch (bs->type) {
                case Deposit:
                    value += ((double)bs->value) / 100;
                    break;
                case Withdraw:
                    value -= ((double)bs->value) / 100;
                    break;
                case Transfer:
                    if (strcmp(bs->origin_account_pix, acc->pix) == 0) {
                        value -= ((double)bs->value) / 100;
                    } else {
                        value += ((double)bs->value) / 100;
                    }
                    break;
                default:
                    value += ((double)bs->value) / 100;
                    break;
            }
            
            char value_str[50];
            snprintf(value_str, sizeof(value_str), "%.2f", value);
            char value_response[50] = "R$ ";
            strcpy(res->value[res->current_page_size], strcat(value_response, value_str));
            res->current_page_size++;
        }

        i++;
        bs = bs->next;
    }

    res->total_count = i;

    return final_res;
}

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

    add_to_bank_statement(acc->pix, "", value, Deposit);

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

    add_to_bank_statement(acc->pix, "", value, Withdraw);

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

    add_to_bank_statement(destination_acc->pix, acc->pix, value, Deposit);
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