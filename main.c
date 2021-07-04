#define DEF_VECTOR_STR
#define DEF_LIST_INT
#define DEF_DICT_STR_INT
#include "collections.h"
#include <stdio.h>

int main(void)
{
    {
        /*list_int_init();
        struct ListInt *p, *r;
        struct ListInt *n = list_int_push_back(NULL, 10);
        r = p = list_int_push_back(n, 20);
        p = list_int_push_back(p, 30);
        n = list_int_push_front(n, 0);
        n = list_int_delete(n, r);
        list_int_insert(p, 500);
        list_int_insert(p, 600);

        int *v;
        for (v = list_int_iter(n);
            v;
            v = list_int_next(n, v))
        {
            printf("%d\n", *v);
        }

        printf("%p\n", list_int_getv(p, 0));
        printf("%p\n", list_int_getv(p, 1));
        printf("%p\n", list_int_getv(p, 2));
        printf("%p\n", list_int_getv(p, 4));
        printf("%p\n", list_int_set(p, 2, 999));
        printf("%d\n", *list_int_getv(p, 2));

        list_int_free(n);*/
        struct ListInt *head = list_int_push_back(NULL, 10);
        struct ListInt *p = list_int_gete(head);
        head = list_int_delete(head, p);
        printf("Head %p\n", head);
    }

    /* {
        struct VectorStr v = vector_str_init();

        vector_str_push(&v, "Hello");
        vector_str_push(&v, "There");

        const char **s;
        for (s = vector_str_iter(&v);
            s != NULL;
            s = vector_str_next(&v, s))
        {
            printf("%s\n", *s);
        }
    } */

    {
        struct DictStrInt *d = dict_str_int_new();
        dict_str_int_set(d, "a", 10);
        dict_str_int_set(d, "b", 20);
        dict_str_int_set(d, "c", 30);
        dict_str_int_set(d, "d", 40);
        dict_str_int_set(d, "a1", 15);
        dict_str_int_set(d, "b2", 25);
        dict_str_int_set(d, "c3", 35);
        dict_str_int_set(d, "d4", 45);

        struct DictPairStrInt pair;
        for (pair = dict_str_int_iter(d);
             pair.key;
             pair = dict_str_int_next(d, pair))
        {
            printf("%s: %d\n", *pair.key, *pair.value);
        }

        printf("%p\n", dict_str_int_get(d, "a1"));
        printf("%p\n", dict_str_int_get(d, "12315"));
    }

    return 0;
}
