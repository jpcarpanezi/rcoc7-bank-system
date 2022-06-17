#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 5000

int create_socket();
int connect_socket(int sock_id);
void bind_port(int sock_id);
void set_socket_for_reuse(int sock_id);
void listen_socket(int sock_id);
int send_message(int sock_id, void *message, int message_size);
int receive_message(int sock_id, void *message, int message_size);
void *get_in_addr(struct sockaddr *sa);