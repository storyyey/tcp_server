#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define SERVER_LISTEN_PORT (6666)
#define SERVER_LISTEN_MAX 10

#define ERR_LISTEN_SOCKET 			(-1)
#define ERR_BIND_LISTEN_SOCKET 		(-2)
#define ERR_LISTEN_CLIENT			(-3)

typedef struct server {
	int listen_fd;
	void *send_list;
	void *recv_list;
}server;

#define DEFAULT_SERVER_ADDR "127.0.0.1"

int client_connect_server(char *server_addr)
{
	struct sockaddr_in servaddr;
	int ret = 0;
	char *serv = (server_addr == NULL ? DEFAULT_SERVER_ADDR:server_addr);
	char buff[1024] = {0};

	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd < 0) {
		printf("Create client socket error. \n");
		return ERR_LISTEN_SOCKET;
	}

	memset(&servaddr, 0, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVER_LISTEN_PORT);

	inet_pton(AF_INET, serv, &servaddr.sin_addr);

	printf("client(%d) connect server \n", client_fd);
	ret = connect(client_fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (ret < 0) {
		printf("Connect server failed (%d)\n", ret);
	}


	while (1) {
		scanf("%s", buff);
		if (strcmp("quit", buff) == 0)
			break;
		printf("send: %s \n", buff);
		printf("%s %d\n", __func__, __LINE__);
		if(send(client_fd, buff, strlen(buff), 0) < 0)
			perror("send");
		printf("%s %d\n", __func__, __LINE__);
		memset(buff, 0, sizeof(buff));
		printf("%s %d\n", __func__, __LINE__);
		//recv(client_fd, buff, sizeof(buff), 0);
		//printf("%s %d\n", __func__, __LINE__);
		//printf("recv: %s \n", buff);

	}
	

	printf("client disconnect \n");
	
	close(client_fd);

	return 0;
}

void signal_func(){ 
	printf("recv signal \n");
}

int main(int argc, char *argv[])
{

	signal(SIGPIPE,signal_func);
	
	client_connect_server(argv[1]);

	return 0;
}
