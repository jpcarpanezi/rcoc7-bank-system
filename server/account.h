#include <stdlib.h>

typedef struct new_account
{
    char name[100];
    char cpf[12];
    char password[50];
} new_account;

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

typedef struct login
{
    char cpf[12];
    char password[50];
} login;

typedef struct account_info
{
    char token[37];
} account_info;

typedef struct response
{
    void *response_str;
    size_t response_size;
} response;

struct response create_account(void *info_ptr);
struct account *find_account_by_cpf(char cpf[]);
struct account *find_account_by_pix(char pix[]);
struct account *find_account_by_token(char token[]);
struct response sign_in(void *info_ptr);
struct response check_info(void *info_ptr);