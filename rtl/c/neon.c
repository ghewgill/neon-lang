#include "neon.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Ne_Boolean_assign(Ne_Boolean *dest, const Ne_Boolean *src)
{
    *dest = *src;
}

void Ne_boolean__toString(Ne_String *result, const Ne_Boolean *a)
{
    Ne_String_init_literal(result, *a ? "TRUE" : "FALSE");
}

void Ne_Number_assign(Ne_Number *dest, const Ne_Number *src)
{
    dest->dval = src->dval;
}

void Ne_Number_init_literal(Ne_Number *num, double n)
{
    num->dval = n;
}

void Ne_Number_constructor(Ne_Number **num)
{
    *num = malloc(sizeof(Ne_Number));
    Ne_Number_init_literal(*num, 0);
}

void Ne_Number_destroy(Ne_Number *num)
{
}

void Ne_String_init_literal(Ne_String *str, const char *s)
{
    size_t len = strlen(s);
    str->ptr = malloc(len);
    memcpy(str->ptr, s, len);
    str->len = len;
}

void Ne_String_assign(Ne_String *dest, const Ne_String *src)
{
    dest->ptr = malloc(src->len);
    memcpy(dest->ptr, src->ptr, src->len);
    dest->len = src->len;
}

void Ne_String_destroy(Ne_String *str)
{
    free(str->ptr);
}

void Ne_Number_add(Ne_Number *result, const Ne_Number *a, const Ne_Number *b)
{
    result->dval = a->dval + b->dval;
}

void Ne_Number_subtract(Ne_Number *result, const Ne_Number *a, const Ne_Number *b)
{
    result->dval = a->dval - b->dval;
}

void Ne_Number_multiply(Ne_Number *result, const Ne_Number *a, const Ne_Number *b)
{
    result->dval = a->dval * b->dval;
}

void Ne_Number_divide(Ne_Number *result, const Ne_Number *a, const Ne_Number *b)
{
    result->dval = a->dval / b->dval;
}

void Ne_Number_pow(Ne_Number *result, const Ne_Number *a, const Ne_Number *b)
{
    result->dval = pow(a->dval, b->dval);
}

void Ne_Number_mod(Ne_Number *result, const Ne_Number *a, const Ne_Number *b)
{
    result->dval = (int)a->dval % (int)b->dval;
}

void Ne_Number_negate(Ne_Number *result, const Ne_Number *a)
{
    result->dval = -a->dval;
}

void Ne_Number_increment(Ne_Number *a, int delta)
{
    a->dval += delta;
}

void Ne_Number_equal(int *result, const Ne_Number *a, const Ne_Number *b)
{
    *result = a->dval == b->dval;
}

void Ne_Number_notequal(int *result, const Ne_Number *a, const Ne_Number *b)
{
    *result = a->dval != b->dval;
}

void Ne_Number_less(int *result, const Ne_Number *a, const Ne_Number *b)
{
    *result = a->dval < b->dval;
}

void Ne_Number_greater(int *result, const Ne_Number *a, const Ne_Number *b)
{
    *result = a->dval > b->dval;
}

void Ne_Number_lessequal(int *result, const Ne_Number *a, const Ne_Number *b)
{
    *result = a->dval <= b->dval;
}

void Ne_Number_greaterequal(int *result, const Ne_Number *a, const Ne_Number *b)
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

void Ne_Array_init(Ne_Array *a, int size, void (*constructor)(void **))
{
    a->size = size;
    a->a = malloc(size * sizeof(void *));
    a->constructor = constructor;
    for (int i = 0; i < size; i++) {
        constructor(&a->a[i]);
    }
}

Ne_Object *make_object()
{
    Ne_Object *r = malloc(sizeof(Ne_Object));
    r->type = neNothing;
    Ne_Number_init_literal(&r->num, 0);
    Ne_String_init_literal(&r->str, "");
    return r;
}

void Ne_Object_destroy(Ne_Object *obj)
{
    switch (obj->type) {
        case neNothing:
            break;
        case neNumber:
            Ne_Number_destroy(&obj->num);
            break;
        case neString:
            Ne_String_destroy(&obj->str);
            break;
    }
}

void Ne_object__makeNumber(Ne_Object *obj, const Ne_Number *n)
{
    obj->type = neNumber;
    obj->num = *n;
}

void Ne_object__makeString(Ne_Object *obj, const Ne_String *s)
{
    obj->type = neString;
    Ne_String_assign(&obj->str, s);
}

void Ne_Array_assign(Ne_Array *dest, const Ne_Array *src)
{
    *dest = *src;
}

void Ne_Array_index_int(void **result, Ne_Array *a, int index)
{
    if (index >= a->size) {
        a->a = realloc(a->a, (index+1) * sizeof(void *));
        for (int j = a->size; j < index+1; j++) {
            a->constructor(&a->a[j]);
        }
        a->size = index+1;
    }
    *result = a->a[index];
}

void Ne_Array_index(void **result, Ne_Array *a, const Ne_Number *index)
{
    Ne_Array_index_int(result, a, (int)index->dval);
}

void Ne_array__append(Ne_Array *a, void *element)
{
    a->a = realloc(a->a, (a->size+1) * sizeof(void *));
    a->a[a->size] = element; // TODO: make a copy
    a->size++;
}

void Ne_array__concat(Ne_Array *dest, const Ne_Array *a, const Ne_Array *b)
{
    dest->size = a->size + b->size;
    dest->a = malloc(dest->size * sizeof(void *));
    dest->constructor = a->constructor;
    for (int i = 0; i < a->size; i++) {
        dest->a[i] = a->a[i]; // TODO: make a copy
    }
    for (int i = 0; i < b->size; i++) {
        dest->a[a->size+i] = b->a[i]; // TODO: make a copy
    }
}

void Ne_array__extend(Ne_Array *dest, const Ne_Array *src)
{
    int newsize = dest->size + src->size;
    dest->a = realloc(dest->a, newsize * sizeof(void *));
    for (int i = 0; i < src->size; i++) {
        dest->a[dest->size+i] = src->a[i]; // TODO: make a copy
    }
    dest->size = newsize;
}

void Ne_array__size(Ne_Number *result, const Ne_Array *a)
{
    result->dval = a->size;
}

void Ne_array__toString__number(Ne_String *r, const Ne_Array *a)
{
    char buf[100];
    strcpy(buf, "[");
    for (int i = 0; i < a->size; i++) {
        if (i > 0) {
            strcat(buf, ", ");
        }
        Ne_Number *n = a->a[i];
        snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), "%g", n->dval);
    }
    strcat(buf, "]");
    Ne_String_init_literal(r, buf);
}

void Ne_number__toString(Ne_String *result, const Ne_Number *n)
{
    char buf[20];
    snprintf(buf, sizeof(buf), "%g", n->dval);
    Ne_String_init_literal(result, buf);
}

void Ne_print(const Ne_Object *obj)
{
    switch (obj->type) {
        case neNothing:
            printf("NIL\n");
            break;
        case neNumber:
            printf("%g\n", obj->num.dval);
            break;
        case neString:
            printf("%.*s\n", obj->str.len, obj->str.ptr);
            break;
        default:
            printf("[unknown object type %d]\n", obj->type);
            break;
    }
}

void Ne_str(Ne_String *result, const Ne_Number *n)
{
    char buf[20];
    snprintf(buf, sizeof(buf), "%g", n->dval);
    Ne_String_init_literal(result, buf);
}

void Ne_string__concat(Ne_String *dest, const Ne_String *a, const Ne_String *b)
{
    dest->len = a->len + b->len;
    dest->ptr = malloc(dest->len);
    memcpy(dest->ptr, a->ptr, a->len);
    memcpy(dest->ptr+a->len, b->ptr, b->len);
}

void Ne_string__length(Ne_Number *result, const Ne_String *str)
{
    result->dval = str->len;
}
