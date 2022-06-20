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

struct response make_deposit(void *info_ptr);
struct response make_withdraw(void *info_ptr);
struct response make_transfer(void *info_ptr);