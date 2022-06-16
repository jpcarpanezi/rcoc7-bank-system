#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 5000

int createSocket();
int connectSocket(int sockid);
void setSocketForReuse(int sockid);
void bindPort(int sockid);
void listenSocket(int sockid);
int sendMessage(int sockid, void *message, int messageSize);
int receiveMessage(int sockid, void *message, int messageSize);
void *getInAddr(struct sockaddr *sa);