#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define Ne_EXPORT __declspec(dllexport)
#else
#define Ne_EXPORT
#endif

#define Ne_SUCCESS 0
#define Ne_EXCEPTION 1

struct Ne_ParameterList;
struct Ne_Cell;
struct Ne_Boolean;
struct Ne_Number;
struct Ne_String;
struct Ne_Bytes;
struct Ne_ArrayNumber;
struct Ne_ArrayString;
struct Ne_DictionaryNumber;
struct Ne_DictionaryString;

struct Ne_MethodTable {
    int (*parameterlist_get_size)(const struct Ne_ParameterList *list);
    int (*parameterlist_check_types)(const struct Ne_ParameterList *list, const char *types);
    const struct Ne_Cell *(*parameterlist_get_cell)(const struct Ne_ParameterList *list, int i);
    struct Ne_Cell *(*parameterlist_set_cell)(struct Ne_ParameterList *list, int i);

    int (*cell_get_boolean)(const struct Ne_Cell *cell);
    void (*cell_set_boolean)(struct Ne_Cell *cell, int value);
    int (*cell_get_number_int)(const struct Ne_Cell *cell);
    void (*cell_set_number_int)(struct Ne_Cell *cell, int value);
    const char *(*cell_get_string)(const struct Ne_Cell *cell);
    void (*cell_set_string)(struct Ne_Cell *cell, const char *value);
    const unsigned char *(*cell_get_bytes)(const struct Ne_Cell *cell);
    int (*cell_get_bytes_size)(const struct Ne_Cell *cell);
    void (*cell_set_bytes)(struct Ne_Cell *cell, const unsigned char *value, int size);
    void *(*cell_get_pointer)(const struct Ne_Cell *cell);
    void (*cell_set_pointer)(struct Ne_Cell *cell, void *p);
    int (*cell_get_array_size)(const struct Ne_Cell *cell);
    const struct Ne_Cell *(*cell_get_array_cell)(const struct Ne_Cell *cell, int index);
    struct Ne_Cell *(*cell_set_array_cell)(struct Ne_Cell *cell, int index);
    const struct Ne_Cell *(*cell_get_dictionary_cell)(const struct Ne_Cell *cell, const char *key);
    struct Ne_Cell *(*cell_set_dictionary_cell)(struct Ne_Cell *cell, const char *key);

    int (*raise_exception)(struct Ne_Cell *retval, const char *name, const char *info, int code);
};

Ne_EXPORT int Ne_Init(const struct Ne_MethodTable *methodtable);
typedef int (*Ne_ExternalFunction)(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params);

#ifdef __cplusplus
} // extern "C"
#endif
