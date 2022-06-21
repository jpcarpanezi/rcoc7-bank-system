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

// Início da lista encadeada de extrato
struct bank_statement *bank_statement_head = NULL;

/**
 * @brief Adiciona ao extrato uma transação realizada
 * 
 * @param destination_account_pix Conta de origem da transação
 * @param origin_account_pix Conta de destino da transação (apenas em transferências)
 * @param value Valor da transação
 * @param type Enum contendo o tipo da transação
 */
void add_to_bank_statement(char destination_account_pix[], char origin_account_pix[], int value, transaction_type type)
{
    // Aloca o espaço necessário para a struct de resposta bank_statement
    struct bank_statement *new_bank_statement = malloc(sizeof(struct bank_statement));
    bzero(new_bank_statement, sizeof(struct bank_statement));

    // Copia as informações da transação
    new_bank_statement->value = value;
    new_bank_statement->type = type;
    strcpy(new_bank_statement->destination_account_pix, destination_account_pix);
    strcpy(new_bank_statement->origin_account_pix, origin_account_pix);

    if (bank_statement_head == NULL)
    {
        bank_statement_head = new_bank_statement;
    }
    else
    {
        struct bank_statement *last = bank_statement_head;

        while (last->next != NULL)
        {
            last = last->next;
        }

        // Insere a transação no final da lista encadeada
        last->next = new_bank_statement;
    }

    return;
}

/**
 * @brief Get the bank statement object
 * 
 * @param info_ptr Ponteiro com os parâmetros para busca de acordo com a struct account_info
 * @return Struct contendo um ponteiro para a struct de resposta do tipo list_bank_statement_response 
 */
struct response get_bank_statement(void *info_ptr)
{
    // Realiza o cast de info_ptr para a struct list_bank_statement
    struct list_bank_statement *list = (struct list_bank_statement *)info_ptr;

    // Aloca o espaço necessário para a struct de resposta list_bank_statement_response
    struct list_bank_statement_response *res = malloc(sizeof(struct list_bank_statement_response));
    bzero(res, sizeof(struct list_bank_statement_response));
    struct response final_res;

    // Encontra a conta de acordo com o token informado
    struct account *acc = find_account_by_token(list->token);

    final_res.response_str = res;
    final_res.response_size = sizeof(struct list_bank_statement_response);

    if (acc == NULL)
    {
        return final_res;
    }

    res->page_index = list->page;
    res->current_page_size = 0;

    struct bank_statement *bs = bank_statement_head;
    unsigned int i = 0;

    // Faz um loop na lista encadeada
    while (bs != NULL)
    {
        unsigned int current_page = (unsigned int)ceil(i / PAGE_SIZE);

        // Verifica se a conta está presente na transação
        if (current_page == res->page_index && (strcmp(bs->destination_account_pix, acc->pix) == 0 || strcmp(bs->origin_account_pix, acc->pix) == 0))
        {
            // Valida se o usuário recebeu ou enviou o dinheiro
            double value = 0.0;
            switch (bs->type)
            {
            case Deposit:
                value += ((double)bs->value) / 100;
                break;
            case Withdraw:
                value -= ((double)bs->value) / 100;
                break;
            case Transfer:
                if (strcmp(bs->origin_account_pix, acc->pix) == 0)
                {
                    value -= ((double)bs->value) / 100;
                }
                else
                {
                    value += ((double)bs->value) / 100;
                }
                break;
            default:
                value += ((double)bs->value) / 100;
                break;
            }

            // Faz a formatação do valor e adiciona o prefixo R$
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

/**
 * @brief Realiza um depósito na conta
 * 
 * @param info_ptr Ponteiro com os parâmetros para busca de acordo com a struct account_info
 * @return Struct contendo um ponteiro para a struct de resposta do tipo deposit_response
 */
struct response make_deposit(void *info_ptr)
{
    // Realiza o cast de info_ptr para a struct deposit
    struct deposit *info = (struct deposit *)info_ptr;

    // Aloca o espaço necessário para a struct de resposta deposit_response
    struct deposit_response *res = malloc(sizeof(struct deposit_response));
    bzero(res, sizeof(struct deposit_response));
    struct response final_res;

    final_res.response_str = res;
    final_res.response_size = sizeof(struct deposit_response);

    // Encontra a conta de acordo com o token informado
    struct account *acc = find_account_by_token(info->token);
    if (acc == NULL)
    {
        res->success = 0;
        strcpy(res->response, "Conta não encontrada");
        return final_res;
    }

    int value = (int)(info->value * 100);

    // Verifica se o valor da transação é válido
    if (value < 0)
    {
        res->success = 0;
        strcpy(res->response, "Valor inválido");
        return final_res;
    }

    // Adiciona ao extrato bancário e adiciona o saldo na conta
    add_to_bank_statement(acc->pix, "", value, Deposit);
    acc->balance += value;

    // Converte o saldo para decimal
    double balance = ((double)acc->balance) / 100;
    char balance_str[50];
    snprintf(balance_str, sizeof(balance_str), "%.2f", balance);

    res->success = 1;

    char response[100] = "Depósito realizado com sucesso, balanço atual: R$ ";
    strcpy(res->response, strcat(response, balance_str));

    return final_res;
}

/**
 * @brief Realiza um saque na conta
 * 
 * @param info_ptr Ponteiro com os parâmetros para busca de acordo com a struct account_info
 * @return Struct contendo um ponteiro para a struct de resposta do tipo withdraw_response
 */
struct response make_withdraw(void *info_ptr)
{
    // Realiza o cast de info_ptr para a struct withdraw
    struct withdraw *info = (struct withdraw *)info_ptr;

    // Aloca o espaço necessário para a struct de resposta withdraw_response
    struct withdraw_response *res = malloc(sizeof(struct withdraw_response));
    bzero(res, sizeof(struct withdraw_response));
    struct response final_res;

    final_res.response_str = res;
    final_res.response_size = sizeof(struct withdraw_response);

    // Encontra a conta de acordo com o token informado
    struct account *acc = find_account_by_token(info->token);
    if (acc == NULL)
    {
        res->success = 0;
        strcpy(res->response, "Conta não encontrada");
        return final_res;
    }

    int value = (int)(info->value * 100);

    // Verifica se o valor da transação é válido
    if (value < 0)
    {
        res->success = 0;
        strcpy(res->response, "Valor inválido");
        return final_res;
    }

    // Verifica se há saldo disponível
    if (value > acc->balance)
    {
        res->success = 0;
        strcpy(res->response, "Valor informado maior que o saldo disponível");
        return final_res;
    }

    // Adiciona ao extrato bancário e debita o saldo da conta
    add_to_bank_statement(acc->pix, "", value, Withdraw);
    acc->balance -= value;

    // Converte o saldo para decimal
    double balance = ((double)acc->balance) / 100;
    char balance_str[50];
    snprintf(balance_str, sizeof(balance_str), "%.2f", balance);

    res->success = 1;

    char response[100] = "Saque realizado com sucesso, balanço atual: R$ ";
    strcpy(res->response, strcat(response, balance_str));

    return final_res;
}

/**
 * @brief Realiza uma transferência
 * 
 * @param info_ptr Ponteiro com os parâmetros para busca de acordo com a struct account_info
 * @return Struct contendo um ponteiro para a struct de resposta do tipo transfer_response 
 */
struct response make_transfer(void *info_ptr)
{
    // Realiza o cast de info_ptr para a struct transfer
    struct transfer *info = (struct transfer *)info_ptr;

    // Aloca o espaço necessário para a struct de resposta transfer_response
    struct transfer_response *res = malloc(sizeof(struct transfer_response));
    bzero(res, sizeof(struct transfer_response));
    struct response final_res;

    final_res.response_str = res;
    final_res.response_size = sizeof(struct transfer_response);

    // Encontra a conta de acordo com o token informado
    struct account *acc = find_account_by_token(info->token);
    if (acc == NULL)
    {
        res->success = 0;
        strcpy(res->response, "Conta não encontrada");
        return final_res;
    }

    // Encontra a conta de acordo com o pix informado
    struct account *destination_acc = find_account_by_pix(info->destination_account_pix);
    if (destination_acc == NULL)
    {
        res->success = 0;
        strcpy(res->response, "Conta destino não encontrada");
        return final_res;
    }

    // Converte o valor para inteiro
    int value = (int)(info->value * 100);

    // Verifica se é um valor válido
    if (value < 0)
    {
        res->success = 0;
        strcpy(res->response, "Valor inválido");
        return final_res;
    }

    // Verifica se há saldo disponível
    if (value > acc->balance)
    {
        res->success = 0;
        strcpy(res->response, "Valor informado maior que o saldo disponível");
        return final_res;
    }

    // Adiciona ao extrato bancário, adiciona o saldo na conta destino e debita da conta de origem
    add_to_bank_statement(destination_acc->pix, acc->pix, value, Deposit);
    acc->balance -= value;
    destination_acc->balance += value;

    // Converte o saldo para decimal
    double balance = ((double)acc->balance) / 100;
    char balance_str[50];
    snprintf(balance_str, sizeof(balance_str), "%.2f", balance);

    res->success = 1;

    char response[100] = "Transferência realizada com sucesso, balanço atual: R$ ";
    strcpy(res->response, strcat(response, balance_str));

    return final_res;
}