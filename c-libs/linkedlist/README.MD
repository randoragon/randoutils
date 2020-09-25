# LinkedList

## Description

This is a small self-written library implementing a linked list struct and related functions.
I wrote it so that I can use it in my other C projects.

## Dependencies

- [RND\_ErrMsg](https://github.com/randoragon/randoutils/tree/master/c-libs/errmsg)

## Installation

Run the following code, if necessary as root:

```sh
make install
```

The header file will be copied to `/usr/include/` and the library file to `/usr/lib/`.

## Usage

Make sure to `#include <RND_LinkedList.h>` and link with `librnd_linkedlist.so` (this is usually
done by passing the `-lrnd_linkedlist` flag to the compiler).

## Examples

For details and more functions please refer to the source code, it should be intuitive enough
for anyone having a grasp on linked lists (also take a look at `example.c`).
The examples shown here are only a portion of the library.

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

Also note that `RND_linkedListGet` is only meant to be used for single retrievals, it is
very inefficient for iterative purposes, so do NOT do the following:

```c
for (int i = 0; i < SIZE; i++) {
    RND_linkedListGet(&list, i); // this is BAD!
    ...
}
```

Instead, rely on the implementation of the list and do this:

```c
for (RND_LinkedList *elem = list; list; list = list->next) {
    ...
}
```
