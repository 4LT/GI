#include "linkedlist.h"
#include <stdlib.h>
#include <stdio.h>

int main(char *args[])
{
    llist_t *list = llist_new();
    int *x = (int *)malloc(sizeof (int));
    int *y = (int *)malloc(sizeof (int));
    int *z = (int *)malloc(sizeof (int));
    *x = 3;
    *y = 4;
    *z = 5;
    llist_append(list, x);
    llist_append(list, y);
    llist_append(list, z);

    llist_node_t *node = list->first;
    for (int i = 0; i < 3; i++, node = node->next) {
        printf("%d ", *((int *) node->datum));
    }
    printf("\n");
}
