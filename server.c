#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include "list.h"
#include "server.h"

int server_listen_socket_create(struct server *server)
{
	struct sockaddr_in servaddr;
	int ret = 0;

	server->listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server->listen_socket_fd < 0) {
		printf("Create listen socket error. \n");
		return ERR_LISTEN_SOCKET;
	}

	memset(&servaddr, 0, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVER_LISTEN_PORT);

	ret = bind(server->listen_socket_fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (ret < 0) {
		printf("Bind listen socket error. \n");
		return ERR_BIND_LISTEN_SOCKET;
	}


	ret = listen(server->listen_socket_fd, SERVER_LISTEN_MAX);
	if (ret < 0) {
		printf("Listen client error. \n");
		return ERR_LISTEN_CLIENT;
	}

	printf("server listen socket (%d) \n", server->listen_socket_fd);

	return SERVER_NOLMAL;
}

void server_wait_client_connect(struct server *server)
{
	struct epoll_event event;
	struct epoll_event event_s[8];

	memset(&event, 0, sizeof(event));

	event.data.fd = server->listen_socket_fd;
	event.events = EPOLLIN | EPOLLPRI | EPOLLET;

	if (epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, server->listen_socket_fd, &event) < 0) {
		printf("epoll ctl error \n");
		goto WAIT_FINSH;
	}
	
	while (1) {
		int nfds = epoll_wait(server->epoll_fd, event_s, 8, -1);
		printf("%d of clients waiting to connect \n", nfds);
		for (int n = 0; n < nfds; n++) {
			if (event_s[n].data.fd == server->listen_socket_fd) {
				server_connect_client(server, server->listen_socket_fd);
			}
		}
	}

WAIT_FINSH:
	server_exit(server);

	return ;
}

int server_recv_from_client_msg(struct server *server, struct epoll_event *ev, int nfd)
{
	struct client *client = NULL;
	struct node *node = ((struct list*)(server->client_list))->node;
	char buff[4096] = {0};

	if (node == NULL)
		return 0;

	for (int n = 0; n < nfd; n++) {
		do {
			client = (struct client *)node->data;
			if (ev[n].data.fd == client->fd && ev[n].events & EPOLLIN) {
				recv(client->fd, buff, sizeof(buff), 0);
				printf("RECV(%s)Len(%ld): %s \n", client->ip_addr, strlen(buff), buff);
				if (strlen(buff) == 0) {
					server_disconnect_client(server, client);
				}
				else {
					client_msg_list_add(client, buff, strlen(buff));
				}
				node = ((struct list*)(server->client_list))->node;
				break;
			}
		
			node = node->next;
		}while (node != ((struct list*)(server->client_list))->node);
	}

	return 0;
}

int server_send_to_client_msg(struct server *server)
{


	return 0;
}


int server_epoll_init(struct server *server)
{
	server->epoll_fd = epoll_create(1024);
	if (server->epoll_fd < 0) {
		printf("Listen epoll create faile \n");
		return ERR_SERVER_EPOLL_INIT;
	}

	server->epoll_recv_fd = epoll_create(1024);
	if (server->epoll_recv_fd < 0) {
		printf("Recv epoll create faile \n");
		return ERR_SERVER_EPOLL_INIT;
	}

	server->epoll_send_fd = epoll_create(1024);
	if (server->epoll_send_fd < 0) {
		printf("Send epoll create faile \n");
		return ERR_SERVER_EPOLL_INIT;
	}

	
	return SERVER_NOLMAL;
}



struct node* server_client_node_sort(struct node *node_head, struct node *node)
{
	struct node *insert = NULL;
	struct client *client = NULL;
	struct node *nt = node_head;
	struct client *ct = NULL;

	if (node->data != NULL) {
		client = (struct client *)node->data;
		do {
			ct = (struct client *)nt->data;
			if (client->fd < ct->fd) { 
				insert = nt->prve;
				break;
			}

			nt = nt->next;
		}while (nt != node_head);
	}	

	if (insert == NULL)
		insert = node_head->prve;
	
	return insert;
}

void server_client_node_free(struct node *node)
{
	printf("    DELETE (client info) NODE (%p) \n", node);

	struct client *client = (struct client *)node->data;

	close(client->fd);
	
	client_msg_list_destory((struct list **)&client->msg_list);

	free(client);
	
	free(node);
}

void server_client_list_free(struct list *list)
{
	struct node *node = NULL;

	printf("DELETE (client info) LIST (%p) \n", list);
	for (node = list->node; node != NULL; node = list->node)
		list_node_del(list, node);

	free(list);
}

void  server_client_node_print(struct list *list)
{
	struct node *node = list->node;
	struct client *client= NULL;

	if (node == NULL) {
		printf("list node is null \n");
		return ;
	}

	do {
		printf("    (client info) NODE ADDRESS (%p) (%p) NEXT (%p) PRVE (%p) \n", list, node, node->next, node->prve);
		
		client = (struct client *)node->data;
		if (client) {
			printf("    FD                    : %d \n", client->fd);
			printf("    Client connect time   : %d \n", client->first_connect_time);
			printf("    Last keepalive time   : %d \n", client->last_keepalive_time);	
			printf("    IP address            : %s \n", client->ip_addr);
		}
	
		node = node->next;
	}while (node != list->node);

}

void server_client_list_print(struct list *list_head)
{
	struct list *lt = list_head;
	int n = 0;

	if (lt == NULL) {
		printf("list is null \n");
		return ;
	}

	do {
		printf("(client info) LIST ADDRESS (%p) (%d) \n", lt, n++);
		if (list_head->list_node_print) {
			((list_node_print)list_head->list_node_print)(lt);
		}
		lt = lt->next;
	}while(lt != list_head);

}

int server_client_list_init(struct server *server)
{
	struct list *client_list = NULL;
	
	client_list = new_list();
	if (client_list == NULL) {
		printf("Init recv list error \n");
		return ERR_SERVER_RECV_LIST_INIT;
	}
	client_list->node_sort = server_client_node_sort;
	client_list->node_free = server_client_node_free;
	client_list->list_free = server_client_list_free;
	client_list->list_node_print = server_client_node_print;
	client_list->list_print = server_client_list_print;
	
	list_add((struct list**)&server->client_list, client_list);

	return SERVER_NOLMAL;
}

void *server_recv_client_msg(void *arg)
{
	struct server *server = (struct server *)arg;
	struct epoll_event event_s[1024];
	int nfds = 0;
	
	if (server == NULL) 
		pthread_exit(NULL);

	for (; ;) {
		nfds = epoll_wait(server->epoll_recv_fd, event_s, 1024, -1);
		server_recv_from_client_msg(server, event_s, nfds);
	}

	return NULL;
}

int server_recv_task_init(struct server *server) 
{
	int ret = 0;

	ret = pthread_create(&server->recv_task, NULL, server_recv_client_msg, (void*)server);
	if (ret < 0) {
		printf("Recv pthread creat error \n");
		return ERR_SERVER_RECV_PTHREAD_CREATE;
	}

	pthread_detach(server->recv_task);

	return SERVER_NOLMAL;
}

void *server_send_client_msg(void *arg)
{
	for (; ; ) {

		
	}

	return NULL;
}

int srever_send_task_init(struct server *server)
{
	int ret = 0;

	ret = pthread_create(&server->send_task, NULL, server_send_client_msg, (void*)server);
	if (ret < 0) {
		printf("Send pthread creat error \n");
		return ERR_SERVER_RECV_PTHREAD_CREATE;
	}

	pthread_detach(server->send_task);

	return SERVER_NOLMAL;
}

void client_msg_node_free(struct node *node)
{
	struct message *msg = (struct message *)node->data;

	printf("    DELETE (client message) NODE (%p) \n", node);

	if (msg->msg) {
		free(msg->msg);
	}

	free(msg);
	
	free(node);
}

int  client_msg_list_free(struct list *list)
{
	struct node *node = NULL;

	printf("DELETE (client message) LIST (%p) \n", list);
	for (node = list->node; node != NULL; node = list->node)
		list_node_del(list, node);

	free(list);

	return SERVER_NOLMAL;
}

void  client_msg_node_print(struct list *list)
{
	struct node *node = list->node;
	struct message *msg = NULL;

	if (node == NULL) {
		printf("list node is null \n");
		return ;
	}

	do {
		printf("    (client message) NODE ADDRESS (%p) (%p) NEXT (%p) PRVE (%p) \n", list, node, node->next, node->prve);
		
		msg = (struct message *)node->data;
		if (msg) {
			printf("MSG(%d): %s \n", msg->len, msg->msg);
			printf("MSG TYPE；%d \n", msg->type);
			printf("MSG priority: %d \n", msg->priority);
		}
	
		node = node->next;
	}while (node != list->node);

}

void client_msg_list_print(struct list *list_head)
{
	struct list *lt = list_head;
	int n = 0;

	if (lt == NULL) {
		printf("list is null \n");
		return ;
	}

	do {
		printf("(client message) LIST ADDRESS (%p) (%d) \n", lt, n++);
		if (list_head->list_node_print) {
			((list_node_print)list_head->list_node_print)(lt);
		}
		lt = lt->next;
	}while(lt != list_head);

}

int client_msg_list_init(struct list **list)
{
	struct list *msg_list = NULL;
	
	msg_list = new_list();
	if (msg_list == NULL) {
		printf("Init client send list error \n");
		return ERR_SERVER_CLIENT_SEND_INIT;
	}
	
	msg_list->node_free = client_msg_node_free;
	msg_list->list_free = client_msg_list_free;
	msg_list->list_node_print = client_msg_node_print;
	msg_list->list_print = client_msg_list_print;

	list_add(list, msg_list);

	return SERVER_NOLMAL;
}

int client_msg_list_destory(struct list **list)
{
	if (*list != NULL) {
		list_destory(list);
	}

	return SERVER_NOLMAL;
}

int client_msg_list_add(struct client *client, char *str, int str_len)
{
	struct message *msg = NULL;
	char *msg_str = NULL;

	msg = malloc(sizeof(struct message));
	if (msg == NULL)
		return 0;
	memset(msg, 0, sizeof(struct message));

	msg_str = malloc(str_len);
	if (msg_str == NULL)
		return 0;

	memcpy(msg_str, str, str_len);
	msg->msg = msg_str;
	msg->len = str_len;
	
	struct node *node = new_node(msg);
	if (node == NULL) {
		printf("Failed to add a new node \n");
		return -1;
	}

	list_node_add((list *)client->msg_list, node);

	return 0;
}

int server_connect_client(struct server *server, int fd)
{
	struct epoll_event event;
	int ret = 0;
	int connect_fd = 0;
	struct sockaddr_in cliaddr;
	int sin_size = sizeof(struct sockaddr_in);
	char client_addr_p[32] = {0};

	connect_fd = accept(fd, (struct sockaddr *)&cliaddr, &sin_size);
	if (!(connect_fd > 0)) {
		printf("Accept connect client \n");
		return 0;
	}
	
	printf(" -------- Client %s connect(%d) success ------ \n", inet_ntop(AF_INET,&cliaddr.sin_addr, client_addr_p, sizeof(client_addr_p)), connect_fd);
	struct client *client = malloc(sizeof(struct client));
	if (client == NULL) {
		printf("add client info error \n");
		return -1;
	}
	
	memset(client, 0, sizeof(struct client));
	client->fd = connect_fd;
	strcpy(client->ip_addr, client_addr_p);			
	client->first_connect_time = time(NULL);
	client->last_keepalive_time = time(NULL);

	ret = client_msg_list_init(&client->msg_list); /*malloc client msg list*/
	if (ret < 0) {
		printf("Add new client error \n");
		return 0;
	}
	
	memset(&event, 0, sizeof(event));
	event.data.fd = client->fd;
	event.events = EPOLLIN | EPOLLET;

	if (epoll_ctl(server->epoll_recv_fd, EPOLL_CTL_ADD, client->fd, &event) < 0) {
		printf("add client epoll ctl error \n");
	}
	
	struct node *node = new_node(client);
	if (node == NULL) {
		printf("Failed to add a new node \n");
		return -1;
	}

	list_node_add((list *)server->client_list, node); /*malloc and add client */

	return 0;
}

int server_disconnect_client(struct server *server, struct client *client)
{
	struct node *node = NULL;
	struct epoll_event event;
	int ret = 0;

	printf(" -------- Client %s disconnect(%d) success ------ \n", client->ip_addr, client->fd);

	memset(&event, 0, sizeof(event));
	event.data.fd = client->fd;

	if (epoll_ctl(server->epoll_recv_fd, EPOLL_CTL_DEL, client->fd, &event) < 0) {
		printf("delete client epoll ctl error \n");
	}

	close(client->fd);

	ret = client_msg_list_destory(&client->msg_list);     /*free client send msg list*/
	if (ret < 0) {
		printf("Add new client error \n");
		return 0;
	}	

	/* Delete client info */
	node = data_find_node(((list *)server->client_list)->node, (void *)client);
	if (node == NULL) {
		printf("No node was found \n");
		return -1;
	}

	list_node_del((list *)server->client_list, node);   /*free client */

	return 0;
}

int test_server()
{
	struct server *server = NULL;
	struct list *list = NULL;

	server = server_init();

	for (int n = 0; n < 12; n++) {
		list = new_list();
		if (list == NULL) {
			printf("Init recv list error \n");
			continue;
		}
		
		list->node_sort = server_client_node_sort;
		list->node_free = server_client_node_free;
		list->list_free = server_client_list_free;

		list_add((struct list**)&server->client_list, list);

		if (list->list_print) {
			((list_print)list->list_print)(server->client_list);
		}
		for (int k = 0; k < 12; k++) {
			struct client *client = malloc(sizeof(struct client));
			memset(client, 0, sizeof(struct client));
			client->first_connect_time = time(NULL);
			client->last_keepalive_time = time(NULL);
			client->fd = k + 10;
			struct node *node = new_node(client);
			list_node_add((struct list *)list, node);
		}

		if (n == 0) {
			list_del((struct list**)&server->client_list, (struct list*)server->client_list);
		}
		if (list->list_print) {
			((list_print)list->list_print)(server->client_list);
		}
	}

	//server_exit(server);
	
	//list_print((struct list*)server->recv_list);
	
	return 0;
}

struct server * server_init()
{
	int ret = 0;
	struct server *server = NULL;
	
	printf("============== SERVER INIT ============= \n");
	
	server = malloc(sizeof(struct server));
	if (server == NULL) {
		printf("Sever init failed \n");
		goto SERVER_INIT_FAILE;
	}
	memset(server, 0, sizeof(server));

	server->epoll_fd = SERVER_FD_INIT_VALUE;
	server->epoll_recv_fd = SERVER_FD_INIT_VALUE;
	ret = server_epoll_init(server);
	if (ret != SERVER_NOLMAL) {
		goto SERVER_INIT_FAILE;
	}
	
	server->listen_socket_fd = SERVER_FD_INIT_VALUE;
	ret = server_listen_socket_create(server);
	if (ret != SERVER_NOLMAL) {
		goto SERVER_INIT_FAILE;	
	}
	
	ret = server_client_list_init(server);
	if (ret != SERVER_NOLMAL) {
		goto SERVER_INIT_FAILE;
	}

	ret = server_recv_task_init(server);
	if (ret != SERVER_NOLMAL) {
		goto SERVER_INIT_FAILE;
	}

	ret = srever_send_task_init(server);
	if (ret != SERVER_NOLMAL) {
		goto SERVER_INIT_FAILE;
	}
	
	return server;
	
SERVER_INIT_FAILE:
	server_exit(server);

	return NULL;
}

int server_exit(struct server *server)
{
	printf("============== SERVER EXIT ============= \n");

	if (server == NULL)
		return 0;

	if (server->listen_socket_fd > 0) {
		close(server->listen_socket_fd);
	}
	server->listen_socket_fd = SERVER_FD_INIT_VALUE;


	if (server->epoll_fd > 0) {
		close(server->epoll_fd );
	}
	server->epoll_fd = SERVER_FD_INIT_VALUE;

	if (server->epoll_recv_fd > 0) {
		close(server->epoll_recv_fd);
	}
	server->epoll_recv_fd = SERVER_FD_INIT_VALUE;

	if (server->epoll_send_fd > 0) {
		close(server->epoll_send_fd);
	}
	server->epoll_send_fd = SERVER_FD_INIT_VALUE;

	if (server->client_list != NULL) {
		list_destory((struct list**)&server->client_list);
	}
	
	free(server);
	
	return 0;
}
struct server *gserver;
void signal_server_exit()
{ 
	server_exit(gserver);
}

int main(int argc, char *argv[])
{
	signal(SIGPIPE, signal_server_exit);

	struct server *server = NULL;
	
	//test_server();
	
	gserver = server = server_init();
	
	if (server)
		server_wait_client_connect(server);

	return 0;
}
