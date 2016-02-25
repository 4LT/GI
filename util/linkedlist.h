#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct llist_node
{
    void *datum;
    struct llist_node *next;
} llist_node_t;

typedef struct
{
    llist_node_t *first;
    llist_node_t *last;
} llist_t;

llist_t *llist_new();

void llist_free(llist_t *list);

void llist_append(llist_t *list, void *datum);

#endif
