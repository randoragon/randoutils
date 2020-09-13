#include <stdlib.h>

typedef struct LinkedList LinkedList;
struct LinkedList
{
    void *data;
    LinkedList *next;
};

LinkedList *linkedListCreate();
int linkedListAdd(LinkedList **list, void *data);
int linkedListInsert(LinkedList **list, size_t index, void *data);
int linkedListRemove(LinkedList **list, void *data);
int linkedListRemoveAt(LinkedList **list, size_t index);
int linkedListDestroy(LinkedList **list, int (*dtor)(void *));
size_t linkedListSize(LinkedList **list);

int linkedListDtorFree(void *data);