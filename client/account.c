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

void create_account()
{
    struct new_account acc;
    bzero(&(acc), sizeof(acc));
    strcpy(acc.cpf, "11111111111");
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

    int sock_id = create_socket();
    connect_socket(sock_id, server_ip);
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

char *sign_in()
{
    struct login login;
    bzero(&(login), sizeof(login));

    printf("Digite seu CPF (sem pontuação): ");
    scanf("%s", login.cpf);

    printf("Digite sua senha: ");
    scanf("%s", login.password);

    int sock_id = create_socket();
    connect_socket(sock_id, server_ip);

    size_t size = METHOD_SIZE + sizeof(login);
    void *data = malloc(size);
    bzero(data, METHOD_SIZE);

    char *method = malloc(METHOD_SIZE);
    bzero(method, METHOD_SIZE);
    strcpy(method, LOGIN_METHOD);

    memcpy(data, method, METHOD_SIZE);
    memcpy(data + METHOD_SIZE, &(login), sizeof(login));

    send_message(sock_id, data, size);

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