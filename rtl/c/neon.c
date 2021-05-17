#include "neon.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Ne_Number_assign(struct Ne_Number *dest, const struct Ne_Number *src)
{
    dest->dval = src->dval;
}

struct Ne_Number *Ne_number_literal(double n)
{
    struct Ne_Number *r = malloc(sizeof(struct Ne_Number));
    r->dval = n;
    return r;
}

struct Ne_String *Ne_string_literal(const char *s)
{
    struct Ne_String *r = malloc(sizeof(struct Ne_String));
    size_t len = strlen(s);
    r->ptr = malloc(len);
    memcpy(r->ptr, s, len);
    r->len = len;
    return r;
}

struct Ne_String *string_copy(const struct Ne_String *src)
{
    struct Ne_String *r = malloc(sizeof(struct Ne_String));
    r->ptr = malloc(src->len);
    memcpy(r->ptr, src->ptr, src->len);
    r->len = src->len;
    return r;
}

struct Ne_Object *make_object()
{
    struct Ne_Object *r = malloc(sizeof(struct Ne_Object));
    r->str = NULL;
    return r;
}

struct Ne_Object *Ne_object__makeString(const struct Ne_String *s)
{
    struct Ne_Object *r = make_object();
    r->str = string_copy(s);
    return r;
}

void Ne_print(struct Ne_Object *obj)
{
    printf("%.*s\n", obj->str->len, obj->str->ptr);
}
