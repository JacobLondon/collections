/**
 * $NN IDENTIFIER_NAME (UPPER)
 * $Nn IdentifierName (Pascal)
 * $nn identifier_name (lower)
 * $T C type (int, char *, ...)
 */

#ifndef DEF_VECTORDEF_$NN_H
#define DEF_VECTORDEF_$NN_H

#include "shared.h"
#include <stddef.h>

#if 0
#define DEF_VECTOR_$NN
#ifndef $T
#define $T int
#endif
#endif

struct Vector$Nn {
    $T *buf;
    size_t size;
    size_t cap;
};

struct VectorArgs$Nn {
    size_t cap;
    void (* ifree)($T *item);
};

DEF_PROTO struct Vector$Nn _vector_$nn_init(struct VectorArgs$Nn args);         // initialize a stack vector
DEF_PROTO struct Vector$Nn *_vector_$nn_new(struct VectorArgs$Nn args);         // initialize a heap vector
#define vector_$nn_init(...) _vector_$nn_init((struct VectorArgs$Nn){cap: 1, ifree: NULL, __VA_ARGS__})
#define vector_$nn_new(...) _vector_$nn_new((struct VectorArgs$Nn){cap: 1, ifree: NULL, __VA_ARGS__})
DEF_PROTO void vector_$nn_deinit(struct Vector$Nn *v);                          // deinitialize a stack vector
DEF_PROTO void vector_$nn_free(struct Vector$Nn *v);                            // delete a heap vector
DEF_PROTO void vector_$nn_reserve(struct Vector$Nn *v, size_t cap);             // set reserve capacity
DEF_PROTO void vector_$nn_push(struct Vector$Nn *v, $T value);                  // put a new item at the end
DEF_PROTO void vector_$nn_pop(struct Vector$Nn *v);                             // remove the item from the end, calling the destructor as necessary
DEF_PROTO $T vector_$nn_get(struct Vector$Nn *v, size_t ndx);                   // get the item at the index. Does _NO_ bounds checking
DEF_PROTO void vector_$nn_set(struct Vector$Nn *v, size_t ndx, $T value);       // set the item at the index. Does _NO_ bounds checking
DEF_PROTO $T *vector_$nn_iter(struct Vector$Nn *v);                             // get an iterator to call next on, or NULL if empty
DEF_PROTO $T *vector_$nn_next(struct Vector$Nn *v, $T *cursor);                 // return pointer to next item or NULL if done

extern void (* vector_$nn_ifree)($T *item);
#ifdef DEF_VECTOR_$NN
#include <assert.h>
#include <stdlib.h>
#include <memory.h>

void (* vector_$nn_ifree)($T *item) = NULL;

struct Vector$Nn _vector_$nn_init(struct VectorArgs$Nn args)
{
    struct Vector$Nn v;
    v.size = 0;
    v.cap = args.cap;
    v.buf = malloc(v.cap * sizeof($T));
    assert(v.buf);

    if (args.ifree) {
        vector_$nn_ifree = args.ifree;
    }
    return v;
}

struct Vector$Nn *_vector_$nn_new(struct VectorArgs$Nn args)
{
    struct Vector$Nn *v;
    v = malloc(sizeof(*v));
    assert(v);
    *v = _vector_$nn_init(args);
    return v;
}

void vector_$nn_deinit(struct Vector$Nn *v)
{
    assert(v);
    if (v->buf) {
        if (vector_$nn_ifree) {
            size_t i;
            for (i = 0; i < v->size; i++) {
                vector_$nn_ifree(&v->buf[i]);
            }
        }
        free(v->buf);
    }
    memset(v, 0, sizeof(*v));
}

void vector_$nn_free(struct Vector$Nn *v)
{
    assert(v);
    vector_$nn_deinit(v);
    free(v);
}

void vector_$nn_reserve(struct Vector$Nn *v, size_t cap)
{
    void *tmp;
    assert(v);
    tmp = realloc(v->buf, sizeof($T) * cap);
    assert(tmp);
    v->buf = tmp;
    v->cap = cap;
    if (v->size > cap) {
        v->size = cap;
    }
}

void vector_$nn_push(struct Vector$Nn *v, $T value)
{
    assert(v);
    if (v->size == v->cap) {
        vector_$nn_reserve(v, v->cap * 2);
    }
    v->buf[v->size] = value;
    v->size++;
}

void vector_$nn_pop(struct Vector$Nn *v)
{
    assert(v);
    if (v->size == 0) {
        return;
    }

    if (vector_$nn_ifree) {
        vector_$nn_ifree(&v->buf[v->size - 1]);
    }
    v->size--;
}

$T vector_$nn_get(struct Vector$Nn *v, size_t ndx)
{
    assert(v);
    return v->buf[ndx];
}

void vector_$nn_set(struct Vector$Nn *v, size_t ndx, $T value)
{
    assert(v);
    v->buf[ndx] = value;
}

$T *vector_$nn_iter(struct Vector$Nn *v)
{
    assert(v);
    if (v->size == 0) {
        return NULL;
    }

    $T *p = &v->buf[0];
    return (void *)p;
}

$T *vector_$nn_next(struct Vector$Nn *v, $T *cursor)
{
    assert(v);
    assert(cursor);
    if ((size_t)(cursor - v->buf) >= v->size - 1) {
        return NULL;
    }

    $T *p = &cursor[1];
    return p;
}

#endif // DEF_VECTOR_$NN

#endif // DEF_VECTORDEF_$NN_H
