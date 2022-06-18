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

typedef struct response
{
    void *response_str;
    size_t response_size;
} response;

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

typedef struct account
{
    char pix[37];
    char name[100];
    char cpf[12];
    int balance;
    char password[50];
    char token[37];
    struct account *next;
} account;

struct account *head = NULL;

char *gen_uuid()
{
    char v[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    // 8 dash 4 dash 4 dash 4 dash 12
    static char buf[37] = {0};

    // gen random for all spaces because lazy
    for (int i = 0; i < 36; ++i)
    {
        buf[i] = v[rand() % 16];
    }

    // put dashes in place
    buf[8] = '-';
    buf[13] = '-';
    buf[18] = '-';
    buf[23] = '-';

    // needs end byte
    buf[36] = '\0';

    return buf;
}

int validate_cpf(char cpf[])
{
    if (strlen(cpf) != 11)
    {
        return 0;
    }

    int num_arr[11];

    for (int i = 0; i < 11; i++)
    {
        if (cpf[i] < '0' || cpf[i] > '9')
        {
            return 0;
        }
        num_arr[i] = (int)cpf[i] - '0';
    }

    for (int i = 9; i < 11; i++)
    {
        int j = 0, k = 0;
        for (; k < i; k++)
        {
            j += num_arr[k] * (i + 1 - k);
        }

        j = 10 * j % 11 % 10;

        if (num_arr[k] != j)
        {
            return 0;
        }
    }

    return 1;
}

struct account *find_account_by_cpf(char cpf[])
{
    struct account *acc = head;
    struct account *search_acc = NULL;
    while (acc != NULL)
    {
        if (strcmp(cpf, acc->cpf) == 0)
        {
            search_acc = acc;
            break;
        }
        acc = head->next;
    }

    return search_acc;
}

struct account *find_account_by_pix(char pix[])
{
    struct account *acc = head;
    struct account *search_acc = NULL;
    while (acc != NULL)
    {
        if (strcmp(pix, acc->pix) == 0)
        {
            search_acc = acc;
            break;
        }
        acc = head->next;
    }

    return search_acc;
}

struct account *find_account_by_token(char token[])
{
    struct account *acc = head;
    struct account *search_acc = NULL;
    while (acc != NULL)
    {
        if (strcmp(token, acc->token) == 0)
        {
            search_acc = acc;
            break;
        }
        acc = head->next;
    }

    return search_acc;
}

struct response create_account(void *info_ptr)
{
    struct new_account *info = (struct new_account *)info_ptr;

    struct new_account_response res;
    bzero(&(res), sizeof(res));
    struct response final_res;

    final_res.response_str = &(res);
    final_res.response_size = sizeof(res);

    if (!validate_cpf(info->cpf))
    {
        printf("Create account request failed, invalid CPF\n");

        res.success = 0;
        strcpy(res.response, "CPF inválido");
        return final_res;
    }

    if (find_account_by_cpf(info->cpf))
    {
        printf("Create account request failed, CPF already exists\n");

        res.success = 0;
        strcpy(res.response, "CPF já cadastrado");
        return final_res;
    }

    if (strlen(info->password) <= 4)
    {
        printf("Create account request failed, weak password\n");

        res.success = 0;
        strcpy(res.response, "Senha muito fraca");
        return final_res;
    }

    if (strlen(info->name) == 0)
    {
        printf("Create account request failed, empty name\n");

        res.success = 0;
        strcpy(res.response, "Nome vazio");
        return final_res;
    }

    struct account *new_account = malloc(sizeof(struct account));
    bzero(new_account, sizeof(struct account));
    strcpy(new_account->name, info->name);
    strcpy(new_account->cpf, info->cpf);
    strcpy(new_account->password, info->password);

    int retry_count = 0;
    do
    {
        strcpy(new_account->pix, gen_uuid());
        retry_count++;

        if (retry_count >= 5)
        {
            printf("Create account request failed, could not generate Pix UUID\n");

            res.success = 0;
            strcpy(res.response, "Não foi possível realizar o cadastro");
            return final_res;
        }
    } while (find_account_by_pix(new_account->pix) != NULL);

    retry_count = 0;
    do
    {
        strcpy(new_account->token, gen_uuid());
        retry_count++;

        if (retry_count >= 5)
        {
            printf("Create account request failed, could not generate Token UUID\n");

            res.success = 0;
            strcpy(res.response, "Não foi possível realizar o cadastro");
            return final_res;
        }
    } while (find_account_by_token(new_account->token) != NULL);

    if (head == NULL)
    {
        head = new_account;
    }
    else
    {
        struct account *last = head;
        while (last->next != NULL)
        {
            last = last->next;
        }

        last->next = new_account;
    }

    res.success = 1;
    strcpy(res.response, "Conta cadastrada com sucesso");
    strcpy(res.pix, new_account->pix);
    strcpy(res.token, new_account->token);

    printf("New account ID %s created\n", new_account->pix);
    return final_res;
}

struct response sign_in(void *info_ptr)
{
    struct login *login = (struct login *)info_ptr;

    struct login_response res;
    bzero(&(res), sizeof(res));
    struct response final_res;

    final_res.response_str = &(res);
    final_res.response_size = sizeof(res);

    struct account *acc = find_account_by_cpf(login->cpf);

    if (acc == NULL)
    {
        printf("Login attempt error, CPF not found\n");
        res.success = 0;
        strcpy(res.response, "Credenciais invalidas");

        return final_res;
    }

    if (strcmp(acc->password, login->password) != 0)
    {
        printf("Invalid password on login attempt in account ID %s\n", acc->pix);
        res.success = 0;
        strcpy(res.response, "Credenciais invalidas");

        return final_res;
    }

    char *uuid = gen_uuid();
    res.success = 1;
    strcpy(res.response, "Login realizado com sucesso");
    strcpy(res.token, gen_uuid());
    strcpy(acc->token, uuid);
    printf("Successfuly login in account ID %s\n", acc->pix);

    return final_res;
}