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

/* ======================= MÉTODOS DE EXEMPLO ======================= */

typedef struct str_one
{
    int num_conta;
    char nome[10000];
} str_one;

typedef struct str_two
{
    float valor;
} str_two;

void method1(void *infoPtr)
{
    struct str_one *info = (struct str_one *)infoPtr;

    printf("Num conta: %i\n", info->num_conta);
    printf("Nome: %s\n", info->nome);
}

void method2(void *infoPtr)
{
    struct str_two *info = (struct str_two *)infoPtr;
    printf("Valor: %f\n", info->valor);
}

/* ======================= FIM MÉTODOS DE EXEMPLO ======================= */

typedef void (*func)(void *info);
typedef struct method
{
    char name[100];
    size_t structSize;
    func function;
} method;

int main()
{
    struct method methods[] = {
        {.name = "Metodo1", .structSize = sizeof(str_one), .function = method1},
        {.name = "Metodo2", .structSize = sizeof(str_two), .function = method2},
    };
    int numOfMethods = sizeof(methods) / sizeof(method);

    printf("Starting\n");

    int sockid = createSocket();
    setSocketForReuse(sockid);
    printf("Created socket\n");

    bindPort(sockid);
    printf("Binded port\n");

    listenSocket(sockid);
    printf("Listening for connections on port %i\n", SERVER_PORT);

    while (1)
    {
        printf("Starting accept\n");
        fflush(stdout);

        int c = sizeof(struct sockaddr_in);
        struct sockaddr client;

        int acceptSocketId = accept(sockid, (struct sockaddr *)&client, (socklen_t *)&c);
        if (acceptSocketId < 0)
        {
            int err = errno;
            printf("Failed to accept connection with error %s (%i)\n", errnoname(err), err);
        }

        char clientIp[INET6_ADDRSTRLEN];
        void *addr = getInAddr((struct sockaddr *)&client);
        inet_ntop(client.sa_family, addr, clientIp, INET6_ADDRSTRLEN);
        printf("Connection received from %s\n", clientIp);

        char method[100];
        receiveMessage(acceptSocketId, method, sizeof(method));
        printf("Received method %s\n", method);

        fflush(stdout);

        int methodFound = 0;
        for (int i = 0; i < numOfMethods; i++)
        {
            if (strcmp(method, methods[i].name) == 0)
            {
                methodFound = 1;

                void *info = malloc(methods[i].structSize);
                bzero(info, methods[i].structSize);

                receiveMessage(acceptSocketId, info, methods[i].structSize);

                methods[i].function(info);
            }
        }

        if (methodFound == 0)
        {
            printf("Invalid method submitted\n");
        }

        close(acceptSocketId);

        printf("Closed connection\n\n");
    }
    return 0;
}
