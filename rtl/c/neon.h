typedef struct {
    void (*constructor)(void **);
    void (*destructor)(void *);
    void (*copy)(void *, const void *);
    int (*equals)(const void *, const void *);
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

void Ne_Boolean_copy(Ne_Boolean *dest, const Ne_Boolean *src);
void Ne_Boolean_deinit(Ne_Boolean *bool);
void Ne_boolean__toString(Ne_String *result, const Ne_Boolean *a);
void Ne_Number_init_literal(Ne_Number *num, double n);
void Ne_Number_init_copy(Ne_Number *dest, const Ne_Number *src);
void Ne_Number_constructor(Ne_Number **num);
void Ne_Number_copy(Ne_Number *dest, const Ne_Number *src);
void Ne_Number_destructor(Ne_Number *num);
void Ne_Number_deinit(Ne_Number *num);
int Ne_Number_equals(const Ne_Number *a, const Ne_Number *b);
void Ne_Number_add(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_subtract(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_multiply(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_divide(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_pow(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_mod(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_negate(Ne_Number *result, const Ne_Number *a);
void Ne_Number_increment(Ne_Number *a, int delta);
void Ne_Number_equal(int *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_notequal(int *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_less(int *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_greater(int *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_lessequal(int *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_greaterequal(int *result, const Ne_Number *a, const Ne_Number *b);
void Ne_String_init_literal(Ne_String *str, const char *s);
void Ne_String_init_copy(Ne_String *dest, const Ne_String *src);
void Ne_String_constructor(Ne_String **str);
void Ne_String_destructor(Ne_String *str);
void Ne_String_copy(Ne_String *dest, const Ne_String *src);
void Ne_String_deinit(Ne_String *str);
int Ne_String_equals(const Ne_String *a, const Ne_String *b);
void Ne_String_less(int *result, const Ne_String *a, const Ne_String *b);
void Ne_Array_init(Ne_Array *a, int size, const MethodTable *mtable);
void Ne_Array_init_copy(Ne_Array *dest, const Ne_Array *src);
void Ne_Array_copy(Ne_Array *dest, const Ne_Array *src);
void Ne_Array_deinit(Ne_Array *a);
void Ne_Array_in(Ne_Boolean *result, const Ne_Array *a, void *element);
void Ne_Array_index_int(void **result, Ne_Array *a, int index);
void Ne_Array_index(void **result, Ne_Array *a, const Ne_Number *index);
void Ne_array__append(Ne_Array *a, const void *element);
void Ne_array__concat(Ne_Array *dest, const Ne_Array *a, const Ne_Array *b);
void Ne_array__extend(Ne_Array *dest, const Ne_Array *src);
void Ne_array__size(Ne_Number *result, const Ne_Array *a);
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
void Ne_print(const Ne_Object *obj);
void Ne_str(Ne_String *result, const Ne_Number *n);
void Ne_string__concat(Ne_String *dest, const Ne_String *a, const Ne_String *b);
void Ne_string__length(Ne_Number *result, const Ne_String *str);

void Ne_sys_exit(const Ne_Number *n);

extern void *textio$stderr;
void Ne_textio_writeLine(void *f, const Ne_String *s);
