#include <windows.h>

#include "cell.h"
#include "exec.h"

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
    virtual utf8string toString() const { return utf8string("<ComObject:" + name + ">"); }
};

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

namespace ne_global {

std::shared_ptr<Object> object__invokeMethod(const std::shared_ptr<Object> &obj, const utf8string &name, std::vector<std::shared_ptr<Object>> /*args*/)
{
    ComObject *comobj = dynamic_cast<ComObject *>(obj.get());
    if (comobj == nullptr || comobj->obj == NULL) {
        return nullptr; // TODO: Exception
    }
    IDispatch *disp = comobj->obj;
    OLECHAR wname[200];
    MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, wname, sizeof(wname)/sizeof(wname[0]));
    OLECHAR *a = wname;
    DISPID dispid;
    HRESULT r = disp->GetIDsOfNames(IID_NULL, &a, 1, GetUserDefaultLCID(), &dispid);
    if (r != S_OK) {
        return nullptr; // TODO: Exception
    }
    DISPPARAMS params = {NULL, NULL, 0, 0};
    VARIANT result;
    VariantInit(&result);
    // TODO: exceptions, argument errors
    r = disp->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD, &params, &result, NULL, NULL);
    if (r != S_OK) {
        return nullptr; // TODO: Exception
    }
    return nullptr; // TODO: result
}

} // namespace ne_global

} // namespace rtl
