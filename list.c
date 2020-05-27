#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "list.h"
#include "server.h"




#define MALLOCE_DEBUG 0

#if MALLOCE_DEBUG
#define MALLOC_P_MAX  2048
int malloc_total_size = 0;
void *malloc_p[2][MALLOC_P_MAX] = {0};
int malloc_n = 0;
#define NODE_STR "node"
#define LIST_STR "list"
#endif

void *_malloc_(int size, const char *des) 
{
	void *p = NULL;
	p = malloc(size);

#if MALLOCE_DEBUG
	
	for (int n = 0; n < MALLOC_P_MAX; n++) {	
		if (malloc_p[0][n] == NULL) {
			malloc_p[0][n] = (void *)p;
			malloc_p[1][n] = (void *)des;
			malloc_n++;
			malloc_total_size += size;
			break;			
		}
	}

	printf("MALLOC NEW ADDR (%p) (%s) (%d)\n", p, des, size);
	malloc_print();
#endif

	return p;
};

void *_free_(void *p, int size) 
{

#if MALLOCE_DEBUG
	for (int n = 0; n < MALLOC_P_MAX; n++) {
		if (malloc_p[0][n] == p) {
			malloc_p[0][n] = NULL;
			malloc_p[1][n] = NULL;
			malloc_n--;
			malloc_total_size -= size;
			break;
		}
	}
	printf("FREE ADDR (%p) (%d)\n", p, size);
	malloc_print();
#endif

	free(p);
	
	return NULL;
};


void malloc_print()
{
#if MALLOCE_DEBUG

	int mn = 0;

	for (int n = 0; n < MALLOC_P_MAX; n++) {
		if (malloc_p[0][n]) {
			printf("(%d)MALLOC ADDR (%p) des (%s) \n", mn++, malloc_p[0][n], (char *)(malloc_p[1][n] == NULL ? "NULL" : malloc_p[1][n]));
		}
	}
	printf("total malloc num (%d) size (%d) \n", malloc_n, malloc_total_size);
#endif
}

int list_node_add(struct list *list, struct node *node)
{
	struct node *nt = list->node;

	if (list->node == NULL) {
		list->node = node;
		node->next = node;
		node->prve = node;
		list->node_num ++;
		if (list->list_node_print) {
			((list_node_print)list->list_node_print)(list);
		}
		
		return 0;
	}

	if (list->node_sort) {
		((node_sort)list->node_sort)(&list->node, node);
	} 
	else {
		list->node->prve->next = node;
		node->prve = list->node->prve;
		node->next = list->node;
		list->node->prve = node;
	}
	
	list->node_num ++;
	
	if (list->list_node_print) {
		((list_node_print)list->list_node_print)(list);
	}

	
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
	
	list->node_num --;
	
	if (list->list_node_print) {
		((list_node_print)list->list_node_print)(list);
	}

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

int list_destory(struct list **list_head)
{
	struct list *tl = NULL;
	
	for (tl = *list_head; tl != NULL; tl = *list_head)
		list_del(list_head, tl);

	*list_head = NULL;

	return 0;
}


struct node *new_node(void *data)
{
	struct node* node = _malloc_(sizeof(struct node), "list");
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
	struct list* list = _malloc_(sizeof(struct list), "node");
	if (list == NULL) {
		printf("new list failed \n");
	}
	
	memset(list, 0, sizeof(list));
	
	return list;
}

struct node * node_search_by_data(struct node *node_head, void *data)
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



