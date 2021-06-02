typedef struct {
    void (*constructor)(void **);
    void (*destructor)(void *);
    void (*copy)(void *, const void *);
    int (*compare)(const void *, const void *);
} MethodTable;

extern const MethodTable Ne_Number_mtable;
extern const MethodTable Ne_String_mtable;

typedef int Ne_Boolean;

typedef struct {
    double dval;
} Ne_Number;

typedef struct {
    unsigned char *ptr;
    int len;
} Ne_String;

typedef struct {
    unsigned char *data;
    int len;
} Ne_Bytes;

enum Ne_ObjectType {
    neNothing,
    neNumber,
    neString
};

typedef struct {
    enum Ne_ObjectType type;
    Ne_Number num;
    Ne_String str;
} Ne_Object;

typedef struct {
    int size;
    void **a;
    const MethodTable *mtable;
} Ne_Array;

typedef struct {
    int size;
    struct KV {
        Ne_String key;
        void *value;
    } *d;
    const MethodTable *mtable;
} Ne_Dictionary;

typedef struct {
    const char *name;
    Ne_Object info;
} Ne_Exception;

void Ne_Boolean_init(Ne_Boolean *bool);
void Ne_Boolean_init_copy(Ne_Boolean *dest, const Ne_Boolean *src);
void Ne_Boolean_copy(Ne_Boolean *dest, const Ne_Boolean *src);
void Ne_Boolean_deinit(Ne_Boolean *bool);
int Ne_Boolean_compare(const Ne_Boolean *a, const Ne_Boolean *b);
void Ne_boolean__toString(Ne_String *result, const Ne_Boolean *a);
void Ne_Number_init(Ne_Number *num);
void Ne_Number_init_literal(Ne_Number *num, double n);
void Ne_Number_init_copy(Ne_Number *dest, const Ne_Number *src);
void Ne_Number_constructor(Ne_Number **num);
void Ne_Number_copy(Ne_Number *dest, const Ne_Number *src);
void Ne_Number_destructor(Ne_Number *num);
void Ne_Number_deinit(Ne_Number *num);
int Ne_Number_compare(const Ne_Number *a, const Ne_Number *b);
void Ne_Number_add(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_subtract(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_multiply(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_divide(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_pow(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_mod(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_negate(Ne_Number *result, const Ne_Number *a);
void Ne_Number_increment(Ne_Number *a, int delta);
void Ne_String_init(Ne_String *str);
void Ne_String_init_literal(Ne_String *str, const char *s);
void Ne_String_init_copy(Ne_String *dest, const Ne_String *src);
void Ne_String_constructor(Ne_String **str);
void Ne_String_destructor(Ne_String *str);
void Ne_String_copy(Ne_String *dest, const Ne_String *src);
void Ne_String_deinit(Ne_String *str);
int Ne_String_compare(const Ne_String *a, const Ne_String *b);
void Ne_Bytes_init(Ne_Bytes *bytes);
void Ne_Bytes_init_literal(Ne_Bytes *bytes, const unsigned char *data, int len);
void Ne_Bytes_deinit(Ne_Bytes *bytes);
void Ne_Bytes_copy(Ne_Bytes *dest, const Ne_Bytes *src);
int Ne_Bytes_compare(const Ne_Bytes *a, const Ne_Bytes *b);
void Ne_bytes__concat(Ne_Bytes *r, const Ne_Bytes *a, const Ne_Bytes *b);
void Ne_bytes__size(Ne_Number *r, const Ne_Bytes *bytes);
void Ne_bytes__toArray(Ne_Array *result, const Ne_Bytes *bytes);
void Ne_bytes__toString(Ne_String *result, const Ne_Bytes *bytes);
void Ne_Array_init(Ne_Array *a, int size, const MethodTable *mtable);
void Ne_Array_init_copy(Ne_Array *dest, const Ne_Array *src);
void Ne_Array_copy(Ne_Array *dest, const Ne_Array *src);
void Ne_Array_deinit(Ne_Array *a);
int Ne_Array_compare(const Ne_Array *a, const Ne_Array *b);
void Ne_Array_in(Ne_Boolean *result, const Ne_Array *a, void *element);
void Ne_Array_index_int(void **result, Ne_Array *a, int index);
void Ne_Array_index(void **result, Ne_Array *a, const Ne_Number *index);
void Ne_array__append(Ne_Array *a, const void *element);
void Ne_array__concat(Ne_Array *dest, const Ne_Array *a, const Ne_Array *b);
void Ne_array__extend(Ne_Array *dest, const Ne_Array *src);
void Ne_array__find(Ne_Number *result, const Ne_Array *a, void *e);
void Ne_array__remove(Ne_Array *a, const Ne_Number *index);
void Ne_array__resize(Ne_Array *a, const Ne_Number *size);
void Ne_array__reversed(Ne_Array *dest, const Ne_Array *src);
void Ne_array__size(Ne_Number *result, const Ne_Array *a);
void Ne_array__toBytes__number(Ne_Bytes *r, const Ne_Array *a);
void Ne_array__toString__number(Ne_String *r, const Ne_Array *a);
void Ne_Dictionary_init(Ne_Dictionary *d, const MethodTable *mtable);
void Ne_Dictionary_init_copy(Ne_Dictionary *dest, const Ne_Dictionary *src);
void Ne_Dictionary_copy(Ne_Dictionary *dest, const Ne_Dictionary *src);
void Ne_Dictionary_deinit(Ne_Dictionary *d);
void Ne_Dictionary_index(void **result, Ne_Dictionary *d, const Ne_String *index);
void Ne_dictionary__keys(Ne_Array *result, const Ne_Dictionary *d);
void Ne_number__toString(Ne_String *result, const Ne_Number *n);
void Ne_Object_init(Ne_Object *obj);
void Ne_Object_init_copy(Ne_Object *dest, const Ne_Object *src);
void Ne_Object_copy(Ne_Object *dest, const Ne_Object *src);
void Ne_Object_deinit(Ne_Object *obj);
void Ne_object__isNull(Ne_Boolean *r, Ne_Object *obj);
void Ne_object__makeNull(Ne_Object *obj);
void Ne_object__makeNumber(Ne_Object *obj, const Ne_Number *n);
void Ne_object__makeString(Ne_Object *obj, const Ne_String *s);
void Ne_pointer__toString(Ne_String *r, const void *p);
void Ne_print(const Ne_Object *obj);
void Ne_str(Ne_String *result, const Ne_Number *n);
void Ne_string__append(Ne_String *dest, const Ne_String *s);
void Ne_string__concat(Ne_String *dest, const Ne_String *a, const Ne_String *b);
void Ne_string__length(Ne_Number *result, const Ne_String *str);
Ne_Exception *Ne_Exception_raise(const char *name);
int Ne_Exception_trap(const char *name);
void Ne_Exception_unhandled();

extern Ne_Array sys$args;
void Ne_sys__init(int argc, const char *argv[]);
void Ne_sys_exit(const Ne_Number *n);

extern void *textio$stderr;
void Ne_textio_writeLine(void *f, const Ne_String *s);
