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
#include "account.h"
#include "transactions.h"
#include "methods.h"

void abrir_menu()
{
    char account_token[37];
    int logged_in = 0;

    while (1)
    {
        printf("\n********************** m e n u *******************\n");
        printf("|                                                 |\n");
        printf("| Conta:                                          |\n");
        printf("|   1 - Cadastrar conta                           |\n");
        printf("|   2 - Realizar login                            |\n");
        printf("|   3 - Listar contas                             |\n");
        printf("|   4 - Exibir informações da conta               |\n");
        printf("|                                                 |\n");
        printf("| Transações:                                     |\n");
        printf("|   5 - Depósito                                  |\n");
        printf("|   6 - Saque                                     |\n");
        printf("|   7 - Transferência                             |\n");
        printf("|   8 - Verificar extrato                         |\n");
        printf("|                                                 |\n");
        printf("| Outros:                                         |\n");
        printf("|   9 - Sair                                      |\n");
        printf("|                                                 |\n");
        printf("***************************************************\n");
        printf("Selecione uma opção: ");

        int choice;
        scanf("%i", &choice);

        printf("\n");

        // Verifica se é necessário estar logado para usar a função selecionada
        if (logged_in == 0)
        {
            int fail = 0;
            const int logged_in_methods[] = {4, 5, 6, 7, 8};
            for (int i = 0; i < sizeof(logged_in_methods) / sizeof(logged_in_methods[0]); i++)
            {
                if (logged_in_methods[i] == choice)
                {
                    printf("É necessário fazer login para realizar esta função\n");
                    fail = 1;
                    break;
                }
            }

            if (fail)
            {
                continue;
            }
        }

        switch (choice)
        {
        case 1: // Cadastrar conta
            char *token = create_account();
            if (token != NULL)
            {
                strcpy(account_token, token);
                logged_in = 1;
            }
            break;
        case 2: // Realizar login
            char *sign_in_token = sign_in();
            if (sign_in_token != NULL)
            {
                strcpy(account_token, sign_in_token);
                logged_in = 1;
            }
            break;
        case 3: // Listar contas
            list_accounts();
            break;
        case 4: // Exibir informações da conta
            show_account_info(account_token);
            break;
        case 5: // Depósito
            make_deposit(account_token);
            break;
        case 6: // Saque
            make_withdraw(account_token);
            break;
        case 7: // Transferência
            make_transfer(account_token);
            break;
        case 8: // Verificar extrato
            get_bank_statement(account_token);
            break;
        case 9: // Sair
            exit(0);
            break;
        default:
            printf("Opção inválida. Tente novamente:\n");
            break;
        }
    }
}

int main()
{
    printf("Digite o endereço IP do servidor, ou digite 1 para utilizar 127.0.0.1:\n");
    
    scanf("%s", server_ip);

    if (strcmp("1", server_ip) == 0)
    {
        strcpy(server_ip, "127.0.0.1");
    }

    abrir_menu();

    return 0;
}