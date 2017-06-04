#include "neonext.h"

const struct Ne_MethodTable *Ne;

int g_Number = 5;

Ne_EXPORT int Ne_Init(const struct Ne_MethodTable *methodtable)
{
    Ne = methodtable;
    return Ne_SUCCESS;
}

Ne_EXPORT int constNumber(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_number_int(retval, 5);
    return Ne_SUCCESS;
}

Ne_EXPORT int varNumber(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_number_int(retval, g_Number);
    return Ne_SUCCESS;
}

Ne_EXPORT int funcNothing(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    return Ne_SUCCESS;
}

Ne_EXPORT int funcBoolean(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_boolean(retval, 1);
    return Ne_SUCCESS;
}

Ne_EXPORT int funcNumber(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_number_int(retval, 5);
    return Ne_SUCCESS;
}

Ne_EXPORT int funcString(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_string(retval, "five");
    return Ne_SUCCESS;
}

Ne_EXPORT int funcBytes(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_bytes(retval, (unsigned char *)"\x05\x05\x05\x05\x05", 5);
    return Ne_SUCCESS;
}

Ne_EXPORT int funcArray(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_number_int(Ne->cell_set_array_cell(retval, 0), 5);
    return Ne_SUCCESS;
}

Ne_EXPORT int funcDictionary(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_number_int(Ne->cell_set_dictionary_cell(retval, "five"), 5);
    return Ne_SUCCESS;
}

Ne_EXPORT int funcNumberAdd(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    int x = Ne->cell_get_number_int(Ne->parameterlist_get_cell(in_params, 0));
    int y = Ne->cell_get_number_int(Ne->parameterlist_get_cell(in_params, 1));
    Ne->cell_set_number_int(retval, x + y);
    return Ne_SUCCESS;
}

Ne_EXPORT int funcNumberOut(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_number_int(Ne->parameterlist_set_cell(out_params, 0), 5);
    return Ne_SUCCESS;
}

Ne_EXPORT int funcNumberOut2(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    Ne->cell_set_number_int(Ne->parameterlist_set_cell(out_params, 0), 10);
    Ne->cell_set_number_int(Ne->parameterlist_set_cell(out_params, 1), 20);
    return Ne_SUCCESS;
}

Ne_EXPORT int setVarNumber(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    int n = Ne->cell_get_number_int(Ne->parameterlist_get_cell(in_params, 0));
    g_Number = n;
    return Ne_SUCCESS;
}

Ne_EXPORT int raiseException(struct Ne_Cell *retval, struct Ne_ParameterList *in_params, struct Ne_ParameterList *out_params)
{
    return Ne->raise_exception(retval, "SampleException", "info", 5);
}
