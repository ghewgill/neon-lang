#include "neon.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const MethodTable Ne_Number_mtable = {
    .constructor = (void (*)(void **))Ne_Number_constructor,
    .destructor = (void (*)(void *))Ne_Number_destructor,
    .copy = (void (*)(void *, const void *))Ne_Number_copy,
    .equals = (int (*)(const void *, const void *))Ne_Number_equals,
};

const MethodTable Ne_String_mtable = {
    .constructor = (void (*)(void **))Ne_String_constructor,
    .destructor = (void (*)(void *))Ne_String_destructor,
    .copy = (void (*)(void *, const void *))Ne_String_copy,
    .equals = (int (*)(const void *, const void *))Ne_String_equals,
};

void Ne_Boolean_init(Ne_Boolean *bool)
{
    *bool = 0;
}

void Ne_Boolean_init_copy(Ne_Boolean *dest, const Ne_Boolean *src)
{
    *dest = *src;
}

void Ne_Boolean_copy(Ne_Boolean *dest, const Ne_Boolean *src)
{
    *dest = *src;
}

void Ne_Boolean_deinit(Ne_Boolean *bool)
{
}

int Ne_Boolean_equals(const Ne_Boolean *a, const Ne_Boolean *b)
{
    return *a == *b;
}

void Ne_boolean__toString(Ne_String *result, const Ne_Boolean *a)
{
    Ne_String_init_literal(result, *a ? "TRUE" : "FALSE");
}

void Ne_Number_init(Ne_Number *num)
{
    num->dval = 0;
}

void Ne_Number_init_copy(Ne_Number *dest, const Ne_Number *src)
{
    dest->dval = src->dval;
}

void Ne_Number_copy(Ne_Number *dest, const Ne_Number *src)
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

void Ne_Number_destructor(Ne_Number *num)
{
    free(num);
}

void Ne_Number_deinit(Ne_Number *num)
{
}

int Ne_Number_equals(const Ne_Number *a, const Ne_Number *b)
{
    return a->dval == b->dval;
}

void Ne_String_init(Ne_String *str)
{
    str->ptr = NULL;
    str->len = 0;
}

void Ne_String_init_literal(Ne_String *str, const char *s)
{
    size_t len = strlen(s);
    str->ptr = malloc(len);
    memcpy(str->ptr, s, len);
    str->len = len;
}

void Ne_String_constructor(Ne_String **str)
{
    *str = malloc(sizeof(Ne_String));
    Ne_String_init(*str);
}

void Ne_String_destructor(Ne_String *str)
{
    Ne_String_deinit(str);
    free(str);
}

void Ne_String_init_copy(Ne_String *dest, const Ne_String *src)
{
    dest->ptr = malloc(src->len);
    memcpy(dest->ptr, src->ptr, src->len);
    dest->len = src->len;
}

void Ne_String_copy(Ne_String *dest, const Ne_String *src)
{
    Ne_String_deinit(dest);
    Ne_String_init_copy(dest, src);
}

void Ne_String_deinit(Ne_String *str)
{
    free(str->ptr);
}

int Ne_String_equals(const Ne_String *a, const Ne_String *b)
{
    return a->len == b->len && memcmp(a->ptr, b->ptr, a->len) == 0;
}

void Ne_String_less(int *result, const Ne_String *a, const Ne_String *b)
{
    int i = 0;
    for (;;) {
        if (i >= a->len) {
            *result = 1;
            return;
        }
        if (i >= b->len) {
            *result = 0;
            return;
        }
        if (a->ptr[i] < b->ptr[i]) {
            *result = 1;
            return;
        }
        i++;
    }
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

void Ne_Bytes_init(Ne_Bytes *bytes)
{
    bytes->data = NULL;
    bytes->len = 0;
}

void Ne_Bytes_init_literal(Ne_Bytes *bytes, const unsigned char *data, int len)
{
    bytes->data = malloc(len);
    memcpy(bytes->data, data, len);
    bytes->len = len;
}

void Ne_Bytes_deinit(Ne_Bytes *bytes)
{
    free(bytes->data);
}

void Ne_Bytes_copy(Ne_Bytes *dest, const Ne_Bytes *src)
{
    free(dest->data);
    dest->data = malloc(src->len);
    memcpy(dest->data, src->data, src->len);
    dest->len = src->len;
}

void Ne_Bytes_equal(int *result, const Ne_Bytes *a, const Ne_Bytes *b)
{
    *result = (a->len == b->len) && memcmp(a->data, b->data, a->len) == 0;
}

void Ne_bytes__concat(Ne_Bytes *r, const Ne_Bytes *a, const Ne_Bytes *b)
{
    r->data = malloc(a->len + b->len);
    memcpy(r->data, a->data, a->len);
    memcpy(r->data + a->len, b->data, b->len);
    r->len = a->len + b->len;
}

void Ne_bytes__size(Ne_Number *r, const Ne_Bytes *bytes)
{
    r->dval = bytes->len;
}

void Ne_bytes__toArray(Ne_Array *result, const Ne_Bytes *bytes)
{
    Ne_Array_init(result, bytes->len, &Ne_Number_mtable);
    for (int i = 0; i < bytes->len; i++) {
        Ne_Number *p;
        Ne_Array_index_int((void **)&p, result, i);
        // TODO: Copy the number right into the array instead of using a temporary.
        Ne_Number n;
        Ne_Number_init_literal(&n, bytes->data[i]);
        Ne_Number_copy(p, &n);
        Ne_Number_deinit(&n);
    }
}

void Ne_bytes__toString(Ne_String *result, const Ne_Bytes *bytes)
{
    char *buf = malloc(8 + 1 + 1 + 3*bytes->len + 1 + 1);
    strcpy(buf, "HEXBYTES \"");
    char *p = buf + strlen(buf);
    for (int i = 0; i < bytes->len; i++) {
        if (i > 0) {
            *p++ = ' ';
        }
        p += sprintf(p, "%02x", bytes->data[i]);
    }
    *p++ = '"';
    *p = 0;
    Ne_String_init_literal(result, buf);
    free(buf);
}

void Ne_Array_init(Ne_Array *a, int size, const MethodTable *mtable)
{
    a->size = size;
    a->a = malloc(size * sizeof(void *));
    a->mtable = mtable;
    for (int i = 0; i < size; i++) {
        mtable->constructor(&a->a[i]);
    }
}

void Ne_Object_init(Ne_Object *obj)
{
    obj->type = neNothing;
    Ne_Number_init_literal(&obj->num, 0);
    Ne_String_init(&obj->str);
}

void Ne_Object_copy(Ne_Object *dest, const Ne_Object *src)
{
    Ne_Object_deinit(dest);
    dest->type = src->type;
    switch (dest->type) {
        case neNothing:
            break;
        case neNumber:
            Ne_Number_init_copy(&dest->num, &src->num);
            break;
        case neString:
            Ne_String_init_copy(&dest->str, &src->str);
            break;
    }
}

void Ne_Object_deinit(Ne_Object *obj)
{
    switch (obj->type) {
        case neNothing:
            break;
        case neNumber:
            Ne_Number_deinit(&obj->num);
            break;
        case neString:
            Ne_String_deinit(&obj->str);
            break;
    }
}

void Ne_object__isNull(Ne_Boolean *r, Ne_Object *obj)
{
    *r = obj->type == neNothing;
}

void Ne_object__makeNull(Ne_Object *obj)
{
    Ne_Object_init(obj);
    obj->type = neNothing;
}

void Ne_object__makeNumber(Ne_Object *obj, const Ne_Number *n)
{
    Ne_Object_init(obj);
    obj->type = neNumber;
    Ne_Number_copy(&obj->num, n);
}

void Ne_object__makeString(Ne_Object *obj, const Ne_String *s)
{
    Ne_Object_init(obj);
    obj->type = neString;
    Ne_String_copy(&obj->str, s);
}

void Ne_Array_copy(Ne_Array *dest, const Ne_Array *src)
{
    Ne_Array_deinit(dest);
    dest->a = malloc(src->size * sizeof(void *));
    assert(src->mtable == NULL || src->mtable == dest->mtable);
    for (int i = 0; i < src->size; i++) {
        dest->mtable->constructor(&dest->a[i]);
        dest->mtable->copy(dest->a[i], src->a[i]);
    }
    dest->size = src->size;
}

void Ne_Array_deinit(Ne_Array *a)
{
    for (int i = 0; i < a->size; i++) {
        a->mtable->destructor(a->a[i]);
    }
    free(a->a);
}

int Ne_Array_equals(const Ne_Array *a, const Ne_Array *b)
{
    if (a->size != b->size) {
        return 0;
    }
    for (int i = 0; i < a->size; i++) {
        if (!a->mtable->equals(a->a[i], b->a[i])) {
            return 0;
        }
    }
    return 1;
}

void Ne_Array_equal(int *result, const Ne_Array *a, const Ne_Array *b)
{
    if (a->size != b->size) {
        *result = 0;
        return;
    }
    for (int i = 0; i < a->size; i++) {
        if (!a->mtable->equals(a->a[i], b->a[i])) {
            *result = 0;
            return;
        }
    }
    *result = 1;
}

void Ne_Array_in(Ne_Boolean *result, const Ne_Array *a, void *element)
{
    *result = 0;
    for (int i = 0; i < a->size; i++) {
        if (a->mtable->equals(a->a[i], element)) {
            *result = 1;
            return;
        }
    }
}

void Ne_Array_index_int(void **result, Ne_Array *a, int index)
{
    if (index >= a->size) {
        a->a = realloc(a->a, (index+1) * sizeof(void *));
        for (int j = a->size; j < index+1; j++) {
            a->mtable->constructor(&a->a[j]);
        }
        a->size = index+1;
    }
    *result = a->a[index];
}

void Ne_Array_index(void **result, Ne_Array *a, const Ne_Number *index)
{
    Ne_Array_index_int(result, a, (int)index->dval);
}

void Ne_array__append(Ne_Array *a, const void *element)
{
    a->a = realloc(a->a, (a->size+1) * sizeof(void *));
    a->mtable->constructor(&a->a[a->size]);
    a->mtable->copy(a->a[a->size], element);
    a->size++;
}

void Ne_array__concat(Ne_Array *dest, const Ne_Array *a, const Ne_Array *b)
{
    dest->size = a->size + b->size;
    dest->a = malloc(dest->size * sizeof(void *));
    dest->mtable = a->mtable;
    for (int i = 0; i < a->size; i++) {
        dest->mtable->constructor(&dest->a[i]);
        dest->mtable->copy(dest->a[i], a->a[i]);
    }
    for (int i = 0; i < b->size; i++) {
        dest->mtable->constructor(&dest->a[a->size+i]);
        dest->mtable->copy(dest->a[a->size+i], b->a[i]);
    }
}

void Ne_array__extend(Ne_Array *dest, const Ne_Array *src)
{
    int newsize = dest->size + src->size;
    dest->a = realloc(dest->a, newsize * sizeof(void *));
    for (int i = 0; i < src->size; i++) {
        dest->mtable->constructor(&dest->a[dest->size+i]);
        dest->mtable->copy(dest->a[dest->size+i], src->a[i]);
    }
    dest->size = newsize;
}

void Ne_array__find(Ne_Number *result, const Ne_Array *a, void *e)
{
    for (int i = 0; i < a->size; i++) {
        if (a->mtable->equals(a->a[i], e)) {
            Ne_Number_init_literal(result, i);
            return;
        }
    }
    // TODO: exception
}

void Ne_array__remove(Ne_Array *a, const Ne_Number *index)
{
    int i = (int)index->dval;
    if (i < 0 || i >= a->size) {
        // TODO: exception
        exit(1);
    }
    a->mtable->destructor(a->a[i]);
    memmove(&a->a[i], &a->a[i+1], (a->size-i-1) * sizeof(void *));
    a->size--;
}

void Ne_array__resize(Ne_Array *a, const Ne_Number *size)
{
    int newsize = (int)size->dval;
    if (newsize < a->size) {
        for (int i = newsize; i < a->size; i++) {
            a->mtable->destructor(a->a[i]);
        }
        a->a = realloc(a->a, newsize * sizeof(void *));
    } else {
        a->a = realloc(a->a, newsize * sizeof(void *));
        for (int i = a->size; i < newsize; i++) {
            a->mtable->constructor(&a->a[i]);
        }
    }
    a->size = newsize;
}

void Ne_array__reversed(Ne_Array *dest, const Ne_Array *src)
{
    Ne_Array_init(dest, src->size, src->mtable);
    for (int i = 0; i < src->size; i++) {
        dest->mtable->copy(dest->a[i], src->a[src->size-1-i]);
    }
}

void Ne_array__size(Ne_Number *result, const Ne_Array *a)
{
    Ne_Number_init_literal(result, a->size);
}

void Ne_array__toBytes__number(Ne_Bytes *r, const Ne_Array *a)
{
    r->data = malloc(a->size);
    for (int i = 0; i < a->size; i++) {
        r->data[i] = (int)((Ne_Number *)a->a[i])->dval;
    }
    r->len = a->size;
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

void Ne_Dictionary_init(Ne_Dictionary *d, const MethodTable *mtable)
{
    d->size = 0;
    d->d = NULL;
    d->mtable = mtable;
}

void Ne_Dictionary_copy(Ne_Dictionary *dest, const Ne_Dictionary *src)
{
    Ne_Dictionary_deinit(dest);
    dest->d = malloc(src->size * sizeof(struct KV));
    assert(src->mtable == NULL || src->mtable == dest->mtable);
    for (int i = 0; i < src->size; i++) {
        Ne_String_init_copy(&dest->d[i].key, &src->d[i].key);
        dest->mtable->constructor(&dest->d[i].value);
        dest->mtable->copy(dest->d[i].value, src->d[i].value);
    }
    dest->size = src->size;
}

void Ne_Dictionary_deinit(Ne_Dictionary *d)
{
    for (int i = 0; i < d->size; i++) {
        Ne_String_deinit(&d->d[i].key);
        d->mtable->destructor(d->d[i].value);
    }
    free(d->d);
}

void Ne_Dictionary_index(void **result, Ne_Dictionary *d, const Ne_String *index)
{
    int i = 0;
    while (i < d->size) {
        if (Ne_String_equals(&d->d[i].key, index)) {
            *result = d->d[i].value;
            return;
        }
        int less;
        Ne_String_less(&less, index, &d->d[i].key);
        if (less) {
            break;
        }
        i++;
    }
    d->d = realloc(d->d, (d->size+1) * sizeof(struct KV));
    memmove(&d->d[i+1], &d->d[i], (d->size-i) * sizeof(struct KV));
    d->size++;
    Ne_String_init_copy(&d->d[i].key, index);
    d->mtable->constructor(&d->d[i].value);
    *result = d->d[i].value;
}

void Ne_dictionary__keys(Ne_Array *result, const Ne_Dictionary *d)
{
    Ne_Array_init(result, d->size, &Ne_String_mtable);
    for (int i = 0; i < d->size; i++) {
        Ne_String *s;
        Ne_Array_index_int((void **)&s, result, i);
        Ne_String_copy(s, &d->d[i].key);
    }
}

void Ne_number__toString(Ne_String *result, const Ne_Number *n)
{
    char buf[20];
    snprintf(buf, sizeof(buf), "%g", n->dval);
    Ne_String_init_literal(result, buf);
}

void Ne_pointer__toString(Ne_String *r, const void *p)
{
    char buf[20];
    snprintf(buf, sizeof(buf), "%p", p);
    Ne_String_init_literal(r, buf);
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

void Ne_string__append(Ne_String *dest, const Ne_String *s)
{
    dest->ptr = realloc(dest->ptr, dest->len + s->len);
    memcpy(dest->ptr + dest->len, s->ptr, s->len);
    dest->len += s->len;
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

Ne_Array sys$args;

void Ne_sys__init(int argc, const char *argv[])
{
    Ne_Array_init(&sys$args, argc, &Ne_String_mtable);
    for (int i = 0; i < argc; i++) {
        Ne_String s;
        Ne_String_init_literal(&s, argv[i]);
        Ne_String *x;
        Ne_Array_index_int((void **)&x, &sys$args, i);
        Ne_String_copy(x, &s);
        Ne_String_deinit(&s);
    }
}

void Ne_sys_exit(const Ne_Number *n)
{
    exit(n->dval);
}

void *textio$stderr;

void Ne_textio_writeLine(void *f, const Ne_String *s)
{
    fprintf(stderr, "%.*s\n", s->len, s->ptr);
}
