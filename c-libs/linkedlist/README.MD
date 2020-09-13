# LinkedList

## Description

This is a small self-written library implementing a linked list struct and related functions.
I wrote it so that I can use it in my other C projects.

## Example Usages

Create and destroy a list with elements [1, 2, 3]:

```c
int a = 1, b = 2, c = 3;
LinkedList *list = linkedListCreate();
linkedListAdd(&list, &a);
linkedListAdd(&list, &b);
linkedListInsert(&list, 2, &c);
printf("Second element is %d", *((int*)linkedListGet(&list, 1)));
linkedListDestroy(&list, NULL);
```

The second argument of `linkedListDestroy` is a pointer to a destructor function:

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

    LinkedList *list = linkedListCreate();
    linkedListAdd(&list, elem);
    linkedListDestroy(&list, dtor);
}
```
