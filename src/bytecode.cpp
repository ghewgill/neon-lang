#include "bytecode.h"

#include <assert.h>

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
    assert(i+2 <= obj.size());
    uint16_t r = (obj[i] << 8) | obj[i+1];
    i += 2;
    return r;
}

static uint32_t get_uint32(const std::vector<unsigned char> &obj, size_t &i)
{
    assert(i+4 <= obj.size());
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

    global_size = get_uint16(obj, i);

    unsigned int strtablesize = get_uint32(obj, i);
    assert(i+strtablesize <= obj.size());
    strtable = getstrtable(&obj[i], &obj[i] + strtablesize);
    i += strtablesize;

    unsigned int typesize = get_uint16(obj, i);
    while (typesize > 0) {
        Type t;
        t.name = get_uint16(obj, i);
        t.descriptor = get_uint16(obj, i);
        types.push_back(t);
        typesize--;
    }

    unsigned int constantsize = get_uint16(obj, i);
    while (constantsize > 0) {
        Constant c;
        c.name = get_uint16(obj, i);
        c.type = get_uint16(obj, i);
        unsigned int size = get_uint16(obj, i);
        assert(i+size <= obj.size());
        c.value = Bytes(&obj[i], &obj[i+size]);
        i += size;
        constants.push_back(c);
        constantsize--;
    }

    unsigned int variablesize = get_uint16(obj, i);
    while (variablesize > 0) {
        Variable v;
        v.name = get_uint16(obj, i);
        v.type = get_uint16(obj, i);
        v.index = get_uint16(obj, i);
        variables.push_back(v);
        variablesize--;
    }

    unsigned int functionsize = get_uint16(obj, i);
    while (functionsize > 0) {
        Function f;
        f.name = get_uint16(obj, i);
        f.descriptor = get_uint16(obj, i);
        f.entry = get_uint16(obj, i);
        functions.push_back(f);
        functionsize--;
    }

    unsigned int exceptionexportsize = get_uint16(obj, i);
    while (exceptionexportsize > 0) {
        ExceptionExport e;
        e.name = get_uint16(obj, i);
        exception_exports.push_back(e);
        exceptionexportsize--;
    }

    unsigned int importsize = get_uint16(obj, i);
    while (importsize > 0) {
        std::pair<unsigned int, std::string> imp;
        imp.first = get_uint16(obj, i);
        assert(i+32 <= obj.size());
        imp.second = std::string(&obj[i], &obj[i]+32);
        i += 32;
        imports.push_back(imp);
        importsize--;
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

    code = Bytes(obj.begin() + i, obj.end());
}

Bytecode::Bytes Bytecode::getBytes() const
{
    std::vector<unsigned char> obj;

    assert(source_hash.length() == 32);
    for (int i = 0; i < 32; i++) {
        obj.push_back(source_hash[i]);
    }

    put_uint16(obj, global_size);

    std::vector<unsigned char> t;
    for (auto s: strtable) {
        put_uint32(t, s.length());
        std::copy(s.begin(), s.end(), std::back_inserter(t));
    }
    put_uint32(obj, t.size());
    std::copy(t.begin(), t.end(), std::back_inserter(obj));

    put_uint16(obj, types.size());
    for (auto t: types) {
        put_uint16(obj, t.name);
        put_uint16(obj, t.descriptor);
    }

    put_uint16(obj, constants.size());
    for (auto c: constants) {
        put_uint16(obj, c.name);
        put_uint16(obj, c.type);
        put_uint16(obj, c.value.size());
        std::copy(c.value.begin(), c.value.end(), std::back_inserter(obj));
    }

    put_uint16(obj, variables.size());
    for (auto v: variables) {
        put_uint16(obj, v.name);
        put_uint16(obj, v.type);
        put_uint16(obj, v.index);
    }

    put_uint16(obj, functions.size());
    for (auto f: functions) {
        put_uint16(obj, f.name);
        put_uint16(obj, f.descriptor);
        put_uint16(obj, f.entry);
    }

    put_uint16(obj, exception_exports.size());
    for (auto e: exception_exports) {
        put_uint16(obj, e.name);
    }

    put_uint16(obj, imports.size());
    for (auto i: imports) {
        put_uint16(obj, i.first);
        assert(i.second.length() == 32);
        for (int j = 0; j < 32; j++) {
            obj.push_back(i.second[j]);
        }
    }

    put_uint16(obj, exceptions.size());
    for (auto e: exceptions) {
        put_uint16(obj, e.start);
        put_uint16(obj, e.end);
        put_uint16(obj, e.excid);
        put_uint16(obj, e.handler);
    }

    std::copy(code.begin(), code.end(), std::back_inserter(obj));
    return obj;
}
