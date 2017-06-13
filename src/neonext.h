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
    unsigned int (*cell_get_number_uint)(const struct Ne_Cell *cell);
    void (*cell_set_number_int)(struct Ne_Cell *cell, int value);
    void (*cell_set_number_uint)(struct Ne_Cell *cell, unsigned int value);
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

Ne_EXPORT int Ne_INIT(const struct Ne_MethodTable *methodtable);
typedef int (*Ne_ExtensionFunction)(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params);

#ifdef _MSC_VER
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4127) // conditional expression is constant
#endif

#define Ne_FUNC(name) Ne_EXPORT int name(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)

#define Ne_IN_PARAM(i) Ne->parameterlist_get_cell(in_params, (i))
#define Ne_OUT_PARAM(i) Ne->parameterlist_set_cell(out_params, (i))

#define Ne_PARAM_BOOL(i) Ne->cell_get_boolean(Ne_IN_PARAM(i))
#define Ne_PARAM_INT(i) Ne->cell_get_number_int(Ne_IN_PARAM(i))
#define Ne_PARAM_UINT(i) Ne->cell_get_number_uint(Ne_IN_PARAM(i))
#define Ne_PARAM_STRING(i) Ne->cell_get_string(Ne_IN_PARAM(i))
#define Ne_PARAM_POINTER(type, i) (type *)(Ne->cell_get_pointer(Ne_IN_PARAM(i)))

#define Ne_RETURN_BOOL(r) do { Ne->cell_set_boolean(retval, (r)); return Ne_SUCCESS; } while (0)
#define Ne_RETURN_INT(r) do { Ne->cell_set_number_int(retval, (r)); return Ne_SUCCESS; } while (0)
#define Ne_RETURN_UINT(r) do { Ne->cell_set_number_uint(retval, (r)); return Ne_SUCCESS; } while (0)
#define Ne_RETURN_STRING(r) do { Ne->cell_set_string(retval, (r)); return Ne_SUCCESS; } while (0)
#define Ne_RETURN_POINTER(r) do { Ne->cell_set_pointer(retval, (r)); return Ne_SUCCESS; } while (0)

#define Ne_CONST_INT(name, value) Ne_FUNC(name) { Ne_RETURN_INT(value); }

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus

extern const Ne_MethodTable *Ne;

inline std::vector<int> Ne_PARAM_ARRAY_INT(Ne_ParameterList *in_params, int i)
{
    const Ne_Cell *a = Ne->parameterlist_get_cell(in_params, (i));
    std::vector<int> r;
    int n = Ne->cell_get_array_size(a);
    for (int j = 0; j < n; j++) {
        r.push_back(Ne->cell_get_number_int(Ne->cell_get_array_cell(a, j)));
    }
    return r;
}

#define Ne_RETURN_ARRAY_INT(r) do { for (size_t i = 0; i < r.size(); i++) { Ne->cell_set_number_int(Ne->cell_set_array_cell(retval, i), r[i]); } return Ne_SUCCESS; } while (0)

#endif // __cplusplus
