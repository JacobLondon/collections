/**
 * $NN IDENTIFIER_NAME (UPPER)
 * $Nn IdentifierName (Pascal)
 * $nn identifier_name (lower)
 * $T C type (int, char *, ...)
 */

#ifndef DEF_LISTDEF_$NN_H
#define DEF_LISTDEF_$NN_H

#define DEF_PROTO
#include <stddef.h>

#if 0
#define DEF_LIST_$NN
#ifndef $T
#define $T int
#endif
#endif

#define DEF_LIST_$NN_ZEROVALUE(a) $ZEROVALUE
#define DEF_LIST_$NN_FREEVALUE(a) $FREEVALUE

struct List$Nn {
    $T value;
    struct List$Nn *next;
};

DEF_PROTO void list_$nn_free(struct List$Nn *head);                            // head may be NULL, do nothing
DEF_PROTO struct List$Nn *list_$nn_push_back(struct List$Nn *any, $T value);   // any may be NULL, return the last element
DEF_PROTO struct List$Nn *list_$nn_push_front(struct List$Nn *head, $T value); // head may be NULL, return the new front element
DEF_PROTO struct List$Nn *list_$nn_insert(struct List$Nn *any, $T value);      // any may be NULL, return newly placed element
DEF_PROTO struct List$Nn * list_$nn_delete(struct List$Nn *head, struct List$Nn *any);     // any may be NULL, return head element
DEF_PROTO struct List$Nn *list_$nn_gete(struct List$Nn *any);                  // any may be NULL, return NULL, the end element
DEF_PROTO struct List$Nn *list_$nn_get(struct List$Nn *any, size_t ndx);       // any may be NULL, return NULL, index element
DEF_PROTO $T *list_$nn_getv(struct List$Nn *any, size_t ndx);                  // any may be NULL, return NULL, value
DEF_PROTO struct List$Nn *list_$nn_set(struct List$Nn *any, size_t ndx, $T value); // return NULL if ndx is beyond reach, or the element set
DEF_PROTO $T *list_$nn_iter(struct List$Nn *any);                              // any may be NULL, will return NULL. Can call next on
DEF_PROTO $T *list_$nn_next(struct List$Nn *any, $T *cursor);                  // get the next value or NULL

#endif // DEF_LISTDEF_$NN_H

#ifdef DEF_LIST_$NN
#include <assert.h>
#include <stdlib.h>
#include <memory.h>

void list_$nn_free(struct List$Nn *head)
{
    if (!head) {
        return;
    }

    if (!DEF_LIST_$NN_ZEROVALUE(head->value)) {
        DEF_LIST_$NN_FREEVALUE(head->value);
    }

    list_$nn_free(head->next);
    memset(head, 0, sizeof(*head));
    free(head);
}

struct List$Nn *list_$nn_push_back(struct List$Nn *any, $T value)
{
    struct List$Nn *p;
    struct List$Nn *n;
    p = list_$nn_gete(any);
    n = list_$nn_insert(p, value);
    return n;
}

struct List$Nn *list_$nn_push_front(struct List$Nn *head, $T value)
{
    struct List$Nn *n;
    n = malloc(sizeof(*n));
    assert(n);
    n->next = head;
    n->value = value;
    return n;
}

struct List$Nn *list_$nn_insert(struct List$Nn *any, $T value)
{
    struct List$Nn *n;
    n = malloc(sizeof(*n));
    assert(n);
    n->value = value;

    if (!any) {
        n->next = NULL;
        return n;
    }

    n->next = any->next;
    any->next = n;
    return n;
}

struct List$Nn *list_$nn_delete(struct List$Nn *head, struct List$Nn *any)
{
    struct List$Nn *p;
    struct List$Nn *toremove;
    struct List$Nn *rv = head;

    if (!head || !any) {
        return rv;
    }

    // yikes special case
    if (head == any) {
        p = head;
        rv = p->next;
        toremove = p;
    }
    // normal case
    else {
        for (p = head; p && p->next != any; p = p->next) {
            ;
        }
        // not present
        if (!p || !p->next) {
            return rv;
        }
        toremove = p->next;
    }

    if (!DEF_LIST_$NN_ZEROVALUE(toremove->value)) {
        DEF_LIST_$NN_FREEVALUE(toremove->value);
    }

    p->next = toremove->next;
    memset(toremove, 0, sizeof(*toremove));
    free(toremove);
    return rv;
}

struct List$Nn *list_$nn_gete(struct List$Nn *any)
{
    struct List$Nn *p;
    for (p = any; p && p->next; p = p->next) {
        ;
    }
    return p;
}

struct List$Nn *list_$nn_get(struct List$Nn *any, size_t ndx)
{
    struct List$Nn *p;
    for (p = any; p && ndx != 0; ndx--, p = p->next) {
        ;
    }

    return p;
}

$T *list_$nn_getv(struct List$Nn *any, size_t ndx)
{
    struct List$Nn *p = list_$nn_get(any, ndx);
    return p ? &p->value : NULL;
}

// return NULL if ndx is beyond reach, or the element set
struct List$Nn *list_$nn_set(struct List$Nn *any, size_t ndx, $T value)
{
    struct List$Nn *p = list_$nn_get(any, ndx);
    if (p) {
        p->value = value;
    }
    return p;
}

// any may be NULL, will return NULL
$T *list_$nn_iter(struct List$Nn *any)
{
    return any ? &any->value : NULL;
}

$T *list_$nn_next(struct List$Nn *any, $T *cursor)
{
    struct List$Nn *p;
    if (!any || !cursor) {
        return NULL;
    }

    p = (struct List$Nn *)cursor;
    return p->next ? &p->next->value : NULL;
}

#endif // DEF_LIST_$NN
