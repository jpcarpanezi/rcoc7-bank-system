typedef struct deposit
{
    double value;
    char token[37];
} deposit;

typedef struct withdraw
{
    double value;
    char token[37];
} withdraw;

typedef struct transfer
{
    double value;
    char token[37];
    char destination_account_pix[37];
} transfer;

typedef struct list_bank_statement
{
    char token[37];
    unsigned int page;
} list_bank_statement;

struct response make_deposit(void *info_ptr);
struct response make_withdraw(void *info_ptr);
struct response make_transfer(void *info_ptr);
struct response get_bank_statement(void *info_ptr);