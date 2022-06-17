#include <stdlib.h>

typedef struct new_account
{
    char name[100];
    char cpf[12];
    char senha[50];
} new_account;

typedef struct response
{
    void *response_str;
    size_t response_size;
} response;

struct response create_account(void *info_ptr);