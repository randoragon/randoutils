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
RND_gameObjectAdd("ObjectPlayer", OBJECT_INDEX_PLAYER, sizeof(ObjectPlayer));
```

The first argument is a string representing the object. This can be anything you
want, but it cannot be empty. This string is used for example in warning/error
messages to make it easier to identify objects, because for humans it's faster
to recognize than an index number.

Typically you want the name string to be the same as the name of the object struct.
Because it's a bit painful to have to write the same thing twice, there's a macro
that automatically calls `RND_gameObjectAdd` with the right parameters:

```c
#define RND_GAME_OBJECT_ADD(struct, index) RND_gameObjectAdd(#struct, index, sizeof(struct))
```

Hence, this is the **recommended** way of adding new objects:

```c
RND_GAME_OBJECT_ADD(ObjectPlayer, OBJECT_INDEX_PLAYER);
```

You can obtain an object's name string with `RND_gameObjectGetName(index)`.

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

`RND_gameInstanceSpawn()` will return a unique **instance id** which can be used
in various instance functions. Pretty much all functions that operate on
instances use the instance id as an identifier.

An instance id is a 64-bit unsigned integer, and every single instance must
have a unique one during a single runtime, so theoretically if you ran out of
(2^64-1) numbers (0 is reserved for errors and exceptions), the library would
crash with a fatal error.  To give some perspective though, to grind through
2^64 ids you would need to spawn 1000000 instances per tick in a 60FPS game
over the course of nearly 10 thousand years, so you would most definitely
run out of memory first anyway.

### 2.3 The Instances Array

All spawned instances that have not yet been killed reside in the global
`RND_instances` array. The array is composed of `RND_GameInstance` structs:

```c
struct RND_GameInstance
{
    bool is_alive;
    RND_GameObjectIndex index;
    void *data;
};
```

The array is indexed by instance ids with `id=0` reserved as "no instance".
Instance ids are 64-bit integers and it's impossible to allocate that big
of an array, so the starting size is 2^16 elements, and if the instance
count during runtime crosses that threshold, the array size is doubled.
The array's current size is stored in `RND_instances_size`.
It is therefore possible to iterate through all existing instances and
access their object structs by doing the following:

```c
for (RND_GameInstanceId id = 1; id < RND_instances_size; id++) {
    RND_GameInstance *inst = RND_instances + id;
    // For example, to set all players' health to 0:
    if (inst->is_alive && inst->index == OBJECT_INDEX_PLAYER) {
        ((ObjectPlayer*)inst->data)->health = 0;
    }
}
```

### 2.4 Accessing an Instance

In order to access instance variables (variables from the object struct),
you need its unique instance id and the corresponding object type:

```c
ObjectPlayer *data = (ObjectPlayer*)(RND_instances[id].data);
data->x = 15;
```

Since it is very common to operate on instance variables, and a little
encumbering to have to recast void pointers every single time, there's
a macro that slightly simplifies this process:

```c
#define RND_GAME_INST(id, struct) (*((struct*)(RND_instances[id].data))) 
```

Here's the equivalent of the first operation on `ObjectPlayer`, this time
using the macro shortcut:

```c
RND_GAME_INST(id, ObjectPlayer).x = 15;
```

### 2.5 Killing an Instance

Once an instance's job is finished, you should deallocate its memory and
set its `is_alive` member variable to `false`. This can be accomplished
by using the `RND_gameInstanceKill` function:

```c
RND_gameInstanceKill(id);
```

If no instance with the supplied id is alive, the function will print
a warning message and peacefully exit, returning 0 for success.

## 3. Event Handlers

### 3.1 Overview

The majority of game engines operate on events which execute specific code.
For example, for each object type there's separate code that gets executed
every step, but there also might be code that gets executed on button events,
or some custom events orchestrated by the developer that run scripts.

In short, the right way to handle events for all spawned instances is to
create a new event handler. If execution order is important, then you will need
a **priority function**, which is basically a function that intakes object indices
and returns `int` values that represent that object's priority.  For example,
in the context of handling a draw event, lower priority means instances of that
object will be drawn earlier, meaning any instances of a higher priority will
be drawn on top.

### 3.2 Example Usage

Here's an example of how to create a button event handler
for our `ObjectPlayer` which will cut its health in half.

First, we need to decide whether or not we need a priority function. For this
example we will skip this step, because it is not needed when we're considering
a button even with only one receiving object type. However, to illustrate what
priority functions look like, here's an arbitrary example:

```c
int examplePriorityFunc(RND_GameObjectIndex index)
{
    switch(index) {
        case ObjectPlayer: return 2;
        case ObjectWall:   return -2;
        case ObjectEnemy:  return 1;
        default:           return 0;
    }
}
```

The important thing to note is that it doesn't matter what numbers you use
for priorities, it's only the relations between them that matter.

Step 2, write a handler function for each object that needs it (in our case
it's only the player, but normally you would have multiple functions, one
for each object type that's relevant):

```c
int objectPlayerEventButtonHandler(void *object)
{
    ((ObjectPlayer*)object)->health /= 2.0;
    return 0;
}
```

Then, initialize a new handlers array and add the handler to it:

```c
/* RND_gameHandlerCreate intakes a priority function pointer,
 * but you can pass it NULL and everything will be treated equally
 * (like a normal, non-priority queue).
 */
RND_GameHandler *button_handlers = RND_gameHandlerCreate(NULL);
if (button_handlers) {
    // Assign the right handler function to an object index
    RND_gameHandlerAdd(button_handlers, OBJECT_INDEX_PLAYER, objectPlayerEventButtonHandler);
} else {
    fprintf(stderr, "failed to allocate heap memory\n");
}
```

That's everything as far as setting up goes. From this point onward, every single
instance that gets spawned or dies will be added or removed from the handler's
priority queue. This makes executing the handler code as simple as iterating through
the priority queue and calling each instance's respective handler function, which is
precisely what the `RND_gameHandlerRun` function does:

```c
if (button_event_received) {
    RND_gameHandlersRun(button_handlers);
}
```

That's all there is to it. Please note that event handlers have nothing to do with event
detection, they merely **handle** them. In the above code snippet, it is up to some external
library to detect when the button in question is actually pressed (`button_event_received`).

