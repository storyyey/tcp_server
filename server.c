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

#define SERVER_LISTEN_PORT (6666)
#define SERVER_LISTEN_MAX 10

#define SERVER_FD_INIT_VALUE			(-1)
#define SERVER_NOLMAL					(0)
#define SERVER_FAILED					(-11111)


#define ERR_LISTEN_SOCKET 				(-1)
#define ERR_BIND_LISTEN_SOCKET 			(-2)
#define ERR_LISTEN_CLIENT				(-3)
#define ERR_SERVER_EPOLL_INIT			(-4)
#define ERR_SERVER_INIT					(-5)
#define ERR_SERVER_RECV_LIST_INIT		(-6)
#define ERR_SERVER_RECV_PTHREAD_CREATE 	(-7)

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({\
	 					const typeof( ((type *)0)->member ) *__mptr = (ptr);\
 						(type *)( (char *)__mptr - offsetof(type,member) );})




typedef struct client {
	int fd;
	int first_connect_time;
	int last_keepalive_time;
	char ip_addr[32];
	struct list *send_msg_list;
}client;

typedef struct server {
	int listen_socket_fd;
	int epoll_fd;
	int epoll_recv_fd;
	pthread_t recv_task;
	void *send_list;
	void *recv_list;
}server;

typedef struct node {
	struct node *prve;
	struct node *next;
	void *data;
}node;

typedef struct list {
	struct list *prve;
	struct list *next;
	struct node *node;
	void *node_sort;
	void *node_free;
	void *list_free;
	void *data;
}list;


typedef struct node *(*node_sort)(struct node *, struct node *);
typedef void (*node_free)(struct node *);
typedef void (*list_free)(struct list *list);

extern struct server * server_init();
extern int server_exit(struct server *server);

extern int server_add_client(struct server *server, struct client *client);
extern int server_del_client(struct server *server, struct client *client);
extern int list_node_del(struct list *list, struct node *node);


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
	struct epoll_event event_s[1024];
	struct sockaddr_in cliaddr;
	int sin_size = sizeof(struct sockaddr_in);
	char client_addr_p[32] = {0};

	memset(&event, 0, sizeof(event));
	memset(&cliaddr, 0, sizeof(cliaddr));

	event.data.fd = server->listen_socket_fd;
	event.events = EPOLLIN | EPOLLPRI | EPOLLET;

	if (epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, server->listen_socket_fd, &event) < 0) {
		printf("epoll ctl error \n");
		goto WAIT_FINSH;
	}
	
	while (1) {
		int nfds = epoll_wait(server->epoll_fd, event_s, 1024, -1);
		printf("%d of clients waiting to connect \n", nfds);
		for (int n = 0; n < nfds; n++) {
			if (event_s[n].data.fd == server->listen_socket_fd) {
				int connfd = accept(server->listen_socket_fd, (struct sockaddr *)&cliaddr, &sin_size);
				if (connfd > 0) {
				printf(" -------- Client %s connect(%d) success ------ \n", inet_ntop(AF_INET,&cliaddr.sin_addr, client_addr_p, sizeof(client_addr_p)), connfd);
					struct client *client = malloc(sizeof(struct client));
					if (client) {
						memset(client, 0, sizeof(struct client));
						client->fd = connfd;
						strcpy(client->ip_addr, client_addr_p);
						server_add_client(server, client);
					}
				}
			}
		}
	}

WAIT_FINSH:
	server_exit(server);

	return ;
}


int server_client_disconnect(struct server *server, struct client *client)
{
	struct epoll_event event;

	printf(" -------- Client %s disconnect(%d) success ------ \n", client->ip_addr, client->fd);

	memset(&event, 0, sizeof(event));
	event.data.fd = client->fd;

	if (epoll_ctl(server->epoll_recv_fd, EPOLL_CTL_DEL, client->fd, &event) < 0) {
		printf("delete client epoll ctl error \n");
	}

	close(client->fd);

	server_del_client(server, client);

	return 0;
}
int server_recv_from_client_msg(struct server *server, struct epoll_event *ev, int nfd)
{
	struct client *client = NULL;
	struct node *node = ((struct list*)(server->recv_list))->node;
	char buff[1024] = {0};

	if (node == NULL)
		return 0;

	for (int n = 0; n < nfd; n++) {
		do {
			client = (struct client *)node->data;
			if (ev[n].data.fd == client->fd && ev[n].events & EPOLLIN) {
				recv(client->fd, buff, sizeof(buff), 0);
				printf("RECV(%s)Len(%ld): %s \n", client->ip_addr, strlen(buff), buff);
				if (strlen(buff) == 0) {
					server_client_disconnect(server, client);
				}
				node = ((struct list*)(server->recv_list))->node;
				break;
			}
		
			node = node->next;
		}while (node != ((struct list*)(server->recv_list))->node);
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
	
	return SERVER_NOLMAL;
}

void list_node_print(struct list *list)
{
	struct node *node = list->node;
	struct client *client= NULL;

	if (node == NULL) {
		printf("list node is null \n");
		return ;
	}

	do {
		printf("    NODE ADDRESS (%p) (%p) NEXT (%p) PRVE (%p) \n", list, node, node->next, node->prve);
		
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

void list_print(struct list *list_head)
{
	struct list *lt = list_head;
	int n = 0;

	if (lt == NULL) {
		printf("list is null \n");
		return ;
	}

	do {
		printf("LIST ADDRESS (%p) (%d) \n", lt, n++);
		list_node_print(lt);
		lt = lt->next;
	}while(lt != list_head);

}

struct node* list_node_sort(struct node *node_head, struct node *node)
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

void list_node_destroy(struct node *node)
{
	struct client *c = (struct client *)node->data;

	printf("    DELETE NODE (%p) \n", node);
	free(c);
	
	free(node);
}

void list_destroy(struct list *list)
{
	struct node *node = NULL;

	printf("DELETE LIST (%p) \n", list);
	for (node = list->node; node != NULL; node = list->node)
		list_node_del(list, node);

	free(list);
}

int list_node_add(struct list *list, struct node *node)
{
	struct node *nt = list->node;
	struct node *insert = NULL;


	if (list->node == NULL) {
		list->node = node;
		node->next = node;
		node->prve = node;

		list_node_print(list);
		
		return 0;
	}

	if (list->node_sort) {
		insert = ((node_sort)list->node_sort)(list->node, node);
	} 
	else {
		insert = list->node->prve;
	}

	node->next = insert->next;
	node->prve = insert;
	insert->next->prve = node;
	insert->next = node;

	list_node_print(list);
		
	return 0;
}

int list_node_del(struct list *list, struct node *node)
{
	if (list->node == node) {
		if (list->node->next == list->node) {
			list->node = NULL;
		}
		else {
			list->node = node->next;
			node->next->prve = node->prve;
			node->prve->next = node->next;
		}
	}
	else {
		node->next->prve = node->prve;
		node->prve->next = node->next;
	}

	if (list->node_free) {
		((node_free)list->node_free)(node);
	}

	list_node_print(list);

	return 0;
}


int list_add(struct list **list_head, struct list *list)
{
	printf("NEW LIST ADDR (%p)\n", list);


	if (*list_head == NULL) {
		*list_head = list;
		list->next = list;
		list->prve = list;
		return 0;
	}

	list->next = (*list_head)->next;
	list->prve = (*list_head);
	(*list_head)->next->prve = list;
	(*list_head)->next = list;

	return 0;
}



int list_del(struct list **list_head, struct list *list)
{
	if ((*list_head) == list) {
		if ((*list_head)->next == list)
			(*list_head) = NULL;
		else {
			(*list_head) = list->next;
			list->next->prve = list->prve;
			list->prve->next = list->next;
		}
	}
	else {
		list->next->prve = list->prve;
		list->prve->next = list->next;
	}
	
	if (list->list_free) {
		((list_free)list->list_free)(list);
	}
		
	return 0;
}



struct node *new_node(void *data)
{
	struct node* node = malloc(sizeof(struct node));
	if (node == NULL) {
		printf("new node failed \n");
	}
	else {
		node->data = data;		
	}

	memset(node, 0, sizeof(node));
	
	printf("NEW NODE ADDR (%p) DATA (%p)\n", node, node->data);
	
	return node;
}

struct list* new_list()
{
	struct list* list = malloc(sizeof(struct list));
	if (list == NULL) {
		printf("new list failed \n");
	}
	
	memset(list, 0, sizeof(list));
	
	return list;
}

struct node * data_find_node(struct node *node_head, void *data)
{
	struct node *nt = node_head;
	struct node *node_find = NULL;

	if (node_head == NULL || data == NULL)
		return NULL;

	do {
		if (data == nt->data) { 
			node_find = nt;
			break;
		}
		nt = nt->next;
	}while (nt != node_head);

	printf("FIND ADDRESS (%p) NEXT (%p) PRVE (%p) DATA (%p)\n", node_find, node_find->next, node_find->prve, node_find->data);
	
	return node_find;
}

int server_list_destory_all(struct list **list_head)
{
	struct list *tl = NULL;
	
	for (tl = *list_head; tl != NULL; tl = *list_head)
		list_del(list_head, tl);

	*list_head = NULL;

	return 0;
}

int server_recv_list_init(struct server *server)
{
	struct list *recv_list = NULL;
	recv_list = new_list();
	if (recv_list == NULL) {
		printf("Init recv list error \n");
		return ERR_SERVER_RECV_LIST_INIT;
	}
	recv_list->node_sort = list_node_sort;
	recv_list->node_free = list_node_destroy;
	recv_list->list_free = list_destroy;
	
	list_add((struct list**)&server->recv_list, recv_list);

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
	
	ret = server_recv_list_init(server);
	if (ret != SERVER_NOLMAL) {
		goto SERVER_INIT_FAILE;
	}

	ret = server_recv_task_init(server);
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

	if (server->recv_list != NULL) {
		server_list_destory_all((struct list**)&server->recv_list);
	}
	
	free(server);
	
	return 0;
}

int server_add_client(struct server *server, struct client *client)
{
	struct epoll_event event;

	if (client == NULL) {
		printf("add client info error \n");
		return -1;
	}

	client->first_connect_time = time(NULL);
	client->last_keepalive_time = time(NULL);

	struct node *node = new_node(client);
	if (node == NULL) {
		printf("Failed to add a new node \n");
		return -1;
	}

	list_node_add((list *)server->recv_list, node);

	
	memset(&event, 0, sizeof(event));
	event.data.fd = client->fd;
	event.events = EPOLLIN | EPOLLET;

	if (epoll_ctl(server->epoll_recv_fd, EPOLL_CTL_ADD, client->fd, &event) < 0) {
		printf("add client epoll ctl error \n");
	}

	return 0;
}

int server_del_client(struct server *server, struct client *client)
{
	struct node *node = NULL;

	node = data_find_node(((list *)server->recv_list)->node, (void *)client);
	if (node == NULL) {
		printf("No node was found \n");
		return -1;
	}

	list_node_del((list *)server->recv_list, node);

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
		

		list->node_sort = list_node_sort;
		list->node_free = list_node_destroy;
		list->list_free = list_destroy;

		list_add((struct list**)&server->recv_list, list);

		list_print((struct list*)server->recv_list);
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
			list_del((struct list**)&server->recv_list, (struct list*)server->recv_list);
		}
		list_print((struct list*)server->recv_list);
	}

	//server_exit(server);
	
	//list_print((struct list*)server->recv_list);
	
	return 0;
}

int main(int argc, char *argv[])
{
	struct server *server = NULL;
	
	//test_server();
	
	server = server_init();
	
	if (server)
		server_wait_client_connect(server);

	return 0;
}
