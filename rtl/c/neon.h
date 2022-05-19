#include <stdlib.h>

typedef struct {
    void (*constructor)(void **);
    void (*destructor)(void *);
    void (*copy)(void *, const void *);
    int (*compare)(const void *, const void *);
} MethodTable;

extern const MethodTable Ne_Number_mtable;
extern const MethodTable Ne_String_mtable;
extern const MethodTable Ne_Object_mtable;

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
    neBoolean,
    neNumber,
    neString
};

typedef struct {
    enum Ne_ObjectType type;
    Ne_Boolean boo;
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

typedef void *void_Ptr;
static void void_Ptr_init(void_Ptr *p) {}
static void void_Ptr_deinit(void_Ptr *p) {}
static void void_Ptr_copy(void_Ptr *d, const void_Ptr *s) { *d = *s; }
static int void_Ptr_compare(const void_Ptr *p, const void_Ptr *q) { return *p == *q ? 0 : *p > *q ? 1 : -1; }

typedef Ne_Exception *(*Ne_FunctionPointer)();
static void Ne_FunctionPointer_init(Ne_FunctionPointer *p) { *p = NULL; }
static void Ne_FunctionPointer_deinit(Ne_FunctionPointer *p) {}
static void Ne_FunctionPointer_copy(Ne_FunctionPointer *d, const Ne_FunctionPointer *s) { *d = *s; }
static int Ne_FunctionPointer_compare(const Ne_FunctionPointer *p, const Ne_FunctionPointer *q) { return *p == *q ? 0 : 1; }

void Ne_Boolean_init(Ne_Boolean *bool);
void Ne_Boolean_init_copy(Ne_Boolean *dest, const Ne_Boolean *src);
void Ne_Boolean_copy(Ne_Boolean *dest, const Ne_Boolean *src);
void Ne_Boolean_deinit(Ne_Boolean *bool);
int Ne_Boolean_compare(const Ne_Boolean *a, const Ne_Boolean *b);
Ne_Exception *Ne_boolean__toString(Ne_String *result, const Ne_Boolean *a);
void Ne_Number_init(Ne_Number *num);
void Ne_Number_init_literal(Ne_Number *num, double n);
void Ne_Number_init_copy(Ne_Number *dest, const Ne_Number *src);
void Ne_Number_constructor(Ne_Number **num);
void Ne_Number_copy(Ne_Number *dest, const Ne_Number *src);
void Ne_Number_destructor(Ne_Number *num);
void Ne_Number_deinit(Ne_Number *num);
int Ne_Number_compare(const Ne_Number *a, const Ne_Number *b);
Ne_Exception *Ne_Number_add(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
Ne_Exception *Ne_Number_subtract(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
Ne_Exception *Ne_Number_multiply(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
Ne_Exception *Ne_Number_divide(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
Ne_Exception *Ne_Number_pow(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
Ne_Exception *Ne_Number_mod(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
Ne_Exception *Ne_Number_negate(Ne_Number *result, const Ne_Number *a);
Ne_Exception *Ne_Number_increment(Ne_Number *a, int delta);
void Ne_String_init(Ne_String *str);
void Ne_String_init_literal(Ne_String *str, const char *s);
void Ne_String_init_copy(Ne_String *dest, const Ne_String *src);
void Ne_String_constructor(Ne_String **str);
void Ne_String_destructor(Ne_String *str);
void Ne_String_copy(Ne_String *dest, const Ne_String *src);
void Ne_String_deinit(Ne_String *str);
int Ne_String_compare(const Ne_String *a, const Ne_String *b);
Ne_Exception *Ne_String_index(Ne_String *dest, const Ne_String *s, const Ne_Number *index);
Ne_Exception *Ne_String_range(Ne_String *dest, const Ne_String *s, const Ne_Number *first, Ne_Boolean first_from_end, const Ne_Number *last, Ne_Boolean last_from_end);
Ne_Exception *Ne_String_splice(Ne_String *d, const Ne_String *t, const Ne_Number *first, Ne_Boolean first_from_end, const Ne_Number *last, Ne_Boolean last_from_end);
void Ne_Bytes_init(Ne_Bytes *bytes);
void Ne_Bytes_init_literal(Ne_Bytes *bytes, const unsigned char *data, int len);
void Ne_Bytes_deinit(Ne_Bytes *bytes);
void Ne_Bytes_copy(Ne_Bytes *dest, const Ne_Bytes *src);
int Ne_Bytes_compare(const Ne_Bytes *a, const Ne_Bytes *b);
Ne_Exception *Ne_Bytes_index(Ne_Number *dest, const Ne_Bytes *b, const Ne_Number *index);
Ne_Exception *Ne_Bytes_range(Ne_Bytes *dest, const Ne_Bytes *b, const Ne_Number *first, Ne_Boolean first_from_end, const Ne_Number *last, Ne_Boolean last_from_end);
Ne_Exception *Ne_Bytes_splice(const Ne_Bytes *src, Ne_Bytes *b, const Ne_Number *first, Ne_Boolean first_from_end, const Ne_Number *last, Ne_Boolean last_from_end);
Ne_Exception *Ne_Bytes_store(const Ne_Number *b, Ne_Bytes *s, const Ne_Number *index);
Ne_Exception *Ne_bytes__append(Ne_Bytes *r, const Ne_Bytes *b);
Ne_Exception *Ne_bytes__concat(Ne_Bytes *r, const Ne_Bytes *a, const Ne_Bytes *b);
Ne_Exception *Ne_bytes__decodeToString(Ne_String *r, const Ne_Bytes *bytes);
Ne_Exception *Ne_bytes__size(Ne_Number *r, const Ne_Bytes *bytes);
Ne_Exception *Ne_bytes__toArray(Ne_Array *result, const Ne_Bytes *bytes);
Ne_Exception *Ne_bytes__toString(Ne_String *result, const Ne_Bytes *bytes);
void Ne_Array_init(Ne_Array *a, int size, const MethodTable *mtable);
void Ne_Array_init_copy(Ne_Array *dest, const Ne_Array *src);
void Ne_Array_copy(Ne_Array *dest, const Ne_Array *src);
void Ne_Array_deinit(Ne_Array *a);
int Ne_Array_compare(const Ne_Array *a, const Ne_Array *b);
Ne_Exception *Ne_Array_in(Ne_Boolean *result, const Ne_Array *a, void *element);
Ne_Exception *Ne_Array_index_int(void **result, Ne_Array *a, int index, Ne_Boolean always_create);
Ne_Exception *Ne_Array_index(void **result, Ne_Array *a, const Ne_Number *index, Ne_Boolean always_create);
Ne_Exception *Ne_Array_range(Ne_Array *result, const Ne_Array *a, const Ne_Number *first, Ne_Boolean first_from_end, const Ne_Number *last, Ne_Boolean last_from_end);
Ne_Exception *Ne_Array_splice(const Ne_Array *b, Ne_Array *a, const Ne_Number *first, Ne_Boolean first_from_end, const Ne_Number *last, Ne_Boolean last_from_end);
Ne_Exception *Ne_array__append(Ne_Array *a, const void *element);
Ne_Exception *Ne_array__concat(Ne_Array *dest, const Ne_Array *a, const Ne_Array *b);
Ne_Exception *Ne_array__extend(Ne_Array *dest, const Ne_Array *src);
Ne_Exception *Ne_array__find(Ne_Number *result, const Ne_Array *a, void *e);
Ne_Exception *Ne_array__remove(Ne_Array *a, const Ne_Number *index);
Ne_Exception *Ne_array__resize(Ne_Array *a, const Ne_Number *size);
Ne_Exception *Ne_array__reversed(Ne_Array *dest, const Ne_Array *src);
Ne_Exception *Ne_array__size(Ne_Number *result, const Ne_Array *a);
Ne_Exception *Ne_array__toBytes__number(Ne_Bytes *r, const Ne_Array *a);
Ne_Exception *Ne_array__toString__number(Ne_String *r, const Ne_Array *a);
Ne_Exception *Ne_array__toString__string(Ne_String *r, const Ne_Array *a);
void Ne_Dictionary_init(Ne_Dictionary *d, const MethodTable *mtable);
void Ne_Dictionary_init_copy(Ne_Dictionary *dest, const Ne_Dictionary *src);
void Ne_Dictionary_copy(Ne_Dictionary *dest, const Ne_Dictionary *src);
void Ne_Dictionary_deinit(Ne_Dictionary *d);
Ne_Exception *Ne_Dictionary_in(Ne_Boolean *result, const Ne_Dictionary *d, const Ne_String *key);
Ne_Exception *Ne_Dictionary_index(void **result, Ne_Dictionary *d, const Ne_String *index, Ne_Boolean always_create);
Ne_Exception *Ne_dictionary__keys(Ne_Array *result, const Ne_Dictionary *d);
Ne_Exception *Ne_dictionary__remove(Ne_Dictionary *d, const Ne_String *key);
Ne_Exception *Ne_dictionary__toString__string(Ne_String *r, const Ne_Dictionary *d);
Ne_Exception *Ne_num(Ne_Number *result, const Ne_String *s);
Ne_Exception *Ne_number__toString(Ne_String *result, const Ne_Number *n);
void Ne_Object_init(Ne_Object *obj);
void Ne_Object_init_copy(Ne_Object *dest, const Ne_Object *src);
void Ne_Object_constructor(Ne_Object **obj);
void Ne_Object_copy(Ne_Object *dest, const Ne_Object *src);
void Ne_Object_destructor(Ne_Object *obj);
void Ne_Object_deinit(Ne_Object *obj);
int Ne_Object_compare(const Ne_Object *a, const Ne_Object *b);
Ne_Exception *Ne_object__getBoolean(Ne_Boolean *r, Ne_Object *obj);
Ne_Exception *Ne_object__getNumber(Ne_Number *r, Ne_Object *obj);
Ne_Exception *Ne_object__getString(Ne_String *r, Ne_Object *obj);
Ne_Exception *Ne_object__isNull(Ne_Boolean *r, Ne_Object *obj);
Ne_Exception *Ne_object__makeNull(Ne_Object *obj);
Ne_Exception *Ne_object__makeBoolean(Ne_Object *obj, const Ne_Boolean *b);
Ne_Exception *Ne_object__makeNumber(Ne_Object *obj, const Ne_Number *n);
Ne_Exception *Ne_object__makeString(Ne_Object *obj, const Ne_String *s);
Ne_Exception *Ne_object__toString(Ne_String *result, const Ne_Object *obj);
Ne_Exception *Ne_pointer__toString(Ne_String *r, const void *p);
Ne_Exception *Ne_print(const Ne_Object *obj);
Ne_Exception *Ne_str(Ne_String *result, const Ne_Number *n);
Ne_Exception *Ne_string__append(Ne_String *dest, const Ne_String *s);
Ne_Exception *Ne_string__concat(Ne_String *dest, const Ne_String *a, const Ne_String *b);
Ne_Exception *Ne_string__length(Ne_Number *result, const Ne_String *str);
Ne_Exception *Ne_string__toBytes(Ne_Bytes *result, const Ne_String *str);
Ne_Exception *Ne_Exception_raise(const char *name);
Ne_Exception *Ne_Exception_raise_info(const char *name, const Ne_Object *info);
Ne_Exception *Ne_Exception_raise_info_literal(const char *name, const char *info);
void Ne_Exception_clear();
int Ne_Exception_trap(const char *name);
Ne_Exception *Ne_Exception_propagate();
void Ne_Exception_unhandled();

Ne_Exception *Ne_binary_and32(Ne_Number *r, Ne_Number *x, Ne_Number *y);
Ne_Exception *Ne_binary_and64(Ne_Number *r, Ne_Number *x, Ne_Number *y);
Ne_Exception *Ne_binary_andBytes(Ne_Bytes *r, Ne_Bytes *x, Ne_Bytes *y);
Ne_Exception *Ne_binary_bitCount32(Ne_Number *r, Ne_Number *x);
Ne_Exception *Ne_binary_bitCount64(Ne_Number *r, Ne_Number *x);
Ne_Exception *Ne_binary_bitCountBytes(Ne_Number *r, Ne_Bytes *x);
Ne_Exception *Ne_binary_extract32(Ne_Number *r, Ne_Number *x, Ne_Number *n, Ne_Number *w);
Ne_Exception *Ne_binary_extract64(Ne_Number *r, Ne_Number *x, Ne_Number *n, Ne_Number *w);
Ne_Exception *Ne_binary_get32(Ne_Boolean *r, Ne_Number *x, Ne_Number *n);
Ne_Exception *Ne_binary_get64(Ne_Boolean *r, Ne_Number *x, Ne_Number *n);
Ne_Exception *Ne_binary_not32(Ne_Number *r, Ne_Number *x);
Ne_Exception *Ne_binary_not64(Ne_Number *r, Ne_Number *x);
Ne_Exception *Ne_binary_notBytes(Ne_Bytes *r, Ne_Bytes *x);
Ne_Exception *Ne_binary_or32(Ne_Number *r, Ne_Number *x, Ne_Number *y);
Ne_Exception *Ne_binary_or64(Ne_Number *r, Ne_Number *x, Ne_Number *y);
Ne_Exception *Ne_binary_orBytes(Ne_Bytes *r, Ne_Bytes *x, Ne_Bytes *y);
Ne_Exception *Ne_binary_replace32(Ne_Number *r, Ne_Number *x, Ne_Number *n, Ne_Number *w, Ne_Number *y);
Ne_Exception *Ne_binary_replace64(Ne_Number *r, Ne_Number *x, Ne_Number *n, Ne_Number *w, Ne_Number *y);
Ne_Exception *Ne_binary_set32(Ne_Number *r, Ne_Number *x, Ne_Number *n, Ne_Boolean *v);
Ne_Exception *Ne_binary_set64(Ne_Number *r, Ne_Number *x, Ne_Number *n, Ne_Boolean *v);
Ne_Exception *Ne_binary_shiftLeft32(Ne_Number *r, Ne_Number *x, Ne_Number *n);
Ne_Exception *Ne_binary_shiftLeft64(Ne_Number *r, Ne_Number *x, Ne_Number *n);
Ne_Exception *Ne_binary_shiftRight32(Ne_Number *r, Ne_Number *x, Ne_Number *n);
Ne_Exception *Ne_binary_shiftRight64(Ne_Number *r, Ne_Number *x, Ne_Number *n);
Ne_Exception *Ne_binary_shiftRightSigned32(Ne_Number *r, Ne_Number *x, Ne_Number *n);
Ne_Exception *Ne_binary_shiftRightSigned64(Ne_Number *r, Ne_Number *x, Ne_Number *n);
Ne_Exception *Ne_binary_xor32(Ne_Number *r, Ne_Number *x, Ne_Number *y);
Ne_Exception *Ne_binary_xor64(Ne_Number *r, Ne_Number *x, Ne_Number *y);
Ne_Exception *Ne_binary_xorBytes(Ne_Bytes *r, Ne_Bytes *x, Ne_Bytes *y);

Ne_Exception *Ne_file__CONSTANT_Separator(Ne_String *result);
Ne_Exception *Ne_file_copy(const Ne_String *filename, const Ne_String *destination);
Ne_Exception *Ne_file_copyOverwriteIfExists(const Ne_String *filename, const Ne_String *destination);
Ne_Exception *Ne_file_delete(const Ne_String *filename);
Ne_Exception *Ne_file_exists(Ne_Boolean *result, const Ne_String *filename);
Ne_Exception *Ne_file_files(Ne_Array *result, const Ne_String *path);
Ne_Exception *Ne_file_getInfo(void/*TODO*/ *result, const Ne_String *name);
Ne_Exception *Ne_file_isDirectory(Ne_Boolean *result, const Ne_String *path);
Ne_Exception *Ne_file_mkdir(const Ne_String *path);
Ne_Exception *Ne_file_readBytes(Ne_Bytes *result, const Ne_String *filename);
Ne_Exception *Ne_file_readLines(Ne_Array *result, const Ne_String *filename);
Ne_Exception *Ne_file_removeEmptyDirectory(const Ne_String *path);
Ne_Exception *Ne_file_rename(const Ne_String *oldname, const Ne_String *newname);
Ne_Exception *Ne_file_symlink(const Ne_String *target, const Ne_String *newlink, const Ne_Boolean *targetIsDirectory);
Ne_Exception *Ne_file_writeBytes(const Ne_String *filename, const Ne_Bytes *data);
Ne_Exception *Ne_file_writeLines(const Ne_String *filename, const Ne_Array *data);

Ne_Exception *Ne_math__CONSTANT_PRECISION_DIGITS(Ne_Number *result);
Ne_Exception *Ne_math_abs(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_acos(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_acosh(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_asin(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_asinh(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_atan(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_atanh(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_atan2(Ne_Number *result, const Ne_Number *y, const Ne_Number *x);
Ne_Exception *Ne_math_cbrt(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_ceil(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_cos(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_cosh(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_erf(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_erfc(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_exp(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_exp2(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_expm1(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_floor(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_frexp(Ne_Number *result, const Ne_Number *x, Ne_Number *exp);
Ne_Exception *Ne_math_hypot(Ne_Number *result, const Ne_Number *x, const Ne_Number *y);
Ne_Exception *Ne_math_intdiv(Ne_Number *result, const Ne_Number *x, const Ne_Number *y);
Ne_Exception *Ne_math_ldexp(Ne_Number *result, const Ne_Number *x, const Ne_Number *exp);
Ne_Exception *Ne_math_lgamma(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_log(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_log10(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_log1p(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_log2(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_max(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
Ne_Exception *Ne_math_min(Ne_Number *result, const Ne_Number *a, const Ne_Number *b);
Ne_Exception *Ne_math_nearbyint(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_odd(Ne_Boolean *result, const Ne_Number *x);
Ne_Exception *Ne_math_powmod(Ne_Number *result, const Ne_Number *b, const Ne_Number *e, const Ne_Number *m);
Ne_Exception *Ne_math_round(Ne_Number *result, const Ne_Number *places, const Ne_Number *value);
Ne_Exception *Ne_math_sign(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_sin(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_sinh(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_sqrt(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_tan(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_tanh(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_tgamma(Ne_Number *result, const Ne_Number *x);
Ne_Exception *Ne_math_trunc(Ne_Number *result, const Ne_Number *x);

Ne_Exception *Ne_random_bytes(Ne_Bytes *result, const Ne_Number *n);
Ne_Exception *Ne_random_uint32(Ne_Number *result);

Ne_Exception *Ne_runtime_isModuleImported(Ne_Boolean *r, Ne_String *module);

Ne_Exception *Ne_string_find(Ne_Number *result, const Ne_String *s, const Ne_String *t);
Ne_Exception *Ne_string_fromCodePoint(Ne_String *result, const Ne_Number *n);
Ne_Exception *Ne_string_lower(Ne_String *result, const Ne_String *s);
Ne_Exception *Ne_string_toCodePoint(Ne_Number *result, const Ne_String *s);
Ne_Exception *Ne_string_trimCharacters(Ne_String *result, const Ne_String *s, const Ne_String *trimLeadingChars, const Ne_String *trimTrailingChars);

extern Ne_Array sys_x24args;
void Ne_sys__init(int argc, const char *argv[]);
Ne_Exception *Ne_sys_exit(const Ne_Number *n);

extern void *textio_x24stderr;
Ne_Exception *Ne_textio_writeLine(void *f, const Ne_String *s);

Ne_Exception *Ne_time_now(Ne_Number *result);
Ne_Exception *Ne_time_tick(Ne_Number *result);
