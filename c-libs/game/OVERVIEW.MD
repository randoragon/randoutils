# Game Library Overview

Here's a brief explainer on some features of the library, the inner-workings
of some structs and functions, and most importantly, the general thought
behind each struct, the intended workflow, etc. This is **not** a complete
documentation, I just spill some of my thoughts in here to keep me on track
with what I want to do with the library.

**NOTE:** Some of the concepts explained here may be outdated or not available in the
library yet. Again, I'm writing this file mainly to organize everything in my head.

## Quick Rundown (TL;DR)

- Objects are immutable templates for creating mutable instances.
- Event handlers are arrays of function pointers, accessed by object indices.

If you don't understand what this means, that's okay, everything from this point
onward is going to be explained in great depth. Don't forget that you can always
refer to the [source code](https://github.com/randoragon/randoutils/tree/master/c-libs/game)
for more details!

## 1. Objects

### 1.1 Overview

An "object", theoretically speaking, is a blueprint or schematic for something.
That "something" can be a video game entity (a character, enemy, wall, etc.),
or something completely different, like a particle, or just some invisible to
the player piece of code running calculations in the background.

Objects are identified by an ID number (`RND_GameObjectIndex` = 16-bit integer).
Objects do not themselves get "spawned" in the game, they act as a template
for creating an *instance*. No instance can exist without a corresponding
object, although objects can exist without corresponding instances. Moreover,
multiple instances of the same object can coexist at the same time.

### 1.2 Creating an Object

Internally, an object is a struct of variables which get passed onto every
instance of that object. Creating a new object involves writing a new struct
and populating it with member variables that an instance will need. For example:

```c
typedef struct
{
    int x, y;
    float health;
    char *name;
} ObjectPlayer;
```

It is up to the user to pick an appropriate object index and keep track of it.
The recommended way of doing this is with a separate header file for macros:

```c
#define OBJECT_INDEX_PLAYER 0
#define OBJECT_INDEX_WALL   1
#define OBJECT_INDEX_ENEMY  2
...
```

Once you have these two things, you can "add" the object and the library will
remember it:

```c
RND_gameObjectAdd(OBJECT_INDEX_PLAYER, sizeof(ObjectPlayer));
```

---

*Optionally*, you can write an object constructor and destructor. Both of them
need to be functions that intake a void pointer (to the object struct) and
return an error code (0 for success):

```c
int objectPlayerCtor(void *object)
{
    ObjectPlayer *o = (ObjectPlayer*)object;
    o->x = 0; o->y = 0; o->health = 15.0;
    if (!(o->name = (char*)malloc(sizeof(char) * 35)))
        return 1;
    return 0;
}

int objectPlayerDtor(void *object) {
    free(((ObjectPlayer*)object)->name);
    return 0;
}
```

Once they're written, all you need to do is assign them to the `RND_ctors`
and `RND_dtors` arrays. They are both built-in and globally included by
the library:

```c
RND_ctors[OBJECT_INDEX_PLAYER] = objectPlayerCtor;
RND_dtors[OBJECT_INDEX_PLAYER] = objectPlayerDtor;
```

If either the constructor or destructor returns non-0, an adequate warning
message will be printed to `stderr` during instance initialization/destruction.

---

Again, it is up to the user to utilize these library mechanisms the way they
were intended to, nothing will prevent one from disobeying these conventions.

## 2. Instances

### 2.1 Overview

An instance is an independent copy of an object's struct data. For example,
a typical game will have a player and an enemy object, but there's only
one *instance* of a player object, while there may be multiple instances
of enemy objects. Instances are containers for meaningful information about
what's happening in the game. That information can be altered during runtime
as you see fit.

### 2.2 Spawning an Instance

Once you've created an object, you can create an instance of it like so:

```c
RND_GameInstanceId id = RND_gameInstanceSpawn(OBJECT_INDEX_PLAYER);
```

`RND_gameInstanceSpawn()` will return a unique instance id which can be used
in various instance functions. Pretty much all functions that operate on
instances use the instance id as an identifier.

### 2.3 The `RND_instances` Array

All spawned instances that have not yet been despawned reside in a global
`RND_instances` array. The array is composed of `RND_GameInstance` structs:

```c
struct RND_GameInstance
{
    RND_GameInstanceId id;
    RND_GameObjectIndex index;
    void *data;
};

```

The array is indexed by instance ids with `id=0` reserved as "no instance".


### 2.3 Accessing an Instance's Data



### 1.3 Handling Events

The majority of game engines operates on events which execute specific code.
For example, for each object type there's separate code that gets executed
every step, but there also might be code that gets executed on button events,
or some custom events orchestrated by the developer that run scripts.

This library uses `RND_Handlers` to create arrays of function
pointers. The functions behind these pointers intake pointers to object structs
and return an integer error code (or 0 for success). This makes it easy to
access a specific object's code associated with a specific event.

Internally, `RND_Handlers` is just a typedef to an array of function pointers:

```c
#define RND_OBJECT_MAX 0xffff
...
typedef int (*RND_Handlers[RND_OBJECT_MAX])(void*);
```

---

Here's an example of how to create a button event handler
for our `ObjectPlayer` which will cut its health in half:

First, write a handler function:

```c
int objectPlayerEventButtonHandler(void *object)
{
    ((ObjectPlayer*)object)->health /= 2.0;
    return 0;
}
```

Then, initialize a new handlers array and add the handler to it:

```c
RND_Handlers button_handlers = {0};
button_handlers[OBJECT_INDEX_PLAYER] = objectPlayerEventButtonHandler;
```

Finally, run all handlers stored this way when it's appropriate:

```c
if (button_event_received) {
    /* The call below iterates through all instances, and those of them
     * that have a handler stored in button_handlers will have that 
     * handler run with their unique copy of object struct data.
     * If any handler returns non-0, a warning will be printed to stderr.
     */
    RND_gameRunHandlers(button_handlers);
}

```

---
