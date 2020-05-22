#ifndef __LIST_H__
#define __LIST_H__


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
	void *list_node_print;
	void *list_print;
	void *data;
	int node_num;
}list;

typedef struct node *(*node_sort)(struct node *, struct node *);
typedef void (*node_free)(struct node *);
typedef void (*list_free)(struct list *);
typedef void  (*list_node_print)(struct list *);
typedef void (*list_print)(struct list *);

extern int list_node_del(struct list *list, struct node *node);
extern int list_node_add(struct list *list, struct node *node);
extern int list_node_del(struct list *list, struct node *node);
extern int list_add(struct list **list_head, struct list *list);
extern int list_del(struct list **list_head, struct list *list);
extern struct node *new_node(void *data);
extern struct list* new_list();
extern struct node * data_find_node(struct node *node_head, void *data);
extern int list_destory(struct list **list_head);
#endif /* __LIST_h__ */

















