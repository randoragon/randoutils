#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include "linkedlist.h"

LinkedList *linkedListCreate()
{
    return NULL;
}

int linkedListAdd(LinkedList **list, void *data)
{
    if (list) {
        LinkedList *new, *last = *list;
        for (; last->next; last = last->next);
        if (!(new = (LinkedList*)malloc(sizeof(LinkedList)))) {
            return 1;
        } 
        new->data = data;
        last->next = new;
    } else {
        if (!(*list = (LinkedList*)malloc(sizeof(LinkedList)))) {
            return 1;
        }
    }
    return 0;
}

int linkedListInsert(LinkedList **list, size_t index, void *data)
{
    return 0;
}

int linkedListRemove(LinkedList **list, void *data)
{
    return 0;
}

int linkedListRemoveAt(LinkedList **list, size_t index)
{
    return 0;
}

int linkedListDestroy(LinkedList **list, int (*dtor)(void *))
{
    LinkedList *i = list;
    while (i) {
        LinkedList *j = i->next;
        free(i->data); 
        if (dtor) {
            int err;
            if ((err = dtor(i->data))) {
                return err;
            }
        }
        i = j;
    }
    return 0;
}

size_t linkedListSize(LinkedList **list)
{
    return 0;
}

int linkedListDtorFree(void *data)
{
    free(data);
    return 0;
}

int main(int argc, char **argv)
{
    printf("test\n");
    return EXIT_SUCCESS;
}
