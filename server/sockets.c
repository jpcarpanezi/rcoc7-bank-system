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

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 5000
#define LISTEN_BACKLOG 10

#define MAX_STREAM_SIZE 10000
#define METHOD_SIZE sizeof(char) * 100

/**
 * @brief Cria um socket.
 * Encerra o programa com o código 1 caso não seja possível criar o socket
 *
 * @return ID do socket criado
 */
int create_socket()
{
    int sock_id = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_id == -1)
    {
        int err = errno;
        printf("Failed to create socket with error %s (%i)\n", errnoname(err), err);
        exit(1);
    }

    return sock_id;
}

/**
 * @brief Realiza a conexão a um socket.
 * Utiliza o IP definido em SERVER_ADDRESS e porta definida em SERVER_PORT.
 * Encerra o programa com o código 1 caso não seja possível conectar o socket
 *
 * @param sock_id ID do socket a ser conectado
 */
void connect_socket(int sock_id)
{
    struct sockaddr_in sock;
    bzero(&(sock), sizeof(sock));

    sock.sin_family = AF_INET;
    sock.sin_port = htons(SERVER_PORT);

    inet_pton(AF_INET, SERVER_ADDRESS, &sock.sin_addr);

    int con = connect(sock_id, (struct sockaddr *)&sock, sizeof(sock));

    if (con < 0)
    {
        int err = errno;
        printf("Failed to connect to socket with error %s (%i)\n", errnoname(err), err);
        exit(1);
    }

    printf("Socket connected\n");
}

/**
 * @brief Vincula o socket a porta definida em SERVER_PORT
 * Encerra o programa com o código 1 caso não seja possível vinular a porta
 *
 * @param sock_id ID do socket a ser vinculado
 */
void bind_port(int sock_id)
{
    struct sockaddr_in server;

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(SERVER_PORT);

    if (bind(sock_id, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        int err = errno;
        printf("Failed to bind port with error %s (%i)\n", errnoname(err), err);
        close(sock_id);
        exit(1);
    }
}

/**
 * @brief Configura o socket para ser reutilizável
 *
 * @param sock_id ID do socket a ser configurado
 */
void set_socket_for_reuse(int sock_id)
{
    int option = 1;
    setsockopt(sock_id, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
}

/**
 * @brief Prepara o socket para aceitar pedidos de conexão
 * Encerra o programa com o código 1 caso não seja possível preparar o socket
 *
 * @param sock_id ID do socket a ser preparado
 */
void listen_socket(int sock_id)
{
    if (listen(sock_id, LISTEN_BACKLOG) < 0)
    {
        int err = errno;
        printf("Failed to listen to socket with error %s (%i)\n", errnoname(err), err);
        close(sock_id);
        exit(1);
    }
}

/**
 * @brief Envia uma mensagem através do socket especificado
 *
 * @param sock_id ID do socket para envio da mensagem
 * @param message Mensagem a ser enviada
 * @param message_size Tamanho da mensagem a ser enviada
 * @return 0 caso o envio seja bem sucedido, 1 caso não seja
 */
int send_message(int sock_id, void *message, int message_size)
{
    int msg = send(sock_id, message, message_size, 0);
    if (msg == -1)
    {
        int err = errno;
        printf("Failed to send message with error %s (%i)\n", errnoname(err), err);
        return 1;
    }

    printf("Message with size %d sent\n", message_size);

    return 0;
}

/**
 * @brief Recebe uma mensagem através do socket especificado
 *
 * @param sock_id ID do socket para recebimento da mensagem
 * @param message Ponteiro para armazenamento da mensagem recebida
 * @param message_size Tamanho máximo que pode ser armazenado no parâmetro message
 * @return 0 caso o recebimento seja bem sucedido, 1 caso não seja
 */
int receive_message(int sock_id, void *message, int message_size)
{
    int msg = recv(sock_id, message, message_size, 0);

    if (msg == -1)
    {
        int err = errno;
        printf("Failed to receive message with error %s (%i)\n", errnoname(err), err);
        return 1;
    }

    printf("Received message with size %i\n", msg);
    return 0;
}

/**
 * @brief Busca a struct de tipo sin_addr ou sin6_addr dentro de uma struct sockaddr
 *
 * @param sa Struct sockaddr a ser buscada
 * @return Ponteiro para uma struct do tipo sin_addr ou sin6_addr
 */
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in *)sa)->sin_addr);
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}