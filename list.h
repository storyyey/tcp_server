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

typedef struct node *(*node_sort)(struct node **, struct node *);
typedef void (*node_free)(struct node *);
typedef void (*list_free)(struct list *);
typedef void  (*list_node_print)(struct list *);
typedef void (*list_print)(struct list *);

extern int list_single_node_add(struct list *list, struct node *node);
extern int list_single_node_del(struct list *list, struct node *node);
extern int list_single_add(struct list **list_head, struct list *list);
extern int list_single_del(struct list **list_head, struct list *list);
extern struct node *new_node(void *data);
extern struct list* new_list();
extern struct node * node_search_by_data(struct node *node_head, void *data);
extern int list_all_del(struct list **list_head);


extern void malloc_print();
extern void *_free_(void *p, int size) ;
extern void *_malloc_(int size, const char *des) ;


#define LIST_NODE_LOOKUP(head, n) \
			for (struct node *nt = NULL, *node1 = (head); n = node1, node1 != nt; nt = (head), node1 = node1->next)\
				if (n)
				
#define LIST_LOOKUP(head, n) \
			for (struct list *nt = NULL, *node1 = (head); n = node1, node1 != nt; nt = (head), node1 = node1->next)\
				if (n)

#endif /* __LIST_h__ */

















