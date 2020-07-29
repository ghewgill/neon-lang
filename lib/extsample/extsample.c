#include <stdlib.h>

#include "neonext.h"

const struct Ne_MethodTable *Ne;
struct Ne_Cell *g_cb;

int g_Number = 5;

Ne_EXPORT int Ne_INIT(const struct Ne_MethodTable *methodtable)
{
    Ne = methodtable;
    g_cb = Ne->cell_alloc();
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_constNumber(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_number_int(retval, 5);
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_varNumber(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_number_int(retval, g_Number);
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_funcNothing(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_funcBoolean(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_boolean(retval, 1);
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_funcNumber(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_number_int(retval, 5);
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_funcString(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_string(retval, "five");
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_funcBytes(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_bytes(retval, (unsigned char *)"\x05\x05\x05\x05\x05", 5);
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_funcArray(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_number_int(Ne->cell_set_array_cell(retval, 0), 5);
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_funcDictionary(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_number_int(Ne->cell_set_dictionary_cell(retval, "five"), 5);
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_funcNumberAdd(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    int x = Ne->cell_get_number_int(Ne->parameterlist_get_cell(in_params, 0));
    int y = Ne->cell_get_number_int(Ne->parameterlist_get_cell(in_params, 1));
    Ne->cell_set_number_int(retval, x + y);
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_funcArraySize(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_number_int(retval, Ne->cell_get_array_size(Ne->parameterlist_get_cell(in_params, 0)));
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_funcNumberOut(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_number_int(Ne->parameterlist_set_cell(out_params, 0), 5);
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_funcNumberOut2(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_number_int(Ne->parameterlist_set_cell(out_params, 0), 10);
    Ne->cell_set_number_int(Ne->parameterlist_set_cell(out_params, 1), 20);
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_setVarNumber(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    int n = Ne->cell_get_number_int(Ne->parameterlist_get_cell(in_params, 0));
    g_Number = n;
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_callback1(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    const struct Ne_Cell *cb = Ne->parameterlist_get_cell(in_params, 0);
    Ne->exec_callback(cb, NULL, NULL);
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_callback2(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    const struct Ne_Cell *cb = Ne->parameterlist_get_cell(in_params, 0);
    const char *s = Ne->cell_get_string(Ne->parameterlist_get_cell(in_params, 1));
    struct Ne_ParameterList *params = Ne->parameterlist_alloc(1);
    Ne->cell_set_string(Ne->parameterlist_set_cell(params, 0), s);
    Ne->exec_callback(cb, params, NULL);
    Ne->parameterlist_free(params);
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_callback3(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    const struct Ne_Cell *cb = Ne->parameterlist_get_cell(in_params, 0);
    int a = Ne->cell_get_number_int(Ne->parameterlist_get_cell(in_params, 1));
    int b = Ne->cell_get_number_int(Ne->parameterlist_get_cell(in_params, 2));
    struct Ne_ParameterList *params = Ne->parameterlist_alloc(2);
    Ne->cell_set_number_int(Ne->parameterlist_set_cell(params, 0), a);
    Ne->cell_set_number_int(Ne->parameterlist_set_cell(params, 1), b);
    Ne->exec_callback(cb, params, retval);
    Ne->parameterlist_free(params);
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_setCallback(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    const struct Ne_Cell *cb = Ne->parameterlist_get_cell(in_params, 0);
    Ne->cell_copy(g_cb, cb);
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_callback4(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    int a = Ne->cell_get_number_int(Ne->parameterlist_get_cell(in_params, 0));
    int b = Ne->cell_get_number_int(Ne->parameterlist_get_cell(in_params, 1));
    struct Ne_ParameterList *params = Ne->parameterlist_alloc(2);
    Ne->cell_set_number_int(Ne->parameterlist_set_cell(params, 0), a);
    Ne->cell_set_number_int(Ne->parameterlist_set_cell(params, 1), b);
    struct Ne_Cell *r = Ne->cell_alloc();
    Ne->exec_callback(g_cb, params, r);
    Ne->parameterlist_free(params);
    Ne->cell_set_number_int(retval, Ne->cell_get_number_int(r) * 2);
    Ne->cell_free(r);
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_raiseException(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    return Ne->raise_exception(retval, "SampleException", "info", 5);
}

Ne_EXPORT int Ne_allocHandle(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    void *p = malloc(1);
    Ne->cell_set_pointer(retval, p);
    return Ne_SUCCESS;
}

Ne_EXPORT int Ne_freeHandle(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    void *p = Ne->cell_get_pointer(retval);
    free(p);
    return Ne_SUCCESS;
}
