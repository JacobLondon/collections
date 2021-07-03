/**
 * Keys
 * $TT IDENTIFIER_NAME (UPPER)
 * $Tt IdentifierName (Pascal)
 * $tt identifier_name (lower)
 * $T C type (int, char *, ...)
 * 
 * Values
 * $VV IDENTIFIER_NAME (UPPER)
 * $Vv IdentifierName (Pascal)
 * $vv identifier_name (lower)
 * $V C type (int, char *, ...)
 * 
 * Functions
 * $REFKEY    $T*($T), if T is 'char *' then return $T
 * $FREEKEY   void($T)
 * $FREEVALUE void($V)
 * $SIZEOFKEY size_t($T*)
 * $CMPKEY    int($T, $T)
 * 
 * If a value in the dictionary contains the 'zero value' for
 * its type, then it is considered to not contain anything.
 */
#ifndef DEF_DICTDEF_$TT_$VV_H
#define DEF_DICTDEF_$TT_$VV_H

#include "shared.h"

#if 0
#define DEF_DICT_$TT_$VV
#ifdef DEF_DICT_$TT_$VV
#define $T int
#define $V int
#endif
#endif

#define DEF_DICT_$TT_$VV_REFKEY(a) $REFKEY
#define DEF_DICT_$TT_$VV_FREEKEY(a) $FREEKEY
#define DEF_DICT_$TT_$VV_FREEVALUE(a) $FREEVALUE
#define DEF_DICT_$TT_$VV_SIZEOFKEY(a) $SIZEOFKEY
#define DEF_DICT_$TT_$VV_CMPKEY(a, b) $CMPKEY
#define DEF_DICT_$TT_$VV_ZEROKEY(a) $ZEROKEY
#define DEF_DICT_$TT_$VV_ZEROVALUE(a) $ZEROVALUE

struct Dict$Tt$Vv {
    $T *keys;
    $V *values;
    size_t size;
    size_t cap;
};

struct DictArgs$Tt$Vv {
    size_t cap;
};

struct DictPair$Tt$Vv {
    $T *key;
    $V *value;
};

typedef size_t (* Dict$Tt$VvHashFunc)($T key, size_t bias, size_t max);

DEF_PROTO struct Dict$Tt$Vv _dict_$tt_$vv_init(struct DictArgs$Tt$Vv args);     // create a stack dict
#define dict_$tt_$vv_init(...) _dict_$tt_$vv_init((struct DictArgs$Tt$Vv){cap: 6, __VA_ARGS__})
DEF_PROTO struct Dict$Tt$Vv *_dict_$tt_$vv_new(struct DictArgs$Tt$Vv args);     // create a heap dict
#define dict_$tt_$vv_new(...) _dict_$tt_$vv_new((struct DictArgs$Tt$Vv){cap: 6, __VA_ARGS__})
DEF_PROTO void dict_$tt_$vv_deinit(struct Dict$Tt$Vv *d);                       // clean up a stack dict
DEF_PROTO void dict_$tt_$vv_free(struct Dict$Tt$Vv *d);                         // free a heap dict
DEF_PROTO void dict_$tt_$vv_reserve(struct Dict$Tt$Vv *d, size_t cap);          // cap must be larger, can check by '.cap == ?'
DEF_PROTO void dict_$tt_$vv_set(struct Dict$Tt$Vv *d, $T key, $V value);        // set the value in the dict, will replace existing values
DEF_PROTO $V *dict_$tt_$vv_get(struct Dict$Tt$Vv *d, $T key);                   // NULL if not present, else the value
DEF_PROTO struct DictPair$Tt$Vv dict_$tt_$vv_iter(struct Dict$Tt$Vv *d);        // get a pair iterator, has key/value pointer
DEF_PROTO struct DictPair$Tt$Vv dict_$tt_$vv_next(struct Dict$Tt$Vv *d, struct DictPair$Tt$Vv cursor); // get next from iterator
DEF_PROTO void dict_$tt_$vv_sethash(Dict$Tt$VvHashFunc hash);                   // set the hash function

#ifdef DEF_DICT_$TT_$VV
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static size_t dict_$tt_$vv_index(struct Dict$Tt$Vv *d, $T key);
static size_t dict_$tt_$vv_hash_default($T key, size_t bias, size_t max);

static Dict$Tt$VvHashFunc dict_$tt_$vv_hash = dict_$tt_$vv_hash_default;

static size_t dict_$tt_$vv_index(struct Dict$Tt$Vv *d, $T key)
{
    assert(d);
    size_t ndx;
    size_t bias = 0;
    do {
        ndx = dict_$tt_$vv_hash(key, bias, d->cap);
        bias += 7;
        // valid key and keys not equal
    } while (d->keys[ndx] && (DEF_DICT_$TT_$VV_CMPKEY(d->keys[ndx], key) != 0));
    return ndx;
}

static size_t dict_$tt_$vv_hash_default($T key, size_t bias, size_t max)
{
    // FNV1A
    unsigned char *buf = (unsigned char *)DEF_DICT_$TT_$VV_REFKEY(key);
    size_t size = DEF_DICT_$TT_$VV_SIZEOFKEY(buf);
    size_t hash = 0x811C9DC5 + bias;
    size_t i;
    for (i = 0; i < size; i++) {
        hash = (buf[i] ^ hash) * 0x01000193;
    }
    return hash % max;
}

struct Dict$Tt$Vv _dict_$tt_$vv_init(struct DictArgs$Tt$Vv args)
{
    struct Dict$Tt$Vv d;
    d.cap = args.cap;
    d.size = 0;
    d.keys = calloc(d.cap, sizeof(*d.keys));
    assert(d.keys);
    d.values = calloc(d.cap, sizeof(*d.values));
    assert(d.values);
    return d;
}

struct Dict$Tt$Vv *_dict_$tt_$vv_new(struct DictArgs$Tt$Vv args)
{
    struct Dict$Tt$Vv *d;
    d = malloc(sizeof(*d));
    assert(d);
    *d = _dict_$tt_$vv_init(args);
    return d;
}

void dict_$tt_$vv_deinit(struct Dict$Tt$Vv *d)
{
    size_t i;
    assert(d);
    assert(d->keys);
    assert(d->values);

    for (i = 0; i < d->cap; i++) {
        if (!DEF_DICT_$TT_$VV_ZEROKEY(d->keys[i])) {
            DEF_DICT_$TT_$VV_FREEKEY(d->keys[i]);
        }
        if (!DEF_DICT_$TT_$VV_ZEROVALUE(d->values[i])) {
            DEF_DICT_$TT_$VV_FREEVALUE(d->values[i]);
        }
    }
    memset(d->keys, 0, sizeof($T) * d->cap);
    memset(d->values, 0, sizeof($V) * d->cap);
    free(d->keys);
    free(d->values);
    memset(d, 0, sizeof(*d));
}

void dict_$tt_$vv_delete(struct Dict$Tt$Vv *d, $T key)
{
    $V *v = dict_$tt_$vv_get(d, key);
    if (v) {
        DEF_DICT_$TT_$VV_FREEKEY()
    }
}

void dict_$tt_$vv_reserve(struct Dict$Tt$Vv *d, size_t cap)
{
    $T *oldkeys;
    $V *oldvalues;
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
        if (!DEF_DICT_$TT_$VV_ZEROKEY(oldkeys[i])) {
            ndx = dict_$tt_$vv_index(d, oldkeys[i]);
            d->keys[ndx] = oldkeys[i];
            d->values[ndx] = oldvalues[i];
        }
    }

    memset(oldkeys, 0, oldcap * sizeof(*oldkeys));
    memset(oldvalues, 0, oldcap * sizeof(*oldvalues));
    free(oldkeys);
    free(oldvalues);
}

void dict_$tt_$vv_set(struct Dict$Tt$Vv *d, $T key, $V value)
{
    size_t ndx;
    assert(d);
    assert(d->keys);
    assert(d->values);

    ndx = dict_$tt_$vv_index(d, key);
    //printf("Setting at %zu, size = %zu\n", ndx, d->size);

    // existing key
    if (!DEF_DICT_$TT_$VV_ZEROKEY(d->keys[ndx])) {
        DEF_DICT_$TT_$VV_FREEKEY(d->keys[ndx]);
        DEF_DICT_$TT_$VV_FREEVALUE(d->values[ndx]);
        d->keys[ndx] = key;
        d->values[ndx] = value;
    }
    // new key
    else {
        d->keys[ndx] = key;
        d->values[ndx] = value;
        d->size++;

        if (d->size > d->cap * 2 / 3) {
            dict_$tt_$vv_reserve(d, d->cap * 2);
        }
    }
}

$V *dict_$tt_$vv_get(struct Dict$Tt$Vv *d, $T key)
{
    size_t ndx;
    assert(d);
    assert(d->keys);
    assert(d->values);

    ndx = dict_$tt_$vv_index(d, key);
    return DEF_DICT_$TT_$VV_ZEROKEY(d->values[ndx]) ? NULL: &d->values[ndx];
}

struct DictPair$Tt$Vv dict_$tt_$vv_iter(struct Dict$Tt$Vv *d)
{
    size_t i;
    assert(d);
    if (d->size == 0) {
        return (struct DictPair$Tt$Vv){NULL, NULL};
    }

    for (i = 0; i < d->cap; i++) {
        if (!DEF_DICT_$TT_$VV_ZEROKEY(d->keys[i])) {
            return (struct DictPair$Tt$Vv){
                .key = &d->keys[i],
                .value = &d->values[i],
            };
        }
    }
    return (struct DictPair$Tt$Vv){NULL, NULL};
}

struct DictPair$Tt$Vv dict_$tt_$vv_next(struct Dict$Tt$Vv *d, struct DictPair$Tt$Vv cursor)
{
    size_t i;
    assert(d);
    assert(d->keys);
    assert(d->values);

    if (cursor.key == NULL) {
        return (struct DictPair$Tt$Vv){NULL, NULL};
    }

    i = (size_t)(cursor.key - d->keys + 1);
    for ( ; i < d->cap; i++) {
        if (!DEF_DICT_$TT_$VV_ZEROKEY(d->keys[i])) {
            return (struct DictPair$Tt$Vv){
                .key = &d->keys[i],
                .value = &d->values[i],
            };
        }
    }
    return (struct DictPair$Tt$Vv){NULL, NULL};
}

void dict_$tt_$vv_sethash(Dict$Tt$VvHashFunc hash)
{
    assert(hash);
    dict_$tt_$vv_hash = hash;
}

#endif // DEF_DICT_$TT_$VV

#endif // DEF_DICTDEF_$TT_$VV_H
