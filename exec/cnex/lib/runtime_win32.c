#include "runtime.h"

#define COBJMACROS
#define strdup _strdup

#include <io.h>
#include <windows.h>

#include "assert.h"
#include "cell.h"
#include "enums.h"
#include "exec.h"
#include "object.h"
#include "stack.h"



Object *com_createComObject(char *name, IDispatch *pdisp);

typedef struct tagTComObject {
    char *name;
    IDispatch *obj;
} ComObject;

// ToDo: Implement UTF8 strings!
BSTR bstr_from_utf8string(const char *s)
{
    int nchars = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
    OLECHAR *wstr = malloc(sizeof(OLECHAR) * nchars);
    MultiByteToWideChar(CP_ACP, 0, s, -1, wstr, nchars);
    BSTR bstr = SysAllocString(wstr);
    free(wstr);
    return bstr;
}

// ToDo: Implement UTF8 strings!
char *utf8string_from_bstr(BSTR bstr)
{
    if (bstr == NULL) {
        return "null";
    }
    int nchars = WideCharToMultiByte(CP_ACP, 0, bstr, -1, NULL, 0, NULL, NULL);
    char *buf = malloc(nchars);
    WideCharToMultiByte(CP_ACP, 0, bstr, -1, buf, nchars, NULL, NULL);
    return buf;
}

BOOL VariantFromObject(TExecutor *exec, VARIANT *v, Object *obj)
{
    if (obj == NULL) {
        v->vt = VT_NULL;
        return TRUE;
    }
    BOOL b;
    if (obj->type == oBoolean) {
        b = ((Cell*)obj->ptr)->boolean;
        v->vt = VT_BOOL;
        v->boolVal = b ? -1 : 0;
        return TRUE;
    }
    Number n;
    if (obj->type == oNumber) {
        n = ((Cell*)obj->ptr)->number;
        if (number_is_integer(n)) {
            v->vt = VT_I8;
            v->llVal = number_to_sint64(n);
        } else {
            v->vt = VT_R8;
            v->dblVal = number_to_double(n);
        }
        return TRUE;
    }
    if (obj->type == oString) {
        const char *s = string_ensureNullTerminated(((Cell*)obj->ptr)->string);
        v->vt = VT_BSTR;
        v->bstrVal = bstr_from_utf8string(s);
        return TRUE;
    }
    ComObject *co = (ComObject*)obj->ptr;
    if (co != NULL) {
        v->vt = VT_DISPATCH;
        v->pdispVal = co->obj;
        co->obj->lpVtbl->AddRef(co->obj);
        return TRUE;
    }
    exec->rtl_raise(exec, "NativeObjectException", "could not convert Object to VARIANT");
    return FALSE;
}

Object *ObjectFromVariant(TExecutor *exec, VARIANT *v)
{
    switch (v->vt) {
        case VT_EMPTY:
        case VT_NULL:
            return NULL;
        case VT_BOOL:
            return object_createBooleanObject(v->boolVal != 0);
        case VT_I1:
            return object_createNumberObject(number_from_sint8(v->bVal));
        case VT_I4:
            return object_createNumberObject(number_from_sint32(v->lVal));
        case VT_BSTR:
        {
            char *utf8 = utf8string_from_bstr(v->bstrVal);
            TString *str = string_createCString(utf8);
            free(utf8);
            Object *o = object_createStringObject(str);
            string_freeString(str);
            return o;
        }
        case VT_DISPATCH:
            v->pdispVal->lpVtbl->AddRef(v->pdispVal);
            Object *o = com_createComObject("IDispatch", v->pdispVal);
            return o;
    }
    char err[64];
    snprintf(err, sizeof(err), "could not convert VARIANT (vt=%d) to Object", v->vt);
    exec->rtl_raise(exec, "NativeObjectException", err);
    return NULL;
}

BOOL handleInvokeResult(TExecutor *exec, HRESULT r, UINT cArgs, EXCEPINFO *ei, UINT argerr)
{
    char err[512];
    switch (r) {
        case S_OK:
            break;
        case DISP_E_BADPARAMCOUNT:
            snprintf(err, sizeof(err), "incorrect number of arguments for this method");
            exec->rtl_raise(exec, "NativeObjectException", err);
            return FALSE;
        case DISP_E_BADVARTYPE:
            snprintf(err, sizeof(err), "invalid variant type in arguments");
            exec->rtl_raise(exec, "NativeObjectException", err);
            return FALSE;
        case DISP_E_EXCEPTION:
        {
            char *desc = utf8string_from_bstr(ei->bstrDescription);
            exec->rtl_raise(exec, "NativeObjectException", desc);
            free(desc);
            return FALSE;
        }
        case DISP_E_MEMBERNOTFOUND:
            exec->rtl_raise(exec, "NativeObjectException", "the requested member does not exist");
            return FALSE;
        case DISP_E_NONAMEDARGS:
            exec->rtl_raise(exec, "NativeObjectException", "this method does not support named arguments");
            return FALSE;
        case DISP_E_OVERFLOW:
            exec->rtl_raise(exec, "NativeObjectException", "invalid type in parameters");
            return FALSE;
        case DISP_E_PARAMNOTFOUND:
            snprintf(err, sizeof(err), "parameter not found (%d)", cArgs - argerr);
            exec->rtl_raise(exec, "NativeObjectException", err);
            return FALSE;
        case DISP_E_TYPEMISMATCH:
            snprintf(err, sizeof(err), "type mismatch in parameter(%d)", cArgs - argerr);
            exec->rtl_raise(exec, "NativeObjectException", err);
            return FALSE;
        case DISP_E_UNKNOWNINTERFACE:
            snprintf(err, sizeof(err), "unknown interface");
            exec->rtl_raise(exec, "NativeObjectException", err);
            return FALSE;
        case DISP_E_UNKNOWNLCID:
            snprintf(err, sizeof(err), "unknown lcid");
            exec->rtl_raise(exec, "NativeObjectException", err);
            return FALSE;
        case DISP_E_PARAMNOTOPTIONAL:
            snprintf(err, sizeof(err), "a required parameter was omitted");
            exec->rtl_raise(exec, "NativeObjectException", err);
            return FALSE;
        default:
            snprintf(err, sizeof(err), "unknown error from IDispatch::Invoke (0x%08X)", r);
            exec->rtl_raise(exec, "NativeObjectException", err);
            return FALSE;
    }
    return TRUE;
}



void com_releaseObject(Object *o)
{
    if (o != NULL) {
        assert(o->refcount > 0);
        o->refcount--;

        if (o->refcount <= 0) {
            if (o->ptr != NULL) {
                free(((ComObject*)o->ptr)->name);
                IDispatch_Release(((ComObject*)o->ptr)->obj);
            }
            free(o->ptr);
            free(o);
        }
    }
}

Cell *com_toString(Object *self)
{
    char s[256];
    snprintf(s, sizeof(s), "<ComObject:%s>", ((ComObject*)self->ptr)->name);
    Cell *r = cell_fromCString(s);
    return r;
}

BOOL com_invokeMethod(Object *self, TExecutor *exec, char *methodname, Cell *args, Cell **result)
{
    IDispatch *com = ((ComObject*)self->ptr)->obj;
    BSTR mname = bstr_from_utf8string(methodname);
    OLECHAR *a = mname;
    DISPID dispid;
    HRESULT r = com->lpVtbl->GetIDsOfNames(com, &IID_NULL, &a, 1, GetUserDefaultLCID(), &dispid);
    SysFreeString(mname);
    if (r != S_OK) {
        char err[512];
        snprintf(err, sizeof(err), "method not found in object (%s)", methodname);
        exec->rtl_raise(exec, "NativeObjectException", err);
        return FALSE;
    }
    DISPPARAMS params = { NULL, NULL, 0, 0 };
    Array *pargs = args->array;
    params.cArgs = (UINT)pargs->size;
    params.rgvarg = malloc(sizeof(VARIANTARG) * params.cArgs);
    for (size_t i = 0; i < pargs->size; i++) {
        VARIANT *v = &params.rgvarg[params.cArgs - 1 - i];
        VariantInit(v);
        if (!VariantFromObject(exec, v, pargs->data[i].object)) {
            return FALSE;
        }
    }
    VARIANT rv;
    VariantInit(&rv);
    EXCEPINFO ei;
    UINT argerr;
    r = com->lpVtbl->Invoke(com, dispid, &IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD, &params, &rv, &ei, &argerr);
    for (size_t i = 0; i < pargs->size; i++) {
        VariantClear(&params.rgvarg[i]);
    }
    free(params.rgvarg);
    if (!handleInvokeResult(exec, r, params.cArgs, &ei, argerr)) {
        return FALSE;
    }

    (*result)->object = ObjectFromVariant(exec, &rv);
    (*result)->type = cObject;
    VariantClear(&rv);
    return TRUE;
}

BOOL com_setProperty(Object *self, TExecutor *exec, Object *index, Object *value)
{
    ComObject *obj = ((ComObject*)self->ptr);
    if (index->type != oString) {
        exec->rtl_raise(exec, "NativeObjectException", "property name must be a string");
        return FALSE;
    }

    char *name = string_asCString(((Cell*)index->ptr)->string);
    BSTR pname = bstr_from_utf8string(name);
    OLECHAR *a = pname;
    DISPID dispid;
    HRESULT r = obj->obj->lpVtbl->GetIDsOfNames(obj->obj, &IID_NULL, &a, 1, GetUserDefaultLCID(), &dispid);
    SysFreeString(pname);
    if (r != S_OK) {
        char err[256];
        snprintf(err, sizeof(err), "property not found in object(%s)", name);
        free(name);
        exec->rtl_raise(exec, "NativeObjectException", err);
        return FALSE;
    }
    DISPID dispidNamed = DISPID_PROPERTYPUT;
    VARIANTARG arg;
    VariantInit(&arg);
    if (!VariantFromObject(exec, &arg, value)) {
        return FALSE;
    }
    DISPPARAMS params;
    params.cArgs = 1;
    params.cNamedArgs = 1;
    params.rgdispidNamedArgs = &dispidNamed;
    params.rgvarg = &arg;

    EXCEPINFO ei;
    UINT argerr;
    r = obj->obj->lpVtbl->Invoke(obj->obj, dispid, &IID_NULL, GetUserDefaultLCID(), DISPATCH_PROPERTYPUT, &params, NULL, &ei, &argerr);
    handleInvokeResult(exec, r, params.cArgs, &ei, argerr);
    free(name);
    return TRUE;
}

BOOL com_subscript(Object *self, TExecutor *exec, Object *index, Object **result)
{
    ComObject *obj = ((ComObject*)self->ptr);
    if (index->type != oString) {
        exec->rtl_raise(exec, "NativeObjectException", "property name must be a string");
        return FALSE;
    }

    char *name = string_asCString(((Cell*)index->ptr)->string);
    BSTR pname = bstr_from_utf8string(name);
    OLECHAR *a = pname;
    DISPID dispid;
    HRESULT r = obj->obj->lpVtbl->GetIDsOfNames(obj->obj, &IID_NULL, &a, 1, GetUserDefaultLCID(), &dispid);
    SysFreeString(pname);
    if (r != S_OK) {
        char err[256];
        snprintf(err, sizeof(err), "property not found in object(%s)", name);
        free(name);
        exec->rtl_raise(exec, "NativeObjectException", err);
        return FALSE;
    }
    DISPPARAMS params = { NULL, NULL, 0, 0 };
    VARIANT rv;
    VariantInit(&rv);
    EXCEPINFO ei;
    UINT argerr;
    r = obj->obj->lpVtbl->Invoke(obj->obj, dispid, &IID_NULL, GetUserDefaultLCID(), DISPATCH_PROPERTYGET, &params, &rv, &ei, &argerr);
    if (!handleInvokeResult(exec, r, params.cArgs, &ei, argerr)) {
        return FALSE;
    }
    *result = ObjectFromVariant(exec, &rv);
    VariantClear(&rv);
    free(name);
    return TRUE;
}

Object *com_createComObject(char *name, IDispatch *pdisp)
{
    Object *r = object_createObject();
    r->type = oNative;
    r->release = com_releaseObject;
    r->toString = com_toString;
    r->invokeMethod = com_invokeMethod;
    r->setProperty = com_setProperty;
    r->subscript = com_subscript;
    r->ptr = malloc(sizeof(ComObject));
    ((ComObject*)r->ptr)->name = strdup(name);
    ((ComObject*)r->ptr)->obj = pdisp;
    return r;
}



void runtime_createObject(TExecutor *exec)
{
    char *name = string_asCString(top(exec->stack)->string); pop(exec->stack);

    static BOOL com_init = FALSE;
    if (!com_init) {
        HRESULT r = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        if (r == S_OK || r == S_FALSE) {
            com_init = TRUE;
        } else {
            char err[64];
            snprintf(err, sizeof(err), "CoInitializeEx error (0x%08X)", r);
            exec->rtl_raise(exec, "NativeObjectException", err);
            return;
        }
    }

    Object *obj = com_createComObject(name, NULL);
    OLECHAR wname[200];
    MultiByteToWideChar(CP_ACP, 0, name, -1, wname, sizeof(wname) / sizeof(wname[0]));
    CLSID clsid;
    HRESULT r = CLSIDFromProgID(wname, &clsid);
    if (r != S_OK) {
        obj->release(obj);
        char err[256];
        snprintf(err, sizeof(err), "class not found (%s)", name);
        exec->rtl_raise(exec, "NativeObjectException", err);
        free(name);
        return;
    }

    r = CoCreateInstance(&clsid, NULL, CLSCTX_ALL, &IID_IDispatch, (void **)(&((ComObject*)obj->ptr)->obj));
    if (r != S_OK) {
        obj->release(obj);
        char err[300];
        snprintf(err, sizeof(err), "could not create object (%s) result (0x%08X)", name, r);
        exec->rtl_raise(exec, "NativeObjectException", err);
        free(name);
        return;
    }

    push(exec->stack, cell_fromObject(obj));
    free(name);
}
