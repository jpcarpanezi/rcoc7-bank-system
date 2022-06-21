#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 5000

#define MAX_STREAM_SIZE 10000
#define METHOD_SIZE sizeof(char) * 100

int create_socket();
void connect_socket(int sock_id, char *server_ip);
void bind_port(int sock_id);
void set_socket_for_reuse(int sock_id);
void listen_socket(int sock_id);
int send_message(int sock_id, void *message, int message_size);
int receive_message(int sock_id, void *message, int message_size);
void *get_in_addr(struct sockaddr *sa);