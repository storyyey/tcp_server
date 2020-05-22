#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/epoll.h>

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
#define ERR_SERVER_CLIENT_SEND_INIT		(-8)

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({\
	 					const typeof( ((type *)0)->member ) *__mptr = (ptr);\
 						(type *)( (char *)__mptr - offsetof(type,member) );})




enum message_type {
	MSG_TYPE_NONSENSE,
	MSG_TYPE_UNICAST,
	MSG_TYPE_BROADCAST,
	MSG_TYPE_MULTICAST,
	MSG_TYPE_KEEPALIVE,
};

typedef struct message {
	char *msg;
	int len;
	enum message_type type;	
	int priority;
	struct client *src_client;
}message;

typedef struct client {
	int fd;
	int permission;
	int first_connect_time;
	int last_keepalive_time;
	char ip_addr[32];
	int wait_forward_msg;
	struct list *msg_list;
}client;

typedef struct server {
	int listen_socket_fd;
	int epoll_fd;
	int epoll_recv_fd;
	int epoll_send_fd;
	pthread_t recv_task;
	pthread_t send_task;
	void *trans_list;
	void *client_list;
}server;

extern struct server * server_init();
extern int server_exit(struct server *server);

extern int server_connect_client(struct server *server, int fd);
extern int server_disconnect_client(struct server *server, struct client *client);
extern int server_listen_socket_create(struct server *server);
extern void server_wait_client_connect(struct server *server);
extern int server_recv_from_client_msg(struct server *server, struct epoll_event *ev, int nfd);
extern int server_epoll_init(struct server *server);
extern int server_list_destory(struct list **list_head);
extern struct node* server_client_node_sort(struct node *node_head, struct node *node);
extern void server_client_node_free(struct node *node);
extern void server_client_list_free(struct list *list);
extern int server_client_list_init(struct server *server);
extern void *server_recv_client_msg(void *arg);
extern int server_recv_task_init(struct server *server) ;
extern  struct server * server_init();
extern int server_exit(struct server *server);
extern int server_add_client(struct server *server, struct client *client);
extern int server_del_client(struct server *server, struct client *client);
extern int client_msg_list_destory(struct list **list);



extern int client_msg_list_add(struct client *client, char *str, int str_len);

#endif /*__SERVER_H__*/








