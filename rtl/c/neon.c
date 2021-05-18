#include "neon.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Ne_Number_assign(Ne_Number *dest, const Ne_Number *src)
{
    dest->dval = src->dval;
}

void Ne_Number_init_literal(Ne_Number *num, double n)
{
    num->dval = n;
}

void Ne_String_init_literal(Ne_String *str, const char *s)
{
    size_t len = strlen(s);
    str->ptr = malloc(len);
    memcpy(str->ptr, s, len);
    str->len = len;
}

void Ne_Number_add(Ne_Number *result, Ne_Number *a, Ne_Number *b)
{
    result->dval = a->dval + b->dval;
}

void Ne_Number_subtract(Ne_Number *result, Ne_Number *a, Ne_Number *b)
{
    result->dval = a->dval - b->dval;
}

void Ne_Number_multiply(Ne_Number *result, Ne_Number *a, Ne_Number *b)
{
    result->dval = a->dval * b->dval;
}

void Ne_Number_divide(Ne_Number *result, Ne_Number *a, Ne_Number *b)
{
    result->dval = a->dval / b->dval;
}

void Ne_Number_pow(Ne_Number *result, Ne_Number *a, Ne_Number *b)
{
    result->dval = pow(a->dval, b->dval);
}

void Ne_Number_mod(Ne_Number *result, Ne_Number *a, Ne_Number *b)
{
    result->dval = (int)a->dval % (int)b->dval;
}

void Ne_Number_equal(int *result, Ne_Number *a, Ne_Number *b)
{
    *result = a->dval == b->dval;
}

void Ne_Number_notequal(int *result, Ne_Number *a, Ne_Number *b)
{
    *result = a->dval != b->dval;
}

void Ne_Number_less(int *result, Ne_Number *a, Ne_Number *b)
{
    *result = a->dval < b->dval;
}

void Ne_Number_greater(int *result, Ne_Number *a, Ne_Number *b)
{
    *result = a->dval > b->dval;
}

void Ne_Number_lessequal(int *result, Ne_Number *a, Ne_Number *b)
{
    *result = a->dval <= b->dval;
}

void Ne_Number_greaterequal(int *result, Ne_Number *a, Ne_Number *b)
{
    *result = a->dval >= b->dval;
}

Ne_String *string_copy(const Ne_String *src)
{
    Ne_String *r = malloc(sizeof(Ne_String));
    r->ptr = malloc(src->len);
    memcpy(r->ptr, src->ptr, src->len);
    r->len = src->len;
    return r;
}

Ne_Object *make_object()
{
    Ne_Object *r = malloc(sizeof(Ne_Object));
    r->str = NULL;
    return r;
}

void Ne_object__makeString(Ne_Object *obj, const Ne_String *s)
{
    obj->str = string_copy(s);
}

void Ne_print(const Ne_Object *obj)
{
    printf("%.*s\n", obj->str->len, obj->str->ptr);
}

void Ne_str(Ne_String *result, const Ne_Number *n)
{
    char buf[20];
    snprintf(buf, sizeof(buf), "%g", n->dval);
    Ne_String_init_literal(result, buf);
}
