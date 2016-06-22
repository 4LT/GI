#include <stdlib.h>
#include "linked_list.h"

void free_nothing(void *datum) {}

Llist_t *Llist_new()
{
    Llist_t *list = (Llist_t *)malloc(sizeof (Llist_t));
    *list = (Llist_t) { .first = NULL, .last = NULL, .length = 0 };
    return list;
}

void free_nodes(Llist_node_t *node, free_datum_fp free_datum)
{
    if (node != NULL) {
        Llist_node_t *next = node->next;
        free_datum(node->datum);
        free(node);
        free_nodes(next, free_datum);
    }
}

void Llist_free_all(Llist_t *list, free_datum_fp free_func)
{
    free_nodes(list->first, free_func);
    free(list);
}

void Llist_free_list(Llist_t *list)
{
    Llist_free_all(list, free_nothing);
}

void Llist_append(Llist_t *list, void *datum)
{
    Llist_node_t *new_node = (Llist_node_t *)malloc(sizeof (Llist_node_t));
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

void *Llist_remove(Llist_t *list)
{
    if (list->first == NULL)
        return NULL;
    Llist_node_t *node = list->first;
    list->first = node->next;
    list->length--;
    if (list->first == NULL)
        list->last = NULL;
    void *datum = node->datum;
    free(node);
    return datum;
}
