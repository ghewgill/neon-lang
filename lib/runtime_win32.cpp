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
    virtual bool invokeMethod(const utf8string &name, const std::vector<std::shared_ptr<Object>> &args, std::shared_ptr<Object> &result) const override;
    virtual bool subscript(std::shared_ptr<Object>, std::shared_ptr<Object> &) const override;
    virtual utf8string toString() const { return utf8string("<ComObject:" + name + ">"); }
};

bool ComObject::invokeMethod(const utf8string &name, const std::vector<std::shared_ptr<Object>> &args, std::shared_ptr<Object> &result) const
{
    if (obj == NULL) {
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
    r = obj->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD, &params, &rv, NULL, NULL);
    if (r != S_OK) {
        return false; // TODO: Exception
    }
    return true;
}

bool ComObject::subscript(std::shared_ptr<Object> index, std::shared_ptr<Object> &result) const
{
    if (obj == NULL) {
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
    switch (rv.vt) {
        case VT_BSTR: {
            char buf[200];
            WideCharToMultiByte(CP_UTF8, 0, rv.bstrVal, -1, buf, sizeof(buf), NULL, NULL);
            result = std::make_shared<ObjectString>(utf8string(buf));
            break;
        }
        default:
            return false; // TODO: Exception
    }
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
