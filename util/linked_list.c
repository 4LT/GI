#include <stdlib.h>
#include "linked_list.h"

void free_nothing(void *datum) {}

llist_t *llist_new()
{
    llist_t *list = (llist_t *)malloc(sizeof (llist_t));
    *list = (llist_t) { .first = NULL, .last = NULL, .length = 0 };
    return list;
}

void free_nodes(llist_node_t *node, free_datum_fp free_datum)
{
    if (node != NULL) {
        llist_node_t *next = node->next;
        free_datum(node->datum);
        free(node);
        free_nodes(next, free_datum);
    }
}

void llist_free_all(llist_t *list, free_datum_fp free_func)
{
    free_nodes(list->first, free_func);
    free(list);
}

void llist_free_list(llist_t *list)
{
    llist_free_all(list, free_nothing);
}

void llist_append(llist_t *list, void *datum)
{
    llist_node_t *new_node = (llist_node_t *)malloc(sizeof (llist_node_t));
    new_node->datum = datum;
    new_node->next = NULL;
    list->length++;

    if (list->first == NULL) {
        list->first = new_node;
        list->last = new_node;
    }
    else {
        list->last->next = new_node;
        list->last = new_node;
    }
}

void *llist_remove(llist_t *list)
{
    if (list->first == NULL)
        return NULL;
    llist_node_t *node = list->first;
    list->first = node->next;
    list->length--;
    if (list->first == NULL)
        list->last = NULL;
    void *datum = node->datum;
    free(node);
    return datum;
}
