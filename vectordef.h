/**
 * $NN identifier name (UPPER)
 * $Nn identifier name (Pascal)
 * $nn identifier name (lower)
 * $T type
 */

#ifndef JLIBS_VECTORDEF_$NN_H
#define JLIBS_VECTORDEF_$NN_H

#include <stddef.h>

#if 0
#define USE_VECTOR_$NN
#ifndef $T
#define $T int
#endif
#endif

struct Vector$Nn {
    $T *buf;
    size_t size;
    size_t cap;
    size_t isize;
};

struct VectorArgs$Nn {
    size_t cap;
    void (* ifree)($T *item);
};

struct Vector$Nn _vector_$nn_init(struct VectorArgs$Nn args);
struct Vector$Nn *_vector_$nn_new(struct VectorArgs$Nn args);

#define vector_$nn_init(...) _vector_$nn_init((struct VectorArgs$Nn){cap: 1, ifree: NULL, __VA_ARGS__})
#define vector_$nn_new(...) _vector_$nn_new((struct VectorArgs$Nn){cap: 1, ifree: NULL, __VA_ARGS__})
void vector_$nn_deinit(struct Vector$Nn *v);
void vector_$nn_free(struct Vector$Nn *v);

void vector_$nn_reserve(struct Vector$Nn *v, size_t cap);
void vector_$nn_push(struct Vector$Nn *v, $T value);
void vector_$nn_pop(struct Vector$Nn *v);
$T vector_$nn_get(struct Vector$Nn *v, size_t ndx);
void vector_$nn_set(struct Vector$Nn *v, size_t ndx, $T value);
$T *vector_$nn_iter(struct Vector$Nn *v);
$T *vector_$nn_next(struct Vector$Nn *v, $T *cursor);

#ifdef USE_VECTOR_$NN

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <memory.h>

void (* vector_$nn_ifree)($T *item) = NULL;

struct Vector$Nn _vector_$nn_init(struct VectorArgs$Nn args)
{
    struct Vector$Nn v;
    v.size = 0;
    v.cap = args.cap;
    v.isize = sizeof($T);
    v.buf = malloc(v.cap * v.isize);
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
    tmp = realloc(v->buf, v->isize * cap);
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
    if (cursor - v->buf >= v->size - 1) {
        return NULL;
    }

    $T *p = &cursor[1];
    return p;
}

#endif

#endif // JLIBS_VECTORDEF_$NN_H
