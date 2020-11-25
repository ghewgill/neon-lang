#include <iso646.h>
#include <windows.h>

#include "cell.h"
#include "exec.h"
#include "object.h"
#include "rtl_exec.h"

namespace rtl {

namespace ne_runtime {

namespace {

class ComObject: public Object {
public:
    const std::string name;
    IDispatch *obj;
public:
    ComObject(const std::string &name, IDispatch *obj): name(name), obj(obj) {}
    ComObject(const ComObject &) = delete;
    ComObject &operator=(const ComObject &) = delete;
    ~ComObject() {
        obj->Release();
    }
    virtual bool invokeMethod(const utf8string &methodname, const std::vector<std::shared_ptr<Object>> &args, std::shared_ptr<Object> &result) const override;
    virtual bool subscript(std::shared_ptr<Object>, std::shared_ptr<Object> &) const override;
    virtual utf8string toString() const { return utf8string("<ComObject:" + name + ">"); }
};

BSTR bstr_from_utf8string(const utf8string &s)
{
    int nchars = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
    OLECHAR *wstr = new OLECHAR[nchars];
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, wstr, nchars);
    BSTR bstr = SysAllocString(wstr);
    delete[] wstr;
    return bstr;
}

utf8string utf8string_from_bstr(BSTR bstr)
{
    int nchars = WideCharToMultiByte(CP_UTF8, 0, bstr, -1, NULL, 0, NULL, NULL);
    char *buf = new char[nchars];
    WideCharToMultiByte(CP_UTF8, 0, bstr, -1, buf, nchars, NULL, NULL);
    utf8string r { buf };
    delete[] buf;
    return r;
}

void VariantFromObject(VARIANT *v, std::shared_ptr<Object> obj)
{
    if (obj == nullptr) {
        v->vt = VT_NULL;
        return;
    }
    bool b;
    if (obj->getBoolean(b)) {
        v->vt = VT_BOOL;
        v->boolVal = b ? -1 : 0;
        return;
    }
    Number n;
    if (obj->getNumber(n)) {
        if (number_is_integer(n)) {
            v->vt = VT_I8;
            v->llVal = number_to_sint64(n);
        } else {
            v->vt = VT_R8;
            v->dblVal = number_to_double(n);
        }
        return;
    }
    utf8string s;
    if (obj->getString(s)) {
        v->vt = VT_BSTR;
        v->bstrVal = bstr_from_utf8string(s);
        return;
    }
    ComObject *co = dynamic_cast<ComObject *>(obj.get());
    if (co != nullptr) {
        v->vt = VT_DISPATCH;
        v->pdispVal = co->obj;
        co->obj->AddRef();
        return;
    }
    throw RtlException(Exception_NativeObjectException, utf8string("could not convert Object to VARIANT"));
}

std::shared_ptr<Object> ObjectFromVariant(VARIANT &v)
{
    switch (v.vt) {
        case VT_EMPTY:
        case VT_NULL:
            return nullptr;
        case VT_BOOL:
            return std::make_shared<ObjectBoolean>(v.boolVal != 0);
        case VT_I1:
            return std::make_shared<ObjectNumber>(number_from_sint8(v.bVal));
        case VT_I4:
            return std::make_shared<ObjectNumber>(number_from_sint32(v.lVal));
        case VT_BSTR:
            return std::make_shared<ObjectString>(utf8string_from_bstr(v.bstrVal));
        case VT_DISPATCH:
            v.pdispVal->AddRef();
            return std::make_shared<ComObject>("IDispatch", v.pdispVal);
    }
    throw RtlException(Exception_NativeObjectException, utf8string("could not convert VARIANT (vt=" + std::to_string(v.vt) + ") to Object"));
}

void handleInvokeResult(HRESULT r, UINT cArgs, EXCEPINFO &ei, UINT &argerr)
{
    switch (r) {
        case S_OK:
            break;
        case DISP_E_BADPARAMCOUNT:
            throw RtlException(Exception_NativeObjectException, utf8string("incorrect number of arguments for this method"));
        case DISP_E_BADVARTYPE:
            throw RtlException(Exception_NativeObjectException, utf8string("invalid variant type in arguments"));
        case DISP_E_EXCEPTION:
            throw RtlException(Exception_NativeObjectException, utf8string_from_bstr(ei.bstrDescription));
        case DISP_E_MEMBERNOTFOUND:
            throw RtlException(Exception_NativeObjectException, utf8string("the requested member does not exist"));
        case DISP_E_NONAMEDARGS:
            throw RtlException(Exception_NativeObjectException, utf8string("this method does not support named arguments"));
        case DISP_E_OVERFLOW:
            throw RtlException(Exception_NativeObjectException, utf8string("invalid type in parameters"));
        case DISP_E_PARAMNOTFOUND:
            throw RtlException(Exception_NativeObjectException, utf8string("parameter not found (" + std::to_string(cArgs - argerr) + ")"));
        case DISP_E_TYPEMISMATCH:
            throw RtlException(Exception_NativeObjectException, utf8string("type mismatch in parameter (" + std::to_string(cArgs - argerr) + ")"));
        case DISP_E_UNKNOWNINTERFACE:
            throw RtlException(Exception_NativeObjectException, utf8string("unknown interface"));
        case DISP_E_UNKNOWNLCID:
            throw RtlException(Exception_NativeObjectException, utf8string("unknown lcid"));
        case DISP_E_PARAMNOTOPTIONAL:
            throw RtlException(Exception_NativeObjectException, utf8string("a required parameter was omitted"));
        default:
            throw RtlException(Exception_NativeObjectException, utf8string("unknown error from IDispatch::Invoke (" + std::to_string(r) + ")"));
    }
}

bool ComObject::invokeMethod(const utf8string &methodname, const std::vector<std::shared_ptr<Object>> &args, std::shared_ptr<Object> &result) const
{
    BSTR mname = bstr_from_utf8string(methodname);
    OLECHAR *a = mname;
    DISPID dispid;
    HRESULT r = obj->GetIDsOfNames(IID_NULL, &a, 1, GetUserDefaultLCID(), &dispid);
    SysFreeString(mname);
    if (r != S_OK) {
        throw RtlException(Exception_NativeObjectException, utf8string("method not found in object (" + methodname.str() + ")"));
    }
    DISPPARAMS params = {NULL, NULL, 0, 0};
    params.cArgs = args.size();
    params.rgvarg = new VARIANTARG[params.cArgs];
    for (size_t i = 0; i < args.size(); i++) {
        VARIANT *v = &params.rgvarg[params.cArgs - 1 - i];
        VariantInit(v);
        VariantFromObject(v, args[i]);
    }
    VARIANT rv;
    VariantInit(&rv);
    EXCEPINFO ei;
    UINT argerr;
    r = obj->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD, &params, &rv, &ei, &argerr);
    for (size_t i = 0; i < args.size(); i++) {
        VariantClear(&params.rgvarg[i]);
    }
    delete[] params.rgvarg;
    handleInvokeResult(r, params.cArgs, ei, argerr);
    result = ObjectFromVariant(rv);
    VariantClear(&rv);
    return true;
}

bool ComObject::subscript(std::shared_ptr<Object> index, std::shared_ptr<Object> &result) const
{
    utf8string name;
    if (not index->getString(name)) {
        throw RtlException(Exception_NativeObjectException, utf8string("property name must be a string"));
    }
    BSTR pname = bstr_from_utf8string(name);
    OLECHAR *a = pname;
    DISPID dispid;
    HRESULT r = obj->GetIDsOfNames(IID_NULL, &a, 1, GetUserDefaultLCID(), &dispid);
    SysFreeString(pname);
    if (r != S_OK) {
        throw RtlException(Exception_NativeObjectException, utf8string("property not found in object (" + name.str() + ")"));
    }
    DISPPARAMS params = {NULL, NULL, 0, 0};
    VARIANT rv;
    VariantInit(&rv);
    EXCEPINFO ei;
    UINT argerr;
    r = obj->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_PROPERTYGET, &params, &rv, &ei, &argerr);
    handleInvokeResult(r, params.cArgs, ei, argerr);
    result = ObjectFromVariant(rv);
    VariantClear(&rv);
    return true;
}

} // anonymous namespace

std::shared_ptr<Object> createObject(const utf8string &name)
{
    static bool com_init = false;
    if (!com_init) {
        HRESULT r = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        if (r == S_OK || r == S_FALSE) {
            com_init = true;
        } else {
            throw RtlException(Exception_NativeObjectException, utf8string("CoInitializeEx error (" + std::to_string(r) + ")"));
        }
    }
    OLECHAR wname[200];
    MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, wname, sizeof(wname)/sizeof(wname[0]));
    CLSID clsid;
    HRESULT r = CLSIDFromProgID(wname, &clsid);
    if (r != S_OK) {
        throw RtlException(Exception_NativeObjectException, utf8string("class not found (" + name.str() + ")"));
    }
    IDispatch *obj;
    r = CoCreateInstance(clsid, NULL, CLSCTX_ALL, IID_IDispatch, reinterpret_cast<void **>(&obj));
    if (r != S_OK) {
        throw RtlException(Exception_NativeObjectException, utf8string("could not create object (" + name.str() + ") result (" + std::to_string(r) + ")"));
    }
    return std::make_shared<ComObject>(name.str(), obj);
}

} // namespace ne_runtime

} // namespace rtl
