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

typedef void (*free_datum_fp)(void *datum);

llist_t *llist_new();

void llist_free_all(llist_t *list, free_datum_fp);

void llist_free_list(llist_t *list);

void llist_append(llist_t *list, void *datum);

#endif
