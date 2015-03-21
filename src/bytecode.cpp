#include "bytecode.h"

#include <assert.h>

Bytecode::Bytecode()
  : source_hash(),
    global_size(0),
    strtable(),
    types(),
    constants(),
    variables(),
    functions(),
    exceptions(),
    code()
{
}

Bytecode::Bytecode(const std::vector<unsigned char> &obj)
  : source_hash(),
    global_size(0),
    strtable(),
    types(),
    constants(),
    variables(),
    functions(),
    exceptions(),
    code()
{
    size_t i = 0;

    source_hash = std::string(&obj[i], &obj[i]+32);
    i += 32;

    global_size = (obj[i] << 8 | obj[i+1]);
    i += 2;
    unsigned int strtablesize = (obj[i] << 8) | obj[i+1];
    i += 2;
    strtable = getstrtable(&obj[i], &obj[i] + strtablesize, i);

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
