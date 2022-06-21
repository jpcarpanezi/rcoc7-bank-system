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
#include "methods.h"

/* TEST FUNCTIONS
void create_account(int sock_id, int secondary)
{
    struct new_account acc;
    bzero(&(acc), sizeof(acc));
    strcpy(acc.cpf, "111111111111");
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

void get_bank_statement(int sock_id, unsigned int page)
{
    struct list_bank_statement info;
    bzero(&(info), sizeof(info));
    info.page = page;
    strcpy(info.token, acc_token);

    size_t size = METHOD_SIZE + sizeof(info);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, LIST_BANK_STATEMENT_METHOD);

    memcpy(data, method, METHOD_SIZE);
    memcpy(data + METHOD_SIZE, &(info), sizeof(info));

    send_message(sock_id, data, size);

    printf("Sent message asdasdas\n");

    struct list_bank_statement_response *response = malloc(sizeof(struct list_bank_statement_response));
    bzero(response, sizeof(struct list_bank_statement_response));
    receive_message(sock_id, response, sizeof(struct list_bank_statement_response));

    printf("Page index: %u\n", response->page_index);
    printf("Current page size: %u\n", response->current_page_size);
    printf("Total count: %u\n", response->total_count);

    for (int i = 0; i < response->current_page_size; i++)
    {
        printf("%i: %s\n", i, response->value[i]);
    }

    printf("\n");
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
    printf("Current page size: %u\n", response->current_page_size);
    printf("Total count: %u\n", response->total_count);

    for (int i = 0; i < response->current_page_size; i++)
    {
        printf("%i: %s\n", i, response->accounts[i]);
    }

    printf("\n");
}
*/


void abrir_menu()
{
    char account_token[37];
    int logged_in = 0;

    while (1)
    {
        printf("\n********************** m e n u *******************\n");
        printf("|                                                 |\n");
        printf("| Conta:                                          |\n");
        printf("|   1 - Cadastrar conta                           |\n");
        printf("|   2 - Realizar login                            |\n");
        printf("|   3 - Listar contas                             |\n");
        printf("|   4 - Exibir informações da conta               |\n");
        printf("|                                                 |\n");
        printf("| Transações:                                     |\n");
        printf("|   5 - Depósito                                  |\n");
        printf("|   6 - Saque                                     |\n");
        printf("|   7 - Transferência                             |\n");
        printf("|   8 - Verificar extrato                         |\n");
        printf("|                                                 |\n");
        printf("| Outros:                                         |\n");
        printf("|   9 - Sair                                      |\n");
        printf("|                                                 |\n");
        printf("***************************************************\n");
        printf("Selecione uma opção: ");

        int choice;
        scanf("%i", &choice);

        printf("\n");

        if (logged_in == 0)
        {
            int fail = 0;
            const int logged_in_methods[] = {4, 5, 6, 7, 8};
            for (int i = 0; i < sizeof(logged_in_methods) / sizeof(logged_in_methods[0]); i++)
            {
                if (logged_in_methods[i] == choice)
                {
                    printf("É necessário fazer login para realizar esta função\n");
                    fail = 1;
                    break;
                }
            }

            if (fail)
            {
                continue;
            }
        }

        switch (choice)
        {
        case 1: // Cadastrar conta
            create_account();
            break;
        case 2: // Realizar login
            char *token = sign_in();
            if (token != NULL)
            {
                strcpy(account_token, token);
                printf("Token: %s", account_token);
            }
            break;
        case 3: // Listar contas
            
            break;
        case 4: // Exibir informações da conta
            
            break;
        case 5: // Depósito
            
            break;
        case 6: // Saque
            
            break;
        case 7: // Transferência
            
            break;
        case 8: // Verificar extrato
            
            break;
        case 9: // Sair
            exit(0);
            break;
        default:
            printf("Opção inválida. Tente novamente:\n");
            break;
        }
    }
}

int main()
{
    printf("Digite o endereço IP do servidor, ou digite 1 para utilizar 127.0.0.1:\n");
    
    scanf("%s", server_ip);

    if (strcmp("1", server_ip) == 0)
    {
        strcpy(server_ip, "127.0.0.1");
    }

    abrir_menu();

    return 0;
}