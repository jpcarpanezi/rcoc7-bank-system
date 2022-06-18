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

struct response make_deposit(void *info_ptr);
struct response make_withdraw(void *info_ptr);