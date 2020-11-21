#include <iso646.h>
#include <windows.h>

#include "cell.h"
#include "exec.h"
#include "object.h"

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
        OLECHAR wstr[200];
        MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, wstr, sizeof(wstr)/sizeof(wstr[0]));
        v->bstrVal = SysAllocString(wstr);
        return;
    }
    ComObject *co = dynamic_cast<ComObject *>(obj.get());
    if (co != nullptr) {
        v->vt = VT_DISPATCH;
        v->pdispVal = co->obj;
        co->obj->AddRef();
        return;
    }
}

std::shared_ptr<Object> ObjectFromVariant(VARIANT &v)
{
    switch (v.vt) {
        case VT_NULL:
            return nullptr;
        case VT_BOOL:
            return std::make_shared<ObjectBoolean>(v.boolVal != 0);
        case VT_I1:
            return std::make_shared<ObjectNumber>(number_from_sint8(v.bVal));
        case VT_I4:
            return std::make_shared<ObjectNumber>(number_from_sint32(v.lVal));
        case VT_BSTR: {
            char buf[200];
            WideCharToMultiByte(CP_UTF8, 0, v.bstrVal, -1, buf, sizeof(buf), NULL, NULL);
            return std::make_shared<ObjectString>(utf8string(buf));
        }
        case VT_DISPATCH:
            v.pdispVal->AddRef();
            return std::make_shared<ComObject>("IDispatch", v.pdispVal);
    }
    return nullptr;
}

bool ComObject::invokeMethod(const utf8string &methodname, const std::vector<std::shared_ptr<Object>> &args, std::shared_ptr<Object> &result) const
{
    if (obj == NULL) {
        return false; // TODO: Exception
    }
    OLECHAR wname[200];
    MultiByteToWideChar(CP_UTF8, 0, methodname.c_str(), -1, wname, sizeof(wname)/sizeof(wname[0]));
    OLECHAR *a = wname;
    DISPID dispid;
    HRESULT r = obj->GetIDsOfNames(IID_NULL, &a, 1, GetUserDefaultLCID(), &dispid);
    if (r != S_OK) {
        return false; // TODO: Exception
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
    // TODO: exceptions, argument errors
    r = obj->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD, &params, &rv, NULL, NULL);
    for (size_t i = 0; i < args.size(); i++) {
        VariantClear(&params.rgvarg[i]);
    }
    delete[] params.rgvarg;
    if (r != S_OK) {
        return false; // TODO: Exception
    }
    result = ObjectFromVariant(rv);
    VariantClear(&rv);
    return true;
}

bool ComObject::subscript(std::shared_ptr<Object> index, std::shared_ptr<Object> &result) const
{
    if (obj == NULL) {
        return false; // TODO: Exception
    }
    utf8string name;
    if (not index->getString(name)) {
        return false; // TODO: Exception
    }
    OLECHAR wname[200];
    MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, wname, sizeof(wname)/sizeof(wname[0]));
    OLECHAR *a = wname;
    DISPID dispid;
    HRESULT r = obj->GetIDsOfNames(IID_NULL, &a, 1, GetUserDefaultLCID(), &dispid);
    if (r != S_OK) {
        return false; // TODO: Exception
    }
    DISPPARAMS params = {NULL, NULL, 0, 0};
    VARIANT rv;
    VariantInit(&rv);
    // TODO: exceptions, argument errors
    r = obj->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_PROPERTYGET, &params, &rv, NULL, NULL);
    if (r != S_OK) {
        return false; // TODO: Exception
    }
    result = ObjectFromVariant(rv);
    VariantClear(&rv);
    return true;
}

namespace rtl {

namespace ne_runtime {

std::shared_ptr<Object> createObject(const utf8string &name)
{
    static bool com_init = false;
    if (!com_init) {
        HRESULT r = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        if (r == S_OK || r == S_FALSE) {
            com_init = true;
        } else {
            return nullptr; // TODO: Exception
        }
    }
    OLECHAR wname[200];
    MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, wname, sizeof(wname)/sizeof(wname[0]));
    CLSID clsid;
    HRESULT r = CLSIDFromProgID(wname, &clsid);
    if (r != S_OK) {
        return nullptr; // TODO: Exception
    }
    IDispatch *obj;
    r = CoCreateInstance(clsid, NULL, CLSCTX_ALL, IID_IDispatch, reinterpret_cast<void **>(&obj));
    if (r != S_OK) {
        return nullptr; // TODO: Exception
    }
    return std::make_shared<ComObject>(name.str(), obj);
}

} // namespace ne_runtime

} // namespace rtl
