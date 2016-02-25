#include <stdlib.h>
#include "linkedlist.h"

llist_t *llist_new()
{
    llist_t *list = (llist_t *)malloc(sizeof (llist_t));
    *list = (llist_t) { NULL, NULL };
    return list;
}

void free_nodes(llist_node_t *node)
{
    if (node != NULL) {
        llist_node_t *next = node->next;
        free(node->datum);
        free(node);
        free_nodes(next);
    }
}

void llist_free(llist_t *list)
{
    free_nodes(list->first);
    free(list);
}

void llist_append(llist_t *list, void *datum)
{
    llist_node_t *new_node = (llist_node_t *)malloc(sizeof (llist_node_t));
    new_node->datum = datum;
    new_node->next = NULL;
    if (list->first == NULL) {
        list->first = new_node;
        list->last = new_node;
    }
    else {
        list->last->next = new_node;
        list->last = new_node;
    }
}
