#include "bytecode.h"

#include <assert.h>
#include <stdint.h>

class BytecodeException: public std::exception {};

static void put_uint16(std::vector<unsigned char> &obj, uint16_t x)
{
    obj.push_back(static_cast<unsigned char>(x >> 8) & 0xff);
    obj.push_back(static_cast<unsigned char>(x & 0xff));
}

static void put_uint32(std::vector<unsigned char> &obj, uint32_t x)
{
    obj.push_back(static_cast<unsigned char>(x >> 24) & 0xff);
    obj.push_back(static_cast<unsigned char>(x >> 16) & 0xff);
    obj.push_back(static_cast<unsigned char>(x >> 8) & 0xff);
    obj.push_back(static_cast<unsigned char>(x & 0xff));
}

static uint16_t get_uint16(const std::vector<unsigned char> &obj, size_t &i)
{
    if (i+2 > obj.size()) {
        throw BytecodeException();
    }
    uint16_t r = (obj[i] << 8) | obj[i+1];
    i += 2;
    return r;
}

static uint32_t get_uint32(const std::vector<unsigned char> &obj, size_t &i)
{
    if (i+4 > obj.size()) {
        throw BytecodeException();
    }
    uint32_t r = (obj[i] << 24) | (obj[i+1] << 16) | (obj[i+2] << 8) | obj[i+3];
    i += 4;
    return r;
}

static std::vector<std::string> getstrtable(const unsigned char *start, const unsigned char *end)
{
    std::vector<std::string> r;
    while (start != end) {
        size_t len = (start[0] << 24) | (start[1] << 16) | (start[2] << 8) | start[3];
        start += 4;
        r.push_back(std::string(reinterpret_cast<const char *>(start), len));
        start += len;
    }
    return r;
}

Bytecode::Bytecode()
  : obj(),
    source_path(),
    source_hash(),
    global_size(0),
    strtable(),
    export_types(),
    export_constants(),
    export_variables(),
    export_functions(),
    export_exceptions(),
    export_interfaces(),
    imports(),
    functions(),
    exceptions(),
    classes(),
    code()
{
}

bool Bytecode::load(const std::string &a_source_path, const std::vector<unsigned char> &bytes)
{
    source_path = a_source_path;
    obj = bytes;

    size_t i = 0;

    try {
        if (obj.size() < 32) {
            throw BytecodeException();
        }
        source_hash = std::string(&obj[i], &obj[i]+32);
        i += 32;

        global_size = get_uint16(obj, i);

        unsigned int strtablesize = get_uint32(obj, i);
        if (i+strtablesize > obj.size()) {
            throw BytecodeException();
        }
        strtable = getstrtable(&obj[i], &obj[i] + strtablesize);
        i += strtablesize;

        unsigned int typesize = get_uint16(obj, i);
        while (typesize > 0) {
            Type t;
            t.name = get_uint16(obj, i);
            t.descriptor = get_uint16(obj, i);
            export_types.push_back(t);
            typesize--;
        }

        unsigned int constantsize = get_uint16(obj, i);
        while (constantsize > 0) {
            Constant c;
            c.name = get_uint16(obj, i);
            c.type = get_uint16(obj, i);
            unsigned int size = get_uint16(obj, i);
            if (i+size > obj.size()) {
                throw BytecodeException();
            }
            c.value = Bytes(&obj[i], &obj[i+size]);
            i += size;
            export_constants.push_back(c);
            constantsize--;
        }

        unsigned int variablesize = get_uint16(obj, i);
        while (variablesize > 0) {
            Variable v;
            v.name = get_uint16(obj, i);
            v.type = get_uint16(obj, i);
            v.index = get_uint16(obj, i);
            export_variables.push_back(v);
            variablesize--;
        }

        unsigned int functionsize = get_uint16(obj, i);
        while (functionsize > 0) {
            Function f;
            f.name = get_uint16(obj, i);
            f.descriptor = get_uint16(obj, i);
            f.entry = get_uint32(obj, i);
            export_functions.push_back(f);
            functionsize--;
        }

        unsigned int exceptionexportsize = get_uint16(obj, i);
        while (exceptionexportsize > 0) {
            ExceptionExport e;
            e.name = get_uint16(obj, i);
            export_exceptions.push_back(e);
            exceptionexportsize--;
        }

        unsigned int interfaceexportsize = get_uint16(obj, i);
        while (interfaceexportsize > 0) {
            Interface iface;
            iface.name = get_uint16(obj, i);
            unsigned int methoddescriptorsize = get_uint16(obj, i);
            while (methoddescriptorsize > 0) {
                std::pair<unsigned int, unsigned int> m;
                m.first = get_uint16(obj, i);
                m.second = get_uint16(obj, i);
                iface.method_descriptors.push_back(m);
                methoddescriptorsize--;
            }
            export_interfaces.push_back(iface);
            interfaceexportsize--;
        }

        unsigned int importsize = get_uint16(obj, i);
        while (importsize > 0) {
            std::pair<unsigned int, std::string> imp;
            imp.first = get_uint16(obj, i);
            if (i+32 > obj.size()) {
                throw BytecodeException();
            }
            imp.second = std::string(&obj[i], &obj[i]+32);
            i += 32;
            imports.push_back(imp);
            importsize--;
        }

        /*unsigned int*/ functionsize = get_uint16(obj, i);
        while (functionsize > 0) {
            FunctionInfo f;
            f.name = get_uint16(obj, i);
            f.entry = get_uint32(obj, i);
            functions.push_back(f);
            functionsize--;
        }

        unsigned int exceptionsize = get_uint16(obj, i);
        while (exceptionsize > 0) {
            ExceptionInfo e;
            e.start = get_uint16(obj, i);
            e.end = get_uint16(obj, i);
            e.excid = get_uint16(obj, i);
            e.handler = get_uint16(obj, i);
            exceptions.push_back(e);
            exceptionsize--;
        }

        unsigned int classsize = get_uint16(obj, i);
        while (classsize > 0) {
            ClassInfo c;
            c.name = get_uint16(obj, i);
            unsigned int interfacecount = get_uint16(obj, i);
            while (interfacecount > 0) {
                std::vector<unsigned int> methods;
                unsigned int methodcount = get_uint16(obj, i);
                while (methodcount > 0) {
                    methods.push_back(get_uint16(obj, i));
                    methodcount--;
                }
                c.interfaces.push_back(methods);
                interfacecount--;
            }
            classes.push_back(c);
            classsize--;
        }

        code = Bytes(obj.begin() + i, obj.end());

    } catch (BytecodeException &) {
        return false;
    }

    return true;
}

Bytecode::Bytes Bytecode::getBytes() const
{
    std::vector<unsigned char> objret;

    assert(source_hash.length() == 32);
    for (int i = 0; i < 32; i++) {
        objret.push_back(source_hash[i]);
    }

    put_uint16(objret, static_cast<uint16_t>(global_size));

    std::vector<unsigned char> strtable_flat;
    for (auto s: strtable) {
        put_uint32(strtable_flat, static_cast<uint32_t>(s.length()));
        std::copy(s.begin(), s.end(), std::back_inserter(strtable_flat));
    }
    put_uint32(objret, static_cast<uint32_t>(strtable_flat.size()));
    std::copy(strtable_flat.begin(), strtable_flat.end(), std::back_inserter(objret));

    put_uint16(objret, static_cast<uint16_t>(export_types.size()));
    for (auto t: export_types) {
        put_uint16(objret, static_cast<uint16_t>(t.name));
        put_uint16(objret, static_cast<uint16_t>(t.descriptor));
    }

    put_uint16(objret, static_cast<uint16_t>(export_constants.size()));
    for (auto c: export_constants) {
        put_uint16(objret, static_cast<uint16_t>(c.name));
        put_uint16(objret, static_cast<uint16_t>(c.type));
        put_uint16(objret, static_cast<uint16_t>(c.value.size()));
        std::copy(c.value.begin(), c.value.end(), std::back_inserter(objret));
    }

    put_uint16(objret, static_cast<uint16_t>(export_variables.size()));
    for (auto v: export_variables) {
        put_uint16(objret, static_cast<uint16_t>(v.name));
        put_uint16(objret, static_cast<uint16_t>(v.type));
        put_uint16(objret, static_cast<uint16_t>(v.index));
    }

    put_uint16(objret, static_cast<uint16_t>(export_functions.size()));
    for (auto f: export_functions) {
        put_uint16(objret, static_cast<uint16_t>(f.name));
        put_uint16(objret, static_cast<uint16_t>(f.descriptor));
        put_uint32(objret, static_cast<uint32_t>(f.entry));
    }

    put_uint16(objret, static_cast<uint16_t>(export_exceptions.size()));
    for (auto e: export_exceptions) {
        put_uint16(objret, static_cast<uint16_t>(e.name));
    }

    put_uint16(objret, static_cast<uint16_t>(export_interfaces.size()));
    for (auto i: export_interfaces) {
        put_uint16(objret, static_cast<uint16_t>(i.name));
        put_uint16(objret, static_cast<uint16_t>(i.method_descriptors.size()));
        for (auto m: i.method_descriptors) {
            put_uint16(objret, static_cast<uint16_t>(m.first));
            put_uint16(objret, static_cast<uint16_t>(m.second));
        }
    }

    put_uint16(objret, static_cast<uint16_t>(imports.size()));
    for (auto i: imports) {
        put_uint16(objret, static_cast<uint16_t>(i.first));
        assert(i.second.length() == 32);
        for (int j = 0; j < 32; j++) {
            objret.push_back(i.second[j]);
        }
    }

    put_uint16(objret, static_cast<uint16_t>(functions.size()));
    for (auto f: functions) {
        put_uint16(objret, static_cast<uint16_t>(f.name));
        put_uint32(objret, static_cast<uint32_t>(f.entry));
    }

    put_uint16(objret, static_cast<uint16_t>(exceptions.size()));
    for (auto e: exceptions) {
        put_uint16(objret, static_cast<uint16_t>(e.start));
        put_uint16(objret, static_cast<uint16_t>(e.end));
        put_uint16(objret, static_cast<uint16_t>(e.excid));
        put_uint16(objret, static_cast<uint16_t>(e.handler));
    }

    put_uint16(objret, static_cast<uint16_t>(classes.size()));
    for (auto c: classes) {
        put_uint16(objret, static_cast<uint16_t>(c.name));
        put_uint16(objret, static_cast<uint16_t>(c.interfaces.size()));
        for (auto i: c.interfaces) {
            put_uint16(objret, static_cast<uint16_t>(i.size()));
            for (auto m: i) {
                put_uint16(objret, static_cast<uint16_t>(m));
            }
        }
    }

    std::copy(code.begin(), code.end(), std::back_inserter(objret));
    return objret;
}
