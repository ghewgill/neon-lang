#include "bytecode.h"

#include <assert.h>
#include <iterator>
#include <stdint.h>

class BytecodeException: public std::exception {};

void Bytecode::put_vint(std::vector<unsigned char> &obj, unsigned int x)
{
    std::vector<unsigned char> t;
    unsigned char flag = 0x00;
    do {
        t.push_back((x & 0x7f) | flag);
        flag = 0x80;
        x >>= 7;
    } while (x != 0);
    std::copy(t.rbegin(), t.rend(), std::back_inserter(obj));
}

void Bytecode::put_vint(std::vector<unsigned char> &obj, unsigned int x, size_t width)
{
    std::vector<unsigned char> r;
    put_vint(r, x);
    assert(r.size() <= width);
    size_t s = r.size();
    while (s < width) {
        obj.push_back(0x80);
        s++;
    }
    std::copy(r.begin(), r.end(), std::back_inserter(obj));
}

void Bytecode::put_vint(std::vector<unsigned char> &obj, size_t x)
{
    put_vint(obj, static_cast<unsigned int>(x));
}

unsigned int Bytecode::get_vint(const std::vector<unsigned char> &obj, size_t &i)
{
    unsigned int r = 0;
    while (i < obj.size()) {
        unsigned int x = obj[i];
        i++;
        r = (r << 7) | (x & 0x7f);
        if ((x & 0x80) == 0) {
            break;
        }
    }
    return r;
}

static std::vector<std::string> getstrtable(const std::vector<unsigned char> &obj, size_t start, size_t size)
{
    size_t i = 0;
    std::vector<std::string> r;
    while (i < size) {
        size_t j = start + i;
        size_t len = Bytecode::get_vint(obj, j);
        i = j - start;
        r.push_back(std::string(reinterpret_cast<const char *>(&obj[start+i]), len));
        i += len;
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

        global_size = get_vint(obj, i);

        unsigned int strtablesize = get_vint(obj, i);
        if (i+strtablesize > obj.size()) {
            throw BytecodeException();
        }
        strtable = getstrtable(obj, i, strtablesize);
        i += strtablesize;

        unsigned int typesize = get_vint(obj, i);
        while (typesize > 0) {
            Type t;
            t.name = get_vint(obj, i);
            t.descriptor = get_vint(obj, i);
            export_types.push_back(t);
            typesize--;
        }

        unsigned int constantsize = get_vint(obj, i);
        while (constantsize > 0) {
            Constant c;
            c.name = get_vint(obj, i);
            c.type = get_vint(obj, i);
            unsigned int size = get_vint(obj, i);
            if (i+size > obj.size()) {
                throw BytecodeException();
            }
            c.value = Bytes(&obj[i], &obj[i+size]);
            i += size;
            export_constants.push_back(c);
            constantsize--;
        }

        unsigned int variablesize = get_vint(obj, i);
        while (variablesize > 0) {
            Variable v;
            v.name = get_vint(obj, i);
            v.type = get_vint(obj, i);
            v.index = get_vint(obj, i);
            export_variables.push_back(v);
            variablesize--;
        }

        unsigned int functionsize = get_vint(obj, i);
        while (functionsize > 0) {
            Function f;
            f.name = get_vint(obj, i);
            f.descriptor = get_vint(obj, i);
            f.entry = get_vint(obj, i);
            export_functions.push_back(f);
            functionsize--;
        }

        unsigned int exceptionexportsize = get_vint(obj, i);
        while (exceptionexportsize > 0) {
            ExceptionExport e;
            e.name = get_vint(obj, i);
            export_exceptions.push_back(e);
            exceptionexportsize--;
        }

        unsigned int interfaceexportsize = get_vint(obj, i);
        while (interfaceexportsize > 0) {
            Interface iface;
            iface.name = get_vint(obj, i);
            unsigned int methoddescriptorsize = get_vint(obj, i);
            while (methoddescriptorsize > 0) {
                std::pair<unsigned int, unsigned int> m;
                m.first = get_vint(obj, i);
                m.second = get_vint(obj, i);
                iface.method_descriptors.push_back(m);
                methoddescriptorsize--;
            }
            export_interfaces.push_back(iface);
            interfaceexportsize--;
        }

        unsigned int importsize = get_vint(obj, i);
        while (importsize > 0) {
            std::pair<unsigned int, std::string> imp;
            imp.first = get_vint(obj, i);
            if (i+32 > obj.size()) {
                throw BytecodeException();
            }
            imp.second = std::string(&obj[i], &obj[i]+32);
            i += 32;
            imports.push_back(imp);
            importsize--;
        }

        /*unsigned int*/ functionsize = get_vint(obj, i);
        while (functionsize > 0) {
            FunctionInfo f;
            f.name = get_vint(obj, i);
            f.entry = get_vint(obj, i);
            functions.push_back(f);
            functionsize--;
        }

        unsigned int exceptionsize = get_vint(obj, i);
        while (exceptionsize > 0) {
            ExceptionInfo e;
            e.start = get_vint(obj, i);
            e.end = get_vint(obj, i);
            e.excid = get_vint(obj, i);
            e.handler = get_vint(obj, i);
            exceptions.push_back(e);
            exceptionsize--;
        }

        unsigned int classsize = get_vint(obj, i);
        while (classsize > 0) {
            ClassInfo c;
            c.name = get_vint(obj, i);
            unsigned int interfacecount = get_vint(obj, i);
            while (interfacecount > 0) {
                std::vector<unsigned int> methods;
                unsigned int methodcount = get_vint(obj, i);
                while (methodcount > 0) {
                    methods.push_back(get_vint(obj, i));
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

    put_vint(objret, global_size);

    std::vector<unsigned char> strtable_flat;
    for (auto s: strtable) {
        put_vint(strtable_flat, s.length());
        std::copy(s.begin(), s.end(), std::back_inserter(strtable_flat));
    }
    put_vint(objret, strtable_flat.size());
    std::copy(strtable_flat.begin(), strtable_flat.end(), std::back_inserter(objret));

    put_vint(objret, export_types.size());
    for (auto t: export_types) {
        put_vint(objret, t.name);
        put_vint(objret, t.descriptor);
    }

    put_vint(objret, export_constants.size());
    for (auto c: export_constants) {
        put_vint(objret, c.name);
        put_vint(objret, c.type);
        put_vint(objret, c.value.size());
        std::copy(c.value.begin(), c.value.end(), std::back_inserter(objret));
    }

    put_vint(objret, export_variables.size());
    for (auto v: export_variables) {
        put_vint(objret, v.name);
        put_vint(objret, v.type);
        put_vint(objret, v.index);
    }

    put_vint(objret, export_functions.size());
    for (auto f: export_functions) {
        put_vint(objret, f.name);
        put_vint(objret, f.descriptor);
        put_vint(objret, f.entry);
    }

    put_vint(objret, export_exceptions.size());
    for (auto e: export_exceptions) {
        put_vint(objret, e.name);
    }

    put_vint(objret, export_interfaces.size());
    for (auto i: export_interfaces) {
        put_vint(objret, i.name);
        put_vint(objret, i.method_descriptors.size());
        for (auto m: i.method_descriptors) {
            put_vint(objret, m.first);
            put_vint(objret, m.second);
        }
    }

    put_vint(objret, imports.size());
    for (auto i: imports) {
        put_vint(objret, i.first);
        assert(i.second.length() == 32);
        for (int j = 0; j < 32; j++) {
            objret.push_back(i.second[j]);
        }
    }

    put_vint(objret, functions.size());
    for (auto f: functions) {
        put_vint(objret, f.name);
        put_vint(objret, f.entry);
    }

    put_vint(objret, exceptions.size());
    for (auto e: exceptions) {
        put_vint(objret, e.start);
        put_vint(objret, e.end);
        put_vint(objret, e.excid);
        put_vint(objret, e.handler);
    }

    put_vint(objret, classes.size());
    for (auto c: classes) {
        put_vint(objret, c.name);
        put_vint(objret, c.interfaces.size());
        for (auto i: c.interfaces) {
            put_vint(objret, i.size());
            for (auto m: i) {
                put_vint(objret, m);
            }
        }
    }

    std::copy(code.begin(), code.end(), std::back_inserter(objret));
    return objret;
}
