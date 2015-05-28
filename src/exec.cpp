#include "exec.h"

#include <algorithm>
#include <assert.h>
#include <iso646.h>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <stdlib.h>

#include <ffi.h>

#include "bytecode.h"
#include "cell.h"
#include "debuginfo.h"
#include "number.h"
#include "opcode.h"
#include "opstack.h"
#include "rtl_exec.h"
#include "rtl_platform.h"
#include "support.h"

namespace {

std::vector<std::string> split(const std::string &s, char d)
{
    std::vector<std::string> r;
    if (not s.empty()) {
        std::string::size_type i = 0;
        auto start = i;
        while (i < s.length()) {
            if (s.at(i) == d) {
                r.push_back(s.substr(start, i-start));
                start = i + 1;
            }
            i++;
        }
        r.push_back(s.substr(start, i-start));
    }
    return r;
}

} // namespace

class ExternalCallInfo {
public:
    typedef void (*marshal_f)(Cell &cell, void *&p, size_t &space);
    typedef Cell (*unmarshal_f)(void *p);
    ExternalCallInfo(): types(), marshal(), unmarshal(nullptr), cif(), fp(nullptr) {}
    std::vector<ffi_type *> types;
    std::vector<marshal_f> marshal;
    unmarshal_f unmarshal;
    ffi_cif cif;
    void_function_t fp;
};

#ifdef _MSC_VER
#define alignof(T) sizeof(T)
#endif

// This implementation of std::align from https://code.google.com/p/c-plus/source/browse/src/util.h
// is provided for compilers that don't have this C++11 function yet.
static void *align(std::size_t alignment, std::size_t size, void *&ptr, std::size_t &space)
{
    auto pn = reinterpret_cast< std::size_t >( ptr );
    auto aligned = ( pn + alignment - 1 ) & - static_cast<long>(alignment);
    auto new_space = space - ( aligned - pn );
    if ( new_space < size ) return nullptr;
    space = new_space;
    return ptr = reinterpret_cast< void * >( aligned );
}

template <typename T> struct number_conversion {
    static T convert_to_native(Number);
    static Number convert_from_native(T);
};

template <> struct number_conversion<uint8_t > { static uint8_t  convert_to_native(Number x) { return number_to_uint8 (x); } static Number convert_from_native(uint8_t  x) { return number_from_uint8 (x); } };
template <> struct number_conversion< int8_t > { static  int8_t  convert_to_native(Number x) { return number_to_sint8 (x); } static Number convert_from_native( int8_t  x) { return number_from_sint8 (x); } };
template <> struct number_conversion<uint16_t> { static uint16_t convert_to_native(Number x) { return number_to_uint16(x); } static Number convert_from_native(uint16_t x) { return number_from_uint16(x); } };
template <> struct number_conversion< int16_t> { static  int16_t convert_to_native(Number x) { return number_to_sint16(x); } static Number convert_from_native( int16_t x) { return number_from_sint16(x); } };
template <> struct number_conversion<uint32_t> { static uint32_t convert_to_native(Number x) { return number_to_uint32(x); } static Number convert_from_native(uint32_t x) { return number_from_uint32(x); } };
template <> struct number_conversion< int32_t> { static  int32_t convert_to_native(Number x) { return number_to_sint32(x); } static Number convert_from_native( int32_t x) { return number_from_sint32(x); } };
template <> struct number_conversion<uint64_t> { static uint64_t convert_to_native(Number x) { return number_to_uint64(x); } static Number convert_from_native(uint64_t x) { return number_from_uint64(x); } };
template <> struct number_conversion< int64_t> { static  int64_t convert_to_native(Number x) { return number_to_sint64(x); } static Number convert_from_native( int64_t x) { return number_from_sint64(x); } };
template <> struct number_conversion<float   > { static float    convert_to_native(Number x) { return number_to_float (x); } static Number convert_from_native(float    x) { return number_from_float (x); } };
template <> struct number_conversion<double  > { static double   convert_to_native(Number x) { return number_to_double(x); } static Number convert_from_native(double   x) { return number_from_double(x); } };

template <typename T> static void marshal_number(Cell &cell, void *&p, size_t &space)
{
    T *a = reinterpret_cast<T *>(align(alignof(T), sizeof(T), p, space));
    *a = number_conversion<T>::convert_to_native(cell.number());
    p = a + 1;
    space -= sizeof(T);
}

static void marshal_pointer(Cell &cell, void *&p, size_t &space)
{
    void **a = reinterpret_cast<void **>(align(alignof(void *), sizeof(void *), p, space));
    *a = const_cast<char *>(cell.string().data());
    p = a + 1;
    space -= sizeof(void *);
}

static void marshal_pointer_a(Cell &cell, void *&p, size_t &space)
{
    void **a = reinterpret_cast<void **>(align(alignof(void *), sizeof(void *), p, space));
    *a = const_cast<char *>(cell.address()->string().data());
    p = a + 1;
    space -= sizeof(void *);
}

template <typename T> static Cell unmarshal_number(void *p)
{
    return Cell(number_conversion<T>::convert_from_native(*reinterpret_cast<T *>(p)));
}

class ActivationFrame {
public:
    ActivationFrame(size_t count): locals(count) {}
    // TODO (for nested functions) std::vector<ActivationFrame *> outer;
    std::vector<Cell> locals;
};

class Executor;

class Module {
public:
    Module(const std::string &name, const Bytecode &object, const DebugInfo *debuginfo, Executor *executor, ICompilerSupport *support);
    const std::string name;
    Bytecode object;
    const DebugInfo *debug;
    std::vector<Cell> globals;
    std::vector<size_t> rtl_call_tokens;
    std::vector<ExternalCallInfo *> external_functions;
private:
    Module(const Module &);
    Module &operator=(const Module &);
};

class Executor {
public:
    Executor(const std::string &source_path, const Bytecode::Bytes &bytes, const DebugInfo *debuginfo, ICompilerSupport *support);
    void exec();
private:
    const std::string source_path;
    std::map<std::string, Module *> modules;
    std::vector<std::string> init_order;
    Module *module;
    Bytecode::Bytes::size_type ip;
    opstack<Cell> stack;
    std::vector<std::pair<Module *, Bytecode::Bytes::size_type>> callstack;
    std::list<ActivationFrame> frames;

    void exec_ENTER();
    void exec_LEAVE();
    void exec_PUSHB();
    void exec_PUSHN();
    void exec_PUSHS();
    void exec_PUSHPG();
    void exec_PUSHPMG();
    void exec_PUSHPL();
    void exec_PUSHPOL();
    void exec_LOADB();
    void exec_LOADN();
    void exec_LOADS();
    void exec_LOADA();
    void exec_LOADD();
    void exec_LOADP();
    void exec_STOREB();
    void exec_STOREN();
    void exec_STORES();
    void exec_STOREA();
    void exec_STORED();
    void exec_STOREP();
    void exec_NEGN();
    void exec_ADDN();
    void exec_SUBN();
    void exec_MULN();
    void exec_DIVN();
    void exec_MODN();
    void exec_EXPN();
    void exec_EQB();
    void exec_NEB();
    void exec_EQN();
    void exec_NEN();
    void exec_LTN();
    void exec_GTN();
    void exec_LEN();
    void exec_GEN();
    void exec_EQS();
    void exec_NES();
    void exec_LTS();
    void exec_GTS();
    void exec_LES();
    void exec_GES();
    void exec_EQA();
    void exec_NEA();
    void exec_EQD();
    void exec_NED();
    void exec_EQP();
    void exec_NEP();
    void exec_ANDB();
    void exec_ORB();
    void exec_NOTB();
    void exec_INDEXAR();
    void exec_INDEXAW();
    void exec_INDEXAV();
    void exec_INDEXAN();
    void exec_INDEXDR();
    void exec_INDEXDW();
    void exec_INDEXDV();
    void exec_INA();
    void exec_IND();
    void exec_CALLP();
    void exec_CALLF();
    void exec_CALLMF();
    void exec_JUMP();
    void exec_JF();
    void exec_JT();
    void exec_JFCHAIN();
    void exec_DUP();
    void exec_DUPX1();
    void exec_DROP();
    void exec_RET();
    void exec_CALLE();
    void exec_CONSA();
    void exec_CONSD();
    void exec_EXCEPT();
    void exec_CLREXC();
    void exec_ALLOC();
    void exec_PUSHNIL();

    void raise(const utf8string &exception, const utf8string &info);

    friend class Module;
private:
    Executor(const Executor &);
    Executor &operator=(const Executor &);
};

Executor::Executor(const std::string &source_path, const Bytecode::Bytes &bytes, const DebugInfo *debuginfo, ICompilerSupport *support)
  : source_path(source_path),
    modules(),
    init_order(),
    module(nullptr),
    ip(0),
    stack(),
    callstack(),
    frames()
{
    module = new Module(source_path, Bytecode(bytes), debuginfo, this, support);
    modules[""] = module;
}

Module::Module(const std::string &name, const Bytecode &object, const DebugInfo *debuginfo, Executor *executor, ICompilerSupport *support)
  : name(name),
    object(object),
    debug(debuginfo),
    globals(object.global_size),
    rtl_call_tokens(object.strtable.size(), SIZE_MAX),
    external_functions()
{
    for (auto i: object.imports) {
        std::string name = object.strtable[i.first];
        if (executor->modules.find(name) != executor->modules.end()) {
            continue;
        }
        Bytecode code;
        if (not support->loadBytecode(name, code)) {
            fprintf(stderr, "couldn't load module: %s\n", name.c_str());
            exit(1);
        }
        // TODO: check hash of exports
        executor->init_order.push_back(name);
        executor->modules[name] = nullptr; // Prevent unwanted recursion.
        executor->modules[name] = new Module(name, code, nullptr, executor, support);
    }
}

void Executor::exec_ENTER()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    frames.push_back(ActivationFrame(val));
}

void Executor::exec_LEAVE()
{
    frames.pop_back();
    ip++;
}

void Executor::exec_PUSHB()
{
    bool val = module->object.code[ip+1] != 0;
    ip += 2;
    stack.push(Cell(val));
}

void Executor::exec_PUSHN()
{
    // TODO: endian
    Number val = *reinterpret_cast<const Number *>(&module->object.code[ip+1]);
    ip += 1 + sizeof(val);
    stack.push(Cell(val));
}

void Executor::exec_PUSHS()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    stack.push(Cell(module->object.strtable[val]));
}

void Executor::exec_PUSHPG()
{
    uint32_t addr = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    assert(addr < module->globals.size());
    stack.push(Cell(&module->globals.at(addr)));
}

void Executor::exec_PUSHPMG()
{
    ip++;
    uint32_t mod = (module->object.code[ip] << 24) | (module->object.code[ip+1] << 16) | (module->object.code[ip+2] << 8) | module->object.code[ip+3];
    ip += 4;
    uint32_t addr = (module->object.code[ip] << 24) | (module->object.code[ip+1] << 16) | (module->object.code[ip+2] << 8) | module->object.code[ip+3];
    ip += 4;
    auto m = modules.find(module->object.strtable[mod]);
    if (m == modules.end()) {
        fprintf(stderr, "fatal: module not found: %s\n", module->object.strtable[mod].c_str());
        exit(1);
    }
    assert(addr < m->second->globals.size());
    stack.push(Cell(&m->second->globals.at(addr)));
}

void Executor::exec_PUSHPL()
{
    uint32_t addr = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    stack.push(Cell(&frames.back().locals.at(addr)));
}

void Executor::exec_PUSHPOL()
{
    fprintf(stderr, "unimplemented: PUSHPOL\n");
    exit(1);
    /*
    ip++;
    uint32_t enclosing = (module->object.code[ip] << 24) | (module->object.code[ip+1] << 16) | (module->object.code[ip+2] << 8) | module->object.code[ip+3];
    ip += 4;
    uint32_t addr = (module->object.code[ip] << 24) | (module->object.code[ip+1] << 16) | (module->object.code[ip+2] << 8) | module->object.code[ip+3];
    ip += 4;
    stack.push(Cell(&frames[frames.size()-1-enclosing].locals.at(addr)));
    */
}

void Executor::exec_LOADB()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    stack.push(Cell(addr->boolean()));
}

void Executor::exec_LOADN()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    stack.push(Cell(addr->number()));
}

void Executor::exec_LOADS()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    addr->string();
    stack.push(Cell(*addr));
}

void Executor::exec_LOADA()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    addr->array();
    stack.push(Cell(*addr));
}

void Executor::exec_LOADD()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    addr->dictionary();
    stack.push(Cell(*addr));
}

void Executor::exec_LOADP()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    stack.push(Cell(addr->address()));
}

void Executor::exec_STOREB()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    bool val = stack.top().boolean(); stack.pop();
    *addr = Cell(val);
}

void Executor::exec_STOREN()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    Number val = stack.top().number(); stack.pop();
    *addr = Cell(val);
}

void Executor::exec_STORES()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    *addr = stack.top(); stack.pop();
    addr->string();
}

void Executor::exec_STOREA()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    *addr = stack.top(); stack.pop();
    addr->array();
}

void Executor::exec_STORED()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    *addr = stack.top(); stack.pop();
    addr->dictionary();
}

void Executor::exec_STOREP()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    Cell *val = stack.top().address(); stack.pop();
    *addr = Cell(val);
}

void Executor::exec_NEGN()
{
    ip++;
    Number x = stack.top().number(); stack.pop();
    stack.push(Cell(number_negate(x)));
}

void Executor::exec_ADDN()
{
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    stack.push(Cell(number_add(a, b)));
}

void Executor::exec_SUBN()
{
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    stack.push(Cell(number_subtract(a, b)));
}

void Executor::exec_MULN()
{
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    stack.push(Cell(number_multiply(a, b)));
}

void Executor::exec_DIVN()
{
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    if (number_is_zero(b)) {
        raise("DivideByZero", "");
        return;
    }
    stack.push(Cell(number_divide(a, b)));
}

void Executor::exec_MODN()
{
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    if (number_is_zero(b)) {
        raise("DivideByZero", "");
        return;
    }
    stack.push(Cell(number_modulo(a, b)));
}

void Executor::exec_EXPN()
{
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    stack.push(Cell(number_pow(a, b)));
}

void Executor::exec_EQB()
{
    ip++;
    bool b = stack.top().boolean(); stack.pop();
    bool a = stack.top().boolean(); stack.pop();
    stack.push(Cell(a == b));
}

void Executor::exec_NEB()
{
    ip++;
    bool b = stack.top().boolean(); stack.pop();
    bool a = stack.top().boolean(); stack.pop();
    stack.push(Cell(a != b));
}

void Executor::exec_EQN()
{
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    stack.push(Cell(number_is_equal(a, b)));
}

void Executor::exec_NEN()
{
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    stack.push(Cell(number_is_not_equal(a, b)));
}

void Executor::exec_LTN()
{
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    stack.push(Cell(number_is_less(a, b)));
}

void Executor::exec_GTN()
{
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    stack.push(Cell(number_is_greater(a, b)));
}

void Executor::exec_LEN()
{
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    stack.push(Cell(number_is_less_equal(a, b)));
}

void Executor::exec_GEN()
{
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    stack.push(Cell(number_is_greater_equal(a, b)));
}

void Executor::exec_EQS()
{
    ip++;
    utf8string b = stack.top().string(); stack.pop();
    utf8string a = stack.top().string(); stack.pop();
    stack.push(Cell(a == b));
}

void Executor::exec_NES()
{
    ip++;
    utf8string b = stack.top().string(); stack.pop();
    utf8string a = stack.top().string(); stack.pop();
    stack.push(Cell(a != b));
}

void Executor::exec_LTS()
{
    ip++;
    utf8string b = stack.top().string(); stack.pop();
    utf8string a = stack.top().string(); stack.pop();
    stack.push(Cell(a < b));
}

void Executor::exec_GTS()
{
    ip++;
    utf8string b = stack.top().string(); stack.pop();
    utf8string a = stack.top().string(); stack.pop();
    stack.push(Cell(a > b));
}

void Executor::exec_LES()
{
    ip++;
    utf8string b = stack.top().string(); stack.pop();
    utf8string a = stack.top().string(); stack.pop();
    stack.push(Cell(a <= b));
}

void Executor::exec_GES()
{
    ip++;
    utf8string b = stack.top().string(); stack.pop();
    utf8string a = stack.top().string(); stack.pop();
    stack.push(Cell(a >= b));
}

void Executor::exec_EQA()
{
    ip++;
    std::vector<Cell> b = stack.top().array(); stack.pop();
    std::vector<Cell> a = stack.top().array(); stack.pop();
    stack.push(Cell(a == b));
}

void Executor::exec_NEA()
{
    ip++;
    std::vector<Cell> b = stack.top().array(); stack.pop();
    std::vector<Cell> a = stack.top().array(); stack.pop();
    stack.push(Cell(a != b));
}

void Executor::exec_EQD()
{
    ip++;
    std::map<utf8string, Cell> b = stack.top().dictionary(); stack.pop();
    std::map<utf8string, Cell> a = stack.top().dictionary(); stack.pop();
    stack.push(Cell(a == b));
}

void Executor::exec_NED()
{
    ip++;
    std::map<utf8string, Cell> b = stack.top().dictionary(); stack.pop();
    std::map<utf8string, Cell> a = stack.top().dictionary(); stack.pop();
    stack.push(Cell(a != b));
}

void Executor::exec_EQP()
{
    ip++;
    Cell *b = stack.top().address(); stack.pop();
    Cell *a = stack.top().address(); stack.pop();
    stack.push(Cell(a == b));
}

void Executor::exec_NEP()
{
    ip++;
    Cell *b = stack.top().address(); stack.pop();
    Cell *a = stack.top().address(); stack.pop();
    stack.push(Cell(a != b));
}

void Executor::exec_ANDB()
{
    ip++;
    bool b = stack.top().boolean(); stack.pop();
    bool a = stack.top().boolean(); stack.pop();
    stack.push(Cell(a && b));
}

void Executor::exec_ORB()
{
    ip++;
    bool b = stack.top().boolean(); stack.pop();
    bool a = stack.top().boolean(); stack.pop();
    stack.push(Cell(a || b));
}

void Executor::exec_NOTB()
{
    ip++;
    bool x = stack.top().boolean(); stack.pop();
    stack.push(Cell(not x));
}

void Executor::exec_INDEXAR()
{
    ip++;
    Number index = stack.top().number(); stack.pop();
    Cell *addr = stack.top().address(); stack.pop();
    if (not number_is_integer(index)) {
        raise("ArrayIndex", number_to_string(index));
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        raise("ArrayIndex", std::to_string(i));
    }
    uint64_t j = static_cast<uint64_t>(i);
    if (j >= addr->array().size()) {
        raise("ArrayIndex", std::to_string(j));
        return;
    }
    stack.push(Cell(&addr->array_index_for_read(j)));
}

void Executor::exec_INDEXAW()
{
    ip++;
    Number index = stack.top().number(); stack.pop();
    Cell *addr = stack.top().address(); stack.pop();
    if (not number_is_integer(index)) {
        raise("ArrayIndex", number_to_string(index));
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        raise("ArrayIndex", std::to_string(i));
    }
    uint64_t j = static_cast<uint64_t>(i);
    stack.push(Cell(&addr->array_index_for_write(j)));
}

void Executor::exec_INDEXAV()
{
    ip++;
    Number index = stack.top().number(); stack.pop();
    const std::vector<Cell> &array = stack.top().array();
    if (not number_is_integer(index)) {
        raise("ArrayIndex", number_to_string(index));
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        raise("ArrayIndex", std::to_string(i));
    }
    uint64_t j = static_cast<uint64_t>(i);
    if (j >= array.size()) {
        raise("ArrayIndex", std::to_string(j));
        return;
    }
    assert(j < array.size());
    Cell val = array.at(j);
    stack.pop();
    stack.push(val);
}

void Executor::exec_INDEXAN()
{
    ip++;
    Number index = stack.top().number(); stack.pop();
    const std::vector<Cell> &array = stack.top().array();
    if (not number_is_integer(index)) {
        raise("ArrayIndex", number_to_string(index));
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        raise("ArrayIndex", std::to_string(i));
    }
    uint64_t j = static_cast<uint64_t>(i);
    Cell val = j < array.size() ? array.at(j) : Cell();
    stack.pop();
    stack.push(val);
}

void Executor::exec_INDEXDR()
{
    ip++;
    utf8string index = stack.top().string(); stack.pop();
    Cell *addr = stack.top().address(); stack.pop();
    auto e = addr->dictionary().find(index);
    if (e == addr->dictionary().end()) {
        raise("DictionaryIndex", index);
        return;
    }
    stack.push(Cell(&addr->dictionary_index_for_read(index)));
}

void Executor::exec_INDEXDW()
{
    ip++;
    utf8string index = stack.top().string(); stack.pop();
    Cell *addr = stack.top().address(); stack.pop();
    stack.push(Cell(&addr->dictionary_index_for_write(index)));
}

void Executor::exec_INDEXDV()
{
    ip++;
    utf8string index = stack.top().string(); stack.pop();
    const std::map<utf8string, Cell> &dictionary = stack.top().dictionary();
    auto e = dictionary.find(index);
    if (e == dictionary.end()) {
        raise("DictionaryIndex", index);
        return;
    }
    Cell val = e->second;
    stack.pop();
    stack.push(val);
}

void Executor::exec_INA()
{
    ip++;
    auto array = stack.top().array(); stack.pop();
    Cell val = stack.top(); stack.pop();
    stack.push(Cell(std::find(array.begin(), array.end(), val) != array.end()));
}

void Executor::exec_IND()
{
    ip++;
    auto dictionary = stack.top().dictionary(); stack.pop();
    utf8string key = stack.top().string(); stack.pop();
    stack.push(Cell(dictionary.find(key) != dictionary.end()));
}

void Executor::exec_CALLP()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    std::string func = module->object.strtable.at(val);
    try {
        rtl_call(stack, func, module->rtl_call_tokens[val]);
    } catch (RtlException &x) {
        ip -= 5;
        raise(x.name, x.info);
    }
}

void Executor::exec_CALLF()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    callstack.push_back(std::make_pair(module, ip));
    ip = val;
}

void Executor::exec_CALLMF()
{
    ip++;
    uint32_t mod = (module->object.code[ip] << 24) | (module->object.code[ip+1] << 16) | (module->object.code[ip+2] << 8) | module->object.code[ip+3];
    ip += 4;
    uint32_t val = (module->object.code[ip] << 24) | (module->object.code[ip+1] << 16) | (module->object.code[ip+2] << 8) | module->object.code[ip+3];
    ip += 4;
    callstack.push_back(std::make_pair(module, ip));
    auto m = modules.find(module->object.strtable[mod]);
    if (m == modules.end()) {
        fprintf(stderr, "fatal: module not found: %s\n", module->object.strtable[mod].c_str());
        exit(1);
    }
    module = m->second;
    ip = val;
}

void Executor::exec_JUMP()
{
    uint32_t target = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    ip = target;
}

void Executor::exec_JF()
{
    uint32_t target = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    bool a = stack.top().boolean(); stack.pop();
    if (not a) {
        ip = target;
    }
}

void Executor::exec_JT()
{
    uint32_t target = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    bool a = stack.top().boolean(); stack.pop();
    if (a) {
        ip = target;
    }
}

void Executor::exec_JFCHAIN()
{
    uint32_t target = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    Cell a = stack.top(); stack.pop();
    if (not a.boolean()) {
        ip = target;
        stack.pop();
        stack.push(a);
    }
}

void Executor::exec_DUP()
{
    ip++;
    stack.push(stack.top());
}

void Executor::exec_DUPX1()
{
    ip++;
    Cell a = stack.top(); stack.pop();
    Cell b = stack.top(); stack.pop();
    stack.push(a);
    stack.push(b);
    stack.push(a);
}

void Executor::exec_DROP()
{
    ip++;
    stack.pop();
}

void Executor::exec_RET()
{
    module = callstack.back().first;
    ip = callstack.back().second;
    callstack.pop_back();
}

void Executor::exec_CALLE()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    ExternalCallInfo *eci = val < module->external_functions.size() ? module->external_functions.at(val) : nullptr;
    if (eci == nullptr) {
        eci = new ExternalCallInfo;
        std::string func = module->object.strtable.at(val);
        auto info = split(func, ':');
        std::string library = info[0];
        std::string name = info[1];
        std::string rettype = info[2];
        auto params = split(info[3], ',');
        std::string exception;
        eci->fp = rtl_external_function(library, name, exception);
        if (eci->fp == nullptr) {
            raise(exception, "TODO");
            return;
        }
        for (auto p: params) {
                 if (p == "uint8" )   { eci->types.push_back(&ffi_type_uint8 );  eci->marshal.push_back(marshal_number<uint8_t >); }
            else if (p == "sint8" )   { eci->types.push_back(&ffi_type_sint8 );  eci->marshal.push_back(marshal_number< int8_t >); }
            else if (p == "uint16")   { eci->types.push_back(&ffi_type_uint16);  eci->marshal.push_back(marshal_number<uint16_t>); }
            else if (p == "sint16")   { eci->types.push_back(&ffi_type_sint16);  eci->marshal.push_back(marshal_number< int16_t>); }
            else if (p == "uint32")   { eci->types.push_back(&ffi_type_uint32);  eci->marshal.push_back(marshal_number<uint32_t>); }
            else if (p == "sint32")   { eci->types.push_back(&ffi_type_sint32);  eci->marshal.push_back(marshal_number< int32_t>); }
            else if (p == "uint64")   { eci->types.push_back(&ffi_type_uint64);  eci->marshal.push_back(marshal_number<uint64_t>); }
            else if (p == "sint64")   { eci->types.push_back(&ffi_type_sint64);  eci->marshal.push_back(marshal_number< int64_t>); }
            else if (p == "float" )   { eci->types.push_back(&ffi_type_float );  eci->marshal.push_back(marshal_number<float   >); }
            else if (p == "double")   { eci->types.push_back(&ffi_type_double);  eci->marshal.push_back(marshal_number<double  >); }
            else if (p == "pointer")  { eci->types.push_back(&ffi_type_pointer); eci->marshal.push_back(marshal_pointer         ); }
            else if (p == "*pointer") { eci->types.push_back(&ffi_type_pointer); eci->marshal.push_back(marshal_pointer_a       ); }
            else {
                fprintf(stderr, "ffi type not supported: %s\n", p.c_str());
                exit(1);
            }
        }
        ffi_type *rtype;
             if (rettype == "uint8" ) { rtype = &ffi_type_uint8;  eci->unmarshal = unmarshal_number<uint8_t  >; }
        else if (rettype == "sint8" ) { rtype = &ffi_type_sint8;  eci->unmarshal = unmarshal_number< int8_t  >; }
        else if (rettype == "uint16") { rtype = &ffi_type_uint16; eci->unmarshal = unmarshal_number<uint16_t >; }
        else if (rettype == "sint16") { rtype = &ffi_type_sint16; eci->unmarshal = unmarshal_number< int16_t >; }
        else if (rettype == "uint32") { rtype = &ffi_type_uint32; eci->unmarshal = unmarshal_number<uint32_t >; }
        else if (rettype == "sint32") { rtype = &ffi_type_sint32; eci->unmarshal = unmarshal_number< int32_t >; }
        else if (rettype == "uint64") { rtype = &ffi_type_uint64; eci->unmarshal = unmarshal_number<uint64_t >; }
        else if (rettype == "sint64") { rtype = &ffi_type_sint64; eci->unmarshal = unmarshal_number< int64_t >; }
        else if (rettype == "float" ) { rtype = &ffi_type_float;  eci->unmarshal = unmarshal_number<float    >; }
        else if (rettype == "double") { rtype = &ffi_type_double; eci->unmarshal = unmarshal_number<double   >; }
        else if (rettype == ""      ) { rtype = &ffi_type_void;   eci->unmarshal = nullptr;                     }
        else {
            fprintf(stderr, "ffi return type not supported: %s\n", rettype.c_str());
            exit(1);
        }
        ffi_status status = ffi_prep_cif(&eci->cif, FFI_DEFAULT_ABI, static_cast<unsigned int>(params.size()), rtype, eci->types.data());
        if (status != FFI_OK) {
            fprintf(stderr, "internal ffi error %d\n", status);
            exit(1);
        }
        if (val >= module->external_functions.size()) {
            module->external_functions.resize(val + 1);
        }
        module->external_functions[val] = eci;
    }
    std::vector<Cell> cells;
    char buf[1024];
    void *args[256];
    size_t size = sizeof(buf);
    void *p = buf;
    void *r = align(eci->cif.rtype->alignment, eci->cif.rtype->size, p, size);
    size_t i = eci->types.size();
    for (auto m = eci->marshal.rbegin(); m != eci->marshal.rend(); ++m) {
        i--;
        args[i] = p;
        cells.push_back(stack.top());
        stack.pop();
        (*m)(cells[cells.size()-1], p, size);
    }
    ffi_call(&eci->cif, eci->fp, r, args);
    if (eci->unmarshal != nullptr) {
        stack.push(eci->unmarshal(r));
    }
}

void Executor::exec_CONSA()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    std::vector<Cell> a;
    while (val > 0) {
        a.push_back(stack.top());
        stack.pop();
        val--;
    }
    stack.push(Cell(a));
}

void Executor::exec_CONSD()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    Cell d;
    while (val > 0) {
        Cell value = stack.top(); stack.pop();
        utf8string key = stack.top().string(); stack.pop();
        d.dictionary_index_for_write(key) = value;
        val--;
    }
    stack.push(d);
}

void Executor::exec_EXCEPT()
{
    utf8string info = stack.top().string(); stack.pop();
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    raise(module->object.strtable[val], info);
}

void Executor::exec_CLREXC()
{
    ip++;
    // The fields here must match the declaration of
    // ExceptionType in ast.cpp.
    module->globals[0].array_index_for_write(0) = Cell("");
    module->globals[0].array_index_for_write(1) = Cell("");
    module->globals[0].array_index_for_write(2) = Cell(number_from_uint32(0));
}

void Executor::exec_ALLOC()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    stack.push(Cell(new Cell(std::vector<Cell>(val))));
}

void Executor::exec_PUSHNIL()
{
    ip++;
    stack.push(Cell(static_cast<Cell *>(nullptr)));
}

void Executor::raise(const utf8string &exception, const utf8string &info)
{
    // The fields here must match the declaration of
    // ExceptionType in ast.cpp.
    module->globals[0].array_index_for_write(0) = Cell(exception);
    module->globals[0].array_index_for_write(1) = Cell(info);
    module->globals[0].array_index_for_write(2) = Cell(number_from_uint32(static_cast<uint32_t>(ip)));

    auto tmodule = module;
    auto tip = ip;
    size_t sp = callstack.size();
    for (;;) {
        for (auto e = tmodule->object.exceptions.begin(); e != tmodule->object.exceptions.end(); ++e) {
            if (tip >= e->start && tip < e->end && exception == tmodule->object.strtable[e->excid]) {
                module = tmodule;
                ip = e->handler;
                callstack.resize(sp);
                return;
            }
        }
        if (sp == 0) {
            break;
        }
        sp -= 1;
        tmodule = callstack[sp].first;
        tip = callstack[sp].second;
    }

    fprintf(stderr, "Unhandled exception %s (%s)\n", exception.c_str(), info.c_str());
    while (ip < module->object.code.size()) {
        if (module->debug != nullptr) {
            auto line = module->debug->line_numbers.end();
            auto p = ip;
            for (;;) {
                line = module->debug->line_numbers.find(p);
                if (line != module->debug->line_numbers.end()) {
                    break;
                }
                if (p == 0) {
                    fprintf(stderr, "No matching debug information found.\n");
                    break;
                }
                p--;
            }
            if (line != module->debug->line_numbers.end()) {
                fprintf(stderr, "  Stack frame #%lu: file %s line %d address %lu\n", static_cast<unsigned long>(callstack.size()), module->debug->source_path.c_str(), line->second, static_cast<unsigned long>(ip));
                fprintf(stderr, "    %s\n", module->debug->source_lines.at(line->second).c_str());
            } else {
                fprintf(stderr, "  Stack frame #%lu: file %s address %lu (line number not found)\n", static_cast<unsigned long>(callstack.size()), module->debug->source_path.c_str(), static_cast<unsigned long>(ip));
            }
        } else {
            fprintf(stderr, "  Stack frame #%lu: file %s address %lu (no debug info available)\n", static_cast<unsigned long>(callstack.size()), source_path.c_str(), static_cast<unsigned long>(ip));
        }
        if (callstack.empty()) {
            break;
        }
        module = callstack.back().first;
        ip = callstack.back().second;
        callstack.pop_back();
    }
    exit(1);
}

void Executor::exec()
{
    // The number of fields here must match the declaration of
    // ExceptionType in ast.cpp.
    // TODO: Should be only one instance of the current exception object.
    for (auto m: modules) {
        m.second->globals[0].array_index_for_write(2);
    }

    callstack.push_back(std::make_pair(module, module->object.code.size()));

    // This sets up the call stack in such a way as to initialize
    // each module in the order determined in init_order, followed
    // by running the code in the main module.
    callstack.push_back(std::make_pair(module, 0));
    for (auto x = init_order.rbegin(); x != init_order.rend(); ++x) {
        callstack.push_back(std::make_pair(modules[*x], 0));
    }
    // Set up ip for first module (or main module if no imports).
    exec_RET();

    while (not callstack.empty() && ip < module->object.code.size()) {
        //std::cerr << "mod " << module->name << " ip " << ip << " op " << (int)module->object.code[ip] << " st " << stack.depth() << "\n";
        auto last_module = module;
        auto last_ip = ip;
        switch (static_cast<Opcode>(module->object.code[ip])) {
            case ENTER:   exec_ENTER(); break;
            case LEAVE:   exec_LEAVE(); break;
            case PUSHB:   exec_PUSHB(); break;
            case PUSHN:   exec_PUSHN(); break;
            case PUSHS:   exec_PUSHS(); break;
            case PUSHPG:  exec_PUSHPG(); break;
            case PUSHPMG: exec_PUSHPMG(); break;
            case PUSHPL:  exec_PUSHPL(); break;
            case PUSHPOL: exec_PUSHPOL(); break;
            case LOADB:   exec_LOADB(); break;
            case LOADN:   exec_LOADN(); break;
            case LOADS:   exec_LOADS(); break;
            case LOADA:   exec_LOADA(); break;
            case LOADD:   exec_LOADD(); break;
            case LOADP:   exec_LOADP(); break;
            case STOREB:  exec_STOREB(); break;
            case STOREN:  exec_STOREN(); break;
            case STORES:  exec_STORES(); break;
            case STOREA:  exec_STOREA(); break;
            case STORED:  exec_STORED(); break;
            case STOREP:  exec_STOREP(); break;
            case NEGN:    exec_NEGN(); break;
            case ADDN:    exec_ADDN(); break;
            case SUBN:    exec_SUBN(); break;
            case MULN:    exec_MULN(); break;
            case DIVN:    exec_DIVN(); break;
            case MODN:    exec_MODN(); break;
            case EXPN:    exec_EXPN(); break;
            case EQB:     exec_EQB(); break;
            case NEB:     exec_NEB(); break;
            case EQN:     exec_EQN(); break;
            case NEN:     exec_NEN(); break;
            case LTN:     exec_LTN(); break;
            case GTN:     exec_GTN(); break;
            case LEN:     exec_LEN(); break;
            case GEN:     exec_GEN(); break;
            case EQS:     exec_EQS(); break;
            case NES:     exec_NES(); break;
            case LTS:     exec_LTS(); break;
            case GTS:     exec_GTS(); break;
            case LES:     exec_LES(); break;
            case GES:     exec_GES(); break;
            case EQA:     exec_EQA(); break;
            case NEA:     exec_NEA(); break;
            case EQD:     exec_EQD(); break;
            case NED:     exec_NED(); break;
            case EQP:     exec_EQP(); break;
            case NEP:     exec_NEP(); break;
            case ANDB:    exec_ANDB(); break;
            case ORB:     exec_ORB(); break;
            case NOTB:    exec_NOTB(); break;
            case INDEXAR: exec_INDEXAR(); break;
            case INDEXAW: exec_INDEXAW(); break;
            case INDEXAV: exec_INDEXAV(); break;
            case INDEXAN: exec_INDEXAN(); break;
            case INDEXDR: exec_INDEXDR(); break;
            case INDEXDW: exec_INDEXDW(); break;
            case INDEXDV: exec_INDEXDV(); break;
            case INA:     exec_INA(); break;
            case IND:     exec_IND(); break;
            case CALLP:   exec_CALLP(); break;
            case CALLF:   exec_CALLF(); break;
            case CALLMF:  exec_CALLMF(); break;
            case JUMP:    exec_JUMP(); break;
            case JF:      exec_JF(); break;
            case JT:      exec_JT(); break;
            case JFCHAIN: exec_JFCHAIN(); break;
            case DUP:     exec_DUP(); break;
            case DUPX1:   exec_DUPX1(); break;
            case DROP:    exec_DROP(); break;
            case RET:     exec_RET(); break;
            case CALLE:   exec_CALLE(); break;
            case CONSA:   exec_CONSA(); break;
            case CONSD:   exec_CONSD(); break;
            case EXCEPT:  exec_EXCEPT(); break;
            case CLREXC:  exec_CLREXC(); break;
            case ALLOC:   exec_ALLOC(); break;
            case PUSHNIL: exec_PUSHNIL(); break;
        }
        if (module == last_module && ip == last_ip) {
            fprintf(stderr, "exec: Unexpected opcode: %d\n", module->object.code[ip]);
            abort();
        }
    }
    assert(stack.empty());
}

void exec(const std::string &source_path, const Bytecode::Bytes &obj, const DebugInfo *debuginfo, ICompilerSupport *support, int argc, char *argv[])
{
    rtl_exec_init(argc, argv);
    Executor(source_path, obj, debuginfo, support).exec();
}
