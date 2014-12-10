#include "verifier.h"

#include <assert.h>
#include <deque>
#include <iso646.h>
#include <stdint.h>

#include "number.h"
#include "opcode.h"

namespace {

std::vector<std::string> split(const std::string &s, char d)
{
    std::vector<std::string> r;
    std::string::size_type i = 0;
    auto start = i;
    while (i < s.length()) {
        if (s.at(i) == d) {
            r.push_back(s.substr(start, i-start));
            start = i + 1;
        }
        i++;
    }
    if (i > start) {
        r.push_back(s.substr(start, i-start));
    }
    return r;
}

} // namespace

class ExceptionInfo {
public:
    unsigned int start;
    unsigned int end;
    unsigned int excid;
    unsigned int handler;
};

class Type {
public:
    enum Kind {
        UNKNOWN,
        ADDRESS,
        BOOLEAN,
        NUMBER,
        STRING,
        ARRAY,
        DICTIONARY
    };
    Type(): kind(UNKNOWN), subtype(nullptr) {}
    explicit Type(const Kind kind, Type *subtype = nullptr): kind(kind), subtype(subtype) {
        if (kind == ADDRESS || kind == ARRAY || kind == DICTIONARY) {
            assert(subtype != nullptr);
        }
    }
    bool operator==(const Type &rhs) const;
    bool operator!=(const Type &rhs) { return not operator==(rhs); }
    std::string str() const;
    enum Kind kind;
    Type *subtype;
};

bool Type::operator==(const Type &rhs) const
{
    if (kind != rhs.kind) {
        return false;
    }
    if (subtype != nullptr && rhs.subtype != nullptr) {
        return *subtype == *rhs.subtype;
    }
    return true;
}

std::string Type::str() const
{
    switch (kind) {
        case UNKNOWN:    return "UNKNOWN";
        case ADDRESS:    return "ADDRESS<" + subtype->str() + ">";
        case BOOLEAN:    return "BOOLEAN";
        case NUMBER:     return "NUMBER";
        case STRING:     return "STRING";
        case ARRAY:      return "ARRAY<" + subtype->str() + ">";
        case DICTIONARY: return "DICTIONARY<" + subtype->str() + ">";
    }
    return "???";
}

std::vector<std::vector<size_t>> build_comefrom(const std::vector<unsigned char> &code)
{
    std::vector<std::vector<size_t>> comefrom(code.size());
    size_t i = 0;
    int last = -1;
    while (i < code.size()) {
        size_t current = i;
        switch (code[i]) {
            case ENTER:   i += 5;                                                   break;
            case LEAVE:   i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case PUSHB:   i += 2; if (last >= 0) comefrom[current].push_back(last); break;
            case PUSHN:   i += 9; if (last >= 0) comefrom[current].push_back(last); break;
            case PUSHS:   i += 5; if (last >= 0) comefrom[current].push_back(last); break;
            case PUSHPG:  i += 5; if (last >= 0) comefrom[current].push_back(last); break;
            case PUSHPL:  i += 5; if (last >= 0) comefrom[current].push_back(last); break;
            case PUSHPOL: i += 5; if (last >= 0) comefrom[current].push_back(last); break;
            case LOADB:   i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case LOADN:   i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case LOADS:   i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case LOADA:   i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case LOADD:   i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case LOADP:   i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case STOREB:  i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case STOREN:  i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case STORES:  i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case STOREA:  i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case STORED:  i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case STOREP:  i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case NEGN:    i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case ADDN:    i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case SUBN:    i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case MULN:    i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case DIVN:    i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case MODN:    i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case EXPN:    i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case EQB:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case NEB:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case EQN:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case NEN:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case LTN:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case GTN:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case LEN:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case GEN:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case EQS:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case NES:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case LTS:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case GTS:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case LES:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case GES:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case EQA:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case NEA:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case EQD:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case NED:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case EQP:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case NEP:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case ANDB:    i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case ORB:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case NOTB:    i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case INDEXAR: i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case INDEXAW: i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case INDEXDR: i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case INDEXDW: i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case INA:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case IND:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case CALLP:   i += 5; if (last >= 0) comefrom[current].push_back(last); break;
            case CALLF: {
                uint32_t target = (code[i+1] << 24) | (code[i+2] << 16) | (code[i+3] << 8) | code[i+4];
                i += 5;
                if (last >= 0) comefrom[current].push_back(last);
                comefrom[target].push_back(current);
                break;
            }
            case JUMP: {
                uint32_t target = (code[i+1] << 24) | (code[i+2] << 16) | (code[i+3] << 8) | code[i+4];
                i += 5;
                if (last >= 0) comefrom[current].push_back(last);
                comefrom[target].push_back(current);
                last = -1;
                continue;
            }
            case JF:
            case JT: {
                uint32_t target = (code[i+1] << 24) | (code[i+2] << 16) | (code[i+3] << 8) | code[i+4];
                i += 5;
                if (last >= 0) comefrom[current].push_back(last);
                comefrom[target].push_back(current);
                break;
            }
            case DUP:     i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case DROP:    i += 1; if (last >= 0) comefrom[current].push_back(last); break;
            case RET:     i += 1; if (last >= 0) comefrom[current].push_back(last); last = -1; continue;
            case CALLE:   i += 5; if (last >= 0) comefrom[current].push_back(last); break;
            case CONSA:   i += 5; if (last >= 0) comefrom[current].push_back(last); break;
            case CONSD:   i += 5; if (last >= 0) comefrom[current].push_back(last); break;
            case EXCEPT:  i += 5; if (last >= 0) comefrom[current].push_back(last); last = -1; continue;
            case ALLOC:   i += 5; if (last >= 0) comefrom[current].push_back(last); break;
            case PUSHNIL: i += 5; if (last >= 0) comefrom[current].push_back(last); break;
            default:
                fprintf(stderr, "build_comefrom opcode:%d\n", code[i]);
                exit(1);
        }
        last = static_cast<int>(current);
    }
    return comefrom;
}

bool verify(const std::vector<unsigned char> &obj)
{
    size_t i = 0;
    if (i+2 > obj.size()) {
        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
        return false;
    }
    const size_t global_size = (obj[i] << 8 | obj[i+1]);
    i += 2;
    if (i+2 > obj.size()) {
        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
        return false;
    }
    const size_t strtablesize = (obj[i] << 8) | obj[i+1];
    i += 2;
    const size_t strtableend = i + strtablesize;
    std::vector<std::string> strtable;
    while (i < strtableend) {
        std::string s;
        while (i < obj.size() && obj[i] != 0) {
            if (i+1 > obj.size()) {
                fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            s.push_back(obj[i]);
            i++;
        }
        strtable.push_back(s);
        i++;
    }
    if (i+2 > obj.size()) {
        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
        return false;
    }
    size_t exceptionsize = (obj[i] << 8) | obj[i+1];
    i += 2;
    std::vector<ExceptionInfo> exceptions;
    while (exceptionsize > 0) {
        if (i+8 > obj.size()) {
            fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
            return false;
        }
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
    const std::vector<unsigned char> code = std::vector<unsigned char>(obj.begin() + i, obj.end());

    for (auto e: exceptions) {
        if (e.start >= code.size()) {
            fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
            return false;
        }
        if (e.end > code.size()) {
            fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
            return false;
        }
        if (e.excid >= strtable.size()) {
            fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
            return false;
        }
        if (e.handler >= code.size()) {
            fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
            return false;
        }
    }

    std::vector<Type> globals = std::vector<Type>(global_size);
    size_t current_function = 0;
    std::vector<Type> locals;
    std::deque<Type> stack;
    std::vector<std::vector<size_t>> comefrom = build_comefrom(code);
    std::vector<std::deque<Type> *> stack_at_ip(code.size());
    std::vector<std::deque<Type> *> stack_after_function(code.size());
    i = 0;
    while (i < code.size()) {
        fprintf(stderr, "%d ", i);
        for (auto x: stack) {
            fprintf(stderr, "%s ", x.str().c_str());
        }
        fprintf(stderr, "\n");
        fflush(stderr);
        if (i+1 > code.size()) {
            fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
            return false;
        }
        if (stack_at_ip[i] != nullptr) {
            if (stack != *stack_at_ip[i]) {
                fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                return false;
            }
        } else {
            stack_at_ip[i] = new std::deque<Type>(stack);
        }
        switch (code[i++]) {
            case ENTER: {
                if (current_function > 0) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                if (i+4 > code.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                uint32_t val = (code[i] << 24) | (code[i+1] << 16) | (code[i+2] << 8) | code[i+3];
                i += 4;
                current_function = i - 5;
                locals = std::vector<Type>(val);
                std::deque<Type> newstack;
                auto j = i;
                bool done = false;
                while (code[j] == PUSHPL && not done) {
                    j += 5;
                    switch (code[j]) {
                        case STOREB: newstack.push_front(Type(Type::BOOLEAN)); break;
                        case STOREN: newstack.push_front(Type(Type::NUMBER));  break;
                        case STORES: newstack.push_front(Type(Type::STRING));  break;
                        default:
                            done = true;
                            break;
                    }
                    j++;
                }
                for (size_t k = 0; k < newstack.size(); k++) {
                    if (newstack[k] != stack[k]) {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                }
                stack = newstack;
                break;
            }
            case LEAVE: {
                if (current_function == 0) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack_after_function[current_function] = new std::deque<Type>(stack);
                current_function = 0;
                stack.clear();
                break;
            }
            case PUSHB: {
                if (i+1 > code.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                i += 1;
                stack.push_front(Type(Type::BOOLEAN));
                break;
            }
            case PUSHN: {
                if (i+sizeof(Number) > code.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                // TODO: validate number
                i += sizeof(Number);
                stack.push_front(Type(Type::NUMBER));
                break;
            }
            case PUSHS: {
                if (i+4 > code.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                uint32_t val = (code[i] << 24) | (code[i+1] << 16) | (code[i+2] << 8) | code[i+3];
                i += 4;
                if (val >= strtable.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.push_front(Type(Type::STRING));
                break;
            }
            case PUSHPG: {
                if (i+4 > code.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                uint32_t val = (code[i] << 24) | (code[i+1] << 16) | (code[i+2] << 8) | code[i+3];
                i += 4;
                if (val >= globals.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.push_front(Type(Type::ADDRESS, &globals[val]));
                break;
            }
            case PUSHPL: {
                if (i+4 > code.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                uint32_t val = (code[i] << 24) | (code[i+1] << 16) | (code[i+2] << 8) | code[i+3];
                i += 4;
                if (val >= locals.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.push_front(Type(Type::ADDRESS, &locals[val]));
                break;
            }
            case LOADB: {
                if (stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                if (stack.front().subtype->kind != Type::UNKNOWN && stack.front().subtype->kind != Type::BOOLEAN) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.front().subtype->kind = Type::BOOLEAN;
                stack.pop_front();
                stack.push_front(Type(Type::BOOLEAN));
                break;
            }
            case LOADN: {
                if (stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                if (stack.front().subtype->kind != Type::UNKNOWN && stack.front().subtype->kind != Type::NUMBER) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.front().subtype->kind = Type::NUMBER;
                stack.pop_front();
                stack.push_front(Type(Type::NUMBER));
                break;
            }
            case LOADS: {
                if (stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                if (stack.front().subtype->kind != Type::UNKNOWN && stack.front().subtype->kind != Type::STRING) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.front().subtype->kind = Type::STRING;
                stack.pop_front();
                stack.push_front(Type(Type::STRING));
                break;
            }
            case LOADA: {
                if (stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                if (stack.front().subtype->kind != Type::ARRAY) {
                    if (stack.front().subtype->kind != Type::UNKNOWN) {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                    stack.front().subtype->kind = Type::ARRAY;
                    stack.front().subtype->subtype = new Type(Type::UNKNOWN);
                }
                Type t = *stack.front().subtype;
                stack.pop_front();
                stack.push_front(t);
                break;
            }
            case LOADD: {
                if (stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                if (stack.front().subtype->kind != Type::DICTIONARY) {
                    if (stack.front().subtype->kind != Type::UNKNOWN) {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                    stack.front().subtype->kind = Type::DICTIONARY;
                    stack.front().subtype->subtype = new Type(Type::UNKNOWN);
                }
                Type t = *stack.front().subtype;
                stack.pop_front();
                stack.push_front(t);
                break;
            }
            case LOADP: {
                if (stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                if (stack.front().subtype->kind != Type::ADDRESS) {
                    if (stack.front().subtype->kind != Type::UNKNOWN) {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                    stack.front().subtype->kind = Type::ADDRESS;
                    stack.front().subtype->subtype = new Type(Type::UNKNOWN);
                }
                Type t = *stack.front().subtype;
                stack.pop_front();
                stack.push_front(t);
                break;
            }
            case STOREB: {
                if (stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                if (stack.front().subtype->kind != Type::UNKNOWN && stack.front().subtype->kind != Type::BOOLEAN) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.front().subtype->kind = Type::BOOLEAN;
                stack.pop_front();
                if (stack.front().kind != Type::BOOLEAN) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                break;
            }
            case STOREN: {
                if (stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                if (stack.front().subtype->kind != Type::UNKNOWN && stack.front().subtype->kind != Type::NUMBER) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.front().subtype->kind = Type::NUMBER;
                stack.pop_front();
                if (stack.front().kind != Type::NUMBER) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                break;
            }
            case STORES: {
                if (stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                if (stack.front().subtype->kind != Type::UNKNOWN && stack.front().subtype->kind != Type::STRING) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.front().subtype->kind = Type::STRING;
                stack.pop_front();
                if (stack.front().kind != Type::STRING) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                break;
            }
            case STOREA: {
                if (stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                if (stack.front().subtype->kind != Type::UNKNOWN && stack.front().subtype->kind != Type::ARRAY) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                Type *top = stack.front().subtype;
                stack.pop_front();
                if (stack.front().kind != Type::ARRAY) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                *stack.front().subtype = *top;
                stack.pop_front();
                break;
            }
            case STORED: {
                if (stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                if (stack.front().subtype->kind != Type::UNKNOWN && stack.front().subtype->kind != Type::DICTIONARY) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.front().subtype->kind = Type::DICTIONARY;
                stack.pop_front();
                if (stack.front().kind != Type::DICTIONARY) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                break;
            }
            case STOREP: {
                if (stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                if (stack.front().subtype->kind != Type::UNKNOWN && stack.front().subtype->kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.front().subtype->kind = Type::ADDRESS;
                stack.pop_front();
                if (stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                break;
            }
            case NEGN: {
                if (stack.front().kind != Type::NUMBER) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                stack.push_front(Type(Type::NUMBER));
                break;
            }
            case ADDN:
            case SUBN:
            case MULN:
            case DIVN:
            case MODN:
            case EXPN: {
                if (stack.front().kind != Type::NUMBER) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                if (stack.front().kind != Type::NUMBER) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                stack.push_front(Type(Type::NUMBER));
                break;
            }
            case EQB:
            case NEB: {
                if (stack.front().kind != Type::BOOLEAN) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                if (stack.front().kind != Type::BOOLEAN) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                stack.push_front(Type(Type::BOOLEAN));
                break;
            }
            case EQN:
            case NEN:
            case LTN:
            case GTN:
            case LEN:
            case GEN: {
                if (stack.front().kind != Type::NUMBER) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                if (stack.front().kind != Type::NUMBER) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                stack.push_front(Type(Type::BOOLEAN));
                break;
            }
            case EQS:
            case NES:
            case LTS:
            case GTS:
            case LES:
            case GES: {
                if (stack.front().kind != Type::STRING) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                if (stack.front().kind != Type::STRING) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                stack.push_front(Type(Type::BOOLEAN));
                break;
            }
            case EQA:
            case NEA: {
                if (stack.front().kind != Type::ARRAY) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                Type t1 = stack.front();
                stack.pop_front();
                if (stack.front().kind != Type::ARRAY) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                Type t2 = stack.front();
                stack.pop_front();
                if (t1 != t2) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.push_front(Type(Type::BOOLEAN));
                break;
            }
            case EQD:
            case NED: {
                if (stack.front().kind != Type::DICTIONARY) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                Type t1 = stack.front();
                stack.pop_front();
                if (stack.front().kind != Type::DICTIONARY) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                Type t2 = stack.front();
                stack.pop_front();
                if (t1 != t2) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.push_front(Type(Type::BOOLEAN));
                break;
            }
            case EQP:
            case NEP: {
                if (stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                Type t1 = stack.front();
                stack.pop_front();
                if (stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                Type t2 = stack.front();
                stack.pop_front();
                if (t1 != t2) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.push_front(Type(Type::BOOLEAN));
                break;
            }
            case ANDB:
            case ORB: {
                // TODO
                fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                return false;
                break;
            }
            case NOTB: {
                // TODO
                fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                return false;
                break;
            }
            case INDEXAR:
            case INDEXAW: {
                if (stack.front().kind != Type::NUMBER) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                if (stack.front().kind != Type::UNKNOWN && stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                Type *t = stack.front().kind != Type::UNKNOWN ? stack.front().subtype : new Type(Type::UNKNOWN);
                stack.pop_front();
                stack.push_front(Type(Type::ADDRESS, t));
                break;
            }
            case INDEXDR:
            case INDEXDW: {
                if (stack.front().kind != Type::STRING) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                if (stack.front().kind != Type::UNKNOWN && stack.front().kind != Type::ADDRESS) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                Type *t = stack.front().kind != Type::UNKNOWN ? stack.front().subtype : new Type(Type::UNKNOWN);
                stack.pop_front();
                stack.push_front(Type(Type::ADDRESS, t));
                break;
            }
            case INA: {
                // TODO
                fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                return false;
                break;
            }
            case IND: {
                // TODO
                fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                return false;
                break;
            }
            case CALLP: {
                if (i+4 > code.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                uint32_t val = (code[i] << 24) | (code[i+1] << 16) | (code[i+2] << 8) | code[i+3];
                i += 4;
                if (strtable[val] == "abs") {
                    if (stack.front().kind != Type::NUMBER) {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                    stack.pop_front();
                    stack.push_front(Type(Type::NUMBER));
                } else if (strtable[val] == "concat" ) {
                    if (stack.front().kind != Type::STRING) {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                    stack.pop_front();
                    if (stack.front().kind != Type::STRING) {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                    stack.pop_front();
                    stack.push_front(Type(Type::STRING));
                } else if (strtable[val] == "print") {
                    if (stack.front().kind != Type::STRING) {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                    stack.pop_front();
                } else if (strtable[val] == "str") {
                    if (stack.front().kind != Type::NUMBER) {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                    stack.pop_front();
                    stack.push_front(Type(Type::STRING));
                } else if (strtable[val] == "strb") {
                    if (stack.front().kind != Type::BOOLEAN) {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                    stack.pop_front();
                    stack.push_front(Type(Type::STRING));
                } else if (strtable[val] == "substring") {
                    if (stack.front().kind != Type::NUMBER) {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                    stack.pop_front();
                    if (stack.front().kind != Type::NUMBER) {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                    stack.pop_front();
                    if (stack.front().kind != Type::STRING) {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                    stack.pop_front();
                    stack.push_front(Type(Type::STRING));
                } else if (strtable[val] == "sys$argv") {
                    stack.push_front(Type(Type::ARRAY, new Type(Type::STRING)));
                } else {
                    fprintf(stderr, "verify %s:%d %s\n", __FILE__, __LINE__, strtable[val].c_str());
                    return false;
                }
                break;
            }
            case CALLF: {
                if (i+4 > code.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                uint32_t target = (code[i] << 24) | (code[i+1] << 16) | (code[i+2] << 8) | code[i+3];
                i += 4;
                if (stack_at_ip[target] != nullptr) {
                    if (stack != *stack_at_ip[target]) {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                } else {
                    stack_at_ip[target] = new std::deque<Type>(stack);
                }
                if (stack_after_function[target] != nullptr) {
                    std::copy(stack_after_function[target]->begin(), stack_after_function[target]->end(), std::front_inserter(stack));
                }
                break;
            }
            case JUMP: {
                if (i+4 > code.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                uint32_t target = (code[i] << 24) | (code[i+1] << 16) | (code[i+2] << 8) | code[i+3];
                i += 4;
                if (stack_at_ip[target] != nullptr) {
                    if (stack != *stack_at_ip[target]) {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                } else {
                    stack_at_ip[target] = new std::deque<Type>(stack);
                }
                if (stack_at_ip[i] != nullptr) {
                    stack = *stack_at_ip[i];
                } else {
                    while (i < code.size() && stack_at_ip[i] == nullptr) {
                        i++;
                    }
                }
                break;
            }
            case JF:
            case JT: {
                if (i+4 > code.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                uint32_t target = (code[i] << 24) | (code[i+1] << 16) | (code[i+2] << 8) | code[i+3];
                i += 4;
                if (stack.front().kind != Type::BOOLEAN) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                stack.pop_front();
                if (stack_at_ip[target] != nullptr) {
                    if (stack != *stack_at_ip[target]) {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                } else {
                    stack_at_ip[target] = new std::deque<Type>(stack);
                }
                break;
            }
            case DUP: {
                stack.push_front(stack.front());
                break;
            }
            case DROP: {
                stack.pop_front();
                break;
            }
            case RET: {
                current_function = 0;
                break;
            }
            case CALLE: {
                if (i+4 > code.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                uint32_t val = (code[i] << 24) | (code[i+1] << 16) | (code[i+2] << 8) | code[i+3];
                i += 4;
                if (val >= strtable.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                std::string func = strtable[val];
                auto info = split(func, ':');
                std::string library = info[0];
                std::string name = info[1];
                std::string rettype = info[2];
                auto params = split(info[3], ',');
                for (auto p: params) {
                    if (p == "uint8"  || p == "sint8"
                     || p == "uint16" || p == "sint16"
                     || p == "uint32" || p == "sint32"
                     || p == "uint64" || p == "sint64"
                     || p == "float"  || p == "double") {
                        if (stack.front().kind != Type::NUMBER) {
                            fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                            return false;
                        }
                        stack.pop_front();
                    } else if (p == "pointer") {
                        if (stack.front().kind != Type::STRING) {
                            fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                            return false;
                        }
                        stack.pop_front();
                    } else {
                        fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                        return false;
                    }
                }
                if (rettype == "uint8"  || rettype == "sint8"
                 || rettype == "uint16" || rettype == "sint16"
                 || rettype == "uint32" || rettype == "sint32"
                 || rettype == "uint64" || rettype == "sint64"
                 || rettype == "float"  || rettype == "double") {
                    stack.push_front(Type(Type::NUMBER));
                } else if (rettype == "string") {
                    stack.push_front(Type(Type::STRING));
                } else {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                break;
            }
            case CONSA: {
                // TODO
                fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                return false;
                break;
            }
            case CONSD: {
                // TODO
                fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                return false;
                break;
            }
            case EXCEPT: {
                if (i+4 > code.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                uint32_t val = (code[i] << 24) | (code[i+1] << 16) | (code[i+2] << 8) | code[i+3];
                i += 4;
                if (val >= strtable.size()) {
                    fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
                break;
            }
            default: {
                fprintf(stderr, "verify %s:%d\n", __FILE__, __LINE__);
                return false;
            }
        }
    }
    return true;
}
