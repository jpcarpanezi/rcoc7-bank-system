#define MSG_ARRAY_SIZE 100000

int createSocket();
int connectSocket(int sockid);
void bindPort(int sockid);
void listenSocket(int sockid);
int sendMessage(int sockid, char *message);
int receiveMessage(int sockid, char txt[MSG_ARRAY_SIZE]);