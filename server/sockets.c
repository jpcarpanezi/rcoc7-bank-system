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

#define MSG_ARRAY_SIZE 100000
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 5000
#define LISTEN_BACKLOG 10

int createSocket()
{
    int sockid = socket(AF_INET, SOCK_STREAM, 0);

    if (sockid == -1)
    {
        int err = errno;
        printf("Failed to create socket with error %s (%i)\n", errnoname(err), err);
        exit(1);
    }

    return sockid;
}

int connectSocket(int sockid)
{
    struct sockaddr_in sock;
    bzero(&(sock), sizeof(sock));

    sock.sin_family = AF_INET;
    sock.sin_port = htons(SERVER_PORT);

    inet_pton(AF_INET, SERVER_ADDRESS, &sock.sin_addr);

    int con = connect(sockid, (struct sockaddr *)&sock, sizeof(sock));

    if (con < 0)
    {
        int err = errno;
        printf("Failed to connect to socket with error %s (%i)\n", errnoname(err), err);
        exit(1);
    }

    printf("Socket connected\n");

    return con;
}

void bindPort(int sockid)
{
    struct sockaddr_in server;

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(SERVER_PORT);

    if (bind(sockid, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        int err = errno;
        printf("Failed to bind port with error %s (%i)\n", errnoname(err), err);
        close(sockid);
        exit(1);
    }
}

void listenSocket(int sockid)
{
    if (listen(sockid, LISTEN_BACKLOG) < 0)
    {
        int err = errno;
        printf("Failed to listen to socket with error %s (%i)\n", errnoname(err), err);
        close(sockid);
        exit(1);
    }
}

int sendMessage(int sockid, char *message)
{
    int msg = send(sockid, message, strlen(message), 0);
    if (msg == -1)
    {
        int err = errno;
        printf("Failed to send message with error %s (%i)\n", errnoname(err), err);
        return 1;
    }

    printf("Message with size %d sent\n", msg);

    return 0;
}

int receiveMessage(int sockid, char txt[MSG_ARRAY_SIZE])
{
    int msg = recv(sockid, txt, MSG_ARRAY_SIZE - 1, 0);

    if (msg == -1)
    {
        int err = errno;
        printf("Failed to receive message with error %s (%i)\n", errnoname(err), err);
        return 1;
    }

    return 0;
}
