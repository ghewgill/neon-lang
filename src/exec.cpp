#include "exec.h"

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iso646.h>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <stdlib.h>
#include <string.h>

#include <ffi.h>
#include <minijson_writer.hpp>

#include "bytecode.h"
#include "cell.h"
#include "debuginfo.h"
#include "httpserver.h"
#include "neonext.h"
#include "number.h"
#include "opcode.h"
#include "opstack.h"
#include "rtl_exec.h"
#include "rtl_platform.h"
#include "support.h"

#ifdef _WIN32
#define PATHSEP "\\"
#define LIBRARY_NAME_PREFIX ""
#else
#define PATHSEP "/"
#define LIBRARY_NAME_PREFIX "lib"
#endif

std::set<std::string> g_ExtensionModules;

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

std::string just_path(const std::string &name)
{
    auto i = name.find_last_of((std::string(PATHSEP) + "/").c_str());
    if (i == std::string::npos) {
        return std::string();
    }
    return name.substr(0, i+1);
}

} // namespace

class ForeignCallInfo {
public:
    typedef void *(*marshal_f)(Cell &cell, void *&p, size_t &space);
    typedef Cell (*unmarshal_f)(void *p);
    ForeignCallInfo(): types(), marshal(), unmarshal(nullptr), cif(), fp(nullptr) {}
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

template <typename T> static void *marshal_number(Cell &cell, void *&p, size_t &space)
{
    T *a = reinterpret_cast<T *>(align(alignof(T), sizeof(T), p, space));
    *a = number_conversion<T>::convert_to_native(cell.number());
    p = a + 1;
    space -= sizeof(T);
    return a;
}

static void *marshal_pointer(Cell &cell, void *&p, size_t &space)
{
    void **a = reinterpret_cast<void **>(align(alignof(void *), sizeof(void *), p, space));
    *a = cell.address();
    p = a + 1;
    space -= sizeof(void *);
    return a;
}

static void *marshal_pointer_a(Cell &cell, void *&p, size_t &space)
{
    void **a = reinterpret_cast<void **>(align(alignof(void *), sizeof(void *), p, space));
    *a = cell.address()->address();
    p = a + 1;
    space -= sizeof(void *);
    return a;
}

static void *marshal_string(Cell &cell, void *&p, size_t &space)
{
    void **a = reinterpret_cast<void **>(align(alignof(void *), sizeof(void *), p, space));
    *a = const_cast<char *>(cell.string().data());
    p = a + 1;
    space -= sizeof(void *);
    return a;
}

static void *marshal_string_a(Cell &cell, void *&p, size_t &space)
{
    void **a = reinterpret_cast<void **>(align(alignof(void *), sizeof(void *), p, space));
    *a = const_cast<char *>(cell.address()->string().data());
    p = a + 1;
    space -= sizeof(void *);
    return a;
}

template <typename T> static Cell unmarshal_boolean(void *p)
{
    return Cell(*reinterpret_cast<T *>(p) != 0);
}

template <typename T> static Cell unmarshal_number(void *p)
{
    return Cell(number_conversion<T>::convert_from_native(*reinterpret_cast<T *>(p)));
}

static Cell unmarshal_pointer(void *p)
{
    return Cell(*(reinterpret_cast<Cell **>(p)));
}

class ActivationFrame {
public:
    ActivationFrame(size_t count): locals(count) {}
    // TODO (for nested functions) std::vector<ActivationFrame *> outer;
    std::vector<Cell> locals;
};

// The fields here must match the declaration of
// ExceptionInfo in ast.cpp.
class ExceptionInfo {
public:
    ExceptionInfo(): info(""), code(number_from_uint32(0)) {}
    explicit ExceptionInfo(const utf8string &info): info(info), code(number_from_uint32(0)) {}
    ExceptionInfo(const utf8string &info, Number code): info(info), code(code) {}
    ExceptionInfo(const utf8string &info, uint32_t code): info(info), code(number_from_uint32(code)) {}
    utf8string info;
    Number code;
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
    std::vector<std::pair<bool, Number>> number_table;
    std::vector<ForeignCallInfo *> foreign_functions;
    std::map<std::pair<std::string, std::string>, std::pair<Module *, Bytecode::Bytes::size_type>> module_functions;
private:
    Module(const Module &);
    Module &operator=(const Module &);
};

class Executor: public IHttpServerHandler {
public:
    Executor(const std::string &source_path, const Bytecode::Bytes &bytes, const DebugInfo *debuginfo, ICompilerSupport *support, bool enable_assert, unsigned short debug_port, std::map<std::string, Cell *> *external_globals);
    virtual ~Executor();

    // Module: debugger
    void breakpoint();
    void log(const std::string &message);

    // Module: runtime
    void garbage_collect();
    size_t get_allocated_object_count();
    bool module_is_main();
    void set_garbage_collection_interval(size_t count);
    void set_recursion_limit(size_t depth);

    void exec();
    void exec_loop(size_t min_callstack_depth);
//private:
    const std::string source_path;
    const bool enable_assert;

    size_t param_garbage_collection_interval;
    size_t param_recursion_limit;

    std::map<std::string, Cell *> *external_globals;
    std::map<std::string, Module *> modules;
    std::vector<std::string> init_order;
    Module *module;
    Bytecode::Bytes::size_type ip;
    opstack<Cell> stack;
    std::vector<std::pair<Module *, Bytecode::Bytes::size_type>> callstack;
    std::list<ActivationFrame> frames;
    std::vector<ActivationFrame *> nested_frames;

    std::list<Cell> allocs;
    unsigned int allocations;

    enum class DebuggerState {
        STOPPED,
        RUN,
        STEP_INSTRUCTION,
        STEP_SOURCE,
        QUIT,
    };
    static const char *DebuggerStateName[];
    HttpServer *debug_server;
    DebuggerState debugger_state;
    size_t debugger_step_source_depth;
    std::set<size_t> debugger_breakpoints;
    std::vector<std::string> debugger_log;

    void exec_ENTER();
    void exec_LEAVE();
    void exec_PUSHB();
    void exec_PUSHN();
    void exec_PUSHS();
    void exec_PUSHT();
    void exec_PUSHPG();
    void exec_PUSHPPG();
    void exec_PUSHPMG();
    void exec_PUSHPL();
    void exec_PUSHPOL();
    void exec_PUSHI();
    void exec_LOADB();
    void exec_LOADN();
    void exec_LOADS();
    void exec_LOADT();
    void exec_LOADA();
    void exec_LOADD();
    void exec_LOADP();
    void exec_STOREB();
    void exec_STOREN();
    void exec_STORES();
    void exec_STORET();
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
    void exec_EQT();
    void exec_NET();
    void exec_LTT();
    void exec_GTT();
    void exec_LET();
    void exec_GET();
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
    void exec_CALLI();
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
    void exec_ALLOC();
    void exec_PUSHNIL();
    void exec_JNASSERT();
    void exec_RESETC();
    void exec_PUSHPEG();
    void exec_JUMPTBL();
    void exec_CALLX();
    void exec_SWAP();
    void exec_DROPN();
    void exec_PUSHM();
    void exec_CALLV();
    void exec_PUSHCI();

    void raise_literal(const utf8string &exception, const ExceptionInfo &info);
    void raise(const ExceptionName &exception, const ExceptionInfo &info);
    void raise(const RtlException &x);

    virtual void handle_GET(const std::string &path, HttpResponse &response);
    virtual void handle_POST(const std::string &path, const std::string &data, HttpResponse &response);

    friend class Module;
private:
    Executor(const Executor &);
    Executor &operator=(const Executor &);
};

const char *Executor::DebuggerStateName[] = {
    "stopped",
    "run",
    "single_step",
    "source_step",
    "quit",
};

static Executor *g_executor;

extern "C" {

struct Ne_ParameterList *parameterlist_alloc(int n)
{
    Cell *c = new Cell();
    c->array_for_write().resize(n);
    return reinterpret_cast<struct Ne_ParameterList *>(c);
}

void parameterlist_free(struct Ne_ParameterList *params)
{
    delete reinterpret_cast<Cell *>(params);
}

int parameterlist_get_size(const struct Ne_ParameterList *list)
{
    return static_cast<int>(reinterpret_cast<Cell *>(const_cast<struct Ne_ParameterList *>(list))->array().size());
}

int parameterlist_check_types(const struct Ne_ParameterList * /*list*/, const char * /*types*/)
{
    //fprintf(stderr, "parameterlist_check_types(%p, %s)\n", list, types);
    return 1;
}

const struct Ne_Cell *parameterlist_get_cell(const struct Ne_ParameterList *list, int i)
{
    // rbegin() is used here because the array ends up having
    // the elements in backwards order.
    return reinterpret_cast<const struct Ne_Cell *>(&*(reinterpret_cast<Cell *>(const_cast<struct Ne_ParameterList *>(list))->array().rbegin() + i));
}

struct Ne_Cell *parameterlist_set_cell(struct Ne_ParameterList *list, int i)
{
    // rbegin() is used here because the array ends up having
    // the elements in backwards order.
    return reinterpret_cast<struct Ne_Cell *>(&*(reinterpret_cast<Cell *>(list)->array_for_write().rbegin() + i));
}

struct Ne_Cell *cell_alloc()
{
    return reinterpret_cast<struct Ne_Cell *>(new Cell());
}

void cell_free(struct Ne_Cell *cell)
{
    delete reinterpret_cast<Cell *>(cell);
}

void cell_copy(struct Ne_Cell *dest, const struct Ne_Cell *src)
{
    *reinterpret_cast<Cell *>(dest) = *reinterpret_cast<const Cell *>(src);
}

int cell_get_boolean(const struct Ne_Cell *cell)
{
    return reinterpret_cast<Cell *>(const_cast<struct Ne_Cell *>(cell))->boolean();
}

void cell_set_boolean(struct Ne_Cell *cell, int value)
{
    reinterpret_cast<Cell *>(cell)->boolean() = (value != 0);
}

int cell_get_number_int(const struct Ne_Cell *cell)
{
    return static_cast<int>(number_to_sint64(reinterpret_cast<Cell *>(const_cast<struct Ne_Cell *>(cell))->number()));
}

unsigned int cell_get_number_uint(const struct Ne_Cell *cell)
{
    return static_cast<unsigned int>(number_to_uint64(reinterpret_cast<Cell *>(const_cast<struct Ne_Cell *>(cell))->number()));
}

void cell_set_number_int(struct Ne_Cell *cell, int value)
{
    reinterpret_cast<Cell *>(cell)->number() = number_from_sint64(value);
}

void cell_set_number_uint(struct Ne_Cell *cell, unsigned int value)
{
    reinterpret_cast<Cell *>(cell)->number() = number_from_uint64(value);
}

const char *cell_get_string(const struct Ne_Cell *cell)
{
    return reinterpret_cast<Cell *>(const_cast<struct Ne_Cell *>(cell))->string().c_str();
}

void cell_set_string(struct Ne_Cell *cell, const char *value)
{
    reinterpret_cast<Cell *>(cell)->string_for_write() = value;
}

const unsigned char *cell_get_bytes(const struct Ne_Cell *cell)
{
    return reinterpret_cast<Cell *>(const_cast<struct Ne_Cell *>(cell))->bytes().data();
}

int cell_get_bytes_size(const struct Ne_Cell *cell)
{
    return static_cast<int>(reinterpret_cast<Cell *>(const_cast<struct Ne_Cell *>(cell))->bytes().size());
}

void cell_set_bytes(struct Ne_Cell *cell, const unsigned char *value, int size)
{
    reinterpret_cast<Cell *>(cell)->bytes_for_write() = std::vector<unsigned char>(value, value+size);
}

void *cell_get_pointer(const struct Ne_Cell *cell)
{
    return reinterpret_cast<Cell *>(const_cast<struct Ne_Cell *>(cell))->address();
}

void cell_set_pointer(struct Ne_Cell *cell, void *p)
{
    reinterpret_cast<Cell *>(cell)->address() = reinterpret_cast<Cell *>(p);
}

int cell_get_array_size(const struct Ne_Cell *cell)
{
    return static_cast<int>(reinterpret_cast<Cell *>(const_cast<struct Ne_Cell *>(cell))->array().size());
}

void cell_array_clear(struct Ne_Cell *cell)
{
    reinterpret_cast<Cell *>(cell)->array_for_write().clear();
}

const struct Ne_Cell *cell_get_array_cell(const struct Ne_Cell *cell, int index)
{
    return reinterpret_cast<const struct Ne_Cell *>(&reinterpret_cast<Cell *>(const_cast<struct Ne_Cell *>(cell))->array_index_for_read(index));
}

struct Ne_Cell *cell_set_array_cell(struct Ne_Cell *cell, int index)
{
    return reinterpret_cast<struct Ne_Cell *>(&reinterpret_cast<Cell *>(cell)->array_index_for_write(index));
}

int cell_get_dictionary_size(const struct Ne_Cell *cell)
{
    return static_cast<int>(reinterpret_cast<Cell *>(const_cast<struct Ne_Cell *>(cell))->dictionary().size());
}

const char *cell_get_dictionary_key(const struct Ne_Cell *cell, int n)
{
    const std::map<utf8string, Cell> &d = reinterpret_cast<Cell *>(const_cast<struct Ne_Cell *>(cell))->dictionary();
    std::map<utf8string, Cell>::const_iterator i = d.begin();
    std::advance(i, n);
    return i->first.c_str();
}

const struct Ne_Cell *cell_get_dictionary_cell(const struct Ne_Cell *cell, const char *key)
{
    return reinterpret_cast<const struct Ne_Cell *>(&reinterpret_cast<Cell *>(const_cast<struct Ne_Cell *>(cell))->dictionary_index_for_read(key));
}

struct Ne_Cell *cell_set_dictionary_cell(struct Ne_Cell *cell, const char *key)
{
    return reinterpret_cast<struct Ne_Cell *>(&reinterpret_cast<Cell *>(cell)->dictionary_index_for_write(key));
}

void exec_callback(const struct Ne_Cell *callback, const struct Ne_ParameterList *params, struct Ne_Cell *retval)
{
    // TODO: move this into a method in Executor that's called by exec_CALLI too
    if (g_executor->callstack.size() >= g_executor->param_recursion_limit) {
        g_executor->raise(rtl::global::Exception_StackOverflowException, ExceptionInfo(""));
        return;
    }
    std::vector<Cell> a = reinterpret_cast<Cell *>(const_cast<struct Ne_Cell *>(callback))->array();
    Module *mod = reinterpret_cast<Module *>(a[0].address());
    Number addr = a[1].number();
    if (mod == nullptr || number_is_zero(addr) || not number_is_integer(addr)) {
        g_executor->raise(rtl::global::Exception_InvalidFunctionException, ExceptionInfo(""));
        return;
    }
    if (params != NULL) {
        auto &ps = reinterpret_cast<Cell *>(const_cast<struct Ne_ParameterList *>(params))->array();
        for (auto i = ps.rbegin(); i != ps.rend(); ++i) {
            g_executor->stack.push(*i);
        }
    }
    uint32_t addri = number_to_uint32(addr);
    g_executor->callstack.push_back(std::make_pair(g_executor->module, g_executor->ip));
    g_executor->module = mod;
    g_executor->ip = addri;
    g_executor->exec_loop(g_executor->callstack.size() - 1);
    if (retval != NULL) {
        *reinterpret_cast<Cell *>(retval) = g_executor->stack.top();
        g_executor->stack.pop();
    }
}

int raise_exception(struct Ne_Cell *retval, const char *name, const char *info, int code)
{
    Cell *r = reinterpret_cast<Cell *>(retval);
    r->array_for_write().resize(3);
    r->array_for_write()[0].string_for_write() = name;
    r->array_for_write()[1].string_for_write() = info;
    r->array_for_write()[2].number() = number_from_sint64(code);
    return Ne_EXCEPTION;
}

} // extern "C"

const Ne_MethodTable ExtensionMethodTable = {
    parameterlist_alloc,
    parameterlist_free,
    parameterlist_get_size,
    parameterlist_check_types,
    parameterlist_get_cell,
    parameterlist_set_cell,
    cell_alloc,
    cell_free,
    cell_copy,
    cell_get_boolean,
    cell_set_boolean,
    cell_get_number_int,
    cell_get_number_uint,
    cell_set_number_int,
    cell_set_number_uint,
    cell_get_string,
    cell_set_string,
    cell_get_bytes,
    cell_get_bytes_size,
    cell_set_bytes,
    cell_get_pointer,
    cell_set_pointer,
    cell_get_array_size,
    cell_array_clear,
    cell_get_array_cell,
    cell_set_array_cell,
    cell_get_dictionary_size,
    cell_get_dictionary_key,
    cell_get_dictionary_cell,
    cell_set_dictionary_cell,
    exec_callback,
    raise_exception
};

Executor::Executor(const std::string &source_path, const Bytecode::Bytes &bytes, const DebugInfo *debuginfo, ICompilerSupport *support, bool enable_assert, unsigned short debug_port, std::map<std::string, Cell *> *external_globals)
  : source_path(source_path),
    enable_assert(enable_assert),
    param_garbage_collection_interval(1000),
    param_recursion_limit(1000),
    external_globals(external_globals),
    modules(),
    init_order(),
    module(nullptr),
    ip(0),
    stack(),
    callstack(),
    frames(),
    nested_frames(),
    allocs(),
    allocations(0),
    debug_server(debug_port ? new HttpServer(debug_port, this) : nullptr),
    debugger_state(DebuggerState::STOPPED),
    debugger_step_source_depth(0),
    debugger_breakpoints(),
    debugger_log()
{
    assert(g_executor == nullptr);
    g_executor = this;
    Bytecode b;
    if (not b.load(source_path, bytes)) {
        fprintf(stderr, "error loading bytecode\n");
        exit(1);
    }
    module = new Module(source_path, b, debuginfo, this, support);
    modules[""] = module;
}

Executor::~Executor()
{
    delete debug_server;
    g_executor = nullptr;
}

Module::Module(const std::string &name, const Bytecode &object, const DebugInfo *debuginfo, Executor *executor, ICompilerSupport *support)
  : name(name),
    object(object),
    debug(debuginfo),
    globals(object.global_size),
    rtl_call_tokens(object.strtable.size(), SIZE_MAX),
    number_table(object.strtable.size()),
    foreign_functions(),
    module_functions()
{
    for (auto i: object.imports) {
        std::string importname = object.strtable[i.first];
        if (executor->modules.find(importname) != executor->modules.end()) {
            continue;
        }
        Bytecode code;
        if (not support->loadBytecode(importname, code)) {
            fprintf(stderr, "couldn't load module: %s\n", importname.c_str());
            exit(1);
        }
        // TODO: check hash of exports
        executor->init_order.push_back(importname);
        executor->modules[importname] = nullptr; // Prevent unwanted recursion.
        executor->modules[importname] = new Module(importname, code, nullptr, executor, support);
    }
}

void Executor::exec_ENTER()
{
    ip++;
    uint32_t nest = (module->object.code[ip] << 24) | (module->object.code[ip+1] << 16) | (module->object.code[ip+2] << 8) | module->object.code[ip+3];
    ip += 4;
    uint32_t val = (module->object.code[ip] << 24) | (module->object.code[ip+1] << 16) | (module->object.code[ip+2] << 8) | module->object.code[ip+3];
    ip += 4;
    frames.push_back(ActivationFrame(val));
    nested_frames.resize(nest-1);
    nested_frames.push_back(&frames.back());
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
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    if (not module->number_table[val].first) {
        module->number_table[val] = std::make_pair(true, number_from_string(module->object.strtable[val]));
    }
    stack.push(Cell(module->number_table[val].second));
}

void Executor::exec_PUSHS()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    stack.push(Cell(module->object.strtable[val]));
}

void Executor::exec_PUSHT()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    stack.push(Cell(std::vector<unsigned char>(reinterpret_cast<const unsigned char *>(module->object.strtable[val].data()), reinterpret_cast<const unsigned char *>(module->object.strtable[val].data()) + module->object.strtable[val].size())));
}

void Executor::exec_PUSHPG()
{
    uint32_t addr = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    assert(addr < module->globals.size());
    stack.push(Cell(&module->globals.at(addr)));
}

void Executor::exec_PUSHPPG()
{
    uint32_t name = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    stack.push(Cell(rtl_variable(module->object.strtable[name])));
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
    ip++;
    uint32_t back = (module->object.code[ip] << 24) | (module->object.code[ip+1] << 16) | (module->object.code[ip+2] << 8) | module->object.code[ip+3];
    ip += 4;
    uint32_t addr = (module->object.code[ip] << 24) | (module->object.code[ip+1] << 16) | (module->object.code[ip+2] << 8) | module->object.code[ip+3];
    ip += 4;
    stack.push(Cell(&nested_frames[nested_frames.size()-1-back]->locals.at(addr)));
}

void Executor::exec_PUSHI()
{
    ip++;
    uint32_t x = (module->object.code[ip] << 24) | (module->object.code[ip+1] << 16) | (module->object.code[ip+2] << 8) | module->object.code[ip+3];
    ip += 4;
    stack.push(Cell(number_from_uint32(x)));
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

void Executor::exec_LOADT()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    addr->bytes();
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

void Executor::exec_STORET()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    *addr = stack.top(); stack.pop();
    addr->bytes();
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
        raise(rtl::global::Exception_DivideByZeroException, ExceptionInfo(""));
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
        raise(rtl::global::Exception_DivideByZeroException, ExceptionInfo(""));
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

void Executor::exec_EQT()
{
    ip++;
    std::vector<unsigned char> b = stack.top().bytes(); stack.pop();
    std::vector<unsigned char> a = stack.top().bytes(); stack.pop();
    stack.push(Cell(a == b));
}

void Executor::exec_NET()
{
    ip++;
    std::vector<unsigned char> b = stack.top().bytes(); stack.pop();
    std::vector<unsigned char> a = stack.top().bytes(); stack.pop();
    stack.push(Cell(a != b));
}

void Executor::exec_LTT()
{
    ip++;
    std::vector<unsigned char> b = stack.top().bytes(); stack.pop();
    std::vector<unsigned char> a = stack.top().bytes(); stack.pop();
    stack.push(Cell(a < b));
}

void Executor::exec_GTT()
{
    ip++;
    std::vector<unsigned char> b = stack.top().bytes(); stack.pop();
    std::vector<unsigned char> a = stack.top().bytes(); stack.pop();
    stack.push(Cell(a > b));
}

void Executor::exec_LET()
{
    ip++;
    std::vector<unsigned char> b = stack.top().bytes(); stack.pop();
    std::vector<unsigned char> a = stack.top().bytes(); stack.pop();
    stack.push(Cell(a <= b));
}

void Executor::exec_GET()
{
    ip++;
    std::vector<unsigned char> b = stack.top().bytes(); stack.pop();
    std::vector<unsigned char> a = stack.top().bytes(); stack.pop();
    stack.push(Cell(a >= b));
}

void Executor::exec_EQA()
{
    ip++;
    const std::vector<Cell> &b = stack.top().array();
    const std::vector<Cell> &a = stack.peek(1).array();
    bool v = a == b;
    stack.pop();
    stack.pop();
    stack.push(Cell(v));
}

void Executor::exec_NEA()
{
    ip++;
    const std::vector<Cell> &b = stack.top().array();
    const std::vector<Cell> &a = stack.peek(1).array();
    bool v = a != b;
    stack.pop();
    stack.pop();
    stack.push(Cell(v));
}

void Executor::exec_EQD()
{
    ip++;
    const std::map<utf8string, Cell> &b = stack.top().dictionary();
    const std::map<utf8string, Cell> &a = stack.peek(1).dictionary();
    bool v = a == b;
    stack.pop();
    stack.pop();
    stack.push(Cell(v));
}

void Executor::exec_NED()
{
    ip++;
    const std::map<utf8string, Cell> &b = stack.top().dictionary();
    const std::map<utf8string, Cell> &a = stack.peek(1).dictionary();
    bool v = a != b;
    stack.pop();
    stack.pop();
    stack.push(Cell(v));
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
        raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(number_to_string(index)));
        return;
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(std::to_string(i)));
        return;
    }
    uint64_t j = static_cast<uint64_t>(i);
    if (j >= addr->array().size()) {
        raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(std::to_string(j)));
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
        raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(number_to_string(index)));
        return;
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(std::to_string(i)));
        return;
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
        raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(number_to_string(index)));
        return;
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(std::to_string(i)));
        return;
    }
    uint64_t j = static_cast<uint64_t>(i);
    if (j >= array.size()) {
        raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(std::to_string(j)));
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
        raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(number_to_string(index)));
        return;
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(std::to_string(i)));
        return;
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
        raise(rtl::global::Exception_DictionaryIndexException, ExceptionInfo(index));
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
        raise(rtl::global::Exception_DictionaryIndexException, ExceptionInfo(index));
        return;
    }
    Cell val = e->second;
    stack.pop();
    stack.push(val);
}

void Executor::exec_INA()
{
    ip++;
    auto &array = stack.top().array();
    Cell val = stack.peek(1);
    bool v = std::find(array.begin(), array.end(), val) != array.end();
    stack.pop();
    stack.pop();
    stack.push(Cell(v));
}

void Executor::exec_IND()
{
    ip++;
    auto &dictionary = stack.top().dictionary();
    utf8string key = stack.peek(1).string();
    bool v = dictionary.find(key) != dictionary.end();
    stack.pop();
    stack.pop();
    stack.push(Cell(v));
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
        raise(x);
    }
}

void Executor::exec_CALLF()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    if (callstack.size() >= param_recursion_limit) {
        raise(rtl::global::Exception_StackOverflowException, ExceptionInfo(""));
        return;
    }
    callstack.push_back(std::make_pair(module, ip));
    ip = val;
}

void Executor::exec_CALLMF()
{
    ip++;
    uint32_t mod = (module->object.code[ip] << 24) | (module->object.code[ip+1] << 16) | (module->object.code[ip+2] << 8) | module->object.code[ip+3];
    ip += 4;
    uint32_t func  = (module->object.code[ip] << 24) | (module->object.code[ip+1] << 16) | (module->object.code[ip+2] << 8) | module->object.code[ip+3];
    ip += 4;
    if (callstack.size() >= param_recursion_limit) {
        raise(rtl::global::Exception_StackOverflowException, ExceptionInfo(""));
        return;
    }
    callstack.push_back(std::make_pair(module, ip));
    auto f = module->module_functions.find(std::make_pair(module->object.strtable[mod], module->object.strtable[func]));
    if (f != module->module_functions.end()) {
        module = f->second.first;
        ip = f->second.second;
    } else {
        auto m = modules.find(module->object.strtable[mod]);
        if (m == modules.end()) {
            fprintf(stderr, "fatal: module not found: %s\n", module->object.strtable[mod].c_str());
            exit(1);
        }
        bool found = false;
        for (auto ef: m->second->object.export_functions) {
            if (m->second->object.strtable[ef.name] + "," + m->second->object.strtable[ef.descriptor] == module->object.strtable[func]) {
                ip = ef.entry;
                module->module_functions[std::make_pair(module->object.strtable[mod], module->object.strtable[func])] = std::make_pair(m->second, ip);
                found = true;
                break;
            }
        }
        if (not found) {
            fprintf(stderr, "fatal: module function not found: %s\n", module->object.strtable[func].c_str());
            exit(1);
        }
        module = m->second;
    }
}

void Executor::exec_CALLI()
{
    ip++;
    if (callstack.size() >= param_recursion_limit) {
        raise(rtl::global::Exception_StackOverflowException, ExceptionInfo(""));
        return;
    }
    std::vector<Cell> a = stack.top().array(); stack.pop();
    Module *mod = reinterpret_cast<Module *>(a[0].address());
    Number addr = a[1].number();
    if (number_is_zero(addr) || not number_is_integer(addr)) {
        raise(rtl::global::Exception_InvalidFunctionException, ExceptionInfo(""));
        return;
    }
    uint32_t addri = number_to_uint32(addr);
    callstack.push_back(std::make_pair(module, ip));
    module = mod;
    ip = addri;
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
    ForeignCallInfo *fci = val < module->foreign_functions.size() ? module->foreign_functions.at(val) : nullptr;
    if (fci == nullptr) {
        fci = new ForeignCallInfo;
        std::string func = module->object.strtable.at(val);
        auto info = split(func, ':');
        std::string library = info[0];
        std::string name = info[1];
        std::string rettype = info[2];
        auto params = split(info[3], ',');
        try {
            fci->fp = rtl_foreign_function(library, name);
        } catch (RtlException &x) {
            raise(x);
            return;
        }
        for (auto p: params) {
                 if (p == "uint8" )   { fci->types.push_back(&ffi_type_uint8 );  fci->marshal.push_back(marshal_number<uint8_t >); }
            else if (p == "sint8" )   { fci->types.push_back(&ffi_type_sint8 );  fci->marshal.push_back(marshal_number< int8_t >); }
            else if (p == "uint16")   { fci->types.push_back(&ffi_type_uint16);  fci->marshal.push_back(marshal_number<uint16_t>); }
            else if (p == "sint16")   { fci->types.push_back(&ffi_type_sint16);  fci->marshal.push_back(marshal_number< int16_t>); }
            else if (p == "uint32")   { fci->types.push_back(&ffi_type_uint32);  fci->marshal.push_back(marshal_number<uint32_t>); }
            else if (p == "sint32")   { fci->types.push_back(&ffi_type_sint32);  fci->marshal.push_back(marshal_number< int32_t>); }
            else if (p == "uint64")   { fci->types.push_back(&ffi_type_uint64);  fci->marshal.push_back(marshal_number<uint64_t>); }
            else if (p == "sint64")   { fci->types.push_back(&ffi_type_sint64);  fci->marshal.push_back(marshal_number< int64_t>); }
            else if (p == "float" )   { fci->types.push_back(&ffi_type_float );  fci->marshal.push_back(marshal_number<float   >); }
            else if (p == "double")   { fci->types.push_back(&ffi_type_double);  fci->marshal.push_back(marshal_number<double  >); }
            else if (p == "string")   { fci->types.push_back(&ffi_type_pointer); fci->marshal.push_back(marshal_string          ); }
            else if (p == "*string")  { fci->types.push_back(&ffi_type_pointer); fci->marshal.push_back(marshal_string_a        ); }
            else if (p == "bytes")    { fci->types.push_back(&ffi_type_pointer); fci->marshal.push_back(marshal_string          ); }
            else if (p == "*bytes")   { fci->types.push_back(&ffi_type_pointer); fci->marshal.push_back(marshal_string_a        ); }
            else if (p == "pointer")  { fci->types.push_back(&ffi_type_pointer); fci->marshal.push_back(marshal_pointer         ); }
            else if (p == "*pointer") { fci->types.push_back(&ffi_type_pointer); fci->marshal.push_back(marshal_pointer_a       ); }
            else {
                fprintf(stderr, "ffi type not supported: %s\n", p.c_str());
                exit(1);
            }
        }
        ffi_type *rtype;
             if (rettype == "boolean"){ rtype = &ffi_type_uint32; fci->unmarshal = unmarshal_boolean<uint32_t>; }
        else if (rettype == "uint8" ) { rtype = &ffi_type_uint8;  fci->unmarshal = unmarshal_number<uint8_t  >; }
        else if (rettype == "sint8" ) { rtype = &ffi_type_sint8;  fci->unmarshal = unmarshal_number< int8_t  >; }
        else if (rettype == "uint16") { rtype = &ffi_type_uint16; fci->unmarshal = unmarshal_number<uint16_t >; }
        else if (rettype == "sint16") { rtype = &ffi_type_sint16; fci->unmarshal = unmarshal_number< int16_t >; }
        else if (rettype == "uint32") { rtype = &ffi_type_uint32; fci->unmarshal = unmarshal_number<uint32_t >; }
        else if (rettype == "sint32") { rtype = &ffi_type_sint32; fci->unmarshal = unmarshal_number< int32_t >; }
        else if (rettype == "uint64") { rtype = &ffi_type_uint64; fci->unmarshal = unmarshal_number<uint64_t >; }
        else if (rettype == "sint64") { rtype = &ffi_type_sint64; fci->unmarshal = unmarshal_number< int64_t >; }
        else if (rettype == "float" ) { rtype = &ffi_type_float;  fci->unmarshal = unmarshal_number<float    >; }
        else if (rettype == "double") { rtype = &ffi_type_double; fci->unmarshal = unmarshal_number<double   >; }
        else if (rettype == "pointer"){ rtype = &ffi_type_pointer;fci->unmarshal = unmarshal_pointer; }
        else if (rettype == ""      ) { rtype = &ffi_type_void;   fci->unmarshal = nullptr;                     }
        else {
            fprintf(stderr, "ffi return type not supported: %s\n", rettype.c_str());
            exit(1);
        }
        ffi_status status = ffi_prep_cif(&fci->cif, FFI_DEFAULT_ABI, static_cast<unsigned int>(params.size()), rtype, fci->types.data());
        if (status != FFI_OK) {
            fprintf(stderr, "internal ffi error %d\n", status);
            exit(1);
        }
        if (val >= module->foreign_functions.size()) {
            module->foreign_functions.resize(val + 1);
        }
        module->foreign_functions[val] = fci;
    }
    std::vector<Cell> cells;
    char buf[1024];
    void *args[256];
    size_t size = sizeof(buf);
    void *p = buf;
    void *r = align(fci->cif.rtype->alignment, fci->cif.rtype->size, p, size);
    size_t i = fci->types.size();
    for (auto m = fci->marshal.rbegin(); m != fci->marshal.rend(); ++m) {
        i--;
        cells.push_back(stack.top());
        stack.pop();
        args[i] = (*m)(cells[cells.size()-1], p, size);
    }
    ffi_call(&fci->cif, fci->fp, r, args);
    if (fci->unmarshal != nullptr) {
        stack.push(fci->unmarshal(r));
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
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    std::vector<Cell> info = stack.top().array(); stack.pop();
    ExceptionInfo ei;
    size_t size = info.size();
    if (size >= 1) {
        ei.info = info[0].string();
    }
    if (size >= 2) {
        ei.code = info[1].number();
    }
    raise_literal(module->object.strtable[val], ei);
}

void Executor::exec_ALLOC()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    allocs.emplace_front(std::vector<Cell>(val), true);
    Cell *cell = &allocs.front();
    stack.push(Cell(cell));
    allocations++;
    if (param_garbage_collection_interval > 0 && allocations >= param_garbage_collection_interval) {
        // https://github.com/ghewgill/neon-lang/issues/180
        // garbage_collect();
    }
}

void Executor::exec_PUSHNIL()
{
    ip++;
    stack.push(Cell(static_cast<Cell *>(nullptr)));
}

void Executor::exec_JNASSERT()
{
    uint32_t target = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    if (not enable_assert) {
        ip = target;
    }
}

void Executor::exec_RESETC()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    *addr = Cell();
}

void Executor::exec_PUSHPEG()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    if (external_globals == nullptr) {
        fprintf(stderr, "internal error: no external globals\n");
        exit(1);
    }
    auto i = external_globals->find(module->object.strtable[val]);
    if (i == external_globals->end()) {
        fprintf(stderr, "internal error: external global does not exist: %s\n", module->object.strtable[val].c_str());
        exit(1);
    }
    stack.push(Cell(i->second));
}

void Executor::exec_JUMPTBL()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    Number n = stack.top().number(); stack.pop();
    if (number_is_integer(n)) {
        uint32_t i = number_to_uint32(n);
        if (i < val) {
            ip += 5 * i;
        } else {
            ip += 5 * val;
        }
    } else {
        ip += 5 * val;
    }
}

void Executor::exec_CALLX()
{
    ip++;
    uint32_t mod = (module->object.code[ip] << 24) | (module->object.code[ip+1] << 16) | (module->object.code[ip+2] << 8) | module->object.code[ip+3];
    ip += 4;
    uint32_t name = (module->object.code[ip] << 24) | (module->object.code[ip+1] << 16) | (module->object.code[ip+2] << 8) | module->object.code[ip+3];
    ip += 4;
    uint32_t out_param_count = (module->object.code[ip] << 24) | (module->object.code[ip+1] << 16) | (module->object.code[ip+2] << 8) | module->object.code[ip+3];
    ip += 4;
    std::string modname = module->object.strtable[mod];
    std::string modlib = just_path(module->object.source_path) + LIBRARY_NAME_PREFIX + "neon_" + modname;
    if (g_ExtensionModules.find(modname) == g_ExtensionModules.end()) {
        try {
            void_function_t init = rtl_foreign_function(modlib, "Ne_INIT");
            reinterpret_cast<int (*)(const Ne_MethodTable *)>(init)(&ExtensionMethodTable);
        } catch (RtlException &) {
            fprintf(stderr, "neon_exec: function Ne_INIT not found in %s\n", modlib.c_str());
            exit(1);
        }
        g_ExtensionModules.insert(modname);
    }
    std::string funcname = module->object.strtable[name];
    void_function_t p;
    try {
        p = rtl_foreign_function(modlib, "Ne_" + funcname);
    } catch (RtlException &) {
        fprintf(stderr, "neon_exec: function Ne_%s not found in %s\n", funcname.c_str(), modlib.c_str());
        exit(1);
    }
    Ne_ExtensionFunction f = reinterpret_cast<Ne_ExtensionFunction>(p);
    Cell out_params;
    out_params.array_for_write().resize(out_param_count);
    Cell retval;
    int r = f(reinterpret_cast<Ne_Cell *>(&retval), reinterpret_cast<Ne_ParameterList *>(&stack.top()), reinterpret_cast<Ne_ParameterList *>(&out_params));
    stack.pop();
    switch (r) {
        case Ne_SUCCESS: {
            stack.push(retval);
            for (auto &c: out_params.array()) {
                stack.push(c);
            }
            break;
        }
        case Ne_EXCEPTION: {
            utf8string exceptionname = Cell(retval.array()[0]).string();
            utf8string info = Cell(retval.array()[1]).string();
            Number code = Cell(retval.array()[2]).number();
            raise_literal(exceptionname, ExceptionInfo(info, code));
            break;
        }
        default:
            fprintf(stderr, "neon: invalid return value %d from extension function %s.%s\n", r, modname.c_str(), funcname.c_str());
            exit(1);
    }
}

void Executor::exec_SWAP()
{
    ip++;
    Cell a = stack.top(); stack.pop();
    Cell b = stack.top(); stack.pop();
    stack.push(a);
    stack.push(b);
}

void Executor::exec_DROPN()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    std::vector<Cell> hold;
    for (uint32_t i = 0; i < val; i++) {
        hold.push_back(stack.top());
        stack.pop();
    }
    stack.pop();
    for (auto i = hold.rbegin(); i != hold.rend(); ++i) {
        stack.push(*i);
    }
}

void Executor::exec_PUSHM()
{
    ip++;
    stack.push(Cell(reinterpret_cast<Cell *>(module)));
}

void Executor::exec_CALLV()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    std::vector<Cell> &pi = stack.top().array_for_write();
    Cell *instance = pi[0].address();
    size_t interface_index = number_to_uint32(pi[1].number());
    Module *m = reinterpret_cast<Module *>(instance->array_for_write()[0].array_for_write()[0].address());
    Bytecode::ClassInfo *classinfo = reinterpret_cast<Bytecode::ClassInfo *>(instance->array_for_write()[0].array_for_write()[1].address());
    stack.pop();
    callstack.push_back(std::make_pair(module, ip));
    module = m;
    ip = classinfo->interfaces[interface_index][val];
}

void Executor::exec_PUSHCI()
{
    uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    ip += 5;
    auto dot = module->object.strtable[val].find('.');
    if (dot == std::string::npos) {
        for (auto &c: module->object.classes) {
            if (c.name == val) {
                Cell ci;
                ci.array_for_write().push_back(Cell(reinterpret_cast<Cell *>(module)));
                ci.array_for_write().push_back(Cell(reinterpret_cast<Cell *>(&c)));
                stack.push(ci);
                return;
            }
        }
    } else {
        std::string modname = module->object.strtable[val].substr(0, dot);
        std::string methodname = module->object.strtable[val].substr(dot+1);
        auto mod = modules.find(modname);
        if (mod != modules.end()) {
            Module *m = mod->second;
            for (auto &c: m->object.classes) {
                if (m->object.strtable[c.name] == methodname) {
                    Cell ci;
                    ci.array_for_write().push_back(Cell(reinterpret_cast<Cell *>(m)));
                    ci.array_for_write().push_back(Cell(reinterpret_cast<Cell *>(&c)));
                    stack.push(ci);
                    return;
                }
            }
        }
    }
    fprintf(stderr, "neon: unknown class name %s\n", module->object.strtable[val].c_str());
    exit(1);
}

void Executor::raise_literal(const utf8string &exception, const ExceptionInfo &info)
{
    // The fields here must match the declaration of
    // ExceptionType in ast.cpp.
    Cell exceptionvar;
    exceptionvar.array_index_for_write(0) = Cell(exception);
    exceptionvar.array_index_for_write(1) = Cell(info.info);
    exceptionvar.array_index_for_write(2) = Cell(info.code);
    exceptionvar.array_index_for_write(3) = Cell(number_from_uint32(static_cast<uint32_t>(ip)));

    auto tmodule = module;
    auto tip = ip;
    size_t sp = callstack.size();
    for (;;) {
        for (auto e = tmodule->object.exceptions.begin(); e != tmodule->object.exceptions.end(); ++e) {
            if (tip >= e->start && tip < e->end) {
                const std::string handler = tmodule->object.strtable[e->excid];
                if (exception == handler
                 || (exception.length() > handler.length() && exception.substr(0, handler.length()) == handler && exception.at(handler.length()) == '.')) {
                    module = tmodule;
                    ip = e->handler;
                    callstack.resize(sp);
                    stack.push(exceptionvar);
                    return;
                }
            }
        }
        if (sp == 0) {
            break;
        }
        sp -= 1;
        // This unwinds the local variable frames as needed, but it's sort of a hack
        // because it has to avoid trying to pop any frames for top level code that's
        // not in a function. Better would be to somehow combine the return address
        // stack and the local variable frames so they are managed together.
        // See t/exception-unwind.neon and https://github.com/ghewgill/neon-lang/issues/189
        if (not frames.empty()) {
            frames.pop_back();
        }
        tmodule = callstack[sp].first;
        tip = callstack[sp].second;
    }

    fprintf(stderr, "Unhandled exception %s (%s) (code %d)\n", exception.c_str(), info.info.c_str(), number_to_sint32(info.code));
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

void Executor::raise(const ExceptionName &exception, const ExceptionInfo &info)
{
    raise_literal(exception.name, info);
}

void Executor::raise(const RtlException &x)
{
    raise_literal(x.name, ExceptionInfo(x.info, x.code));
}

void Executor::breakpoint()
{
    debugger_state = DebuggerState::STOPPED;
}

void Executor::log(const std::string &message)
{
    debugger_log.push_back(message);
}

static void mark(Cell *c)
{
    std::vector<Cell *> todo;
    todo.push_back(c);
    while (not todo.empty()) {
        c = todo.back();
        todo.pop_back();
        if (c == nullptr || (c->gc.alloced && c->gc.marked)) {
            continue;
        }
        c->gc.marked = true;
        switch (c->get_type()) {
            case Cell::Type::None:
            case Cell::Type::Boolean:
            case Cell::Type::Number:
            case Cell::Type::String:
            case Cell::Type::Bytes:
                // nothing
                break;
            case Cell::Type::Address:
                todo.push_back(c->address());
                break;
            case Cell::Type::Array:
                for (auto &x: c->array()) {
                    todo.push_back(const_cast<Cell *>(&x));
                }
                break;
            case Cell::Type::Dictionary:
                for (auto &x: c->dictionary()) {
                    todo.push_back(const_cast<Cell *>(&x.second));
                }
                break;
        }
    }
}

void Executor::garbage_collect()
{
    // Clear marked bits.
    for (Cell &c: allocs) {
        assert(c.gc.alloced);
        c.gc.marked = false;
    }

    // Mark reachable objects.
    for (auto m: modules) {
        for (auto &g: m.second->globals) {
            mark(&g);
        }
    }
    for (auto &f: frames) {
        for (auto &v: f.locals) {
            mark(&v);
        }
    }
    for (size_t i = 0; i < stack.depth(); i++) {
        mark(&stack.peek(i));
    }

    // Sweep unreachable objects.
    for (auto c = allocs.begin(); c != allocs.end(); ) {
        if (not c->gc.marked) {
            auto tmp = c;
            ++c;
            allocs.erase(tmp);
        } else {
            ++c;
        }
    }

    allocations = 0;
}

size_t Executor::get_allocated_object_count()
{
    return allocs.size();
}

bool Executor::module_is_main()
{
    return module == modules[""];
}

void Executor::set_garbage_collection_interval(size_t count)
{
    param_garbage_collection_interval = count;
}

void Executor::set_recursion_limit(size_t depth)
{
    param_recursion_limit = depth;
}

void Executor::exec()
{
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

    exec_loop(0);
    assert(stack.empty());
}

void Executor::exec_loop(size_t min_callstack_depth)
{
    while (callstack.size() > min_callstack_depth && ip < module->object.code.size()) {
        //std::cerr << "mod " << module->name << " ip " << ip << " op " << (int)module->object.code[ip] << " st " << stack.depth() << "\n";
        if (debug_server != nullptr) {
            switch (debugger_state) {
                case DebuggerState::STOPPED:
                    break;
                case DebuggerState::RUN:
                    if (debugger_breakpoints.find(ip) != debugger_breakpoints.end()) {
                        debugger_state = DebuggerState::STOPPED;
                    }
                    break;
                case DebuggerState::STEP_INSTRUCTION:
                    debugger_state = DebuggerState::STOPPED;
                    break;
                case DebuggerState::STEP_SOURCE:
                    if (callstack.size() <= debugger_step_source_depth && module->debug != nullptr && module->debug->line_numbers.find(ip) != module->debug->line_numbers.end()) {
                        debugger_state = DebuggerState::STOPPED;
                    }
                    break;
                case DebuggerState::QUIT:
                    break;
            }
            debug_server->service(false);
            while (debugger_state == DebuggerState::STOPPED) {
                debug_server->service(true);
            }
            if (debugger_state == DebuggerState::QUIT) {
                return;
            }
        }
        switch (static_cast<Opcode>(module->object.code[ip])) {
            case ENTER:   exec_ENTER(); break;
            case LEAVE:   exec_LEAVE(); break;
            case PUSHB:   exec_PUSHB(); break;
            case PUSHN:   exec_PUSHN(); break;
            case PUSHS:   exec_PUSHS(); break;
            case PUSHT:   exec_PUSHT(); break;
            case PUSHPG:  exec_PUSHPG(); break;
            case PUSHPPG: exec_PUSHPPG(); break;
            case PUSHPMG: exec_PUSHPMG(); break;
            case PUSHPL:  exec_PUSHPL(); break;
            case PUSHPOL: exec_PUSHPOL(); break;
            case PUSHI:   exec_PUSHI(); break;
            case LOADB:   exec_LOADB(); break;
            case LOADN:   exec_LOADN(); break;
            case LOADS:   exec_LOADS(); break;
            case LOADT:   exec_LOADT(); break;
            case LOADA:   exec_LOADA(); break;
            case LOADD:   exec_LOADD(); break;
            case LOADP:   exec_LOADP(); break;
            case STOREB:  exec_STOREB(); break;
            case STOREN:  exec_STOREN(); break;
            case STORES:  exec_STORES(); break;
            case STORET:  exec_STORET(); break;
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
            case EQT:     exec_EQT(); break;
            case NET:     exec_NET(); break;
            case LTT:     exec_LTT(); break;
            case GTT:     exec_GTT(); break;
            case LET_:    exec_LET(); break;
            case GET:     exec_GET(); break;
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
            case CALLI:   exec_CALLI(); break;
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
            case ALLOC:   exec_ALLOC(); break;
            case PUSHNIL: exec_PUSHNIL(); break;
            case JNASSERT:exec_JNASSERT(); break;
            case RESETC:  exec_RESETC(); break;
            case PUSHPEG: exec_PUSHPEG(); break;
            case JUMPTBL: exec_JUMPTBL(); break;
            case CALLX:   exec_CALLX(); break;
            case SWAP:    exec_SWAP(); break;
            case DROPN:   exec_DROPN(); break;
            case PUSHM:   exec_PUSHM(); break;
            case CALLV:   exec_CALLV(); break;
            case PUSHCI:  exec_PUSHCI(); break;
            default:
                fprintf(stderr, "exec: Unexpected opcode: %d\n", module->object.code[ip]);
                abort();
        }
    }
}

namespace minijson {

template <> struct default_value_writer<Number> {
    void operator()(std::ostream &stream, const Number &n) {
        detail::write_quoted_string(stream, number_to_string(n).c_str());
    }
};

template <> struct default_value_writer<Cell> {
    void operator()(std::ostream &stream, const Cell &ccell, writer_configuration configuration) {
        object_writer writer(stream, configuration);
        // Parameter needs to be `const`, but we need to call (benign) non-const methods.
        Cell &cell = const_cast<Cell &>(ccell);
        switch (cell.get_type()) {
            case Cell::Type::None:
                writer.write("type", "none");
                writer.write("value", nullptr);
                break;
            case Cell::Type::Address:
                writer.write("type", "address");
                writer.write("value", std::to_string(reinterpret_cast<intptr_t>(cell.address())));
                break;
            case Cell::Type::Boolean:
                writer.write("type", "boolean");
                writer.write("value", cell.boolean());
                break;
            case Cell::Type::Number:
                writer.write("type", "number");
                writer.write("value", cell.number());
                break;
            case Cell::Type::String:
                writer.write("type", "string");
                writer.write("value", cell.string().str());
                break;
            case Cell::Type::Bytes:
                writer.write("type", "bytes");
                writer.write("value", std::string(reinterpret_cast<const char *>(cell.bytes().data()), cell.bytes().size()));
                break;
            case Cell::Type::Array: {
                writer.write("type", "array");
                writer.write_array("value", cell.array_for_write().begin(), cell.array_for_write().end());
                break;
            }
            case Cell::Type::Dictionary: {
                writer.write("type", "dictionary");
                auto d = writer.nested_object("value");
                for (auto &x: cell.dictionary_for_write()) {
                    d.write(x.first.str().c_str(), x.second);
                }
                d.close();
                break;
            }
        }
        writer.close();
    }
};

} // namespace minijson

void Executor::handle_GET(const std::string &path, HttpResponse &response)
{
    std::stringstream r;
    minijson::writer_configuration config = minijson::writer_configuration().pretty_printing(true);
    std::vector<std::string> parts = split(path, '/');
    if (path == "/break") {
        response.code = 200;
        minijson::array_writer writer(r, config);
        for (auto b: debugger_breakpoints) {
            writer.write(b);
        }
        writer.close();
    } else if (path == "/callstack") {
        response.code = 200;
        minijson::array_writer writer(r, config);
        for (auto i = callstack.rbegin(); i != callstack.rend(); ++i) {
            auto w = writer.nested_object();
            w.write("module", i->first->name);
            w.write("ip", i->second);
            w.close();
        }
        writer.close();
    } else if (path == "/frames") {
        response.code = 200;
        minijson::array_writer writer(r, config);
        for (auto i = frames.rbegin(); i != frames.rend(); ++i) {
            auto wf = writer.nested_object();
            auto wl = wf.nested_array("locals");
            for (auto &local: i->locals) {
                wl.write(local);
            }
            wl.close();
            wf.close();
        }
        writer.close();
    } else if (parts.size() >= 3 && parts[1] == "module") {
        std::string modname = parts[2];
        if (modname == "-") {
            modname = "";
        }
        Module *m = modules[modname];
        if (parts.size() == 4 && parts[3] == "bytecode") {
            response.code = 200;
            minijson::write_array(r, m->object.obj.begin(), m->object.obj.end());
        } else if (parts.size() == 4 && parts[3] == "debuginfo") {
            response.code = 200;
            std::ifstream di(m->debug->source_path + "d");
            r << di.rdbuf();
        } else if (parts.size() == 5 && parts[3] == "global") {
            response.code = 200;
            minijson::default_value_writer<Cell>()(r, m->globals[std::stoi(parts[4])], config);
        } else if (parts.size() == 4 && parts[3] == "source") {
            response.code = 200;
            minijson::write_array(r, m->debug->source_lines.begin(), m->debug->source_lines.end());
        }
    } else if (path == "/modules") {
        response.code = 200;
        std::vector<std::string> names;
        for (auto m: modules) {
            names.push_back(m.first);
        }
        minijson::write_array(r, names.begin(), names.end());
    } else if (path == "/opstack") {
        response.code = 200;
        minijson::write_array(r, stack.begin(), stack.end(), config);
    } else if (path == "/status") {
        response.code = 200;
        minijson::object_writer writer(r, config);
        writer.write("state", DebuggerStateName[static_cast<int>(debugger_state)]);
        writer.write("module", module->name);
        writer.write("ip", ip);
        writer.write("log_messages", debugger_log.size());
        writer.close();
    }
    if (response.code == 0) {
        response.code = 404;
        r << "[debug server] path not found: " << path;
    }
    response.content = r.str();
}

void Executor::handle_POST(const std::string &path, const std::string &data, HttpResponse &response)
{
    std::stringstream r;
    minijson::writer_configuration config = minijson::writer_configuration().pretty_printing(true);
    std::vector<std::string> parts = split(path, '/');
    if (parts.size() == 3 && parts[1] == "break") {
        response.code = 200;
        auto addr = std::stoi(parts[2]);
        if (data == "true") {
            debugger_breakpoints.insert(addr);
        } else {
            debugger_breakpoints.erase(addr);
        }
    } else if (path == "/continue") {
        response.code = 200;
        debugger_state = DebuggerState::RUN;
    } else if (path == "/log") {
        response.code = 200;
        minijson::write_array(r, debugger_log.begin(), debugger_log.end(), config);
        debugger_log.clear();
    } else if (path == "/quit") {
        response.code = 200;
        debugger_state = DebuggerState::QUIT;
    } else if (path == "/step/instruction") {
        response.code = 200;
        debugger_state = DebuggerState::STEP_INSTRUCTION;
    } else if (parts.size() == 4 && parts[1] == "step" && parts[2] == "source") {
        response.code = 200;
        debugger_state = DebuggerState::STEP_SOURCE;
        debugger_step_source_depth = callstack.size() + std::stoi(parts[3]);
    } else if (path == "/stop") {
        response.code = 200;
        debugger_state = DebuggerState::STOPPED;
    }
    if (response.code == 0) {
        response.code = 404;
        r << "[debug server] path not found: " << path;
    }
    response.content = r.str();
}

void executor_breakpoint()
{
    g_executor->breakpoint();
}

void executor_log(const std::string &message)
{
    g_executor->log(message);
}

void executor_garbage_collect()
{
    g_executor->garbage_collect();
}

size_t executor_get_allocated_object_count()
{
    return g_executor->get_allocated_object_count();
}

bool executor_module_is_main()
{
    return g_executor->module_is_main();
}

void executor_set_garbage_collection_interval(size_t count)
{
    g_executor->set_garbage_collection_interval(count);
}

void executor_set_recursion_limit(size_t depth)
{
    g_executor->set_recursion_limit(depth);
}

void exec(const std::string &source_path, const Bytecode::Bytes &obj, const DebugInfo *debuginfo, ICompilerSupport *support, bool enable_assert, unsigned short debug_port, int argc, char *argv[], std::map<std::string, Cell *> *external_globals)
{
    rtl_exec_init(argc, argv);
    Executor(source_path, obj, debuginfo, support, enable_assert, debug_port, external_globals).exec();
}
