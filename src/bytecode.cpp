#include "bytecode.h"

#include <assert.h>

Bytecode::Bytecode()
  : obj(),
    source_hash(),
    global_size(0),
    strtable(),
    types(),
    constants(),
    variables(),
    functions(),
    exception_exports(),
    imports(),
    exceptions(),
    code()
{
}

Bytecode::Bytecode(const std::vector<unsigned char> &obj)
  : obj(obj),
    source_hash(),
    global_size(0),
    strtable(),
    types(),
    constants(),
    variables(),
    functions(),
    exception_exports(),
    imports(),
    exceptions(),
    code()
{
    size_t i = 0;

    assert(obj.size() > 32);
    source_hash = std::string(&obj[i], &obj[i]+32);
    i += 32;

    global_size = (obj[i] << 8 | obj[i+1]);
    i += 2;

    unsigned int strtablesize = (obj[i] << 8) | obj[i+1];
    i += 2;
    strtable = getstrtable(&obj[i], &obj[i] + strtablesize, i);

    unsigned int typesize = (obj[i] << 8) | obj[i+1];
    i += 2;
    while (typesize > 0) {
        Type t;
        t.name = (obj[i] << 8) | obj[i+1];
        i += 2;
        t.descriptor = (obj[i] << 8) | obj[i+1];
        i += 2;
        types.push_back(t);
        typesize--;
    }

    unsigned int constantsize = (obj[i] << 8) | obj[i+1];
    i += 2;
    while (constantsize > 0) {
        Constant c;
        c.name = (obj[i] << 8) | obj[i+1];
        i += 2;
        c.type = (obj[i] << 8) | obj[i+1];
        i += 2;
        unsigned int size = (obj[i] << 8) | obj[i+1];
        i += 2;
        c.value = Bytes(&obj[i], &obj[i+size]);
        i += size;
        constants.push_back(c);
        constantsize--;
    }

    unsigned int variablesize = (obj[i] << 8) | obj[i+1];
    i += 2;
    while (variablesize > 0) {
        Variable v;
        v.name = (obj[i] << 8) | obj[i+1];
        i += 2;
        v.type = (obj[i] << 8) | obj[i+1];
        i += 2;
        v.index = (obj[i] << 8) | obj[i+1];
        i += 2;
        variables.push_back(v);
        variablesize--;
    }

    unsigned int functionsize = (obj[i] << 8) | obj[i+1];
    i += 2;
    while (functionsize > 0) {
        Function f;
        f.name = (obj[i] << 8) | obj[i+1];
        i += 2;
        f.descriptor = (obj[i] << 8) | obj[i+1];
        i += 2;
        f.entry = (obj[i] << 8) | obj[i+1];
        i += 2;
        functions.push_back(f);
        functionsize--;
    }

    unsigned int exceptionexportsize = (obj[i] << 8) | obj[i+1];
    i += 2;
    while (exceptionexportsize > 0) {
        ExceptionExport e;
        e.name = (obj[i] << 8) | obj[i+1];
        i += 2;
        exception_exports.push_back(e);
        exceptionexportsize--;
    }

    unsigned int importsize = (obj[i] << 8) | obj[i+1];
    i += 2;
    while (importsize > 0) {
        std::pair<unsigned int, std::string> imp;
        imp.first = (obj[i] << 8) | obj[i+1];
        i += 2;
        imp.second = std::string(&obj[i], &obj[i]+32);
        i += 32;
        imports.push_back(imp);
        importsize--;
    }

    unsigned int exceptionsize = (obj[i] << 8) | obj[i+1];
    i += 2;
    while (exceptionsize > 0) {
        ExceptionInfo e;
        e.start = (obj[i] << 8) | obj[i+1];
        i += 2;
        e.end = (obj[i] << 8) | obj[i+1];
        i += 2;
        e.excid = (obj[i] << 8) | obj[i+1];
        i += 2;
        e.handler = (obj[i] << 8) | obj[i+1];
        i += 2;
        exceptions.push_back(e);
        exceptionsize--;
    }

    code = Bytes(obj.begin() + i, obj.end());
}

std::vector<std::string> Bytecode::getstrtable(const unsigned char *start, const unsigned char *end, size_t &i)
{
    std::vector<std::string> r;
    while (start != end) {
        std::string s;
        while (*start != 0) {
            s.push_back(*start);
            ++start;
            i++;
        }
        r.push_back(s);
        ++start;
        i++;
    }
    return r;
}

Bytecode::Bytes Bytecode::getBytes() const
{
    std::vector<unsigned char> obj;

    assert(source_hash.length() == 32);
    for (int i = 0; i < 32; i++) {
        obj.push_back(source_hash[i]);
    }

    obj.push_back(static_cast<unsigned char>(global_size >> 8) & 0xff);
    obj.push_back(static_cast<unsigned char>(global_size & 0xff));

    std::vector<unsigned char> t;
    for (auto s: strtable) {
        std::copy(s.begin(), s.end(), std::back_inserter(t));
        t.push_back(0);
    }
    obj.push_back(static_cast<unsigned char>(t.size() >> 8) & 0xff);
    obj.push_back(static_cast<unsigned char>(t.size() & 0xff));
    std::copy(t.begin(), t.end(), std::back_inserter(obj));

    obj.push_back(static_cast<unsigned char>(types.size() >> 8) & 0xff);
    obj.push_back(static_cast<unsigned char>(types.size() & 0xff));
    for (auto t: types) {
        obj.push_back(static_cast<unsigned char>(t.name >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(t.name & 0xff));
        obj.push_back(static_cast<unsigned char>(t.descriptor >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(t.descriptor & 0xff));
    }

    obj.push_back(static_cast<unsigned char>(constants.size() >> 8) & 0xff);
    obj.push_back(static_cast<unsigned char>(constants.size() & 0xff));
    for (auto c: constants) {
        obj.push_back(static_cast<unsigned char>(c.name >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(c.name & 0xff));
        obj.push_back(static_cast<unsigned char>(c.type >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(c.type & 0xff));
        obj.push_back(static_cast<unsigned char>(c.value.size() >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(c.value.size() & 0xff));
        std::copy(c.value.begin(), c.value.end(), std::back_inserter(obj));
    }

    obj.push_back(static_cast<unsigned char>(variables.size() >> 8) & 0xff);
    obj.push_back(static_cast<unsigned char>(variables.size() & 0xff));
    for (auto v: variables) {
        obj.push_back(static_cast<unsigned char>(v.name >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(v.name & 0xff));
        obj.push_back(static_cast<unsigned char>(v.type >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(v.type & 0xff));
        obj.push_back(static_cast<unsigned char>(v.index >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(v.index & 0xff));
    }

    obj.push_back(static_cast<unsigned char>(functions.size() >> 8) & 0xff);
    obj.push_back(static_cast<unsigned char>(functions.size() & 0xff));
    for (auto f: functions) {
        obj.push_back(static_cast<unsigned char>(f.name >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(f.name & 0xff));
        obj.push_back(static_cast<unsigned char>(f.descriptor >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(f.descriptor & 0xff));
        obj.push_back(static_cast<unsigned char>(f.entry >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(f.entry & 0xff));
    }

    obj.push_back(static_cast<unsigned char>(exception_exports.size() >> 8) & 0xff);
    obj.push_back(static_cast<unsigned char>(exception_exports.size() & 0xff));
    for (auto e: exception_exports) {
        obj.push_back(static_cast<unsigned char>(e.name >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(e.name & 0xff));
    }

    obj.push_back(static_cast<unsigned char>(imports.size() >> 8) & 0xff);
    obj.push_back(static_cast<unsigned char>(imports.size() & 0xff));
    for (auto i: imports) {
        obj.push_back(static_cast<unsigned char>(i.first >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(i.first & 0xff));
        assert(i.second.length() == 32);
        for (int j = 0; j < 32; j++) {
            obj.push_back(i.second[j]);
        }
    }

    obj.push_back(static_cast<unsigned char>(exceptions.size() >> 8) & 0xff);
    obj.push_back(static_cast<unsigned char>(exceptions.size() & 0xff));
    for (auto e: exceptions) {
        obj.push_back(static_cast<unsigned char>(e.start >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(e.start & 0xff));
        obj.push_back(static_cast<unsigned char>(e.end >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(e.end & 0xff));
        obj.push_back(static_cast<unsigned char>(e.excid >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(e.excid & 0xff));
        obj.push_back(static_cast<unsigned char>(e.handler >> 8) & 0xff);
        obj.push_back(static_cast<unsigned char>(e.handler & 0xff));
    }

    std::copy(code.begin(), code.end(), std::back_inserter(obj));
    return obj;
}
