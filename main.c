#define USE_VECTOR_STR
#include "vectorout.h"
#include <stdio.h>

int main(void)
{
    struct VectorStr v = vector_str_init();

    vector_str_push(&v, "Hello");
    vector_str_push(&v, "There");

    const char **p;
    for (p = vector_str_iter(&v);
         p != NULL;
         p = vector_str_next(&v, p))
    {
        printf("%s\n", *p);
    }

    return 0;
}
