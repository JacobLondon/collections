/**
 * Keys
 * STR IDENTIFIER_NAME (UPPER)
 * Str IdentifierName (Pascal)
 * str identifier_name (lower)
 * const char * C type (int, char *, ...)
 * 
 * Values
 * STR IDENTIFIER_NAME (UPPER)
 * Str IdentifierName (Pascal)
 * str identifier_name (lower)
 * const char * C type (int, char *, ...)
 * 
 * Functions
 * (a)    const char **(const char *), if T is 'char *' then return const char *
 * NULL   void(const char *)
 * NULL void(const char *)
 * strlen((const char *)a) size_t(const char **)
 * strcmp(a, b)    int(const char *, const char *)
 * 
 * If a value in the dictionary contains the 'zero value' for
 * its type, then it is considered to not contain anything.
 */
#ifndef DEF_DICTDEF_STR_STR_H
#define DEF_DICTDEF_STR_STR_H

#define DEF_PROTO
#include <stddef.h>

#if 0
#define DEF_DICT_STR_STR
#ifdef DEF_DICT_STR_STR
#define const char * int
#define const char * int
#endif
#endif

#define DEF_DICT_STR_STR_REFKEY(a) (a)
#define DEF_DICT_STR_STR_FREEKEY(a) NULL
#define DEF_DICT_STR_STR_FREEVALUE(a) NULL
#define DEF_DICT_STR_STR_SIZEOFKEY(a) strlen((const char *)a)
#define DEF_DICT_STR_STR_CMPKEY(a, b) strcmp(a, b)
#define DEF_DICT_STR_STR_ZEROKEY(a) (!a)
#define DEF_DICT_STR_STR_ZEROVALUE(a) (!a)

struct DictStrStr {
    const char * *keys;
    const char * *values;
    size_t size;
    size_t cap;
};

struct DictArgsStrStr {
    size_t cap;
};

struct DictPairStrStr {
    const char * *key;
    const char * *value;
};

typedef size_t (* DictStrStrHashFunc)(const char * key, size_t bias, size_t max);

DEF_PROTO struct DictStrStr _dict_str_str_init(struct DictArgsStrStr args);     // create a stack dict
#define dict_str_str_init(...) _dict_str_str_init((struct DictArgsStrStr){.cap=6, __VA_ARGS__})
DEF_PROTO struct DictStrStr *_dict_str_str_new(struct DictArgsStrStr args);     // create a heap dict
#define dict_str_str_new(...) _dict_str_str_new((struct DictArgsStrStr){.cap=6, __VA_ARGS__})
DEF_PROTO void dict_str_str_deinit(struct DictStrStr *d);                       // clean up a stack dict
DEF_PROTO void dict_str_str_free(struct DictStrStr *d);                         // free a heap dict
DEF_PROTO void dict_str_str_reserve(struct DictStrStr *d, size_t cap);          // cap must be larger, can check by '.cap == ?'
DEF_PROTO void dict_str_str_set(struct DictStrStr *d, const char * key, const char * value);        // set the value in the dict, will replace existing values
DEF_PROTO const char * *dict_str_str_get(struct DictStrStr *d, const char * key);                   // NULL if not present, else the value
DEF_PROTO struct DictPairStrStr dict_str_str_iter(struct DictStrStr *d);        // get a pair iterator, has key/value pointer
DEF_PROTO struct DictPairStrStr dict_str_str_next(struct DictStrStr *d, struct DictPairStrStr cursor); // get next from iterator
DEF_PROTO void dict_str_str_sethash(DictStrStrHashFunc hash);                   // set the hash function

#endif // DEF_DICTDEF_STR_STR_H

#ifdef DEF_DICT_STR_STR
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static size_t dict_str_str_index(struct DictStrStr *d, const char * key);
static size_t dict_str_str_hash_default(const char * key, size_t bias, size_t max);

static DictStrStrHashFunc dict_str_str_hash = dict_str_str_hash_default;

static size_t dict_str_str_index(struct DictStrStr *d, const char * key)
{
    assert(d);
    size_t ndx;
    size_t bias = 0;
    do {
        ndx = dict_str_str_hash(key, bias, d->cap);
        bias += 7;
        // valid key and keys not equal
    } while (!DEF_DICT_STR_STR_ZEROKEY(d->keys[ndx]) && (DEF_DICT_STR_STR_CMPKEY(d->keys[ndx], key) != 0));
    return ndx;
}

static size_t dict_str_str_hash_default(const char * key, size_t bias, size_t max)
{
    // FNV1A
    unsigned char *buf = (unsigned char *)DEF_DICT_STR_STR_REFKEY(key);
    size_t size = DEF_DICT_STR_STR_SIZEOFKEY(buf);
    size_t hash = 0x811C9DC5 + bias;
    size_t i;
    for (i = 0; i < size; i++) {
        hash = (buf[i] ^ hash) * 0x01000193;
    }
    return hash % max;
}

struct DictStrStr _dict_str_str_init(struct DictArgsStrStr args)
{
    struct DictStrStr d;
    d.cap = args.cap;
    d.size = 0;
    d.keys = calloc(d.cap, sizeof(*d.keys));
    assert(d.keys);
    d.values = calloc(d.cap, sizeof(*d.values));
    assert(d.values);
    return d;
}

struct DictStrStr *_dict_str_str_new(struct DictArgsStrStr args)
{
    struct DictStrStr *d;
    d = malloc(sizeof(*d));
    assert(d);
    *d = _dict_str_str_init(args);
    return d;
}

void dict_str_str_deinit(struct DictStrStr *d)
{
    size_t i;
    assert(d);
    assert(d->keys);
    assert(d->values);

    for (i = 0; i < d->cap; i++) {
        if (!DEF_DICT_STR_STR_ZEROKEY(d->keys[i])) {
            DEF_DICT_STR_STR_FREEKEY(d->keys[i]);
        }
        if (!DEF_DICT_STR_STR_ZEROVALUE(d->values[i])) {
            DEF_DICT_STR_STR_FREEVALUE(d->values[i]);
        }
    }
    memset(d->keys, 0, sizeof(const char *) * d->cap);
    memset(d->values, 0, sizeof(const char *) * d->cap);
    free(d->keys);
    free(d->values);
    memset(d, 0, sizeof(*d));
}

void dict_str_str_free(struct DictStrStr *d)
{
    assert(d);
    assert(d->keys);
    assert(d->values);
    dict_str_str_deinit(d);
    free(d);
}

void dict_str_str_reserve(struct DictStrStr *d, size_t cap)
{
    const char * *oldkeys;
    const char * *oldvalues;
    size_t oldcap;
    size_t i;
    size_t ndx;

    assert(d);
    assert(d->keys);
    assert(d->values);
    assert(d->cap < cap);

    oldkeys = d->keys;
    oldvalues = d->values;
    oldcap = d->cap;
    d->cap = cap;

    d->keys = calloc(d->cap, sizeof(*d->keys));
    assert(d->keys);
    d->values = calloc(d->cap, sizeof(*d->values));
    assert(d->values);

    for (i = 0; i < oldcap; i++) {
        if (!DEF_DICT_STR_STR_ZEROKEY(oldkeys[i])) {
            ndx = dict_str_str_index(d, oldkeys[i]);
            d->keys[ndx] = oldkeys[i];
            d->values[ndx] = oldvalues[i];
        }
    }

    memset(oldkeys, 0, oldcap * sizeof(*oldkeys));
    memset(oldvalues, 0, oldcap * sizeof(*oldvalues));
    free(oldkeys);
    free(oldvalues);
}

void dict_str_str_set(struct DictStrStr *d, const char * key, const char * value)
{
    size_t ndx;
    assert(d);
    assert(d->keys);
    assert(d->values);

    ndx = dict_str_str_index(d, key);
    //printf("Setting at %zu, size = %zu\n", ndx, d->size);

    // existing key
    if (!DEF_DICT_STR_STR_ZEROKEY(d->keys[ndx])) {
        DEF_DICT_STR_STR_FREEKEY(d->keys[ndx]);
        DEF_DICT_STR_STR_FREEVALUE(d->values[ndx]);
        d->keys[ndx] = key;
        d->values[ndx] = value;
    }
    // new key
    else {
        d->keys[ndx] = key;
        d->values[ndx] = value;
        d->size++;

        if (d->size > d->cap * 2 / 3) {
            dict_str_str_reserve(d, d->cap * 2);
        }
    }
}

const char * *dict_str_str_get(struct DictStrStr *d, const char * key)
{
    size_t ndx;
    assert(d);
    assert(d->keys);
    assert(d->values);

    ndx = dict_str_str_index(d, key);
    return DEF_DICT_STR_STR_ZEROKEY(d->values[ndx]) ? NULL: &d->values[ndx];
}

struct DictPairStrStr dict_str_str_iter(struct DictStrStr *d)
{
    size_t i;
    assert(d);
    if (d->size == 0) {
        return (struct DictPairStrStr){NULL, NULL};
    }

    for (i = 0; i < d->cap; i++) {
        if (!DEF_DICT_STR_STR_ZEROKEY(d->keys[i])) {
            return (struct DictPairStrStr){
                .key = &d->keys[i],
                .value = &d->values[i],
            };
        }
    }
    return (struct DictPairStrStr){NULL, NULL};
}

struct DictPairStrStr dict_str_str_next(struct DictStrStr *d, struct DictPairStrStr cursor)
{
    size_t i;
    assert(d);
    assert(d->keys);
    assert(d->values);

    if (cursor.key == NULL) {
        return (struct DictPairStrStr){NULL, NULL};
    }

    i = (size_t)(cursor.key - d->keys + 1);
    for ( ; i < d->cap; i++) {
        if (!DEF_DICT_STR_STR_ZEROKEY(d->keys[i])) {
            return (struct DictPairStrStr){
                .key = &d->keys[i],
                .value = &d->values[i],
            };
        }
    }
    return (struct DictPairStrStr){NULL, NULL};
}

void dict_str_str_sethash(DictStrStrHashFunc hash)
{
    assert(hash);
    dict_str_str_hash = hash;
}

#endif // DEF_DICT_STR_STR
/**
 * Keys
 * STR IDENTIFIER_NAME (UPPER)
 * Str IdentifierName (Pascal)
 * str identifier_name (lower)
 * const char * C type (int, char *, ...)
 * 
 * Values
 * VECTOR_DICT_STR_STR IDENTIFIER_NAME (UPPER)
 * VectorDictStrStr IdentifierName (Pascal)
 * vector_dict_str_str identifier_name (lower)
 * struct VectorDictStrStr * C type (int, char *, ...)
 * 
 * Functions
 * (a)    const char **(const char *), if T is 'char *' then return const char *
 * NULL   void(const char *)
 * vector_dict_str_str_free(a) void(struct VectorDictStrStr *)
 * strlen((const char *)a) size_t(const char **)
 * strcmp(a, b)    int(const char *, const char *)
 * 
 * If a value in the dictionary contains the 'zero value' for
 * its type, then it is considered to not contain anything.
 */
#ifndef DEF_DICTDEF_STR_VECTOR_DICT_STR_STR_H
#define DEF_DICTDEF_STR_VECTOR_DICT_STR_STR_H

#define DEF_PROTO
#include <stddef.h>

#if 0
#define DEF_DICT_STR_VECTOR_DICT_STR_STR
#ifdef DEF_DICT_STR_VECTOR_DICT_STR_STR
#define const char * int
#define struct VectorDictStrStr * int
#endif
#endif

#define DEF_DICT_STR_VECTOR_DICT_STR_STR_REFKEY(a) (a)
#define DEF_DICT_STR_VECTOR_DICT_STR_STR_FREEKEY(a) NULL
#define DEF_DICT_STR_VECTOR_DICT_STR_STR_FREEVALUE(a) vector_dict_str_str_free(a)
#define DEF_DICT_STR_VECTOR_DICT_STR_STR_SIZEOFKEY(a) strlen((const char *)a)
#define DEF_DICT_STR_VECTOR_DICT_STR_STR_CMPKEY(a, b) strcmp(a, b)
#define DEF_DICT_STR_VECTOR_DICT_STR_STR_ZEROKEY(a) (!a)
#define DEF_DICT_STR_VECTOR_DICT_STR_STR_ZEROVALUE(a) (!a)

struct DictStrVectorDictStrStr {
    const char * *keys;
    struct VectorDictStrStr * *values;
    size_t size;
    size_t cap;
};

struct DictArgsStrVectorDictStrStr {
    size_t cap;
};

struct DictPairStrVectorDictStrStr {
    const char * *key;
    struct VectorDictStrStr * *value;
};

typedef size_t (* DictStrVectorDictStrStrHashFunc)(const char * key, size_t bias, size_t max);

DEF_PROTO struct DictStrVectorDictStrStr _dict_str_vector_dict_str_str_init(struct DictArgsStrVectorDictStrStr args);     // create a stack dict
#define dict_str_vector_dict_str_str_init(...) _dict_str_vector_dict_str_str_init((struct DictArgsStrVectorDictStrStr){.cap=6, __VA_ARGS__})
DEF_PROTO struct DictStrVectorDictStrStr *_dict_str_vector_dict_str_str_new(struct DictArgsStrVectorDictStrStr args);     // create a heap dict
#define dict_str_vector_dict_str_str_new(...) _dict_str_vector_dict_str_str_new((struct DictArgsStrVectorDictStrStr){.cap=6, __VA_ARGS__})
DEF_PROTO void dict_str_vector_dict_str_str_deinit(struct DictStrVectorDictStrStr *d);                       // clean up a stack dict
DEF_PROTO void dict_str_vector_dict_str_str_free(struct DictStrVectorDictStrStr *d);                         // free a heap dict
DEF_PROTO void dict_str_vector_dict_str_str_reserve(struct DictStrVectorDictStrStr *d, size_t cap);          // cap must be larger, can check by '.cap == ?'
DEF_PROTO void dict_str_vector_dict_str_str_set(struct DictStrVectorDictStrStr *d, const char * key, struct VectorDictStrStr * value);        // set the value in the dict, will replace existing values
DEF_PROTO struct VectorDictStrStr * *dict_str_vector_dict_str_str_get(struct DictStrVectorDictStrStr *d, const char * key);                   // NULL if not present, else the value
DEF_PROTO struct DictPairStrVectorDictStrStr dict_str_vector_dict_str_str_iter(struct DictStrVectorDictStrStr *d);        // get a pair iterator, has key/value pointer
DEF_PROTO struct DictPairStrVectorDictStrStr dict_str_vector_dict_str_str_next(struct DictStrVectorDictStrStr *d, struct DictPairStrVectorDictStrStr cursor); // get next from iterator
DEF_PROTO void dict_str_vector_dict_str_str_sethash(DictStrVectorDictStrStrHashFunc hash);                   // set the hash function

#endif // DEF_DICTDEF_STR_VECTOR_DICT_STR_STR_H

#ifdef DEF_DICT_STR_VECTOR_DICT_STR_STR
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static size_t dict_str_vector_dict_str_str_index(struct DictStrVectorDictStrStr *d, const char * key);
static size_t dict_str_vector_dict_str_str_hash_default(const char * key, size_t bias, size_t max);

static DictStrVectorDictStrStrHashFunc dict_str_vector_dict_str_str_hash = dict_str_vector_dict_str_str_hash_default;

static size_t dict_str_vector_dict_str_str_index(struct DictStrVectorDictStrStr *d, const char * key)
{
    assert(d);
    size_t ndx;
    size_t bias = 0;
    do {
        ndx = dict_str_vector_dict_str_str_hash(key, bias, d->cap);
        bias += 7;
        // valid key and keys not equal
    } while (!DEF_DICT_STR_VECTOR_DICT_STR_STR_ZEROKEY(d->keys[ndx]) && (DEF_DICT_STR_VECTOR_DICT_STR_STR_CMPKEY(d->keys[ndx], key) != 0));
    return ndx;
}

static size_t dict_str_vector_dict_str_str_hash_default(const char * key, size_t bias, size_t max)
{
    // FNV1A
    unsigned char *buf = (unsigned char *)DEF_DICT_STR_VECTOR_DICT_STR_STR_REFKEY(key);
    size_t size = DEF_DICT_STR_VECTOR_DICT_STR_STR_SIZEOFKEY(buf);
    size_t hash = 0x811C9DC5 + bias;
    size_t i;
    for (i = 0; i < size; i++) {
        hash = (buf[i] ^ hash) * 0x01000193;
    }
    return hash % max;
}

struct DictStrVectorDictStrStr _dict_str_vector_dict_str_str_init(struct DictArgsStrVectorDictStrStr args)
{
    struct DictStrVectorDictStrStr d;
    d.cap = args.cap;
    d.size = 0;
    d.keys = calloc(d.cap, sizeof(*d.keys));
    assert(d.keys);
    d.values = calloc(d.cap, sizeof(*d.values));
    assert(d.values);
    return d;
}

struct DictStrVectorDictStrStr *_dict_str_vector_dict_str_str_new(struct DictArgsStrVectorDictStrStr args)
{
    struct DictStrVectorDictStrStr *d;
    d = malloc(sizeof(*d));
    assert(d);
    *d = _dict_str_vector_dict_str_str_init(args);
    return d;
}

void dict_str_vector_dict_str_str_deinit(struct DictStrVectorDictStrStr *d)
{
    size_t i;
    assert(d);
    assert(d->keys);
    assert(d->values);

    for (i = 0; i < d->cap; i++) {
        if (!DEF_DICT_STR_VECTOR_DICT_STR_STR_ZEROKEY(d->keys[i])) {
            DEF_DICT_STR_VECTOR_DICT_STR_STR_FREEKEY(d->keys[i]);
        }
        if (!DEF_DICT_STR_VECTOR_DICT_STR_STR_ZEROVALUE(d->values[i])) {
            DEF_DICT_STR_VECTOR_DICT_STR_STR_FREEVALUE(d->values[i]);
        }
    }
    memset(d->keys, 0, sizeof(const char *) * d->cap);
    memset(d->values, 0, sizeof(struct VectorDictStrStr *) * d->cap);
    free(d->keys);
    free(d->values);
    memset(d, 0, sizeof(*d));
}

void dict_str_vector_dict_str_str_free(struct DictStrVectorDictStrStr *d)
{
    assert(d);
    assert(d->keys);
    assert(d->values);
    dict_str_vector_dict_str_str_deinit(d);
    free(d);
}

void dict_str_vector_dict_str_str_reserve(struct DictStrVectorDictStrStr *d, size_t cap)
{
    const char * *oldkeys;
    struct VectorDictStrStr * *oldvalues;
    size_t oldcap;
    size_t i;
    size_t ndx;

    assert(d);
    assert(d->keys);
    assert(d->values);
    assert(d->cap < cap);

    oldkeys = d->keys;
    oldvalues = d->values;
    oldcap = d->cap;
    d->cap = cap;

    d->keys = calloc(d->cap, sizeof(*d->keys));
    assert(d->keys);
    d->values = calloc(d->cap, sizeof(*d->values));
    assert(d->values);

    for (i = 0; i < oldcap; i++) {
        if (!DEF_DICT_STR_VECTOR_DICT_STR_STR_ZEROKEY(oldkeys[i])) {
            ndx = dict_str_vector_dict_str_str_index(d, oldkeys[i]);
            d->keys[ndx] = oldkeys[i];
            d->values[ndx] = oldvalues[i];
        }
    }

    memset(oldkeys, 0, oldcap * sizeof(*oldkeys));
    memset(oldvalues, 0, oldcap * sizeof(*oldvalues));
    free(oldkeys);
    free(oldvalues);
}

void dict_str_vector_dict_str_str_set(struct DictStrVectorDictStrStr *d, const char * key, struct VectorDictStrStr * value)
{
    size_t ndx;
    assert(d);
    assert(d->keys);
    assert(d->values);

    ndx = dict_str_vector_dict_str_str_index(d, key);
    //printf("Setting at %zu, size = %zu\n", ndx, d->size);

    // existing key
    if (!DEF_DICT_STR_VECTOR_DICT_STR_STR_ZEROKEY(d->keys[ndx])) {
        DEF_DICT_STR_VECTOR_DICT_STR_STR_FREEKEY(d->keys[ndx]);
        DEF_DICT_STR_VECTOR_DICT_STR_STR_FREEVALUE(d->values[ndx]);
        d->keys[ndx] = key;
        d->values[ndx] = value;
    }
    // new key
    else {
        d->keys[ndx] = key;
        d->values[ndx] = value;
        d->size++;

        if (d->size > d->cap * 2 / 3) {
            dict_str_vector_dict_str_str_reserve(d, d->cap * 2);
        }
    }
}

struct VectorDictStrStr * *dict_str_vector_dict_str_str_get(struct DictStrVectorDictStrStr *d, const char * key)
{
    size_t ndx;
    assert(d);
    assert(d->keys);
    assert(d->values);

    ndx = dict_str_vector_dict_str_str_index(d, key);
    return DEF_DICT_STR_VECTOR_DICT_STR_STR_ZEROKEY(d->values[ndx]) ? NULL: &d->values[ndx];
}

struct DictPairStrVectorDictStrStr dict_str_vector_dict_str_str_iter(struct DictStrVectorDictStrStr *d)
{
    size_t i;
    assert(d);
    if (d->size == 0) {
        return (struct DictPairStrVectorDictStrStr){NULL, NULL};
    }

    for (i = 0; i < d->cap; i++) {
        if (!DEF_DICT_STR_VECTOR_DICT_STR_STR_ZEROKEY(d->keys[i])) {
            return (struct DictPairStrVectorDictStrStr){
                .key = &d->keys[i],
                .value = &d->values[i],
            };
        }
    }
    return (struct DictPairStrVectorDictStrStr){NULL, NULL};
}

struct DictPairStrVectorDictStrStr dict_str_vector_dict_str_str_next(struct DictStrVectorDictStrStr *d, struct DictPairStrVectorDictStrStr cursor)
{
    size_t i;
    assert(d);
    assert(d->keys);
    assert(d->values);

    if (cursor.key == NULL) {
        return (struct DictPairStrVectorDictStrStr){NULL, NULL};
    }

    i = (size_t)(cursor.key - d->keys + 1);
    for ( ; i < d->cap; i++) {
        if (!DEF_DICT_STR_VECTOR_DICT_STR_STR_ZEROKEY(d->keys[i])) {
            return (struct DictPairStrVectorDictStrStr){
                .key = &d->keys[i],
                .value = &d->values[i],
            };
        }
    }
    return (struct DictPairStrVectorDictStrStr){NULL, NULL};
}

void dict_str_vector_dict_str_str_sethash(DictStrVectorDictStrStrHashFunc hash)
{
    assert(hash);
    dict_str_vector_dict_str_str_hash = hash;
}

#endif // DEF_DICT_STR_VECTOR_DICT_STR_STR
/**
 * DICT_STR_STR IDENTIFIER_NAME (UPPER)
 * DictStrStr IdentifierName (Pascal)
 * dict_str_str identifier_name (lower)
 * struct DictStrStr * C type (int, char *, ...)
 */

#ifndef DEF_VECTORDEF_DICT_STR_STR_H
#define DEF_VECTORDEF_DICT_STR_STR_H

#define DEF_PROTO
#include <stddef.h>

#if 0
#define DEF_VECTOR_DICT_STR_STR
#ifndef struct DictStrStr *
#define struct DictStrStr * int
#endif
#endif

#define DEF_VECTOR_DICT_STR_STR_ZEROVALUE(a) (!a)
#define DEF_VECTOR_DICT_STR_STR_FREEVALUE(a) dict_str_str_free(a)

struct VectorDictStrStr {
    struct DictStrStr * *buf;
    size_t size;
    size_t cap;
};

struct VectorArgsDictStrStr {
    size_t cap;
};

DEF_PROTO struct VectorDictStrStr _vector_dict_str_str_init(struct VectorArgsDictStrStr args);         // initialize a stack vector
DEF_PROTO struct VectorDictStrStr *_vector_dict_str_str_new(struct VectorArgsDictStrStr args);         // initialize a heap vector
#define vector_dict_str_str_init(...) _vector_dict_str_str_init((struct VectorArgsDictStrStr){.cap=1, __VA_ARGS__})
#define vector_dict_str_str_new(...) _vector_dict_str_str_new((struct VectorArgsDictStrStr){.cap=1, __VA_ARGS__})
DEF_PROTO void vector_dict_str_str_deinit(struct VectorDictStrStr *v);                          // deinitialize a stack vector
DEF_PROTO void vector_dict_str_str_free(struct VectorDictStrStr *v);                            // delete a heap vector
DEF_PROTO void vector_dict_str_str_reserve(struct VectorDictStrStr *v, size_t cap);             // set reserve capacity
DEF_PROTO void vector_dict_str_str_push(struct VectorDictStrStr *v, struct DictStrStr * value);                  // put a new item at the end
DEF_PROTO void vector_dict_str_str_pop(struct VectorDictStrStr *v);                             // remove the item from the end, calling the destructor as necessary
DEF_PROTO struct DictStrStr * *vector_dict_str_str_get(struct VectorDictStrStr *v, size_t ndx);                  // get the item at the index. Does _NO_ bounds checking
DEF_PROTO void vector_dict_str_str_set(struct VectorDictStrStr *v, size_t ndx, struct DictStrStr * value);       // set the item at the index. Does _NO_ bounds checking
DEF_PROTO struct DictStrStr * *vector_dict_str_str_iter(struct VectorDictStrStr *v);                             // get an iterator to call next on, or NULL if empty
DEF_PROTO struct DictStrStr * *vector_dict_str_str_next(struct VectorDictStrStr *v, struct DictStrStr * *cursor);                 // return pointer to next item or NULL if done

#endif // DEF_VECTORDEF_DICT_STR_STR_H

#ifdef DEF_VECTOR_DICT_STR_STR
#include <assert.h>
#include <stdlib.h>
#include <memory.h>

struct VectorDictStrStr _vector_dict_str_str_init(struct VectorArgsDictStrStr args)
{
    struct VectorDictStrStr v;
    v.size = 0;
    v.cap = args.cap;
    v.buf = malloc(v.cap * sizeof(struct DictStrStr *));
    assert(v.buf);

    return v;
}

struct VectorDictStrStr *_vector_dict_str_str_new(struct VectorArgsDictStrStr args)
{
    struct VectorDictStrStr *v;
    v = malloc(sizeof(*v));
    assert(v);
    *v = _vector_dict_str_str_init(args);
    return v;
}

void vector_dict_str_str_deinit(struct VectorDictStrStr *v)
{
    size_t i;
    assert(v);
    assert(v->buf);
    for (i = 0; i < v->size; i++) {
        if (!DEF_VECTOR_DICT_STR_STR_ZEROVALUE(v->buf[i])) {
            DEF_VECTOR_DICT_STR_STR_FREEVALUE(v->buf[i]);
        }
    }
    free(v->buf);
    memset(v, 0, sizeof(*v));
}

void vector_dict_str_str_free(struct VectorDictStrStr *v)
{
    assert(v);
    assert(v->buf);
    vector_dict_str_str_deinit(v);
    free(v);
}

void vector_dict_str_str_reserve(struct VectorDictStrStr *v, size_t cap)
{
    void *tmp;
    assert(v);
    assert(v->buf);
    tmp = realloc(v->buf, sizeof(struct DictStrStr *) * cap);
    assert(tmp);
    v->buf = tmp;
    v->cap = cap;
    if (v->size > cap) {
        v->size = cap;
    }
}

void vector_dict_str_str_push(struct VectorDictStrStr *v, struct DictStrStr * value)
{
    assert(v);
    assert(v->buf);
    if (v->size == v->cap) {
        vector_dict_str_str_reserve(v, v->cap * 2);
    }
    v->buf[v->size] = value;
    v->size++;
}

void vector_dict_str_str_pop(struct VectorDictStrStr *v)
{
    assert(v);
    assert(v->buf);
    if (v->size == 0) {
        return;
    }

    if (!DEF_VECTOR_DICT_STR_STR_ZEROVALUE(v->buf[v->size - 1])) {
        DEF_VECTOR_DICT_STR_STR_FREEVALUE(v->buf[v->size - 1]);
    }
    v->size--;
}

struct DictStrStr * *vector_dict_str_str_get(struct VectorDictStrStr *v, size_t ndx)
{
    assert(v);
    assert(v->buf);
    assert(ndx < v->size);
    return &v->buf[ndx];
}

void vector_dict_str_str_set(struct VectorDictStrStr *v, size_t ndx, struct DictStrStr * value)
{
    assert(v);
    assert(v->buf);
    assert(ndx < v->size);
    v->buf[ndx] = value;
}

struct DictStrStr * *vector_dict_str_str_iter(struct VectorDictStrStr *v)
{
    struct DictStrStr * *p;
    assert(v);
    assert(v->buf);
    if (v->size == 0) {
        return NULL;
    }

    p = &v->buf[0];
    return (void *)p;
}

struct DictStrStr * *vector_dict_str_str_next(struct VectorDictStrStr *v, struct DictStrStr * *cursor)
{
    struct DictStrStr * *p;
    assert(v);
    assert(v->buf);
    assert(cursor);
    if ((size_t)(cursor - v->buf) >= v->size - 1) {
        return NULL;
    }

    p = &cursor[1];
    return p;
}

#endif // DEF_VECTOR_DICT_STR_STR
