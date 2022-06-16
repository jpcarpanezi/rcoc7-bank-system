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

typedef struct str_one
{
    int num_conta;
    char nome[10000];
} str_one;

typedef struct str_two
{
    float valor;
} str_two;

void sendMethod1(int sockid)
{
    struct str_one one;
    bzero(&(one), sizeof(one));
    one.num_conta = 500;
    strcpy(one.nome, "Meu Nome");

    char method[100] = "Metodo1";
    sendMessage(sockid, method, sizeof(method));
    sendMessage(sockid, &one, sizeof(one));
}

void sendMethod2(int sockid)
{
    struct str_two two;
    bzero(&(two), sizeof(two));
    two.valor = 20.04;

    char method[100] = "Metodo2";
    sendMessage(sockid, method, sizeof(method));
    sendMessage(sockid, &two, sizeof(two));
}

int main()
{
    int sockid = createSocket();
    connectSocket(sockid);

    sendMethod1(sockid);

    sockid = createSocket();
    connectSocket(sockid);
    sendMethod2(sockid);
    return 0;
}