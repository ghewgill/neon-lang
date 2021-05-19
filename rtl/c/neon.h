typedef int Ne_Boolean;

typedef struct {
    double dval;
} Ne_Number;

typedef struct {
    unsigned char *ptr;
    int len;
} Ne_String;

typedef struct {
    Ne_String *str;
} Ne_Object;

typedef struct {
    int size;
    void **a;
    void (*constructor)(void **);
} Ne_Array;

void Ne_Boolean_assign(Ne_Boolean *dest, const Ne_Boolean *src);
void Ne_Number_assign(Ne_Number *dest, const Ne_Number *src);
void Ne_Number_init_literal(Ne_Number *num, double n);
void Ne_Number_constructor(Ne_Number **num);
void Ne_Number_add(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_subtract(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_multiply(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_divide(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_pow(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_mod(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_equal(int *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_notequal(int *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_less(int *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_greater(int *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_lessequal(int *result, const Ne_Number *a, const Ne_Number *b);
void Ne_Number_greaterequal(int *result, const Ne_Number *a, const Ne_Number *b);
void Ne_String_init_literal(Ne_String *str, const char *s);
void Ne_String_assign(Ne_String *dest, const Ne_String *src);
void Ne_Array_init(Ne_Array *a, int size, void (*constructor)(void **));
void Ne_Array_assign(Ne_Array *dest, const Ne_Array *src);
void Ne_Array_index(void **result, Ne_Array *aconst , const Ne_Number *index);
void Ne_array__size(Ne_Number *result, const Ne_Array *a);
void Ne_number__toString(Ne_String *result, const Ne_Number *n);
void Ne_object__makeString(Ne_Object *obj, const Ne_String *s);
void Ne_print(const Ne_Object *obj);
void Ne_str(Ne_String *result, const Ne_Number *n);
void Ne_string__concat(Ne_String *dest, const Ne_String *a, const Ne_String *b);
