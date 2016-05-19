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

/* Nodes contain a pointer to data, and a pointer to the next node. */
typedef struct llist_node
{
    void *datum;
    struct llist_node *next;
} llist_node_t;

/* Lists keep track of their length, and contain pointers to the front and back
 * of the list. */
typedef struct
{
    llist_node_t *first;
    llist_node_t *last;
    unsigned int length;
} llist_t;

/* means of freeing data in all nodes */
typedef void (*free_datum_fp)(void *datum);

/* create a new, malloc'd empty list */
llist_t *llist_new();

/* Free all the nodes, all data in the nodes by method provided, and the list
 * itself. */
void llist_free_all(llist_t *list, free_datum_fp);

/* Free the list and its nodes, but not the contained data */
void llist_free_list(llist_t *list);

/* Append a node with the provided data to the list */
void llist_append(llist_t *list, void *datum);

/* Remove a node from the front of the list, retrieving its datum */
void *llist_remove(llist_t *list);

#endif
