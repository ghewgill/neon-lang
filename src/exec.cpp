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

#include <minijson_writer.hpp>

#include "bytecode.h"
#include "cell.h"
#include "debuginfo.h"
#include "disassembler.h"
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

void executor_raise_exception(size_t ip, const utf8string &name, const utf8string &info);

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

// This class is specific to how the BID library notifies its exceptions.
class BidExceptionHandler {
    size_t start_ip;
public:
    BidExceptionHandler(size_t start_ip): start_ip(start_ip) {
        _IDEC_glbflags = 0;
    }
    void check_and_raise(const char *what) {
        if (_IDEC_glbflags & BID_OVERFLOW_EXCEPTION) {
            executor_raise_exception(start_ip, utf8string(rtl::ne_global::Exception_NumberException_Overflow.name), utf8string(what));
        }
        if (_IDEC_glbflags & BID_ZERO_DIVIDE_EXCEPTION) {
            executor_raise_exception(start_ip, utf8string(rtl::ne_global::Exception_NumberException_DivideByZero.name), utf8string(what));
        }
        if (_IDEC_glbflags & BID_INVALID_EXCEPTION) {
            executor_raise_exception(start_ip, utf8string(rtl::ne_global::Exception_NumberException_Invalid.name), utf8string(what));
        }
    }
};

} // namespace

#ifdef _MSC_VER
#define alignof(T) sizeof(T)
#endif

class ActivationFrame {
public:
    ActivationFrame(uint32_t nesting_depth, ActivationFrame *outer, size_t count, size_t opstack_depth): nesting_depth(nesting_depth), outer(outer), locals(count), opstack_depth(opstack_depth) {}
    ActivationFrame(const ActivationFrame &) = delete;
    ActivationFrame &operator=(const ActivationFrame &) = delete;
    uint32_t nesting_depth;
    ActivationFrame *outer;
    std::vector<Cell> locals;
    size_t opstack_depth;
};

class Executor;

class Module {
public:
    Module(const std::string &name, const Bytecode &object, const DebugInfo *debuginfo, Executor *executor, ICompilerSupport *support);
    Module(const Module &) = delete;
    Module &operator=(const Module &) = delete;
    const std::string name;
    Bytecode object;
    const DebugInfo *debug;
    std::vector<Cell> globals;
    std::vector<size_t> rtl_call_tokens;
    std::vector<std::pair<bool, Number>> number_table;
    std::map<std::pair<std::string, std::string>, std::pair<Module *, int>> module_functions;
};

class Executor: public IHttpServerHandler {
public:
    Executor(const std::string &source_path, const Bytecode::Bytes &bytes, const DebugInfo *debuginfo, ICompilerSupport *support, const ExecOptions *options, unsigned short debug_port, std::map<std::string, Cell *> *external_globals);
    Executor(const Executor &) = delete;
    Executor &operator=(const Executor &) = delete;
    virtual ~Executor();

    // Module: debugger
    void breakpoint();
    void log(const std::string &message);

    // Module: runtime
    void garbage_collect();
    size_t get_allocated_object_count();
    bool is_module_imported(const std::string &module);
    bool module_is_main();
    void set_garbage_collection_interval(size_t count);
    void set_recursion_limit(size_t depth);

    int exec();
    int exec_loop(size_t min_callstack_depth);
//private:
    const std::string source_path;
    const ExecOptions *options;

    size_t param_garbage_collection_interval;
    size_t param_recursion_limit;

    std::map<std::string, Cell *> *external_globals;
    std::map<std::string, Module *> modules;
    std::vector<std::string> init_order;
    Module *module;
    int exit_code;
    Bytecode::Bytes::size_type ip;
    opstack<Cell> stack;
    std::vector<std::pair<Module *, Bytecode::Bytes::size_type>> callstack;
    std::list<ActivationFrame> frames;

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

    void exec_PUSHB();
    void exec_PUSHN();
    void exec_PUSHS();
    void exec_PUSHY();
    void exec_PUSHPG();
    void exec_PUSHPPG();
    void exec_PUSHPMG();
    void exec_PUSHPL();
    void exec_PUSHPOL();
    void exec_PUSHI();
    void exec_LOADB();
    void exec_LOADN();
    void exec_LOADS();
    void exec_LOADY();
    void exec_LOADA();
    void exec_LOADD();
    void exec_LOADP();
    void exec_LOADJ();
    void exec_LOADV();
    void exec_STOREB();
    void exec_STOREN();
    void exec_STORES();
    void exec_STOREY();
    void exec_STOREA();
    void exec_STORED();
    void exec_STOREP();
    void exec_STOREJ();
    void exec_STOREV();
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
    void exec_EQY();
    void exec_NEY();
    void exec_LTY();
    void exec_GTY();
    void exec_LEY();
    void exec_GEY();
    void exec_EQA();
    void exec_NEA();
    void exec_EQD();
    void exec_NED();
    void exec_EQP();
    void exec_NEP();
    void exec_EQV();
    void exec_NEV();
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
    void exec_DUP();
    void exec_DUPX1();
    void exec_DROP();
    void exec_RET();
    void exec_CONSA();
    void exec_CONSD();
    void exec_EXCEPT();
    void exec_ALLOC();
    void exec_PUSHNIL();
    void exec_RESETC();
    void exec_PUSHPEG();
    void exec_JUMPTBL();
    void exec_CALLX();
    void exec_SWAP();
    void exec_DROPN();
    void exec_PUSHFP();
    void exec_CALLV();
    void exec_PUSHCI();

    void invoke(Module *m, uint32_t index);
    void raise_literal(const utf8string &exception, std::shared_ptr<Object> info);
    void raise(const ExceptionName &exception, std::shared_ptr<Object> info);
    void raise(const RtlException &x);

    virtual void handle_GET(const std::string &path, HttpResponse &response);
    virtual void handle_POST(const std::string &path, const std::string &data, HttpResponse &response);

    friend class Module;
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
    reinterpret_cast<Cell *>(cell)->string_for_write() = utf8string(value);
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
    reinterpret_cast<Cell *>(cell)->set_bytes(std::vector<unsigned char>(value, value+size));
}

void *cell_get_pointer(const struct Ne_Cell *cell)
{
    return reinterpret_cast<Cell *>(const_cast<struct Ne_Cell *>(cell))->other();
}

void cell_set_pointer(struct Ne_Cell *cell, void *p)
{
    reinterpret_cast<Cell *>(cell)->other() = p;
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
    return reinterpret_cast<const struct Ne_Cell *>(&reinterpret_cast<Cell *>(const_cast<struct Ne_Cell *>(cell))->dictionary_index_for_read(utf8string(key)));
}

struct Ne_Cell *cell_set_dictionary_cell(struct Ne_Cell *cell, const char *key)
{
    return reinterpret_cast<struct Ne_Cell *>(&reinterpret_cast<Cell *>(cell)->dictionary_index_for_write(utf8string(key)));
}

void exec_callback(const struct Ne_Cell *callback, const struct Ne_ParameterList *params, struct Ne_Cell *retval)
{
    // TODO: move this into a method in Executor that's called by exec_CALLI too
    if (g_executor->callstack.size() >= g_executor->param_recursion_limit) {
        g_executor->raise(rtl::ne_global::Exception_StackOverflowException, std::make_shared<ObjectString>(utf8string("")));
        return;
    }
    std::vector<Cell> a = reinterpret_cast<Cell *>(const_cast<struct Ne_Cell *>(callback))->array();
    Module *mod = reinterpret_cast<Module *>(a[0].other());
    Number nindex = a[1].number();
    if (mod == nullptr || number_is_zero(nindex) || not number_is_integer(nindex)) {
        g_executor->raise(rtl::ne_global::Exception_InvalidFunctionException, std::make_shared<ObjectString>(utf8string("")));
        return;
    }
    if (params != NULL) {
        auto &ps = reinterpret_cast<Cell *>(const_cast<struct Ne_ParameterList *>(params))->array();
        for (auto i = ps.rbegin(); i != ps.rend(); ++i) {
            g_executor->stack.push(*i);
        }
    }
    uint32_t index = number_to_uint32(nindex);
    g_executor->invoke(mod, index);
    int r = g_executor->exec_loop(g_executor->callstack.size() - 1);
    if (r != 0) {
        exit(r);
    }
    if (retval != NULL) {
        *reinterpret_cast<Cell *>(retval) = g_executor->stack.top();
        g_executor->stack.pop();
    }
}

int raise_exception(struct Ne_Cell *retval, const char *name, const char *info, int /*code*/)
{
    Cell *r = reinterpret_cast<Cell *>(retval);
    r->array_for_write().resize(2);
    r->array_for_write()[0].string_for_write() = utf8string(name);
    r->array_for_write()[1] = Cell(std::make_shared<ObjectString>(utf8string(info)));
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

Executor::Executor(const std::string &source_path, const Bytecode::Bytes &bytes, const DebugInfo *debuginfo, ICompilerSupport *support, const ExecOptions *options, unsigned short debug_port, std::map<std::string, Cell *> *external_globals)
  : source_path(source_path),
    options(options),
    param_garbage_collection_interval(1000),
    param_recursion_limit(1000),
    external_globals(external_globals),
    modules(),
    init_order(),
    module(nullptr),
    exit_code(0),
    ip(0),
    stack(),
    callstack(),
    frames(),
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
    try {
        b.load(source_path, bytes);
    } catch (BytecodeException &e) {
        fprintf(stderr, "error loading bytecode: %s\n", e.what());
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
    module_functions()
{
    for (auto i: object.imports) {
        std::string importname = object.strtable[i.name];
        if (executor->modules.find(importname) != executor->modules.end()) {
            continue;
        }
        Bytecode code;
        try {
            support->loadBytecode(importname, code);
        } catch (BytecodeException &e) {
            if (i.optional) {
                continue;
            } else {
                fprintf(stderr, "couldn't load module: %s: %s\n", importname.c_str(), e.what());
                exit(1);
            }
        }
        // TODO: check hash of exports
        executor->modules[importname] = nullptr; // Prevent unwanted recursion.
        executor->modules[importname] = new Module(importname, code, nullptr, executor, support);
        executor->init_order.push_back(importname);
    }
}

inline void dump_frames(Executor *exec)
{
    if (false) {
        printf("Frames:\n");
        for (auto &f: exec->frames) {
            printf("  %p { nest=%u outer=%p locals=%zu opstack_depth=%zu }\n", &f, f.nesting_depth, f.outer, f.locals.size(), f.opstack_depth);
        }
    }
}

void Executor::exec_PUSHB()
{
    bool val = module->object.code[ip+1] != 0;
    ip += 2;
    stack.push(Cell(val));
}

void Executor::exec_PUSHN()
{
    ip++;
    uint32_t val = Bytecode::get_vint(module->object.code, ip);
    if (not module->number_table[val].first) {
        module->number_table[val] = std::make_pair(true, number_from_string(module->object.strtable[val]));
    }
    stack.push(Cell(module->number_table[val].second));
}

void Executor::exec_PUSHS()
{
    ip++;
    uint32_t val = Bytecode::get_vint(module->object.code, ip);
    stack.push(Cell(utf8string(module->object.strtable[val])));
}

void Executor::exec_PUSHY()
{
    ip++;
    uint32_t val = Bytecode::get_vint(module->object.code, ip);
    stack.push(Cell(std::vector<unsigned char>(reinterpret_cast<const unsigned char *>(module->object.strtable[val].data()), reinterpret_cast<const unsigned char *>(module->object.strtable[val].data()) + module->object.strtable[val].size())));
}

void Executor::exec_PUSHPG()
{
    ip++;
    uint32_t addr = Bytecode::get_vint(module->object.code, ip);
    assert(addr < module->globals.size());
    stack.push(Cell(&module->globals.at(addr)));
}

void Executor::exec_PUSHPPG()
{
    ip++;
    uint32_t name = Bytecode::get_vint(module->object.code, ip);
    stack.push(Cell(rtl_variable(module->object.strtable[name])));
}

void Executor::exec_PUSHPMG()
{
    ip++;
    uint32_t mod = Bytecode::get_vint(module->object.code, ip);
    uint32_t name = Bytecode::get_vint(module->object.code, ip);
    auto m = modules.find(module->object.strtable[mod]);
    if (m == modules.end()) {
        fprintf(stderr, "fatal: module not found: %s\n", module->object.strtable[mod].c_str());
        exit(1);
    }
    for (auto &v: m->second->object.export_variables) {
        if (m->second->object.strtable[v.name] == module->object.strtable[name]) {
            assert(v.index < m->second->globals.size());
            stack.push(Cell(&m->second->globals.at(v.index)));
            return;
        }
    }
    fprintf(stderr, "fatal: module variable not found: %s\n", module->object.strtable[name].c_str());
    exit(1);
}

void Executor::exec_PUSHPL()
{
    ip++;
    uint32_t addr = Bytecode::get_vint(module->object.code, ip);
    stack.push(Cell(&frames.back().locals.at(addr)));
}

void Executor::exec_PUSHPOL()
{
    ip++;
    uint32_t back = Bytecode::get_vint(module->object.code, ip);
    uint32_t addr = Bytecode::get_vint(module->object.code, ip);
    dump_frames(this);
    ActivationFrame *frame = &frames.back();
    while (back > 0) {
        frame = frame->outer;
        back--;
    }
    stack.push(Cell(&frame->locals.at(addr)));
}

void Executor::exec_PUSHI()
{
    ip++;
    uint32_t x = Bytecode::get_vint(module->object.code, ip);
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

void Executor::exec_LOADY()
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

void Executor::exec_LOADJ()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    stack.push(Cell(addr->object()));
}

void Executor::exec_LOADV()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    stack.push(Cell::makeOther(addr->other()));
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

void Executor::exec_STOREY()
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

void Executor::exec_STOREJ()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    *addr = stack.top(); stack.pop();
}

void Executor::exec_STOREV()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    *addr = stack.top(); stack.pop();
}

void Executor::exec_NEGN()
{
    BidExceptionHandler handler(ip);
    ip++;
    Number x = stack.top().number(); stack.pop();
    stack.push(Cell(number_negate(x)));
    handler.check_and_raise("negate");
}

void Executor::exec_ADDN()
{
    BidExceptionHandler handler(ip);
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    stack.push(Cell(number_add(a, b)));
    handler.check_and_raise("add");
}

void Executor::exec_SUBN()
{
    BidExceptionHandler handler(ip);
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    stack.push(Cell(number_subtract(a, b)));
    handler.check_and_raise("subtract");
}

void Executor::exec_MULN()
{
    BidExceptionHandler handler(ip);
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    stack.push(Cell(number_multiply(a, b)));
    handler.check_and_raise("multiply");
}

void Executor::exec_DIVN()
{
    BidExceptionHandler handler(ip);
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    stack.push(Cell(number_divide(a, b)));
    handler.check_and_raise("divide");
}

void Executor::exec_MODN()
{
    BidExceptionHandler handler(ip);
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    stack.push(Cell(number_modulo(a, b)));
    handler.check_and_raise("modulo");
}

void Executor::exec_EXPN()
{
    BidExceptionHandler handler(ip);
    ip++;
    Number b = stack.top().number(); stack.pop();
    Number a = stack.top().number(); stack.pop();
    stack.push(Cell(number_pow(a, b)));
    handler.check_and_raise("exponentiation");
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

void Executor::exec_EQY()
{
    ip++;
    std::vector<unsigned char> b = stack.top().bytes(); stack.pop();
    std::vector<unsigned char> a = stack.top().bytes(); stack.pop();
    stack.push(Cell(a == b));
}

void Executor::exec_NEY()
{
    ip++;
    std::vector<unsigned char> b = stack.top().bytes(); stack.pop();
    std::vector<unsigned char> a = stack.top().bytes(); stack.pop();
    stack.push(Cell(a != b));
}

void Executor::exec_LTY()
{
    ip++;
    std::vector<unsigned char> b = stack.top().bytes(); stack.pop();
    std::vector<unsigned char> a = stack.top().bytes(); stack.pop();
    stack.push(Cell(a < b));
}

void Executor::exec_GTY()
{
    ip++;
    std::vector<unsigned char> b = stack.top().bytes(); stack.pop();
    std::vector<unsigned char> a = stack.top().bytes(); stack.pop();
    stack.push(Cell(a > b));
}

void Executor::exec_LEY()
{
    ip++;
    std::vector<unsigned char> b = stack.top().bytes(); stack.pop();
    std::vector<unsigned char> a = stack.top().bytes(); stack.pop();
    stack.push(Cell(a <= b));
}

void Executor::exec_GEY()
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

void Executor::exec_EQV()
{
    ip++;
    void *b = stack.top().other(); stack.pop();
    void *a = stack.top().other(); stack.pop();
    stack.push(Cell(a == b));
}

void Executor::exec_NEV()
{
    ip++;
    void *b = stack.top().other(); stack.pop();
    void *a = stack.top().other(); stack.pop();
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
        raise(rtl::ne_global::Exception_ArrayIndexException, std::make_shared<ObjectString>(utf8string(number_to_string(index))));
        return;
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        raise(rtl::ne_global::Exception_ArrayIndexException, std::make_shared<ObjectString>(utf8string(number_to_string(index))));
        return;
    }
    uint64_t j = static_cast<uint64_t>(i);
    if (j >= addr->array().size()) {
        raise(rtl::ne_global::Exception_ArrayIndexException, std::make_shared<ObjectString>(utf8string(number_to_string(index))));
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
        raise(rtl::ne_global::Exception_ArrayIndexException, std::make_shared<ObjectString>(utf8string(number_to_string(index))));
        return;
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        raise(rtl::ne_global::Exception_ArrayIndexException, std::make_shared<ObjectString>(utf8string(number_to_string(index))));
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
        raise(rtl::ne_global::Exception_ArrayIndexException, std::make_shared<ObjectString>(utf8string(number_to_string(index))));
        return;
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        raise(rtl::ne_global::Exception_ArrayIndexException, std::make_shared<ObjectString>(utf8string(number_to_string(index))));
        return;
    }
    uint64_t j = static_cast<uint64_t>(i);
    if (j >= array.size()) {
        raise(rtl::ne_global::Exception_ArrayIndexException, std::make_shared<ObjectString>(utf8string(number_to_string(index))));
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
        raise(rtl::ne_global::Exception_ArrayIndexException, std::make_shared<ObjectString>(utf8string(number_to_string(index))));
        return;
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        raise(rtl::ne_global::Exception_ArrayIndexException, std::make_shared<ObjectString>(utf8string(number_to_string(index))));
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
        raise(rtl::ne_global::Exception_DictionaryIndexException, std::make_shared<ObjectString>(index));
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
        raise(rtl::ne_global::Exception_DictionaryIndexException, std::make_shared<ObjectString>(index));
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
    const size_t start_ip = ip;
    ip++;
    uint32_t val = Bytecode::get_vint(module->object.code, ip);
    std::string func = module->object.strtable.at(val);
    try {
        BidExceptionHandler handler(start_ip);
        rtl_call(stack, func, module->rtl_call_tokens[val]);
        handler.check_and_raise(func.c_str());
    } catch (RtlException &x) {
        ip = start_ip;
        raise(x);
    }
}

void Executor::exec_CALLF()
{
    ip++;
    uint32_t val = Bytecode::get_vint(module->object.code, ip);
    if (callstack.size() >= param_recursion_limit) {
        raise(rtl::ne_global::Exception_StackOverflowException, std::make_shared<ObjectString>(utf8string("")));
        return;
    }
    invoke(module, val);
}

void Executor::exec_CALLMF()
{
    ip++;
    uint32_t mod = Bytecode::get_vint(module->object.code, ip);
    uint32_t func = Bytecode::get_vint(module->object.code, ip);
    if (callstack.size() >= param_recursion_limit) {
        raise(rtl::ne_global::Exception_StackOverflowException, std::make_shared<ObjectString>(utf8string("")));
        return;
    }
    auto f = module->module_functions.find(std::make_pair(module->object.strtable[mod], module->object.strtable[func]));
    if (f != module->module_functions.end()) {
        invoke(f->second.first, f->second.second);
    } else {
        auto m = modules.find(module->object.strtable[mod]);
        if (m == modules.end()) {
            fprintf(stderr, "fatal: module not found: %s\n", module->object.strtable[mod].c_str());
            exit(1);
        }
        for (auto ef: m->second->object.export_functions) {
            if (m->second->object.strtable[ef.name] + "," + m->second->object.strtable[ef.descriptor] == module->object.strtable[func]) {
                module->module_functions[std::make_pair(module->object.strtable[mod], module->object.strtable[func])] = std::make_pair(m->second, ef.index);
                invoke(m->second, ef.index);
                return;
            }
        }
        fprintf(stderr, "fatal: module function not found: %s\n", module->object.strtable[func].c_str());
        exit(1);
    }
}

void Executor::exec_CALLI()
{
    ip++;
    if (callstack.size() >= param_recursion_limit) {
        raise(rtl::ne_global::Exception_StackOverflowException, std::make_shared<ObjectString>(utf8string("")));
        return;
    }
    std::vector<Cell> a = stack.top().array(); stack.pop();
    Module *mod = reinterpret_cast<Module *>(a[0].other());
    Number nindex = a[1].number();
    if (number_is_zero(nindex) || not number_is_integer(nindex)) {
        raise(rtl::ne_global::Exception_InvalidFunctionException, std::make_shared<ObjectString>(utf8string("")));
        return;
    }
    uint32_t index = number_to_uint32(nindex);
    invoke(mod, index);
}

void Executor::exec_JUMP()
{
    ip++;
    uint32_t target = Bytecode::get_vint(module->object.code, ip);
    ip = target;
}

void Executor::exec_JF()
{
    ip++;
    uint32_t target = Bytecode::get_vint(module->object.code, ip);
    bool a = stack.top().boolean(); stack.pop();
    if (not a) {
        ip = target;
    }
}

void Executor::exec_JT()
{
    ip++;
    uint32_t target = Bytecode::get_vint(module->object.code, ip);
    bool a = stack.top().boolean(); stack.pop();
    if (a) {
        ip = target;
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
    frames.pop_back();
    module = callstack.back().first;
    ip = callstack.back().second;
    callstack.pop_back();
}

void Executor::exec_CONSA()
{
    ip++;
    uint32_t val = Bytecode::get_vint(module->object.code, ip);
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
    ip++;
    uint32_t val = Bytecode::get_vint(module->object.code, ip);
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
    const size_t start_ip = ip;
    ip++;
    uint32_t val = Bytecode::get_vint(module->object.code, ip);
    ip = start_ip;
    std::shared_ptr<Object> info = stack.top().object(); stack.pop();
    raise_literal(utf8string(module->object.strtable[val]), info);
}

void Executor::exec_ALLOC()
{
    ip++;
    uint32_t val = Bytecode::get_vint(module->object.code, ip);
    allocs.emplace_front(std::vector<Cell>(val), true);
    Cell *cell = &allocs.front();
    stack.push(Cell(cell));
    allocations++;
    if (param_garbage_collection_interval > 0 && allocations >= param_garbage_collection_interval) {
        garbage_collect();
    }
}

void Executor::exec_PUSHNIL()
{
    ip++;
    stack.push(Cell());
}

void Executor::exec_RESETC()
{
    ip++;
    Cell *addr = stack.top().address(); stack.pop();
    *addr = Cell();
}

void Executor::exec_PUSHPEG()
{
    ip++;
    uint32_t val = Bytecode::get_vint(module->object.code, ip);
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
    ip++;
    uint32_t val = Bytecode::get_vint(module->object.code, ip);
    Number n = stack.top().number(); stack.pop();
    if (number_is_integer(n) && not number_is_negative(n)) {
        uint32_t i = number_to_uint32(n);
        if (i < val) {
            ip += 6 * i;
        } else {
            ip += 6 * val;
        }
    } else {
        ip += 6 * val;
    }
}

void Executor::exec_CALLX()
{
    ip++;
    uint32_t mod = Bytecode::get_vint(module->object.code, ip);
    uint32_t name = Bytecode::get_vint(module->object.code, ip);
    uint32_t out_param_count = Bytecode::get_vint(module->object.code, ip);
    std::string modname = module->object.strtable[mod];
    std::string modlib = just_path(module->object.source_path) + LIBRARY_NAME_PREFIX + "neon_" + modname;
    if (g_ExtensionModules.find(modname) == g_ExtensionModules.end()) {
        try {
            void_function_t init = rtl_foreign_function(modlib, "Ne_INIT");
            reinterpret_cast<int (*)(const Ne_MethodTable *)>(init)(&ExtensionMethodTable);
        } catch (RtlException &e) {
            if (e.name == rtl::ne_global::Exception_LibraryNotFoundException.name) {
                fprintf(stderr, "neon_exec: library %s not found\n", modlib.c_str());
            } else if (e.name == rtl::ne_global::Exception_FunctionNotFoundException.name) {
                fprintf(stderr, "neon_exec: function Ne_INIT not found in %s\n", modlib.c_str());
            } else {
                fprintf(stderr, "neon_exec: unknown exception %s while finding Ne_INIT in %s\n", e.name.c_str(), modlib.c_str());
            }
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
            std::shared_ptr<Object> info = Cell(retval.array()[1]).object();
            raise_literal(exceptionname, info);
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
    ip++;
    uint32_t val = Bytecode::get_vint(module->object.code, ip);
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

void Executor::exec_PUSHFP()
{
    ip++;
    uint32_t val = Bytecode::get_vint(module->object.code, ip);
    std::vector<Cell> a = {Cell::makeOther(module), Cell(number_from_uint32(val))};
    stack.push(Cell(a));
}

void Executor::exec_CALLV()
{
    ip++;
    uint32_t val = Bytecode::get_vint(module->object.code, ip);
    if (callstack.size() >= param_recursion_limit) {
        raise(rtl::ne_global::Exception_StackOverflowException, std::make_shared<ObjectString>(utf8string("")));
        return;
    }
    std::vector<Cell> &pi = stack.top().array_for_write();
    Cell *instance = pi[0].address();
    size_t interface_index = number_to_uint32(pi[1].number());
    Module *m = reinterpret_cast<Module *>(instance->array_for_write()[0].array_for_write()[0].other());
    Bytecode::ClassInfo *classinfo = reinterpret_cast<Bytecode::ClassInfo *>(instance->array_for_write()[0].array_for_write()[1].other());
    stack.pop();
    invoke(m, classinfo->interfaces[interface_index][val]);
}

void Executor::exec_PUSHCI()
{
    ip++;
    uint32_t val = Bytecode::get_vint(module->object.code, ip);
    auto dot = module->object.strtable[val].find('.');
    if (dot == std::string::npos) {
        for (auto &c: module->object.classes) {
            if (c.name == val) {
                Cell ci;
                ci.array_for_write().push_back(Cell::makeOther(module));
                ci.array_for_write().push_back(Cell::makeOther(&c));
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
                    ci.array_for_write().push_back(Cell::makeOther(m));
                    ci.array_for_write().push_back(Cell::makeOther(&c));
                    stack.push(ci);
                    return;
                }
            }
        }
    }
    fprintf(stderr, "neon: unknown class name %s\n", module->object.strtable[val].c_str());
    exit(1);
}

void Executor::invoke(Module *m, uint32_t index)
{
    callstack.push_back(std::make_pair(module, ip));
    ActivationFrame *outer = nullptr;
    unsigned int nest = m->object.functions[index].nest;
    unsigned int params = m->object.functions[index].params;
    unsigned int locals = m->object.functions[index].locals;
    if (frames.size() > 0) {
        assert(nest <= frames.back().nesting_depth+1);
        outer = &frames.back();
        while (outer != nullptr && nest <= outer->nesting_depth) {
            assert(outer->outer == nullptr || outer->nesting_depth == outer->outer->nesting_depth+1);
            outer = outer->outer;
        }
    }
    frames.emplace_back(nest, outer, locals, stack.depth() - params);
    dump_frames(this);
    module = m;
    ip = m->object.functions[index].entry;
}

void Executor::raise_literal(const utf8string &exception, std::shared_ptr<Object> info)
{
    // The fields here must match the declaration of
    // ExceptionType in ast.cpp.
    Cell exceptionvar;
    exceptionvar.array_index_for_write(0) = Cell(exception);
    exceptionvar.array_index_for_write(1) = Cell(info);
    exceptionvar.array_index_for_write(2) = Cell(number_from_uint32(static_cast<uint32_t>(ip)));

    auto tmodule = module;
    auto tip = ip;
    size_t sp = callstack.size();
    for (;;) {
        for (auto e = tmodule->object.exceptions.begin(); e != tmodule->object.exceptions.end(); ++e) {
            if (tip >= e->start && tip < e->end) {
                const std::string handler = tmodule->object.strtable[e->excid];
                if (exception.str() == handler
                 || (exception.length() > handler.length() && exception.substr(0, handler.length()) == handler && exception.at(handler.length()) == '.')) {
                    module = tmodule;
                    ip = e->handler;
                    while (stack.depth() > (frames.empty() ? 0 : frames.back().opstack_depth) + e->stack_depth) {
                        stack.pop();
                    }
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
        if (not frames.empty()) {
            frames.pop_back();
        }
        tmodule = callstack[sp].first;
        tip = callstack[sp].second;
    }

    utf8string detail;
    info->getString(detail);
    fprintf(stderr, "Unhandled exception %s (%s)\n", exception.c_str(), detail.c_str());
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
            fprintf(stderr, "  Stack frame #%lu: module %s address %lu (no debug info available)\n", static_cast<unsigned long>(callstack.size()), module->name.c_str(), static_cast<unsigned long>(ip));
        }
        if (callstack.empty()) {
            break;
        }
        module = callstack.back().first;
        ip = callstack.back().second;
        callstack.pop_back();
    }
    // Setting exit_code here will cause exec_loop to terminate and return this exit code.
    exit_code = 1;
}

void Executor::raise(const ExceptionName &exception, std::shared_ptr<Object> info)
{
    raise_literal(utf8string(exception.name), info);
}

void Executor::raise(const RtlException &x)
{
    raise_literal(utf8string(x.name), std::make_shared<ObjectString>(x.info));
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
            case Cell::Type::Object:
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
            case Cell::Type::Other:
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

bool Executor::is_module_imported(const std::string &mod)
{
    return modules.find(mod) != modules.end();
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

int Executor::exec()
{
    ip = module->object.code.size();
    invoke(module, 0);

    // This sets up the call stack in such a way as to initialize
    // each module in the order determined in init_order, followed
    // by running the code in the main module.
    for (auto x = init_order.rbegin(); x != init_order.rend(); ++x) {
        invoke(modules[*x], 0);
    }

    int r = exec_loop(0);
    if (r == 0) {
        assert(stack.empty());
    }
    return r;
}

int Executor::exec_loop(size_t min_callstack_depth)
{
    while (callstack.size() > min_callstack_depth && ip < module->object.code.size() && exit_code == 0) {
        if (options->enable_trace) {
            auto i = ip;
            std::cerr << "mod " << module->name << " ip " << ip << " (" << stack.depth() << ") " << disassemble_instruction(module->object, i) << "\n";
            if (module->debug != nullptr) {
                auto sd = module->debug->stack_depth.find(ip);
                if (sd != module->debug->stack_depth.end()) {
                    int expected_depth = static_cast<int>(frames.empty() ? 0 : frames.back().opstack_depth) + sd->second;
                    if (expected_depth != static_cast<int>(stack.depth())) {
                        std::cerr << "stack depth mismatch: expected=" << expected_depth << " actual=" << stack.depth() << "\n";
                        abort();
                    }
                }
            }
        }
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
                return 1;
            }
        }
        switch (static_cast<Opcode>(module->object.code[ip])) {
            case Opcode::PUSHB:   exec_PUSHB(); break;
            case Opcode::PUSHN:   exec_PUSHN(); break;
            case Opcode::PUSHS:   exec_PUSHS(); break;
            case Opcode::PUSHY:   exec_PUSHY(); break;
            case Opcode::PUSHPG:  exec_PUSHPG(); break;
            case Opcode::PUSHPPG: exec_PUSHPPG(); break;
            case Opcode::PUSHPMG: exec_PUSHPMG(); break;
            case Opcode::PUSHPL:  exec_PUSHPL(); break;
            case Opcode::PUSHPOL: exec_PUSHPOL(); break;
            case Opcode::PUSHI:   exec_PUSHI(); break;
            case Opcode::LOADB:   exec_LOADB(); break;
            case Opcode::LOADN:   exec_LOADN(); break;
            case Opcode::LOADS:   exec_LOADS(); break;
            case Opcode::LOADY:   exec_LOADY(); break;
            case Opcode::LOADA:   exec_LOADA(); break;
            case Opcode::LOADD:   exec_LOADD(); break;
            case Opcode::LOADP:   exec_LOADP(); break;
            case Opcode::LOADJ:   exec_LOADJ(); break;
            case Opcode::LOADV:   exec_LOADV(); break;
            case Opcode::STOREB:  exec_STOREB(); break;
            case Opcode::STOREN:  exec_STOREN(); break;
            case Opcode::STORES:  exec_STORES(); break;
            case Opcode::STOREY:  exec_STOREY(); break;
            case Opcode::STOREA:  exec_STOREA(); break;
            case Opcode::STORED:  exec_STORED(); break;
            case Opcode::STOREP:  exec_STOREP(); break;
            case Opcode::STOREJ:  exec_STOREJ(); break;
            case Opcode::STOREV:  exec_STOREV(); break;
            case Opcode::NEGN:    exec_NEGN(); break;
            case Opcode::ADDN:    exec_ADDN(); break;
            case Opcode::SUBN:    exec_SUBN(); break;
            case Opcode::MULN:    exec_MULN(); break;
            case Opcode::DIVN:    exec_DIVN(); break;
            case Opcode::MODN:    exec_MODN(); break;
            case Opcode::EXPN:    exec_EXPN(); break;
            case Opcode::EQB:     exec_EQB(); break;
            case Opcode::NEB:     exec_NEB(); break;
            case Opcode::EQN:     exec_EQN(); break;
            case Opcode::NEN:     exec_NEN(); break;
            case Opcode::LTN:     exec_LTN(); break;
            case Opcode::GTN:     exec_GTN(); break;
            case Opcode::LEN:     exec_LEN(); break;
            case Opcode::GEN:     exec_GEN(); break;
            case Opcode::EQS:     exec_EQS(); break;
            case Opcode::NES:     exec_NES(); break;
            case Opcode::LTS:     exec_LTS(); break;
            case Opcode::GTS:     exec_GTS(); break;
            case Opcode::LES:     exec_LES(); break;
            case Opcode::GES:     exec_GES(); break;
            case Opcode::EQY:     exec_EQY(); break;
            case Opcode::NEY:     exec_NEY(); break;
            case Opcode::LTY:     exec_LTY(); break;
            case Opcode::GTY:     exec_GTY(); break;
            case Opcode::LEY:     exec_LEY(); break;
            case Opcode::GEY:     exec_GEY(); break;
            case Opcode::EQA:     exec_EQA(); break;
            case Opcode::NEA:     exec_NEA(); break;
            case Opcode::EQD:     exec_EQD(); break;
            case Opcode::NED:     exec_NED(); break;
            case Opcode::EQP:     exec_EQP(); break;
            case Opcode::NEP:     exec_NEP(); break;
            case Opcode::EQV:     exec_EQV(); break;
            case Opcode::NEV:     exec_NEV(); break;
            case Opcode::ANDB:    exec_ANDB(); break;
            case Opcode::ORB:     exec_ORB(); break;
            case Opcode::NOTB:    exec_NOTB(); break;
            case Opcode::INDEXAR: exec_INDEXAR(); break;
            case Opcode::INDEXAW: exec_INDEXAW(); break;
            case Opcode::INDEXAV: exec_INDEXAV(); break;
            case Opcode::INDEXAN: exec_INDEXAN(); break;
            case Opcode::INDEXDR: exec_INDEXDR(); break;
            case Opcode::INDEXDW: exec_INDEXDW(); break;
            case Opcode::INDEXDV: exec_INDEXDV(); break;
            case Opcode::INA:     exec_INA(); break;
            case Opcode::IND:     exec_IND(); break;
            case Opcode::CALLP:   exec_CALLP(); break;
            case Opcode::CALLF:   exec_CALLF(); break;
            case Opcode::CALLMF:  exec_CALLMF(); break;
            case Opcode::CALLI:   exec_CALLI(); break;
            case Opcode::JUMP:    exec_JUMP(); break;
            case Opcode::JF:      exec_JF(); break;
            case Opcode::JT:      exec_JT(); break;
            case Opcode::DUP:     exec_DUP(); break;
            case Opcode::DUPX1:   exec_DUPX1(); break;
            case Opcode::DROP:    exec_DROP(); break;
            case Opcode::RET:     exec_RET(); break;
            case Opcode::CONSA:   exec_CONSA(); break;
            case Opcode::CONSD:   exec_CONSD(); break;
            case Opcode::EXCEPT:  exec_EXCEPT(); break;
            case Opcode::ALLOC:   exec_ALLOC(); break;
            case Opcode::PUSHNIL: exec_PUSHNIL(); break;
            case Opcode::RESETC:  exec_RESETC(); break;
            case Opcode::PUSHPEG: exec_PUSHPEG(); break;
            case Opcode::JUMPTBL: exec_JUMPTBL(); break;
            case Opcode::CALLX:   exec_CALLX(); break;
            case Opcode::SWAP:    exec_SWAP(); break;
            case Opcode::DROPN:   exec_DROPN(); break;
            case Opcode::PUSHFP:  exec_PUSHFP(); break;
            case Opcode::CALLV:   exec_CALLV(); break;
            case Opcode::PUSHCI:  exec_PUSHCI(); break;
            default:
                fprintf(stderr, "exec: Unexpected opcode: %d\n", module->object.code[ip]);
                abort();
        }
    }
    return exit_code;
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
            case Cell::Type::Object:
                writer.write("type", "object");
                writer.write("value", std::to_string(reinterpret_cast<intptr_t>(cell.object().get())));
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
            case Cell::Type::Other:
                writer.write("type", "other");
                writer.write("value", std::to_string(reinterpret_cast<intptr_t>(cell.other())));
                break;
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
    if (r.str().empty()) {
        r << "{}";
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

bool executor_assertions_enabled()
{
    return g_executor->options->enable_assert;
}

void executor_garbage_collect()
{
    g_executor->garbage_collect();
}

size_t executor_get_allocated_object_count()
{
    return g_executor->get_allocated_object_count();
}

bool executor_is_module_imported(const std::string &module)
{
    return g_executor->is_module_imported(module);
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

void executor_raise_exception(size_t ip, const utf8string &name, const utf8string &info)
{
    g_executor->ip = ip;
    g_executor->raise_literal(name, std::make_shared<ObjectString>(info));
}

int exec(const std::string &source_path, const Bytecode::Bytes &obj, const DebugInfo *debuginfo, ICompilerSupport *support, const ExecOptions *options, unsigned short debug_port, int argc, char *argv[], std::map<std::string, Cell *> *external_globals)
{
    rtl_exec_init(argc, argv);
    return Executor(source_path, obj, debuginfo, support, options, debug_port, external_globals).exec();
}
