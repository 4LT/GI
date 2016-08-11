/* linked_list.h
 *
 * Singly-linked, essentially a queue.
 * author: Seth Rader
 *
 * TODO: Arrays might perform better due to random access, ex. for kd trees
 * TODO: Type should be named Llist_t: capitalize to emphasize use as object
 */
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "collections.h"

/* Nodes contain a pointer to data, and a pointer to the next node. */
typedef struct Llist_node
{
    void *datum;
    struct Llist_node *next;
} Llist_node_t;

/* Lists keep track of their length, and contain pointers to the front and back
 * of the list. */
typedef struct
{
    Llist_node_t *first;
    Llist_node_t *last;
    unsigned int length;
} Llist_t;

/* create a new, malloc'd empty list */
Llist_t *Llist_new();

/* Free all the nodes, all data in the nodes by method provided, and the list
 * itself. */
void Llist_free_all(Llist_t *list, free_datum_fp);

/* Free the list and its nodes, but not the contained data */
void Llist_free_list(Llist_t *list);

/* Append a node with the provided data to the list */
void Llist_append(Llist_t *list, void *datum);

/* Remove a node from the front of the list, retrieving its datum */
void *Llist_remove(Llist_t *list);

#endif
