# LinkedList

## Description

This is a small self-written library implementing a linked list struct and related functions.
I wrote it so that I can use it in my other C projects.

## Example Usages

Create and destroy a list with elements [1, 2, 3]:

```c
int a = 1, b = 2, c = 3;
RND_LinkedList *list = RND_linkedListCreate();
RND_linkedListAdd(&list, &a);
RND_linkedListAdd(&list, &b);
RND_linkedListInsert(&list, 2, &c);
printf("Second element is %d", *((int*)RND_linkedListGet(&list, 1)));
RND_linkedListDestroy(&list, NULL);
```

The second argument of `RND_linkedListDestroy` is a pointer to a destructor function:

```c
typedef struct Example Example;
struct Example
{
    char *str;
};

int dtor(void *data)
{
    free(((Example*)data)->str);
    free(data);
    return 0;
}

int main()
{
    Example *elem = (Example*)malloc(sizeof(Example));
    char *str = (char*)malloc(sizeof(char));
    elem->str = str;

    RND_LinkedList *list = RND_linkedListCreate();
    RND_linkedListAdd(&list, elem);
    RND_linkedListDestroy(&list, dtor);
}
```
