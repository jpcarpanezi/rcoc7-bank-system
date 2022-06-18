#include <stdlib.h>

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

typedef struct login {
    char cpf[12];
    char password[50];
} login;

struct response create_account(void *info_ptr);
struct response sign_in(void *info_ptr);