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
 * @brief Parâmetros para criação de uma nova conta
 *
 */
typedef struct new_account
{
    char name[100];
    char cpf[12];
    char password[50];
} new_account;

/**
 * @brief Parâmetros de resposta para criação de uma nova conta
 *
 */
typedef struct new_account_response
{
    char pix[37];
    char token[37];
    char response[100];
    int success;
} new_account_response;

/**
 * @brief Parâmetros para realizar login
 *
 */
typedef struct login
{
    char cpf[12];
    char password[50];
} login;

/**
 * @brief Parâmetros de resposta do login
 *
 */
typedef struct login_response
{
    char token[37];
    char response[100];
    int success;
} login_response;

/**
 * @brief Parâmetros para solicitar informações sobre a conta
 *
 */
typedef struct account_info
{
    char token[37];
} account_info;

/**
 * @brief Parâmetros de resposta com informações sobre a conta
 *
 */
typedef struct account_info_response
{
    char name[100];
    char pix[37];
    char balance[50];
    char response[100];
    int success;
} account_info_response;

/**
 * @brief Parâmetros para solicitar a lista de contas
 *
 */
typedef struct list_account
{
    unsigned int page;
} list_account;

/**
 * @brief Parâmetros de resposta da lista de contas
 *
 */
typedef struct list_account_response
{
    unsigned int page_index;
    unsigned int current_page_size;
    unsigned int total_count;
    char accounts[PAGE_SIZE][37];
} list_account_response;

char *create_account()
{
    struct new_account acc;
    bzero(&(acc), sizeof(acc));

    printf("Insira seu CPF (sem pontuação): ");
    scanf("%s", acc.cpf);

    printf("Insira seu nome: ");
    scanf("%s", acc.name);

    printf("Insira sua senha: ");
    scanf("%s", acc.password);

    // Aloca o espaço necessário para enviar o nome do método + struct new_account
    size_t size = METHOD_SIZE + sizeof(acc);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    // Aloca o espaço necessário e copia o nome do método
    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, REGISTER_METHOD);

    // Copia o nome do método e a struct para a variável data
    memcpy(data, method, METHOD_SIZE);
    memcpy(data + METHOD_SIZE, &(acc), sizeof(acc));

    int sock_id = create_socket();
    connect_socket(sock_id, server_ip);

    send_message(sock_id, data, size);

    // Aloca o espaço necessário para a struct de resposta new_account_response
    struct new_account_response *response = malloc(sizeof(struct new_account_response));
    bzero(response, sizeof(struct new_account_response));

    receive_message(sock_id, response, sizeof(struct new_account_response));

    if (response->success)
    {
        printf("Conta criada com sucesso! Sua chave pix é: %s\n", response->pix);
        return response->token;
    }

    printf("Ocorreu um erro ao criar a conta: %s\n", response->response);
    return NULL;
}

char *sign_in()
{
    struct login login;
    bzero(&(login), sizeof(login));

    printf("Insira seu CPF (sem pontuação): ");
    scanf("%s", login.cpf);

    printf("Insira sua senha: ");
    scanf("%s", login.password);

    // Aloca o espaço necessário para enviar o nome do método + struct login
    size_t size = METHOD_SIZE + sizeof(login);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    // Aloca o espaço necessário e copia o nome do método
    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, LOGIN_METHOD);

    // Copia o nome do método e a struct para a variável data
    memcpy(data, method, METHOD_SIZE);
    memcpy(data + METHOD_SIZE, &(login), sizeof(login));

    int sock_id = create_socket();
    connect_socket(sock_id, server_ip);

    send_message(sock_id, data, size);

    // Aloca o espaço necessário para a struct de resposta login_response
    struct login_response *response = malloc(sizeof(struct login_response));
    bzero(response, sizeof(struct login_response));
    receive_message(sock_id, response, sizeof(struct login_response));

    if (response->success)
    {
        printf("Login bem sucedido!\n");
        return response->token;
    }

    printf("Ocorreu um erro ao fazer o login: %s\n", response->response);
    return NULL;
}

void list_accounts()
{
    struct list_account info;
    bzero(&(info), sizeof(info));

    // Aloca o espaço necessário para enviar o nome do método + struct list_account
    size_t size = METHOD_SIZE + sizeof(info);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    // Aloca o espaço necessário e copia o nome do método
    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, LIST_ACCOUNTS_METHOD);

    int page = 0;
    do
    {
        info.page = page;

        // Copia o nome do método e a struct para a variável data
        memcpy(data, method, METHOD_SIZE);
        memcpy(data + METHOD_SIZE, &(info), sizeof(info));

        int sock_id = create_socket();
        connect_socket(sock_id, server_ip);
        
        send_message(sock_id, data, size);

        // Aloca o espaço necessário para a struct de resposta list_account_response
        struct list_account_response *response = malloc(sizeof(struct list_account_response));
        bzero(response, sizeof(struct list_account_response));

        receive_message(sock_id, response, sizeof(struct list_account_response));

        printf("Página atual: %u\n", response->page_index);
        printf("Contas na página atual: %u\n", response->current_page_size);
        printf("Total de contas: %u\n", response->total_count);

        if (response->current_page_size)
        {
            printf("Contas:\n");
        }
        else
        {
            printf("Nenhuma conta na página atual\n");
        }

        for (int i = 0; i < response->current_page_size; i++)
        {
            printf("%i: %s\n", (i + 1), response->accounts[i]);
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

void show_account_info(char *account_token)
{
    struct account_info info;
    bzero(&(info), sizeof(info));
    strcpy(info.token, account_token);

    // Aloca o espaço necessário para enviar o nome do método + struct account_info
    size_t size = METHOD_SIZE + sizeof(info);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    // Aloca o espaço necessário e copia o nome do método
    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, ACCOUNT_INFO_METHOD);

    // Copia o nome do método e a struct para a variável data
    memcpy(data, method, METHOD_SIZE);
    memcpy(data + METHOD_SIZE, &(info), sizeof(info));

    int sock_id = create_socket();
    connect_socket(sock_id, server_ip);

    send_message(sock_id, data, size);

    // Aloca o espaço necessário para a struct de resposta account_info_response
    struct account_info_response *response = malloc(sizeof(struct account_info_response));
    bzero(response, sizeof(struct account_info_response));

    receive_message(sock_id, response, sizeof(struct account_info_response));

    if (response->success)
    {
        printf("%s\n", response->response);
        printf("Nome: %s\n", response->name);
        printf("Chave Pix: %s\n", response->pix);
        printf("Saldo: %s\n", response->balance);
    }
    else
    {
        printf("Não foi possível carregar as informações da conta: %s\n", response->response);
    }
}