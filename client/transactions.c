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
 * @brief Enum com os tipos de transações
 * 
 */
typedef enum
{
    Deposit,
    Withdraw,
    Transfer
} transaction_type;

/**
 * @brief Parâmetros para realização de um depósito
 * 
 */
typedef struct deposit
{
    double value;
    char token[37];
} deposit;

/**
 * @brief Parâmetros de resposta para realização de um depósito
 * 
 */
typedef struct deposit_response
{
    char response[100];
    int success;
} deposit_response;

/**
 * @brief Parâmetros para realização de um saque
 * 
 */
typedef struct withdraw
{
    double value;
    char token[37];
} withdraw;


/**
 * @brief Parâmetros de resposta para realização de um saque
 * 
 */
typedef struct withdraw_response
{
    char response[100];
    int success;
} withdraw_response;

/**
 * @brief Parâmetros para realização de uma transferência
 * 
 */
typedef struct transfer
{
    double value;
    char token[37];
    char destination_account_pix[37];
} transfer;

/**
 * @brief Parâmetros de resposta para realização de uma transferência
 * 
 */
typedef struct transfer_response
{
    char response[100];
    int success;
} transfer_response;

/**
 * @brief Lista encadeada com informações de extrato
 * 
 */
typedef struct bank_statement
{
    char destination_account_pix[37];
    char origin_account_pix[37];
    int value;
    transaction_type type;
    struct bank_statement *next;
} bank_statement;

/**
 * @brief Parâmetros para solicitar uma lista de extrato
 * 
 */
typedef struct list_bank_statement
{
    char token[37];
    unsigned int page;
} list_bank_statement;

/**
 * @brief Parâmetros de resposta para solicitar uma lista de extrato
 * 
 */
typedef struct list_bank_statement_response
{
    unsigned int page_index;
    unsigned int current_page_size;
    unsigned int total_count;
    char value[PAGE_SIZE][100];
} list_bank_statemente_response;