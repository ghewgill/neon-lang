#include "ast.h"

#include <assert.h>
#include <fstream>

#include "support.h"

namespace jvm {

#include "functions_compile_jvm.inc"

const uint16_t ACC_PUBLIC = 0x0001;
const uint16_t ACC_STATIC = 0x0008;
const uint16_t ACC_FINAL  = 0x0010;
const uint16_t ACC_SUPER  = 0x0020;
//const uint17_t ACC_ENUM   = 0x4000;

const uint8_t CONSTANT_Class              =  7;
const uint8_t CONSTANT_Fieldref           =  9;
const uint8_t CONSTANT_Methodref          = 10;
const uint8_t CONSTANT_InterfaceMethodref = 11;
const uint8_t CONSTANT_String             =  8;
//const uint8_t CONSTANT_Integer            =  3;
//const uint8_t CONSTANT_Float              =  4;
//const uint8_t CONSTANT_Long               =  5;
const uint8_t CONSTANT_Double             =  6;
const uint8_t CONSTANT_NameAndType        = 12;
const uint8_t CONSTANT_Utf8               =  1;
//const uint8_t CONSTANT_MethodHandle       = 15;
//const uint8_t CONSTANT_MethodType         = 16;
//const uint8_t CONSTANT_InvokeDynamic      = 18;

const uint8_t OP_aconst_null    =   1;
const uint8_t OP_iconst_m1      =   2;
const uint8_t OP_iconst_0       =   3;
const uint8_t OP_iconst_1       =   4;
const uint8_t OP_iconst_2       =   5;
const uint8_t OP_iconst_3       =   6;
const uint8_t OP_iconst_4       =   7;
const uint8_t OP_iconst_5       =   8;
const uint8_t OP_bipush         =  16;
const uint8_t OP_sipush         =  17;
//const uint8_t OP_ldc            =  18;
const uint8_t OP_ldc_w          =  19;
const uint8_t OP_aload          =  25;
//const uint8_t OP_iload_2        =  28;
const uint8_t OP_aload_0        =  42;
//const uint8_t OP_aload_1        =  43;
const uint8_t OP_aaload         =  50;
const uint8_t OP_astore         =  58;
const uint8_t OP_astore_0       =  75;
const uint8_t OP_aastore        =  83;
const uint8_t OP_bastore        =  84;
const uint8_t OP_pop            =  87;
const uint8_t OP_dup            =  89;
const uint8_t OP_dup_x1         =  90;
const uint8_t OP_swap           =  95;
//const uint8_t OP_iadd           =  96;
//const uint8_t OP_dadd           =  99;
//const uint8_t OP_isub           = 100;
//const uint8_t OP_drem           = 115;
//const uint8_t OP_iand           = 126;
const uint8_t OP_ixor           = 130;
const uint8_t OP_ifeq           = 153;
const uint8_t OP_ifne           = 154;
const uint8_t OP_iflt           = 155;
const uint8_t OP_ifge           = 156;
const uint8_t OP_ifgt           = 157;
const uint8_t OP_ifle           = 158;
//const uint8_t OP_if_icmplt      = 161;
const uint8_t OP_if_acmpeq      = 165;
const uint8_t OP_if_acmpne      = 166;
const uint8_t OP_goto           = 167;
const uint8_t OP_areturn        = 176;
const uint8_t OP_return         = 177;
const uint8_t OP_getstatic      = 178;
const uint8_t OP_putstatic      = 179;
const uint8_t OP_getfield       = 180;
const uint8_t OP_putfield       = 181;
const uint8_t OP_invokevirtual  = 182;
const uint8_t OP_invokespecial  = 183;
const uint8_t OP_invokestatic   = 184;
const uint8_t OP_invokeinterface= 185;
const uint8_t OP_new            = 187;
const uint8_t OP_newarray       = 188;
const uint8_t OP_anewarray      = 189;
//const uint8_t OP_arraylength    = 190;
const uint8_t OP_athrow         = 191;
const uint8_t OP_checkcast      = 192;
const uint8_t OP_wide           = 193;

const uint8_t T_BYTE            = 8;

std::vector<uint8_t> &operator<<(std::vector<uint8_t> &a, uint8_t u8)
{
    a.push_back(u8);
    return a;
}

std::vector<uint8_t> &operator<<(std::vector<uint8_t> &a, int16_t s16)
{
    a.push_back((s16 >>  8) & 0xff);
    a.push_back((s16      ) & 0xff);
    return a;
}

std::vector<uint8_t> &operator<<(std::vector<uint8_t> &a, uint16_t u16)
{
    a.push_back((u16 >>  8) & 0xff);
    a.push_back((u16      ) & 0xff);
    return a;
}

std::vector<uint8_t> &operator<<(std::vector<uint8_t> &a, uint32_t u32)
{
    a.push_back((u32 >> 24) & 0xff);
    a.push_back((u32 >> 16) & 0xff);
    a.push_back((u32 >>  8) & 0xff);
    a.push_back((u32      ) & 0xff);
    return a;
}

std::vector<uint8_t> &operator<<(std::vector<uint8_t> &a, const std::vector<uint8_t> &b)
{
    std::copy(b.begin(), b.end(), std::back_inserter(a));
    return a;
}

struct cp_info {
    cp_info(): tag(0), info() {}
    uint8_t tag;
    std::vector<uint8_t> info;

    std::vector<uint8_t> serialize() {
        std::vector<uint8_t> r;
        r << tag;
        r << info;
        return r;
    }
};

struct attribute_info {
    attribute_info(): attribute_name_index(0), info() {}
    uint16_t attribute_name_index;
    //uint32_t attribute_length;
    std::vector<uint8_t> info;

    std::vector<uint8_t> serialize() {
        std::vector<uint8_t> r;
        r << attribute_name_index;
        r << static_cast<uint32_t>(info.size());
        r << info;
        return r;
    }
};

struct Code_attribute {
    Code_attribute(): max_stack(0), max_locals(0), code(), exception_table(), attributes() {}
    uint16_t max_stack;
    uint16_t max_locals;
    //uint32_t code_length;
    std::vector<uint8_t> code;
    //uint16_t exception_table_length;
    struct exception {
        uint16_t start_pc;
        uint16_t end_pc;
        uint16_t handler_pc;
        uint16_t catch_type;
        std::vector<uint8_t> serialize() {
            std::vector<uint8_t> r;
            r << start_pc;
            r << end_pc;
            r << handler_pc;
            r << catch_type;
            return r;
        }
    };
    std::vector<exception> exception_table;
    //uint16_t attributes_count;
    std::vector<attribute_info> attributes;

    std::vector<uint8_t> serialize() {
        std::vector<uint8_t> r;
        r << max_stack;
        r << max_locals;
        r << static_cast<uint32_t>(code.size());
        r << code;
        r << static_cast<uint16_t>(exception_table.size());
        for (auto e: exception_table) {
            r << e.serialize();
        }
        r << static_cast<uint16_t>(attributes.size());
        for (auto a: attributes) {
            r << a.serialize();
        }
        return r;
    }
};

struct field_info {
    field_info(): access_flags(0), name_index(0), descriptor_index(0), attributes() {}
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    //uint16_t attributes_count;
    std::vector<attribute_info> attributes;

    std::vector<uint8_t> serialize() {
        std::vector<uint8_t> r;
        r << access_flags;
        r << name_index;
        r << descriptor_index;
        r << static_cast<uint16_t>(attributes.size());
        for (auto a: attributes) {
            r << a.serialize();
        }
        return r;
    }
};

struct method_info {
    method_info(): access_flags(0), name_index(0), descriptor_index(0), attributes() {}
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    //uint16_t attributes_count;
    std::vector<attribute_info> attributes;

    std::vector<uint8_t> serialize() {
        std::vector<uint8_t> r;
        r << access_flags;
        r << name_index;
        r << descriptor_index;
        r << static_cast<uint16_t>(attributes.size());
        for (auto a: attributes) {
            r << a.serialize();
        }
        return r;
    }
};

class ClassFile {
public:
    ClassFile(const std::string &path, const std::string &name): path(path), name(name), magic(0), minor_version(0), major_version(0), constant_pool_count(0), constant_pool(), access_flags(0), this_class(0), super_class(0), interfaces(), fields(), methods(), attributes(), utf8_constants(), Class_constants(), String_constants(), NameAndType_constants(), Field_constants(), Method_constants(), InterfaceMethod_constants() {}
    ClassFile(const ClassFile &) = delete;
    ClassFile &operator=(const ClassFile &) = delete;
    const std::string path;
    const std::string name;
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    uint16_t constant_pool_count;
    std::vector<cp_info> constant_pool;
    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;
    //uint16_t interfaces_count;
    std::vector<uint16_t> interfaces;
    //uint16_t fields_count;
    std::vector<field_info> fields;
    //uint16_t methods;
    std::vector<method_info> methods;
    //uint16_t attributes_count;
    std::vector<attribute_info> attributes;

    std::map<std::string, uint16_t> utf8_constants;
    uint16_t utf8(const std::string &s) {
        auto i = utf8_constants.find(s);
        if (i != utf8_constants.end()) {
            return i->second;
        }
        cp_info cp;
        cp.tag = CONSTANT_Utf8;
        cp.info << static_cast<uint16_t>(s.length());
        for (auto c: s) {
            cp.info << static_cast<uint8_t>(c);
        }
        constant_pool.push_back(cp);
        constant_pool_count++;
        utf8_constants[s] = constant_pool_count;
        return constant_pool_count;
    }

    std::map<std::string, uint16_t> Class_constants;
    uint16_t Class(const std::string &s) {
        auto i = Class_constants.find(s);
        if (i != Class_constants.end()) {
            return i->second;
        }
        cp_info cp;
        cp.tag = CONSTANT_Class;
        cp.info << utf8(s);
        constant_pool.push_back(cp);
        constant_pool_count++;
        Class_constants[s] = constant_pool_count;
        return constant_pool_count;
    }

    uint16_t Double(double d) {
        cp_info cp;
        cp.tag = CONSTANT_Double;
        uint8_t *p = reinterpret_cast<uint8_t *>(&d);
        cp.info << p[7] << p[6] << p[5] << p[4] << p[3] << p[2] << p[1] << p[0];
        constant_pool.push_back(cp);
        constant_pool_count += 2;
        return constant_pool_count - 1;
    }

    std::map<std::string, uint16_t> String_constants;
    uint16_t String(const std::string &s) {
        auto i = String_constants.find(s);
        if (i != String_constants.end()) {
            return i->second;
        }
        cp_info cp;
        cp.tag = CONSTANT_String;
        cp.info << utf8(s);
        constant_pool.push_back(cp);
        constant_pool_count++;
        String_constants[s] = constant_pool_count;
        return constant_pool_count;
    }

    std::map<std::pair<std::string, std::string>, uint16_t> NameAndType_constants;
    uint16_t NameAndType(const std::string &aname, const std::string &descriptor) {
        auto key = std::make_pair(aname, descriptor);
        auto i = NameAndType_constants.find(key);
        if (i != NameAndType_constants.end()) {
            return i->second;
        }
        cp_info cp;
        cp.tag = CONSTANT_NameAndType;
        cp.info << utf8(aname);
        cp.info << utf8(descriptor);
        constant_pool.push_back(cp);
        constant_pool_count++;
        NameAndType_constants[key] = constant_pool_count;
        return constant_pool_count;
    }

    std::map<std::tuple<std::string, std::string, std::string>, uint16_t> Field_constants;
    uint16_t Field(const std::string &cls, const std::string &aname, const std::string &descriptor) {
        auto key = std::make_tuple(cls, aname, descriptor);
        auto i = Field_constants.find(key);
        if (i != Field_constants.end()) {
            return i->second;
        }
        cp_info cp;
        cp.tag = CONSTANT_Fieldref;
        cp.info << Class(cls);
        cp.info << NameAndType(aname, descriptor);
        constant_pool.push_back(cp);
        constant_pool_count++;
        Field_constants[key] = constant_pool_count;
        return constant_pool_count;
    }

    std::map<std::tuple<std::string, std::string, std::string>, uint16_t> Method_constants;
    uint16_t Method(const std::string &cls, const std::string &aname, const std::string &descriptor) {
        auto key = std::make_tuple(cls, aname, descriptor);
        auto i = Method_constants.find(key);
        if (i != Method_constants.end()) {
            return i->second;
        }
        cp_info cp;
        cp.tag = CONSTANT_Methodref;
        cp.info << Class(cls);
        cp.info << NameAndType(aname, descriptor);
        constant_pool.push_back(cp);
        constant_pool_count++;
        Method_constants[key] = constant_pool_count;
        return constant_pool_count;
    }

    std::map<std::tuple<std::string, std::string, std::string>, uint16_t> InterfaceMethod_constants;
    uint16_t InterfaceMethod(const std::string &cls, const std::string &aname, const std::string &descriptor) {
        auto key = std::make_tuple(cls, aname, descriptor);
        auto i = InterfaceMethod_constants.find(key);
        if (i != InterfaceMethod_constants.end()) {
            return i->second;
        }
        cp_info cp;
        cp.tag = CONSTANT_InterfaceMethodref;
        cp.info << Class(cls);
        cp.info << NameAndType(aname, descriptor);
        constant_pool.push_back(cp);
        constant_pool_count++;
        InterfaceMethod_constants[key] = constant_pool_count;
        return constant_pool_count;
    }

    std::vector<uint8_t> serialize() {
        std::vector<uint8_t> r;
        r << magic;
        r << minor_version;
        r << major_version;
        r << static_cast<uint16_t>(constant_pool_count + 1);
        for (auto c: constant_pool) {
            r << c.serialize();
        }
        r << access_flags;
        r << this_class;
        r << super_class;
        r << static_cast<uint16_t>(interfaces.size());
        for (auto i: interfaces) {
            r << i;
        }
        r << static_cast<uint16_t>(fields.size());
        for (auto f: fields) {
            r << f.serialize();
        }
        r << static_cast<uint16_t>(methods.size());
        for (auto m: methods) {
            r << m.serialize();
        }
        r << static_cast<uint16_t>(attributes.size());
        for (auto a: attributes) {
            r << a.serialize();
        }
        return r;
    }
};

class ClassContext {
public:
    ClassContext(CompilerSupport *support, ClassFile &cf): support(support), cf(cf), generated_types() {}
    ClassContext(const ClassContext &) = delete;
    ClassContext &operator=(const ClassContext &) = delete;
    CompilerSupport *support;
    ClassFile &cf;
    std::set<const class Type *> generated_types;
};

class Context {
public:
    class Label {
        friend class Context;
    public:
        Label(): fixups(), target(UINT16_MAX) {}
    private:
        std::vector<uint16_t> fixups;
        uint16_t target;
    public:
        uint16_t get_target() {
            if (target == UINT16_MAX) {
                internal_error("Label::get_target");
            }
            return target;
        }
    };
    class LoopLabels {
    public:
        LoopLabels(): exit(nullptr), next(nullptr) {}
        LoopLabels(Label *exit, Label *next): exit(exit), next(next) {}
        Label *exit;
        Label *next;
    };
public:
    Context(ClassContext &cc, Code_attribute &ca): cc(cc), cf(cc.cf), ca(ca), label_exit(), loop_labels() {}
    Context(const Context &) = delete;
    Context &operator=(const Context &) = delete;
    ClassContext &cc;
    ClassFile &cf;
    Code_attribute &ca;
    Label label_exit;
    std::map<size_t, LoopLabels> loop_labels;

    Label create_label() {
        return Label();
    }
    void emit_jump(uint8_t b, Label &label) {
        uint16_t this_opcode = static_cast<uint16_t>(ca.code.size());
        ca.code << b;
        if (label.target != UINT16_MAX) {
            ca.code << static_cast<int16_t>(label.target - this_opcode);
        } else {
            label.fixups.push_back(this_opcode);
            ca.code << static_cast<uint16_t>(UINT16_MAX);
        }
    }
    void jump_target(Label &label) {
        assert(label.target == UINT16_MAX);
        label.target = static_cast<uint16_t>(ca.code.size());
        for (auto f: label.fixups) {
            int16_t offset = label.target - f;
            ca.code[f+1] = static_cast<uint8_t>((offset >> 8) & 0xff);
            ca.code[f+2] = static_cast<uint8_t>(offset & 0xff);
        }
    }
    void add_loop_labels(unsigned int loop_id, Label &exit, Label &next) {
        loop_labels[loop_id] = LoopLabels(&exit, &next);
    }
    void remove_loop_labels(unsigned int loop_id) {
        loop_labels.erase(loop_id);
    }
    Label &get_exit_label(unsigned int loop_id) {
        if (loop_labels.find(loop_id) == loop_labels.end()) {
            internal_error("loop_id not found");
        }
        return *loop_labels[loop_id].exit;
    }
    Label &get_next_label(unsigned int loop_id) {
        if (loop_labels.find(loop_id) == loop_labels.end()) {
            internal_error("loop_id not found");
        }
        return *loop_labels[loop_id].next;
    }
    void push_integer(int value) {
        switch (value) {
            case -1: ca.code << OP_iconst_m1; break;
            case  0: ca.code << OP_iconst_0;  break;
            case  1: ca.code << OP_iconst_1;  break;
            case  2: ca.code << OP_iconst_2;  break;
            case  3: ca.code << OP_iconst_3;  break;
            case  4: ca.code << OP_iconst_4;  break;
            case  5: ca.code << OP_iconst_5;  break;
            default:
                if (value >= -128 && value < 128) {
                    ca.code << OP_bipush << static_cast<uint8_t>(value);
                } else if (value >= -32768 && value < 32768) {
                    ca.code << OP_sipush << static_cast<uint16_t>(value);
                } else {
                    assert(false); // TODO: integer constant pool
                }
        }
    }
};

class Type;
class Variable;
class Expression;
class Statement;

static std::map<const ast::Type *, Type *> g_type_cache;
static std::map<const ast::Variable *, Variable *> g_variable_cache;
static std::map<const ast::Expression *, Expression *> g_expression_cache;
static std::map<const ast::Statement *, Statement *> g_statement_cache;

class Type {
public:
    Type(const ast::Type *t, const std::string &classname, const std::string &jtype = ""): classname(classname), jtype(jtype.empty() ? "L" + classname + ";" : jtype) {
        g_type_cache[t] = this;
    }
    Type(const Type &) = delete;
    Type &operator=(const Type &) = delete;
    virtual ~Type() {}
    const std::string classname;
    const std::string jtype;
    virtual void generate_class(Context &) const {}
    virtual void generate_default(Context &context) const = 0;
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const { internal_error("Type::generate_call"); }
};

Type *transform(const ast::Type *t);

class Variable {
public:
    explicit Variable(const ast::Variable *v): type(transform(v->type)) {
        g_variable_cache[v] = this;
    }
    Variable(const Variable &) = delete;
    Variable &operator=(const Variable &) = delete;
    virtual ~Variable() {}
    const Type *type;
    virtual void generate_decl(ClassContext &context, bool exported) const = 0;
    virtual void generate_load(Context &context) const = 0;
    virtual void generate_store(Context &context) const = 0;
    virtual void generate_call(Context &context, const std::vector<const Expression *> &args) const = 0;
};

Variable *transform(const ast::Variable *v);

class Expression {
public:
    explicit Expression(const ast::Expression *node): type(transform(node->type)) {
        g_expression_cache[node] = this;
    }
    Expression(const Expression &) = delete;
    Expression &operator=(const Expression &) = delete;
    virtual ~Expression() {}
    const Type *type;
    virtual void generate(Context &context) const = 0;
    virtual void generate_call(Context &context, const std::vector<const Expression *> &) const = 0;
    virtual void generate_store(Context &context) const = 0;
};

Expression *transform(const ast::Expression *e);

class Statement {
public:
    explicit Statement(const ast::Statement *s) {
        g_statement_cache[s] = this;
    }
    virtual ~Statement() {}
    virtual void generate(Context &context) const = 0;
};

Statement *transform(const ast::Statement *s);

class TypeNothing: public Type {
public:
    explicit TypeNothing(const ast::TypeNothing *tn): Type(tn, "V", "V"), tn(tn) {}
    TypeNothing(const TypeNothing &) = delete;
    TypeNothing &operator=(const TypeNothing &) = delete;
    const ast::TypeNothing *tn;
    virtual void generate_default(Context &) const override { internal_error("TypeNothing"); }
};

class TypeDummy: public Type {
public:
    explicit TypeDummy(const ast::TypeDummy *td): Type(td, ""), td(td) {}
    TypeDummy(const TypeDummy &) = delete;
    TypeDummy &operator=(const TypeDummy &) = delete;
    const ast::TypeDummy *td;
    virtual void generate_default(Context &) const override { internal_error("TypeDummy"); }
};

class TypeBoolean: public Type {
public:
    explicit TypeBoolean(const ast::TypeBoolean *tb): Type(tb, "java/lang/Boolean"), tb(tb) {}
    TypeBoolean(const TypeBoolean &) = delete;
    TypeBoolean &operator=(const TypeBoolean &) = delete;
    const ast::TypeBoolean *tb;
    virtual void generate_default(Context &context) const override {
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "FALSE", "Ljava/lang/Boolean;");
    }
};

class TypeNumber: public Type {
public:
    explicit TypeNumber(const ast::TypeNumber *tn): Type(tn, "neon/type/Number"), tn(tn) {}
    TypeNumber(const TypeNumber &) = delete;
    TypeNumber &operator=(const TypeNumber &) = delete;
    const ast::TypeNumber *tn;
    virtual void generate_default(Context &context) const override {
        context.ca.code << OP_getstatic << context.cf.Field("neon/type/Number", "ZERO", "Lneon/type/Number;");
    }
};

class TypeString: public Type {
public:
    explicit TypeString(const ast::TypeString *ts): Type(ts, "java/lang/String"), ts(ts) {}
    TypeString(const TypeString &) = delete;
    TypeString &operator=(const TypeString &) = delete;
    const ast::TypeString *ts;
    virtual void generate_default(Context &context) const override {
        context.ca.code << OP_ldc_w << context.cf.String("");
    }
};

class TypeBytes: public Type {
public:
    explicit TypeBytes(const ast::TypeBytes *tb): Type(tb, "[B", "[B"), tb(tb) {}
    TypeBytes(const TypeBytes &) = delete;
    TypeBytes &operator=(const TypeBytes &) = delete;
    const ast::TypeBytes *tb;
    virtual void generate_default(Context &context) const override {
        context.push_integer(0);
        context.ca.code << OP_newarray << T_BYTE;
    }
};

class TypeObject: public Type {
public:
    explicit TypeObject(const ast::TypeObject *to): Type(to, "java/lang/Object"), to(to) {} // TODO
    TypeObject(const TypeObject &) = delete;
    TypeObject &operator=(const TypeObject &) = delete;
    const ast::TypeObject *to;
    virtual void generate_default(Context &context) const override {
        context.ca.code << OP_aconst_null; // TODO
    }
};

class TypeFunction: public Type {
public:
    explicit TypeFunction(const ast::TypeFunction *tf): Type(tf, ""), tf(tf), returntype(transform(tf->returntype)), paramtypes() {
        for (auto p: tf->params) {
            paramtypes.push_back(std::make_pair(p->mode, transform(p->type)));
        }
    }
    TypeFunction(const TypeFunction &) = delete;
    TypeFunction &operator=(const TypeFunction &) = delete;
    const ast::TypeFunction *tf;
    const Type *returntype;
    std::vector<std::pair<ast::ParameterType::Mode, const Type *>> paramtypes;
    virtual void generate_default(Context &) const override { internal_error("TypeFunction"); }
};

class TypeArray: public Type {
public:
    explicit TypeArray(const ast::TypeArray *ta): Type(ta, "neon/type/Array"), ta(ta), elementtype(transform(ta->elementtype)) {}
    TypeArray(const TypeArray &) = delete;
    TypeArray &operator=(const TypeArray &) = delete;
    const ast::TypeArray *ta;
    const Type *elementtype;
    virtual void generate_default(Context &context) const override {
        context.ca.code << OP_new << context.cf.Class("neon/type/Array");
        context.ca.code << OP_dup;
        context.ca.code << OP_invokespecial << context.cf.Method("neon/type/Array", "<init>", "()V");
    }
};

class TypeDictionary: public Type {
public:
    explicit TypeDictionary(const ast::TypeDictionary *td): Type(td, "java/util/HashMap"), td(td), elementtype(transform(td->elementtype)) {}
    TypeDictionary(const TypeDictionary &) = delete;
    TypeDictionary &operator=(const TypeDictionary &) = delete;
    const ast::TypeDictionary *td;
    const Type *elementtype;
    virtual void generate_default(Context &context) const override {
        context.ca.code << OP_new << context.cf.Class("java/lang/HashMap");
        context.ca.code << OP_dup;
        context.ca.code << OP_invokespecial << context.cf.Method("java/lang/HashMap", "<init>", "()V");
    }
};

class TypeRecord: public Type {
public:
    explicit TypeRecord(const ast::TypeRecord *tr): Type(tr, tr->module + "$" + tr->name.substr(tr->name.find('.')+1)), tr(tr), field_types() {
        for (auto f: tr->fields) {
            field_types.push_back(transform(f.type));
        }
    }
    TypeRecord(const TypeRecord &) = delete;
    TypeRecord &operator=(const TypeRecord &) = delete;
    const ast::TypeRecord *tr;
    std::vector<const Type *> field_types;

    virtual void generate_default(Context &context) const override {
        context.ca.code << OP_new << context.cf.Class(classname);
        context.ca.code << OP_dup;
        for (size_t i = 0; i < tr->fields.size(); i++) {
            field_types[i]->generate_default(context);
        }
        context.ca.code << OP_invokespecial << context.cf.Method(classname, "<init>", get_init_descriptor());
    }
    virtual void generate_class(Context &context) const override {
        ClassFile cf(context.cf.path, classname);
        cf.magic = 0xCAFEBABE;
        cf.minor_version = 0;
        cf.major_version = 49;
        cf.constant_pool_count = 0;
        cf.access_flags = ACC_PUBLIC | ACC_SUPER;
        cf.this_class = cf.Class(cf.name);
        cf.super_class = cf.Class("java/lang/Object");

        for (size_t i = 0; i < tr->fields.size(); i++) {
            field_info f;
            f.access_flags = 0;
            f.name_index = cf.utf8(tr->fields[i].name.text);
            f.descriptor_index = cf.utf8(field_types[i]->jtype);
            cf.fields.push_back(f);
        }
        {
            method_info m;
            m.access_flags = ACC_PUBLIC;
            m.name_index = cf.utf8("<init>");
            m.descriptor_index = cf.utf8(get_init_descriptor());
            {
                attribute_info code;
                code.attribute_name_index = cf.utf8("Code");
                {
                    Code_attribute ca;
                    ca.max_stack = 16;
                    ca.max_locals = static_cast<uint16_t>(1 + tr->fields.size());
                    ca.code << OP_aload_0;
                    ca.code << OP_invokespecial << cf.Method("java/lang/Object", "<init>", "()V");
                    for (size_t i = 0; i < tr->fields.size(); i++) {
                        ca.code << OP_aload_0;
                        size_t p = i + 1;
                        if (p < 4) {
                            ca.code << static_cast<uint8_t>(OP_aload_0 + p);
                        } else if (p < 256) {
                            ca.code << OP_aload << static_cast<uint8_t>(p);
                        } else {
                            ca.code << OP_wide << OP_aload << static_cast<uint16_t>(p);
                        }
                        ca.code << OP_putfield << cf.Field(classname, tr->fields[i].name.text, field_types[i]->jtype);
                    }
                    ca.code << OP_return;
                    code.info = ca.serialize();
                }
                m.attributes.push_back(code);
            }
            cf.methods.push_back(m);
        }

        auto data = cf.serialize();
        context.cc.support->writeOutput(context.cf.path + cf.name + ".class", data);
    }
    std::string get_init_descriptor() const {
        std::string descriptor = "(";
        for (auto t: field_types) {
            descriptor.append(t->jtype);
        }
        descriptor += ")V";
        return descriptor;
    }
};

class TypePointer: public Type {
public:
    explicit TypePointer(const ast::TypePointer *tp): Type(tp, tp->reftype != nullptr ? transform(tp->reftype)->classname : "java/lang/Object"), tp(tp) {}
    TypePointer(const TypePointer &) = delete;
    TypePointer &operator=(const TypePointer &) = delete;
    const ast::TypePointer *tp;
    virtual void generate_default(Context &context) const override {
        context.ca.code << OP_aconst_null;
    }
};

class TypeInterfacePointer: public Type {
public:
    explicit TypeInterfacePointer(const ast::TypeInterfacePointer *tip): Type(tip, "java/lang/Object" /* TODO transform(tip->interface)->classname */), tip(tip) {}
    TypeInterfacePointer(const TypeInterfacePointer &) = delete;
    TypeInterfacePointer &operator=(const TypeInterfacePointer &) = delete;
    const ast::TypeInterfacePointer *tip;
    virtual void generate_default(Context &context) const override {
        context.ca.code << OP_aconst_null;
    }
};

class TypeFunctionPointer: public Type {
public:
    explicit TypeFunctionPointer(const ast::TypeFunctionPointer *fp): Type(fp, "java/lang/reflect/Method"), fp(fp), functype(dynamic_cast<const TypeFunction *>(transform(fp->functype))) {}
    TypeFunctionPointer(const TypeFunctionPointer &) = delete;
    TypeFunctionPointer &operator=(const TypeFunctionPointer &) = delete;
    const ast::TypeFunctionPointer *fp;
    const TypeFunction *functype;
    virtual void generate_default(Context &context) const override {
        context.ca.code << OP_aconst_null;
    }
    virtual void generate_call(Context &context, const std::vector<const Expression *> &args) const override {
        context.ca.code << OP_aconst_null;
        context.push_integer(static_cast<int>(args.size()));
        context.ca.code << OP_anewarray << context.cf.Class("java/lang/Object");
        int i = 0;
        for (auto a: args) {
            context.ca.code << OP_dup;
            context.push_integer(i);
            a->generate(context);
            context.ca.code << OP_aastore;
            i++;
        }
        context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/reflect/Method", "invoke", "(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;");
        context.ca.code << OP_checkcast << context.cf.Class(functype->returntype->classname);
    }
};

class TypeEnum: public Type {
public:
    explicit TypeEnum(const ast::TypeEnum *te): Type(te, te->module + "$" + te->name), te(te) {}
    TypeEnum(const TypeEnum &) = delete;
    TypeEnum &operator=(const TypeEnum &) = delete;
    const ast::TypeEnum *te;

    virtual void generate_default(Context &context) const override {
        // TODO: not begin(). Whatever value 0 is.
        context.ca.code << OP_getstatic << context.cf.Field(classname, te->names.begin()->first, jtype);
    }
    virtual void generate_class(Context &context) const override {
        ClassFile cf(context.cf.path, classname);
        cf.magic = 0xCAFEBABE;
        cf.minor_version = 0;
        cf.major_version = 49;
        cf.constant_pool_count = 0;
        cf.access_flags = ACC_PUBLIC | ACC_FINAL | ACC_SUPER;
        cf.this_class = cf.Class(cf.name);
        cf.super_class = cf.Class("java/lang/Object");

        for (auto e: te->names) {
            field_info f;
            f.access_flags = ACC_PUBLIC | ACC_STATIC | ACC_FINAL;
            f.name_index = cf.utf8(e.first);
            f.descriptor_index = cf.utf8(jtype);
            cf.fields.push_back(f);
        }
#if 0
        {
            method_info m;
            m.access_flags = 0;
            m.name_index = cf.utf8("<init>");
            m.descriptor_index = cf.utf8("(Ljava/lang/String;I)V");
            {
                attribute_info code;
                code.attribute_name_index = cf.utf8("Code");
                {
                    Code_attribute ca;
                    ca.max_stack = 16;
                    ca.max_locals = 4;
                    Context function_context(context.cc, ca);
                    ca.code << OP_aload_0;
                    ca.code << OP_invokespecial << cf.Method("java/lang/Object", "<init>", "()V");
                    //ca.code << OP_aload_1;
                    //ca.code << OP_putfield << cf.Field(classname, "$NAME", "Ljava/lang/String;");
                    //ca.code << OP_aload_0;
                    //ca.code << OP_new << cf.Class("neon/type/Number");
                    //ca.code << OP_dup;
                    //ca.code << OP_iload_2;
                    //ca.code << OP_invokespecial << cf.Method("neon/type/Number", "<init>", "(I)V");
                    //ca.code << OP_putfield << cf.Field(classname, "$ORDINAL", "Lneon/type/Number;");
                    ca.code << OP_return;
                    code.info = ca.serialize();
                }
                m.attributes.push_back(code);
            }
            cf.methods.push_back(m);
        }
        {
            method_info m;
            m.access_flags = ACC_PUBLIC | ACC_STATIC;
            m.name_index = cf.utf8("enum.toString");
            m.descriptor_index = cf.utf8("(" + jtype + ")Ljava/lang/String;");
            {
                attribute_info code;
                code.attribute_name_index = cf.utf8("Code");
                {
                    Code_attribute ca;
                    ca.max_stack = 16;
                    ca.max_locals = 4;
                    Context function_context(context.cc, ca);
                    ca.code << OP_aload_0;
                    ca.code << OP_getfield << cf.Field(classname, "$NAME", "Ljava/lang/String;");
                    ca.code << OP_areturn;
                    code.info = ca.serialize();
                }
                m.attributes.push_back(code);
            }
            cf.methods.push_back(m);
        }
#endif
        {
            method_info m;
            m.access_flags = ACC_STATIC;
            m.name_index = cf.utf8("<clinit>");
            m.descriptor_index = cf.utf8("()V");
            {
                attribute_info code;
                code.attribute_name_index = cf.utf8("Code");
                {
                    Code_attribute ca;
                    ca.max_stack = 16;
                    ca.max_locals = 4;
                    Context function_context(context.cc, ca);
#if 0
                    for (auto e: te->names) {
                        ca.code << OP_new << cf.Class(classname);
                        ca.code << OP_dup;
                        ca.code << OP_ldc_w << cf.String(e.first);
                        function_context.push_integer(e.second);
                        ca.code << OP_invokespecial << cf.Method(classname, "<init>", "(Ljava/lang/String;I)V");
                        ca.code << OP_putstatic << cf.Field(classname, e.first, jtype);
                    }
#endif
                    ca.code << OP_return;
                    code.info = ca.serialize();
                }
                m.attributes.push_back(code);
            }
            cf.methods.push_back(m);
        }

        auto data = cf.serialize();
        context.cc.support->writeOutput(context.cf.path + cf.name + ".class", data);
    }
};

class TypeChoice: public Type {
public:
    explicit TypeChoice(const ast::TypeChoice *tc): Type(tc, tc->module + "$" + tc->name), tc(tc) {}
    TypeChoice(const TypeChoice &) = delete;
    TypeChoice &operator=(const TypeChoice &) = delete;
    const ast::TypeChoice *tc;

    virtual void generate_default(Context &) const override {
        internal_error("TypeChoice");
    }
};

class PredefinedVariable: public Variable {
public:
    explicit PredefinedVariable(const ast::PredefinedVariable *pv): Variable(pv), pv(pv) {}
    PredefinedVariable(const PredefinedVariable &) = delete;
    PredefinedVariable &operator=(const PredefinedVariable &) = delete;
    const ast::PredefinedVariable *pv;

    virtual void generate_decl(ClassContext &, bool) const override {}
    virtual void generate_load(Context &context) const override {
        if (pv->name == "textio$stderr") {
            context.ca.code << OP_getstatic << context.cf.Field("java/lang/System", "err", "Ljava/io/PrintStream;");
        } else {
            internal_error("PredefinedVariable: " + pv->name);
        }
    }
    virtual void generate_store(Context &) const override { internal_error("PredefinedVariable"); }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("PredefinedVariable"); }
};

class ModuleVariable: public Variable {
public:
    explicit ModuleVariable(const ast::ModuleVariable *mv): Variable(mv), mv(mv) {}
    ModuleVariable(const ModuleVariable &) = delete;
    ModuleVariable &operator=(const ModuleVariable &) = delete;
    const ast::ModuleVariable *mv;

    virtual void generate_decl(ClassContext &, bool) const override {}
    virtual void generate_load(Context &context) const override {
        context.ca.code << OP_getstatic << context.cf.Field(mv->module->name, mv->name, type->jtype);
    }
    virtual void generate_store(Context &context) const override {
        context.ca.code << OP_putstatic << context.cf.Field(mv->module->name, mv->name, type->jtype);
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ModuleVariable"); }
};

class GlobalVariable: public Variable {
public:
    explicit GlobalVariable(const ast::GlobalVariable *gv): Variable(gv), gv(gv), name() {}
    GlobalVariable(const GlobalVariable &) = delete;
    GlobalVariable &operator=(const GlobalVariable &) = delete;
    const ast::GlobalVariable *gv;
    mutable std::string name;

    virtual void generate_decl(ClassContext &context, bool exported) const override {
        // Hopefully this gets called first, before generate_load or generate_store because
        // the actual name used for the field gets created in here.
        field_info f;
        f.access_flags = ACC_STATIC;
        name = exported ? gv->name : gv->name + "__" + std::to_string(reinterpret_cast<intptr_t>(this));
        f.name_index = context.cf.utf8(name);
        f.descriptor_index = context.cf.utf8(type->jtype);
        context.cf.fields.push_back(f);
    }
    virtual void generate_load(Context &context) const override {
        context.ca.code << OP_getstatic << context.cf.Field(context.cf.name, name, type->jtype);
    }
    virtual void generate_store(Context &context) const override {
        context.ca.code << OP_putstatic << context.cf.Field(context.cf.name, name, type->jtype);
    }
    virtual void generate_call(Context &context, const std::vector<const Expression *> &args) const override {
        generate_load(context);
        type->generate_call(context, args);
    }
};

class LocalVariable: public Variable {
public:
    explicit LocalVariable(const ast::LocalVariable *lv): Variable(lv), lv(lv), index(-1) {}
    LocalVariable(const LocalVariable &) = delete;
    LocalVariable &operator=(const LocalVariable &) = delete;
    const ast::LocalVariable *lv;
    mutable int index; // TODO

    virtual void generate_decl(ClassContext &, bool) const override {}
    virtual void generate_load(Context &context) const override {
        if (index < 0) {
            index = context.ca.max_locals;
            context.ca.max_locals++;
        }
        if (index < 4) {
            context.ca.code << static_cast<uint8_t>(OP_aload_0 + index);
        } else if (index < 256) {
            context.ca.code << OP_aload << static_cast<uint8_t>(index);
        } else {
            context.ca.code << OP_wide << OP_aload << static_cast<uint16_t>(index);
        }
    }
    virtual void generate_store(Context &context) const override {
        if (index < 0) {
            index = context.ca.max_locals;
            context.ca.max_locals++;
        }
        if (index < 4) {
            context.ca.code << static_cast<uint8_t>(OP_astore_0 + index);
        } else if (index < 256) {
            context.ca.code << OP_astore << static_cast<uint8_t>(index);
        } else {
            context.ca.code << OP_wide << OP_astore << static_cast<uint16_t>(index);
        }
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("LocalVariable"); }
};

class FunctionParameter: public Variable {
public:
    explicit FunctionParameter(const ast::FunctionParameter *fp, int index): Variable(fp), fp(fp), index(index) {}
    FunctionParameter(const FunctionParameter &) = delete;
    FunctionParameter &operator=(const FunctionParameter &) = delete;
    const ast::FunctionParameter *fp;
    const int index;

    virtual void generate_decl(ClassContext &, bool) const override { internal_error("FunctionParameter"); }
    virtual void generate_load(Context &context) const override {
        if (index < 4) {
            context.ca.code << static_cast<uint8_t>(OP_aload_0 + index);
        } else if (index < 256) {
            context.ca.code << OP_aload << static_cast<uint8_t>(index);
        } else {
            context.ca.code << OP_wide << OP_aload << static_cast<uint16_t>(index);
        }
    }
    virtual void generate_store(Context &context) const override {
        if (index < 4) {
            context.ca.code << static_cast<uint8_t>(OP_astore_0 + index);
        } else if (index < 256) {
            context.ca.code << OP_astore << static_cast<uint8_t>(index);
        } else {
            context.ca.code << OP_wide << OP_astore << static_cast<uint16_t>(index);
        }
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("FunctionParameter"); }
};

class ConstantBooleanExpression: public Expression {
public:
    explicit ConstantBooleanExpression(const ast::ConstantBooleanExpression *cbe): Expression(cbe), cbe(cbe) {}
    ConstantBooleanExpression(const ConstantBooleanExpression &) = delete;
    ConstantBooleanExpression &operator=(const ConstantBooleanExpression &) = delete;
    const ast::ConstantBooleanExpression *cbe;

    virtual void generate(Context &context) const override {
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", cbe->value ? "TRUE" : "FALSE", "Ljava/lang/Boolean;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantBooleanExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantBooleanExpression"); }
};

class ConstantNumberExpression: public Expression {
public:
    explicit ConstantNumberExpression(const ast::ConstantNumberExpression *cne): Expression(cne), cne(cne) {}
    ConstantNumberExpression(const ConstantNumberExpression &) = delete;
    ConstantNumberExpression &operator=(const ConstantNumberExpression &) = delete;
    const ast::ConstantNumberExpression *cne;

    virtual void generate(Context &context) const override {
        if (number_is_zero(cne->value)) {
            context.ca.code << OP_getstatic << context.cf.Field("neon/type/Number", "ZERO", "Lneon/type/Number;");
        } else if (number_is_equal(cne->value, number_from_uint32(1))) {
            context.ca.code << OP_getstatic << context.cf.Field("neon/type/Number", "ONE", "Lneon/type/Number;");
        } else {
            context.ca.code << OP_new << context.cf.Class("neon/type/Number");
            context.ca.code << OP_dup;
            context.ca.code << OP_ldc_w << context.cf.String(number_to_string(cne->value));
            context.ca.code << OP_invokespecial << context.cf.Method("neon/type/Number", "<init>", "(Ljava/lang/String;)V");
        }
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantNumberExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantNumberExpression"); }
};

class ConstantStringExpression: public Expression {
public:
    explicit ConstantStringExpression(const ast::ConstantStringExpression *cse): Expression(cse), cse(cse) {}
    ConstantStringExpression(const ConstantStringExpression &) = delete;
    ConstantStringExpression &operator=(const ConstantStringExpression &) = delete;
    const ast::ConstantStringExpression *cse;

    virtual void generate(Context &context) const override {
        context.ca.code << OP_ldc_w << context.cf.String(cse->value.str());
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantStringExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantStringExpression"); }
};

class ConstantBytesExpression: public Expression {
public:
    explicit ConstantBytesExpression(const ast::ConstantBytesExpression *cbe): Expression(cbe), cbe(cbe) {}
    ConstantBytesExpression(const ConstantBytesExpression &) = delete;
    ConstantBytesExpression &operator=(const ConstantBytesExpression &) = delete;
    const ast::ConstantBytesExpression *cbe;

    virtual void generate(Context &context) const override {
        context.push_integer(static_cast<int>(cbe->contents.size()));
        context.ca.code << OP_newarray << T_BYTE;
        int i = 0;
        for (auto c: cbe->contents) {
            context.ca.code << OP_dup;
            context.push_integer(i);
            context.push_integer(static_cast<int8_t>(c & 0xff));
            context.ca.code << OP_bastore;
            i++;
        }
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantBytesExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantBytesExpression"); }
};

class ConstantEnumExpression: public Expression {
public:
    explicit ConstantEnumExpression(const ast::ConstantEnumExpression *cee): Expression(cee), cee(cee), type(dynamic_cast<const TypeEnum *>(transform(cee->type))) {}
    ConstantEnumExpression(const ConstantEnumExpression &) = delete;
    ConstantEnumExpression &operator=(const ConstantEnumExpression &) = delete;
    const ast::ConstantEnumExpression *cee;
    const TypeEnum *type;

    virtual void generate(Context &context) const override {
        context.cc.generated_types.insert(type); // TODO: do this somewhere better, like at the type declaration itself (which the ast doesn't seem to have right now).
        for (auto e: type->te->names) {
            if (e.second == cee->value) {
                context.ca.code << OP_getstatic << context.cf.Field(type->classname, e.first, type->jtype);
                return;
            }
        }
        internal_error("ConstantEnumExpression");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantEnumExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantEnumExpression"); }
};

class ConstantNilExpression: public Expression {
public:
    explicit ConstantNilExpression(const ast::ConstantNilExpression *cne): Expression(cne), cne(cne) {}
    ConstantNilExpression(const ConstantNilExpression &) = delete;
    ConstantNilExpression &operator=(const ConstantNilExpression &) = delete;
    const ast::ConstantNilExpression *cne;

    virtual void generate(Context &context) const override {
        context.ca.code << OP_aconst_null;
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantNilExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantNilExpression"); }
};

class ConstantNowhereExpression: public Expression {
public:
    explicit ConstantNowhereExpression(const ast::ConstantNowhereExpression *cne): Expression(cne), cne(cne) {}
    ConstantNowhereExpression(const ConstantNowhereExpression &) = delete;
    ConstantNowhereExpression &operator=(const ConstantNowhereExpression &) = delete;
    const ast::ConstantNowhereExpression *cne;

    virtual void generate(Context &context) const override {
        context.ca.code << OP_aconst_null;
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConstantNowhereExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantNowhereExpression"); }
};

class TypeConversionExpression: public Expression {
public:
    explicit TypeConversionExpression(const ast::TypeConversionExpression *tce): Expression(tce), tce(tce), expr(transform(tce->expr)) {}
    TypeConversionExpression(const TypeConversionExpression &) = delete;
    TypeConversionExpression &operator=(const TypeConversionExpression &) = delete;
    const ast::TypeConversionExpression *tce;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        expr->generate(context);
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("TypeConversionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("TypeConversionExpression"); }
};

class ArrayLiteralExpression: public Expression {
public:
    explicit ArrayLiteralExpression(const ast::ArrayLiteralExpression *ale): Expression(ale), ale(ale), elements() {
        for (auto e: ale->elements) {
            elements.push_back(transform(e));
        }
    }
    ArrayLiteralExpression(const ArrayLiteralExpression &) = delete;
    ArrayLiteralExpression &operator=(const ArrayLiteralExpression &) = delete;
    const ast::ArrayLiteralExpression *ale;
    std::vector<const Expression *> elements;

    virtual void generate(Context &context) const override {
        context.ca.code << OP_new << context.cf.Class("neon/type/Array");
        context.ca.code << OP_dup;
        // TODO: initial capacity context.ca.code <<
        context.ca.code << OP_invokespecial << context.cf.Method("neon/type/Array", "<init>", "()V");
        for (auto e: elements) {
            context.ca.code << OP_dup;
            e->generate(context);
            context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Array", "add", "(Ljava/lang/Object;)Z");
            context.ca.code << OP_pop;
        }
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ArrayLiteralExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ArrayLiteralExpression"); }
};

class DictionaryLiteralExpression: public Expression {
public:
    explicit DictionaryLiteralExpression(const ast::DictionaryLiteralExpression *dle): Expression(dle), dle(dle), dict() {
        for (auto d: dle->dict) {
            dict[d.first.str()] = transform(d.second); // TODO: utf8
        }
    }
    DictionaryLiteralExpression(const DictionaryLiteralExpression &) = delete;
    DictionaryLiteralExpression &operator=(const DictionaryLiteralExpression &) = delete;
    const ast::DictionaryLiteralExpression *dle;
    std::map<std::string, const Expression *> dict;

    virtual void generate(Context &context) const override {
        context.ca.code << OP_new << context.cf.Class("java/util/HashMap");
        context.ca.code << OP_dup;
        context.ca.code << OP_invokespecial << context.cf.Method("java/util/HashMap", "<init>", "()V");
        for (auto d: dict) {
            context.ca.code << OP_dup;
            context.ca.code << OP_ldc_w << context.cf.String(d.first);
            d.second->generate(context);
            context.ca.code << OP_invokevirtual << context.cf.Method("java/util/HashMap", "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
            context.ca.code << OP_pop;
        }
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DictionaryLiteralExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DictionaryLiteralExpression"); }
};

class RecordLiteralExpression: public Expression {
public:
    explicit RecordLiteralExpression(const ast::RecordLiteralExpression *rle): Expression(rle), rle(rle), type(dynamic_cast<TypeRecord *>(transform(rle->type))), values() {
        for (auto v: rle->values) {
            values.push_back(transform(v));
        }
    }
    RecordLiteralExpression(const RecordLiteralExpression &) = delete;
    RecordLiteralExpression &operator=(const RecordLiteralExpression &) = delete;
    const ast::RecordLiteralExpression *rle;
    const TypeRecord *type;
    std::vector<const Expression *> values;

    virtual void generate(Context &context) const override {
        context.cc.generated_types.insert(type); // TODO: do this somewhere better, like at the type declaration itself (which the ast doesn't seem to have right now).
        context.ca.code << OP_new << context.cf.Class(type->classname);
        context.ca.code << OP_dup;
        for (auto v: values) {
            v->generate(context);
        }
        context.ca.code << OP_invokespecial << context.cf.Method(type->classname, "<init>", type->get_init_descriptor());
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("RecordLiteralExpression"); }
    virtual void generate_store(Context &) const override { internal_error("RecordLiteralExpression"); }
};

class ClassLiteralExpression: public RecordLiteralExpression {
public:
    explicit ClassLiteralExpression(const ast::ClassLiteralExpression *cle): RecordLiteralExpression(cle), cle(cle) {}
    ClassLiteralExpression(const ClassLiteralExpression &) = delete;
    ClassLiteralExpression &operator=(const ClassLiteralExpression &) = delete;
    const ast::ClassLiteralExpression *cle;
};

class NewClassExpression: public Expression {
public:
    explicit NewClassExpression(const ast::NewClassExpression *nce): Expression(nce), nce(nce), value(transform(nce->value)), type(dynamic_cast<const TypeRecord *>(transform(dynamic_cast<const ast::TypeValidPointer *>(nce->type)->reftype))) {}
    NewClassExpression(const NewClassExpression &) = delete;
    NewClassExpression &operator=(const NewClassExpression &) = delete;
    const ast::NewClassExpression *nce;
    const Expression *value;
    const TypeRecord *type;

    virtual void generate(Context &context) const override {
        context.cc.generated_types.insert(type); // TODO: do this somewhere better, like at the type declaration itself (which the ast doesn't seem to have right now).
        if (value != nullptr) {
            value->generate(context);
        } else {
            type->generate_default(context);
        }
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("NewClassExpression"); }
    virtual void generate_store(Context &) const override { internal_error("NewClassExpression"); }
};

class UnaryMinusExpression: public Expression {
public:
    explicit UnaryMinusExpression(const ast::UnaryMinusExpression *ume): Expression(ume), ume(ume), value(transform(ume->value)) {}
    UnaryMinusExpression(const UnaryMinusExpression &) = delete;
    UnaryMinusExpression &operator=(const UnaryMinusExpression &) = delete;
    const ast::UnaryMinusExpression *ume;
    const Expression *value;

    virtual void generate(Context &context) const override {
        value->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "negate", "()Lneon/type/Number;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("UnaryMinusExpression"); }
    virtual void generate_store(Context &) const override { internal_error("UnaryMinusExpression"); }
};

class LogicalNotExpression: public Expression {
public:
    explicit LogicalNotExpression(const ast::LogicalNotExpression *lne): Expression(lne), lne(lne), value(transform(lne->value)) {}
    LogicalNotExpression(const LogicalNotExpression &) = delete;
    LogicalNotExpression &operator=(const LogicalNotExpression &) = delete;
    const ast::LogicalNotExpression *lne;
    const Expression *value;

    virtual void generate(Context &context) const override {
        value->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/Boolean", "booleanValue", "()Z");
        context.ca.code << OP_iconst_1;
        context.ca.code << OP_ixor;
        context.ca.code << OP_invokestatic << context.cf.Method("java/lang/Boolean", "valueOf", "(Z)Ljava/lang/Boolean;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("LogicalNotExpression"); }
    virtual void generate_store(Context &) const override { internal_error("LogicalNotExpression"); }
};

class ConditionalExpression: public Expression {
public:
    explicit ConditionalExpression(const ast::ConditionalExpression *ce): Expression(ce), ce(ce), condition(transform(ce->condition)), left(transform(ce->left)), right(transform(ce->right)) {}
    ConditionalExpression(const ConditionalExpression &) = delete;
    ConditionalExpression &operator=(const ConditionalExpression &) = delete;
    const ast::ConditionalExpression *ce;
    const Expression *condition;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        condition->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/Boolean", "booleanValue", "()Z");
        auto label_done = context.create_label();
        auto label_false = context.create_label();
        context.emit_jump(OP_ifeq, label_false);
        left->generate(context);
        context.emit_jump(OP_goto, label_done);
        context.jump_target(label_false);
        right->generate(context);
        context.jump_target(label_done);
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConditionalExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConditionalExpression"); }
};

class TryExpressionTrap {
public:
    explicit TryExpressionTrap(const ast::TryTrap *tt): tt(tt), name(transform(tt->name)), handler(), gives(transform(dynamic_cast<const ast::Expression *>(tt->handler))) {
        const ast::ExceptionHandlerStatement *h = dynamic_cast<const ast::ExceptionHandlerStatement *>(tt->handler);
        if (h != nullptr) {
            for (auto s: h->statements) {
                handler.push_back(transform(s));
            }
        }
    }
    TryExpressionTrap(const TryExpressionTrap &) = delete;
    TryExpressionTrap &operator=(const TryExpressionTrap &) = delete;
    const ast::TryTrap *tt;
    const Variable *name;
    std::vector<const Statement *> handler;
    const Expression *gives;
};

class TryExpression: public Expression {
public:
    explicit TryExpression(const ast::TryExpression *te): Expression(te), te(te), expr(transform(te->expr)), catches() {
        for (auto &t: te->catches) {
            catches.push_back(new TryExpressionTrap(&t));
        }
    }
    TryExpression(const TryExpression &) = delete;
    TryExpression &operator=(const TryExpression &) = delete;
    const ast::TryExpression *te;
    const Expression *expr;
    std::vector<const TryExpressionTrap *> catches;

    virtual void generate(Context &context) const override {
        Code_attribute::exception e;
        e.start_pc = static_cast<uint16_t>(context.ca.code.size());
        expr->generate(context);
        e.end_pc = static_cast<uint16_t>(context.ca.code.size());
        auto label_end = context.create_label();
        context.emit_jump(OP_goto, label_end);
        for (auto trap: catches) {
            e.handler_pc = static_cast<uint16_t>(context.ca.code.size());
            context.ca.code << OP_pop; // TODO: store in local if indicated
            if (trap->gives != nullptr) {
                trap->gives->generate(context);
                context.emit_jump(OP_goto, label_end);
            } else {
                for (auto s: trap->handler) {
                    s->generate(context);
                }
            }
            e.catch_type = context.cf.Class("neon/type/NeonException");
            context.ca.exception_table.push_back(e);
        }
        context.jump_target(label_end);
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("TryExpression"); }
    virtual void generate_store(Context &) const override { internal_error("TryExpression"); }
};

class DisjunctionExpression: public Expression {
public:
    explicit DisjunctionExpression(const ast::DisjunctionExpression *de): Expression(de), de(de), left(transform(de->left)), right(transform(de->right)) {}
    DisjunctionExpression(const DisjunctionExpression &) = delete;
    DisjunctionExpression &operator=(const DisjunctionExpression &) = delete;
    const ast::DisjunctionExpression *de;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        context.ca.code << OP_dup;
        context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/Boolean", "booleanValue", "()Z");
        auto label_true = context.create_label();
        context.emit_jump(OP_ifne, label_true);
        context.ca.code << OP_pop;
        right->generate(context);
        context.jump_target(label_true);
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DisjunctionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DisjunctionExpression"); }
};

class ConjunctionExpression: public Expression {
public:
    explicit ConjunctionExpression(const ast::ConjunctionExpression *ce): Expression(ce), ce(ce), left(transform(ce->left)), right(transform(ce->right)) {}
    ConjunctionExpression(const ConjunctionExpression &) = delete;
    ConjunctionExpression &operator=(const ConjunctionExpression &) = delete;
    const ast::ConjunctionExpression *ce;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        context.ca.code << OP_dup;
        context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/Boolean", "booleanValue", "()Z");
        auto label_false = context.create_label();
        context.emit_jump(OP_ifeq, label_false);
        context.ca.code << OP_pop;
        right->generate(context);
        context.jump_target(label_false);
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ConjunctionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConjunctionExpression"); }
};

class ArrayInExpression: public Expression {
public:
    explicit ArrayInExpression(const ast::ArrayInExpression *aie): Expression(aie), aie(aie), left(transform(aie->left)), right(transform(aie->right)) {}
    ArrayInExpression(const ArrayInExpression &) = delete;
    ArrayInExpression &operator=(const ArrayInExpression &) = delete;
    const ast::ArrayInExpression *aie;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        right->generate(context);
        left->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Array", "in", "(Ljava/lang/Object;)Z");
        context.ca.code << OP_invokestatic << context.cf.Method("java/lang/Boolean", "valueOf", "(Z)Ljava/lang/Boolean;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ArrayInExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ArrayInExpression"); }
};

class DictionaryInExpression: public Expression {
public:
    explicit DictionaryInExpression(const ast::DictionaryInExpression *die): Expression(die), die(die), left(transform(die->left)), right(transform(die->right)) {}
    DictionaryInExpression(const DictionaryInExpression &) = delete;
    DictionaryInExpression &operator=(const DictionaryInExpression &) = delete;
    const ast::DictionaryInExpression *die;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        right->generate(context);
        left->generate(context);
        context.ca.code << OP_invokeinterface << context.cf.InterfaceMethod("java/util/Map", "containsKey", "(Ljava/lang/Object;)Z") << static_cast<uint8_t>(2) << static_cast<uint8_t>(0);
        context.ca.code << OP_invokestatic << context.cf.Method("java/lang/Boolean", "valueOf", "(Z)Ljava/lang/Boolean;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DictionaryInExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DictionaryInExpression"); }
};

class ComparisonExpression: public Expression {
public:
    explicit ComparisonExpression(const ast::ComparisonExpression *ce): Expression(ce), ce(ce), left(transform(ce->left)), right(transform(ce->right)) {}
    ComparisonExpression(const ComparisonExpression &) = delete;
    ComparisonExpression &operator=(const ComparisonExpression &) = delete;
    const ast::ComparisonExpression *ce;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        right->generate(context);
        generate_comparison(context);
    }
    virtual void generate_comparison(Context &context) const = 0;

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ComparisonExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ComparisonExpression"); }
};

class ChainedComparisonExpression: public Expression {
public:
    explicit ChainedComparisonExpression(const ast::ChainedComparisonExpression *cce): Expression(cce), cce(cce), comps() {
        for (auto c: cce->comps) {
            const ComparisonExpression *ce = dynamic_cast<const ComparisonExpression *>(transform(c));
            if (ce == nullptr) {
                internal_error("ChainedComparisonExpression");
            }
            comps.push_back(ce);
        }
    }
    ChainedComparisonExpression(const ChainedComparisonExpression &) = delete;
    ChainedComparisonExpression &operator=(const ChainedComparisonExpression &) = delete;
    const ast::ChainedComparisonExpression *cce;
    std::vector<const ComparisonExpression *> comps;

    virtual void generate(Context &context) const override {
        comps[0]->left->generate(context);
        auto label_false = context.create_label();
        size_t i = 0;
        for (auto c: comps) {
            c->right->generate(context);
            context.ca.code << OP_dup_x1;
            c->generate_comparison(context);
            if (i+1 < comps.size()) {
                context.ca.code << OP_dup;
                context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/Boolean", "booleanValue", "()Z");
                context.emit_jump(OP_ifeq, label_false);
                context.ca.code << OP_pop;
            }
            i++;
        }
        context.jump_target(label_false);
        context.ca.code << OP_swap;
        context.ca.code << OP_pop;
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ChainedComparisonExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ChainedComparisonExpression"); }
};

class BooleanComparisonExpression: public ComparisonExpression {
public:
    explicit BooleanComparisonExpression(const ast::BooleanComparisonExpression *bce): ComparisonExpression(bce), bce(bce) {}
    BooleanComparisonExpression(const BooleanComparisonExpression &) = delete;
    BooleanComparisonExpression &operator=(const BooleanComparisonExpression &) = delete;
    const ast::BooleanComparisonExpression *bce;

    virtual void generate_comparison(Context &context) const override {
        context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/Boolean", "booleanValue", "()Z");
        context.ca.code << OP_swap;
        context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/Boolean", "booleanValue", "()Z");
        auto label_true = context.create_label();
        switch (bce->comp) {
            case ast::ComparisonExpression::Comparison::EQ: context.emit_jump(OP_ifeq, label_true); break;
            case ast::ComparisonExpression::Comparison::NE: context.emit_jump(OP_ifne, label_true); break;
            default:
                internal_error("unexpected comparison type");
        }
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "FALSE", "Ljava/lang/Boolean;");
        auto label_false = context.create_label();
        context.emit_jump(OP_goto, label_false);
        context.jump_target(label_true);
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "TRUE", "Ljava/lang/Boolean;");
        context.jump_target(label_false);
    }
};

class NumericComparisonExpression: public ComparisonExpression {
public:
    explicit NumericComparisonExpression(const ast::NumericComparisonExpression *nce): ComparisonExpression(nce), nce(nce) {}
    NumericComparisonExpression(const NumericComparisonExpression &) = delete;
    NumericComparisonExpression &operator=(const NumericComparisonExpression &) = delete;
    const ast::NumericComparisonExpression *nce;

    virtual void generate_comparison(Context &context) const override {
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "compareTo", "(Lneon/type/Number;)I");
        auto label_true = context.create_label();
        switch (nce->comp) {
            case ast::ComparisonExpression::Comparison::EQ: context.emit_jump(OP_ifeq, label_true); break;
            case ast::ComparisonExpression::Comparison::NE: context.emit_jump(OP_ifne, label_true); break;
            case ast::ComparisonExpression::Comparison::LT: context.emit_jump(OP_iflt, label_true); break;
            case ast::ComparisonExpression::Comparison::GT: context.emit_jump(OP_ifgt, label_true); break;
            case ast::ComparisonExpression::Comparison::LE: context.emit_jump(OP_ifle, label_true); break;
            case ast::ComparisonExpression::Comparison::GE: context.emit_jump(OP_ifge, label_true); break;
        }
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "FALSE", "Ljava/lang/Boolean;");
        auto label_false = context.create_label();
        context.emit_jump(OP_goto, label_false);
        context.jump_target(label_true);
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "TRUE", "Ljava/lang/Boolean;");
        context.jump_target(label_false);
    }
};

class EnumComparisonExpression: public ComparisonExpression {
public:
    explicit EnumComparisonExpression(const ast::EnumComparisonExpression *ece): ComparisonExpression(ece), ece(ece) {}
    EnumComparisonExpression(const EnumComparisonExpression &) = delete;
    EnumComparisonExpression &operator=(const EnumComparisonExpression &) = delete;
    const ast::EnumComparisonExpression *ece;

    virtual void generate_comparison(Context &context) const override {
        auto label_true = context.create_label();
        switch (ece->comp) {
            case ast::ComparisonExpression::Comparison::EQ: context.emit_jump(OP_if_acmpeq, label_true); break;
            case ast::ComparisonExpression::Comparison::NE: context.emit_jump(OP_if_acmpne, label_true); break;
            default:
                internal_error("unexpected comparison type");
        }
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "FALSE", "Ljava/lang/Boolean;");
        auto label_false = context.create_label();
        context.emit_jump(OP_goto, label_false);
        context.jump_target(label_true);
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "TRUE", "Ljava/lang/Boolean;");
        context.jump_target(label_false);
    }
};

class StringComparisonExpression: public ComparisonExpression {
public:
    explicit StringComparisonExpression(const ast::StringComparisonExpression *sce): ComparisonExpression(sce), sce(sce) {}
    StringComparisonExpression(const StringComparisonExpression &) = delete;
    StringComparisonExpression &operator=(const StringComparisonExpression &) = delete;
    const ast::StringComparisonExpression *sce;

    virtual void generate_comparison(Context &context) const override {
        context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/String", "compareTo", "(Ljava/lang/String;)I");
        auto label_true = context.create_label();
        switch (sce->comp) {
            case ast::ComparisonExpression::Comparison::EQ: context.emit_jump(OP_ifeq, label_true); break;
            case ast::ComparisonExpression::Comparison::NE: context.emit_jump(OP_ifne, label_true); break;
            case ast::ComparisonExpression::Comparison::LT: context.emit_jump(OP_iflt, label_true); break;
            case ast::ComparisonExpression::Comparison::GT: context.emit_jump(OP_ifgt, label_true); break;
            case ast::ComparisonExpression::Comparison::LE: context.emit_jump(OP_ifle, label_true); break;
            case ast::ComparisonExpression::Comparison::GE: context.emit_jump(OP_ifge, label_true); break;
        }
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "FALSE", "Ljava/lang/Boolean;");
        auto label_false = context.create_label();
        context.emit_jump(OP_goto, label_false);
        context.jump_target(label_true);
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "TRUE", "Ljava/lang/Boolean;");
        context.jump_target(label_false);
    }
};

class BytesComparisonExpression: public ComparisonExpression {
public:
    explicit BytesComparisonExpression(const ast::BytesComparisonExpression *bce): ComparisonExpression(bce), bce(bce) {}
    BytesComparisonExpression(const BytesComparisonExpression &) = delete;
    BytesComparisonExpression &operator=(const BytesComparisonExpression &) = delete;
    const ast::BytesComparisonExpression *bce;

    virtual void generate_comparison(Context &context) const override {
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "bytes__compare", "([B[B)I");
        auto label_true = context.create_label();
        switch (bce->comp) {
            case ast::ComparisonExpression::Comparison::EQ: context.emit_jump(OP_ifeq, label_true); break;
            case ast::ComparisonExpression::Comparison::NE: context.emit_jump(OP_ifne, label_true); break;
            case ast::ComparisonExpression::Comparison::LT: context.emit_jump(OP_iflt, label_true); break;
            case ast::ComparisonExpression::Comparison::GT: context.emit_jump(OP_ifgt, label_true); break;
            case ast::ComparisonExpression::Comparison::LE: context.emit_jump(OP_ifle, label_true); break;
            case ast::ComparisonExpression::Comparison::GE: context.emit_jump(OP_ifge, label_true); break;
        }
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "FALSE", "Ljava/lang/Boolean;");
        auto label_false = context.create_label();
        context.emit_jump(OP_goto, label_false);
        context.jump_target(label_true);
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "TRUE", "Ljava/lang/Boolean;");
        context.jump_target(label_false);
    }
};

class ArrayComparisonExpression: public ComparisonExpression {
public:
    explicit ArrayComparisonExpression(const ast::ArrayComparisonExpression *ace): ComparisonExpression(ace), ace(ace) {}
    ArrayComparisonExpression(const ArrayComparisonExpression &) = delete;
    ArrayComparisonExpression &operator=(const ArrayComparisonExpression &) = delete;
    const ast::ArrayComparisonExpression *ace;

    virtual void generate_comparison(Context &context) const override {
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Array", "equals", "(Ljava/lang/Object;)Z");
        if (ace->comp == ast::ComparisonExpression::Comparison::EQ) {
            // nothing
        } else if (ace->comp == ast::ComparisonExpression::Comparison::NE) {
            context.ca.code << OP_iconst_1;
            context.ca.code << OP_ixor;
        } else {
            internal_error("ArrayComparisonExpression");
        }
        context.ca.code << OP_invokestatic << context.cf.Method("java/lang/Boolean", "valueOf", "(Z)Ljava/lang/Boolean;");
    }
};

class DictionaryComparisonExpression: public ComparisonExpression {
public:
    explicit DictionaryComparisonExpression(const ast::DictionaryComparisonExpression *dce): ComparisonExpression(dce), dce(dce) {}
    DictionaryComparisonExpression(const DictionaryComparisonExpression &) = delete;
    DictionaryComparisonExpression &operator=(const DictionaryComparisonExpression &) = delete;
    const ast::DictionaryComparisonExpression *dce;

    virtual void generate_comparison(Context &context) const override {
        context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/Object", "equals", "(Ljava/lang/Object;)Z");
        if (dce->comp == ast::ComparisonExpression::Comparison::EQ) {
            // nothing
        } else if (dce->comp == ast::ComparisonExpression::Comparison::NE) {
            context.ca.code << OP_iconst_1;
            context.ca.code << OP_ixor;
        } else {
            internal_error("DictionaryComparisonExpression");
        }
        context.ca.code << OP_invokestatic << context.cf.Method("java/lang/Boolean", "valueOf", "(Z)Ljava/lang/Boolean;");
    }
};

class RecordComparisonExpression: public ComparisonExpression {
public:
    explicit RecordComparisonExpression(const ast::RecordComparisonExpression *rce): ComparisonExpression(rce), rce(rce) {}
    RecordComparisonExpression(const RecordComparisonExpression &) = delete;
    RecordComparisonExpression &operator=(const RecordComparisonExpression &) = delete;
    const ast::RecordComparisonExpression *rce;

    virtual void generate_comparison(Context &context) const override {
        // TODO: Need to recursively evaluate equality.
        context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/Object", "equals", "(Ljava/lang/Object;)Z");
        if (rce->comp == ast::ComparisonExpression::Comparison::EQ) {
            // nothing
        } else if (rce->comp == ast::ComparisonExpression::Comparison::NE) {
            context.ca.code << OP_iconst_1;
            context.ca.code << OP_ixor;
        } else {
            internal_error("RecordComparisonExpression");
        }
        context.ca.code << OP_invokestatic << context.cf.Method("java/lang/Boolean", "valueOf", "(Z)Ljava/lang/Boolean;");
    }
};

class PointerComparisonExpression: public ComparisonExpression {
public:
    explicit PointerComparisonExpression(const ast::PointerComparisonExpression *pce): ComparisonExpression(pce), pce(pce) {}
    PointerComparisonExpression(const PointerComparisonExpression &) = delete;
    PointerComparisonExpression &operator=(const PointerComparisonExpression &) = delete;
    const ast::PointerComparisonExpression *pce;

    virtual void generate_comparison(Context &context) const override {
        auto label_true = context.create_label();
        switch (pce->comp) {
            case ast::ComparisonExpression::Comparison::EQ: context.emit_jump(OP_if_acmpeq, label_true); break;
            case ast::ComparisonExpression::Comparison::NE: context.emit_jump(OP_if_acmpne, label_true); break;
            default:
                internal_error("unexpected comparison type");
        }
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "FALSE", "Ljava/lang/Boolean;");
        auto label_false = context.create_label();
        context.emit_jump(OP_goto, label_false);
        context.jump_target(label_true);
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "TRUE", "Ljava/lang/Boolean;");
        context.jump_target(label_false);
    }
};

class ValidPointerExpression: public PointerComparisonExpression {
public:
    explicit ValidPointerExpression(const ast::ValidPointerExpression *vpe): PointerComparisonExpression(vpe), vpe(vpe), var(transform(vpe->var)) {}
    ValidPointerExpression(const ValidPointerExpression &) = delete;
    ValidPointerExpression &operator=(const ValidPointerExpression &) = delete;
    const ast::ValidPointerExpression *vpe;
    const Variable *var;

    virtual void generate(Context &context) const override {
        left->generate(context);
        context.ca.code << OP_dup;
        var->generate_store(context);
        right->generate(context);
        auto label_true = context.create_label();
        context.emit_jump(OP_if_acmpne, label_true);
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "FALSE", "Ljava/lang/Boolean;");
        auto label_false = context.create_label();
        context.emit_jump(OP_goto, label_false);
        context.jump_target(label_true);
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "TRUE", "Ljava/lang/Boolean;");
        context.jump_target(label_false);
    }
};

class FunctionPointerComparisonExpression: public ComparisonExpression {
public:
    explicit FunctionPointerComparisonExpression(const ast::FunctionPointerComparisonExpression *fpce): ComparisonExpression(fpce), fpce(fpce) {}
    FunctionPointerComparisonExpression(const FunctionPointerComparisonExpression &) = delete;
    FunctionPointerComparisonExpression &operator=(const FunctionPointerComparisonExpression &) = delete;
    const ast::FunctionPointerComparisonExpression *fpce;

    virtual void generate_comparison(Context &context) const override {
        auto label_true = context.create_label();
        switch (fpce->comp) {
            case ast::ComparisonExpression::Comparison::EQ: context.emit_jump(OP_if_acmpeq, label_true); break;
            case ast::ComparisonExpression::Comparison::NE: context.emit_jump(OP_if_acmpne, label_true); break;
            default:
                internal_error("unexpected comparison type");
        }
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "FALSE", "Ljava/lang/Boolean;");
        auto label_false = context.create_label();
        context.emit_jump(OP_goto, label_false);
        context.jump_target(label_true);
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "TRUE", "Ljava/lang/Boolean;");
        context.jump_target(label_false);
    }
};

class AdditionExpression: public Expression {
public:
    explicit AdditionExpression(const ast::AdditionExpression *ae): Expression(ae), ae(ae), left(transform(ae->left)), right(transform(ae->right)) {}
    AdditionExpression(const AdditionExpression &) = delete;
    AdditionExpression &operator=(const AdditionExpression &) = delete;
    const ast::AdditionExpression *ae;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        right->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "add", "(Lneon/type/Number;)Lneon/type/Number;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("AdditionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("AdditionExpression"); }
};

class SubtractionExpression: public Expression {
public:
    explicit SubtractionExpression(const ast::SubtractionExpression *se): Expression(se), se(se), left(transform(se->left)), right(transform(se->right)) {}
    SubtractionExpression(const SubtractionExpression &) = delete;
    SubtractionExpression &operator=(const SubtractionExpression &) = delete;
    const ast::SubtractionExpression *se;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        right->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "subtract", "(Lneon/type/Number;)Lneon/type/Number;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("SubtractionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("SubtractionExpression"); }
};

class MultiplicationExpression: public Expression {
public:
    explicit MultiplicationExpression(const ast::MultiplicationExpression *me): Expression(me), me(me), left(transform(me->left)), right(transform(me->right)) {}
    MultiplicationExpression(const MultiplicationExpression &) = delete;
    MultiplicationExpression &operator=(const MultiplicationExpression &) = delete;
    const ast::MultiplicationExpression *me;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        right->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "multiply", "(Lneon/type/Number;)Lneon/type/Number;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("MultiplicationExpression"); }
    virtual void generate_store(Context &) const override { internal_error("MultiplicationExpression"); }
};

class DivisionExpression: public Expression {
public:
    explicit DivisionExpression(const ast::DivisionExpression *de): Expression(de), de(de), left(transform(de->left)), right(transform(de->right)) {}
    DivisionExpression(const DivisionExpression &) = delete;
    DivisionExpression &operator=(const DivisionExpression &) = delete;
    const ast::DivisionExpression *de;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        right->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "divide", "(Lneon/type/Number;)Lneon/type/Number;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DivisionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DivisionExpression"); }
};

class ModuloExpression: public Expression {
public:
    explicit ModuloExpression(const ast::ModuloExpression *me): Expression(me), me(me), left(transform(me->left)), right(transform(me->right)) {}
    ModuloExpression(const ModuloExpression &) = delete;
    ModuloExpression &operator=(const ModuloExpression &) = delete;
    const ast::ModuloExpression *me;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        right->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "remainder", "(Lneon/type/Number;)Lneon/type/Number;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ModuloExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ModuloExpression"); }
};

class ExponentiationExpression: public Expression {
public:
    explicit ExponentiationExpression(const ast::ExponentiationExpression *ee): Expression(ee), ee(ee), left(transform(ee->left)), right(transform(ee->right)) {}
    ExponentiationExpression(const ExponentiationExpression &) = delete;
    ExponentiationExpression &operator=(const ExponentiationExpression &) = delete;
    const ast::ExponentiationExpression *ee;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        right->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "pow", "(Lneon/type/Number;)Lneon/type/Number;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ExponentiationExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ExponentiationExpression"); }
};

class DummyExpression: public Expression {
public:
    explicit DummyExpression(const ast::DummyExpression *de): Expression(de), de(de) {}
    DummyExpression(const DummyExpression &) = delete;
    DummyExpression &operator=(const DummyExpression &) = delete;
    const ast::DummyExpression *de;

    virtual void generate(Context &context) const override { context.ca.code << OP_aconst_null; /* TODO: need this to do nothing for OUT _ parameters; internal_error("DummyExpression"); */ }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DummyExpression"); }
    virtual void generate_store(Context &context) const override {
        context.ca.code << OP_pop;
    }
};

class ArrayReferenceIndexExpression: public Expression {
public:
    explicit ArrayReferenceIndexExpression(const ast::ArrayReferenceIndexExpression *arie): Expression(arie), arie(arie), array(transform(arie->array)), index(transform(arie->index)) {}
    ArrayReferenceIndexExpression(const ArrayReferenceIndexExpression &) = delete;
    ArrayReferenceIndexExpression &operator=(const ArrayReferenceIndexExpression &) = delete;
    const ast::ArrayReferenceIndexExpression *arie;
    const Expression *array;
    const Expression *index;

    virtual void generate(Context &context) const override {
        array->generate(context);
        index->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Array", "get", "(Lneon/type/Number;)Ljava/lang/Object;");
        context.ca.code << OP_checkcast << context.cf.Class(dynamic_cast<const TypeArray *>(array->type)->elementtype->classname);
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ArrayReferenceIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        array->generate(context);
        context.ca.code << OP_swap;
        index->generate(context);
        context.ca.code << OP_swap;
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Array", "set", "(Lneon/type/Number;Ljava/lang/Object;)Ljava/lang/Object;");
        context.ca.code << OP_pop; // TODO: is this right? seems wrong. should be array->generate_store(context)? See BytesReferenceIndexExpression.
    }
};

class ArrayValueIndexExpression: public Expression {
public:
    explicit ArrayValueIndexExpression(const ast::ArrayValueIndexExpression *avie): Expression(avie), avie(avie), array(transform(avie->array)), index(transform(avie->index)) {}
    ArrayValueIndexExpression(const ArrayValueIndexExpression &) = delete;
    ArrayValueIndexExpression &operator=(const ArrayValueIndexExpression &) = delete;
    const ast::ArrayValueIndexExpression *avie;
    const Expression *array;
    const Expression *index;

    virtual void generate(Context &context) const override {
        array->generate(context);
        index->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Array", "get", "(Lneon/type/Number;)Ljava/lang/Object;");
        context.ca.code << OP_checkcast << context.cf.Class(dynamic_cast<const TypeArray *>(array->type)->elementtype->classname);
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ArrayValueIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        array->generate(context);
        context.ca.code << OP_swap;
        index->generate(context);
        context.ca.code << OP_swap;
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Array", "set", "(Lneon/type/Number;Ljava/lang/Object;)Ljava/lang/Object;");
        context.ca.code << OP_pop;
    }
};

class DictionaryReferenceIndexExpression: public Expression {
public:
    explicit DictionaryReferenceIndexExpression(const ast::DictionaryReferenceIndexExpression *drie): Expression(drie), drie(drie), dictionary(transform(drie->dictionary)), index(transform(drie->index)) {}
    DictionaryReferenceIndexExpression(const DictionaryReferenceIndexExpression &) = delete;
    DictionaryReferenceIndexExpression &operator=(const DictionaryReferenceIndexExpression &) = delete;
    const ast::DictionaryReferenceIndexExpression *drie;
    const Expression *dictionary;
    const Expression *index;

    virtual void generate(Context &context) const override {
        dictionary->generate(context);
        index->generate(context);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "dictionary__get", "(Ljava/util/Map;Ljava/lang/String;)Ljava/lang/Object;");
        context.ca.code << OP_checkcast << context.cf.Class(dynamic_cast<const TypeDictionary *>(dictionary->type)->elementtype->classname);
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DictionaryReferenceIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        dictionary->generate(context);
        context.ca.code << OP_swap;
        index->generate(context);
        context.ca.code << OP_swap;
        context.ca.code << OP_invokeinterface << context.cf.InterfaceMethod("java/util/Map", "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;") << static_cast<uint8_t>(3) << static_cast<uint8_t>(0);
        context.ca.code << OP_pop;
    }
};

class DictionaryValueIndexExpression: public Expression {
public:
    explicit DictionaryValueIndexExpression(const ast::DictionaryValueIndexExpression *dvie): Expression(dvie), dvie(dvie), dictionary(transform(dvie->dictionary)), index(transform(dvie->index)) {}
    DictionaryValueIndexExpression(const DictionaryValueIndexExpression &) = delete;
    DictionaryValueIndexExpression &operator=(const DictionaryValueIndexExpression &) = delete;
    const ast::DictionaryValueIndexExpression *dvie;
    const Expression *dictionary;
    const Expression *index;

    virtual void generate(Context &context) const override {
        dictionary->generate(context);
        index->generate(context);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "dictionary__get", "(Ljava/util/Map;Ljava/lang/String;)Ljava/lang/Object;");
        context.ca.code << OP_checkcast << context.cf.Class(dynamic_cast<const TypeDictionary *>(dictionary->type)->elementtype->classname);
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("DictionaryValueIndexExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DictionaryValueIndexExpression"); }
};

class StringReferenceIndexExpression: public Expression {
public:
    explicit StringReferenceIndexExpression(const ast::StringReferenceIndexExpression *srie): Expression(srie), srie(srie), ref(transform(srie->ref)), index(transform(srie->index)) {}
    StringReferenceIndexExpression(const StringReferenceIndexExpression &) = delete;
    StringReferenceIndexExpression &operator=(const StringReferenceIndexExpression &) = delete;
    const ast::StringReferenceIndexExpression *srie;
    const Expression *ref;
    const Expression *index;

    virtual void generate(Context &context) const override {
        ref->generate(context);
        index->generate(context);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "string__index", "(Ljava/lang/String;Lneon/type/Number;)Ljava/lang/String;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("StringReferenceIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        ref->generate(context);
        index->generate(context);
        context.push_integer(false);
        index->generate(context);
        context.push_integer(false);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "string__splice", "(Ljava/lang/String;Ljava/lang/String;Lneon/type/Number;ZLneon/type/Number;Z)Ljava/lang/String;");
        ref->generate_store(context);
    }
};

class StringValueIndexExpression: public Expression {
public:
    explicit StringValueIndexExpression(const ast::StringValueIndexExpression *svie): Expression(svie), svie(svie), str(transform(svie->str)), index(transform(svie->index)) {}
    StringValueIndexExpression(const StringValueIndexExpression &) = delete;
    StringValueIndexExpression &operator=(const StringValueIndexExpression &) = delete;
    const ast::StringValueIndexExpression *svie;
    const Expression *str;
    const Expression *index;

    virtual void generate(Context &context) const override {
        str->generate(context);
        index->generate(context);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "string__index", "(Ljava/lang/String;Lneon/type/Number;)Ljava/lang/String;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("StringValueIndexExpression"); }
    virtual void generate_store(Context&) const override { internal_error("StringValueIndexExpression"); }
};

class StringReferenceRangeIndexExpression: public Expression {
public:
    explicit StringReferenceRangeIndexExpression(const ast::StringReferenceRangeIndexExpression *srie): Expression(srie), srie(srie), ref(transform(srie->ref)), first(transform(srie->first)), last(transform(srie->last)) {}
    StringReferenceRangeIndexExpression(const StringReferenceRangeIndexExpression &) = delete;
    StringReferenceRangeIndexExpression &operator=(const StringReferenceRangeIndexExpression &) = delete;
    const ast::StringReferenceRangeIndexExpression *srie;
    const Expression *ref;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        ref->generate(context);
        first->generate(context);
        context.push_integer(srie->first_from_end);
        last->generate(context);
        context.push_integer(srie->last_from_end);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "string__substring", "(Ljava/lang/String;Lneon/type/Number;ZLneon/type/Number;Z)Ljava/lang/String;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("StringReferenceRangeIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        ref->generate(context);
        first->generate(context);
        context.push_integer(srie->first_from_end);
        last->generate(context);
        context.push_integer(srie->last_from_end);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "string__splice", "(Ljava/lang/String;Ljava/lang/String;Lneon/type/Number;ZLneon/type/Number;Z)Ljava/lang/String;");
        ref->generate_store(context);
    }
};

class StringValueRangeIndexExpression: public Expression {
public:
    explicit StringValueRangeIndexExpression(const ast::StringValueRangeIndexExpression *svie): Expression(svie), svie(svie), str(transform(svie->str)), first(transform(svie->first)), last(transform(svie->last)) {}
    StringValueRangeIndexExpression(const StringValueRangeIndexExpression &) = delete;
    StringValueRangeIndexExpression &operator=(const StringValueRangeIndexExpression &) = delete;
    const ast::StringValueRangeIndexExpression *svie;
    const Expression *str;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        str->generate(context);
        first->generate(context);
        context.push_integer(svie->first_from_end);
        last->generate(context);
        context.push_integer(svie->last_from_end);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "string__substring", "(Ljava/lang/String;Lneon/type/Number;ZLneon/type/Number;Z)Ljava/lang/String;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("StringValueRangeIndexExpression"); }
    virtual void generate_store(Context&) const override { internal_error("StringValueRangeIndexExpression"); }
};

class BytesReferenceIndexExpression: public Expression {
public:
    explicit BytesReferenceIndexExpression(const ast::BytesReferenceIndexExpression *brie): Expression(brie), brie(brie), ref(transform(brie->ref)), index(transform(brie->index)) {}
    BytesReferenceIndexExpression(const BytesReferenceIndexExpression &) = delete;
    BytesReferenceIndexExpression &operator=(const BytesReferenceIndexExpression &) = delete;
    const ast::BytesReferenceIndexExpression *brie;
    const Expression *ref;
    const Expression *index;

    virtual void generate(Context &context) const override {
        ref->generate(context);
        index->generate(context);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "bytes__index", "([BLneon/type/Number;)Lneon/type/Number;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("BytesReferenceIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        ref->generate(context);
        index->generate(context);
        context.push_integer(false);
        index->generate(context);
        context.push_integer(false);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "bytes__splice", "([B[BLneon/type/Number;ZLneon/type/Number;Z)[B");
        ref->generate_store(context);
    }
};

class BytesValueIndexExpression: public Expression {
public:
    explicit BytesValueIndexExpression(const ast::BytesValueIndexExpression *bvie): Expression(bvie), bvie(bvie), data(transform(bvie->str)), index(transform(bvie->index)) {}
    BytesValueIndexExpression(const BytesValueIndexExpression &) = delete;
    BytesValueIndexExpression &operator=(const BytesValueIndexExpression &) = delete;
    const ast::BytesValueIndexExpression *bvie;
    const Expression *data;
    const Expression *index;

    virtual void generate(Context &context) const override {
        data->generate(context);
        index->generate(context);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "bytes__index", "([BLneon/type/Number;)Lneon/type/Number;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("BytesValueIndexExpression"); }
    virtual void generate_store(Context &) const override { internal_error("BytesValueIndexExpression"); }
};

class BytesReferenceRangeIndexExpression: public Expression {
public:
    explicit BytesReferenceRangeIndexExpression(const ast::BytesReferenceRangeIndexExpression *brie): Expression(brie), brie(brie), ref(transform(brie->ref)), first(transform(brie->first)), last(transform(brie->last)) {}
    BytesReferenceRangeIndexExpression(const BytesReferenceRangeIndexExpression &) = delete;
    BytesReferenceRangeIndexExpression &operator=(const BytesReferenceRangeIndexExpression &) = delete;
    const ast::BytesReferenceRangeIndexExpression *brie;
    const Expression *ref;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        ref->generate(context);
        first->generate(context);
        context.push_integer(brie->first_from_end);
        last->generate(context);
        context.push_integer(brie->last_from_end);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "bytes__range", "([BLneon/type/Number;ZLneon/type/Number;Z)[B");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("BytesReferenceRangeIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        ref->generate(context);
        first->generate(context);
        context.push_integer(brie->first_from_end);
        last->generate(context);
        context.push_integer(brie->last_from_end);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "bytes__splice", "([B[BLneon/type/Number;ZLneon/type/Number;Z)[B");
        ref->generate_store(context);
    }
};

class BytesValueRangeIndexExpression: public Expression {
public:
    explicit BytesValueRangeIndexExpression(const ast::BytesValueRangeIndexExpression *bvie): Expression(bvie), bvie(bvie), data(transform(bvie->str)), first(transform(bvie->first)), last(transform(bvie->last)) {}
    BytesValueRangeIndexExpression(const BytesValueRangeIndexExpression &) = delete;
    BytesValueRangeIndexExpression &operator=(const BytesValueRangeIndexExpression &) = delete;
    const ast::BytesValueRangeIndexExpression *bvie;
    const Expression *data;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        data->generate(context);
        first->generate(context);
        context.push_integer(bvie->first_from_end);
        last->generate(context);
        context.push_integer(bvie->last_from_end);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "bytes__range", "([BLneon/type/Number;ZLneon/type/Number;Z)[B");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("BytesValueRangeIndexExpression"); }
    virtual void generate_store(Context &) const override { internal_error("BytesValueRangeIndexExpression"); }
};

class RecordReferenceFieldExpression: public Expression {
public:
    explicit RecordReferenceFieldExpression(const ast::RecordReferenceFieldExpression *rrfe): Expression(rrfe), rrfe(rrfe), ref(transform(rrfe->ref)), rectype(dynamic_cast<const TypeRecord *>(transform(rrfe->ref->type))), fieldtype(transform(rrfe->type)) {}
    RecordReferenceFieldExpression(const RecordReferenceFieldExpression &) = delete;
    RecordReferenceFieldExpression &operator=(const RecordReferenceFieldExpression &) = delete;
    const ast::RecordReferenceFieldExpression *rrfe;
    const Expression *ref;
    const TypeRecord *rectype;
    const Type *fieldtype;

    virtual void generate(Context &context) const override {
        ref->generate(context);
        context.ca.code << OP_getfield << context.cf.Field(rectype->classname, rrfe->field, fieldtype->jtype);
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("RecordReferenceFieldExpression"); }
    virtual void generate_store(Context &context) const override {
        ref->generate(context);
        context.ca.code << OP_swap;
        context.ca.code << OP_putfield << context.cf.Field(rectype->classname, rrfe->field, fieldtype->jtype);
    }
};

class RecordValueFieldExpression: public Expression {
public:
    explicit RecordValueFieldExpression(const ast::RecordValueFieldExpression *rvfe): Expression(rvfe), rvfe(rvfe), rec(transform(rvfe->rec)), rectype(dynamic_cast<const TypeRecord *>(transform(rvfe->rec->type))), fieldtype(transform(rvfe->type)) {}
    RecordValueFieldExpression(const RecordValueFieldExpression &) = delete;
    RecordValueFieldExpression &operator=(const RecordValueFieldExpression &) = delete;
    const ast::RecordValueFieldExpression *rvfe;
    const Expression *rec;
    const TypeRecord *rectype;
    const Type *fieldtype;

    virtual void generate(Context &context) const override {
        rec->generate(context);
        context.ca.code << OP_getfield << context.cf.Field(rectype->classname, rvfe->field, fieldtype->jtype);
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("RecordValueFieldExpression"); }
    virtual void generate_store(Context &) const override { internal_error("RecordValueFieldExpression"); }
};

class ArrayReferenceRangeExpression: public Expression {
public:
    explicit ArrayReferenceRangeExpression(const ast::ArrayReferenceRangeExpression *arre): Expression(arre), arre(arre), ref(transform(arre->ref)), first(transform(arre->first)), last(transform(arre->last)) {}
    ArrayReferenceRangeExpression(const ArrayReferenceRangeExpression &) = delete;
    ArrayReferenceRangeExpression &operator=(const ArrayReferenceRangeExpression &) = delete;
    const ast::ArrayReferenceRangeExpression *arre;
    const Expression *ref;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        ref->generate(context);
        first->generate(context);
        context.push_integer(arre->first_from_end);
        last->generate(context);
        context.push_integer(arre->last_from_end);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "array__slice", "(Lneon/type/Array;Lneon/type/Number;ZLneon/type/Number;Z)Lneon/type/Array;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ArrayReferenceRangeExpression"); }
    virtual void generate_store(Context &context) const override {
        ref->generate(context);
        first->generate(context);
        context.push_integer(arre->first_from_end);
        last->generate(context);
        context.push_integer(arre->last_from_end);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "array__splice", "(Lneon/type/Array;Lneon/type/Array;Lneon/type/Number;ZLneon/type/Number;Z)Lneon/type/Array;");
        ref->generate_store(context);
    }
};

class ArrayValueRangeExpression: public Expression {
public:
    explicit ArrayValueRangeExpression(const ast::ArrayValueRangeExpression *avre): Expression(avre), avre(avre), array(transform(avre->array)), first(transform(avre->first)), last(transform(avre->last)) {}
    ArrayValueRangeExpression(const ArrayValueRangeExpression &) = delete;
    ArrayValueRangeExpression &operator=(const ArrayValueRangeExpression &) = delete;
    const ast::ArrayValueRangeExpression *avre;
    const Expression *array;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &context) const override {
        array->generate(context);
        first->generate(context);
        context.push_integer(avre->first_from_end);
        last->generate(context);
        context.push_integer(avre->last_from_end);
        context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "array__slice", "(Lneon/type/Array;Lneon/type/Number;ZLneon/type/Number;Z)Lneon/type/Array;");
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("ArrayValueRangeExpression"); }
    virtual void generate_store(Context&) const override { internal_error("ArrayValueRangeExpression"); }
};

class PointerDereferenceExpression: public Expression {
public:
    explicit PointerDereferenceExpression(const ast::PointerDereferenceExpression *pde): Expression(pde), pde(pde), ptr(transform(pde->ptr)) {}
    PointerDereferenceExpression(const PointerDereferenceExpression &) = delete;
    PointerDereferenceExpression &operator=(const PointerDereferenceExpression &) = delete;
    const ast::PointerDereferenceExpression *pde;
    const Expression *ptr;

    virtual void generate(Context &context) const override {
        ptr->generate(context);
    }
    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("PointerDereferenceExpression"); }
    virtual void generate_store(Context&) const override { internal_error("PointerDereferenceExpression"); }
};

class VariableExpression: public Expression {
public:
    explicit VariableExpression(const ast::VariableExpression *ve): Expression(ve), ve(ve), var(transform(ve->var)) {}
    VariableExpression(const VariableExpression &) = delete;
    VariableExpression &operator=(const VariableExpression &) = delete;
    const ast::VariableExpression *ve;
    const Variable *var;

    virtual void generate(Context &context) const override {
        var->generate_load(context);
    }

    virtual void generate_call(Context &context, const std::vector<const Expression *> &args) const override {
        var->generate_call(context, args);
    }
    virtual void generate_store(Context &context) const override {
        var->generate_store(context);
    }
};

class InterfaceMethodExpression: public Expression {
public:
    explicit InterfaceMethodExpression(const ast::InterfaceMethodExpression *ime): Expression(ime), ime(ime) {}
    InterfaceMethodExpression(const InterfaceMethodExpression &) = delete;
    InterfaceMethodExpression &operator=(const InterfaceMethodExpression &) = delete;
    const ast::InterfaceMethodExpression *ime;

    virtual void generate(Context &) const override {
        // TODO
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("InterfaceMethodExpression"); }
    virtual void generate_store(Context &) const override { internal_error("InterfaceMethodExpression"); }
};

class FunctionCall: public Expression {
public:
    explicit FunctionCall(const ast::FunctionCall *fc): Expression(fc), fc(fc), func(transform(fc->func)), args() {
        for (auto a: fc->args) {
            args.push_back(transform(a));
        }
    }
    FunctionCall(const FunctionCall &) = delete;
    FunctionCall &operator=(const FunctionCall &) = delete;
    const ast::FunctionCall *fc;
    const Expression *func;
    std::vector<const Expression *> args;

    virtual void generate(Context &context) const override {
        func->generate_call(context, args);
    }

    virtual void generate_call(Context &, const std::vector<const Expression *> &) const override { internal_error("FunctionCall"); }
    virtual void generate_store(Context &) const override { internal_error("FunctionCall"); }
};

class NullStatement: public Statement {
public:
    explicit NullStatement(const ast::NullStatement *ns): Statement(ns), ns(ns) {}
    NullStatement(const NullStatement &) = delete;
    NullStatement &operator=(const NullStatement &) = delete;
    const ast::NullStatement *ns;

    virtual void generate(Context &) const override {}
};

class TypeDeclarationStatement: public Statement {
public:
    explicit TypeDeclarationStatement(const ast::TypeDeclarationStatement *tds): Statement(tds), tds(tds) {}
    TypeDeclarationStatement(const TypeDeclarationStatement &) = delete;
    TypeDeclarationStatement &operator=(const TypeDeclarationStatement &) = delete;
    const ast::TypeDeclarationStatement *tds;

    virtual void generate(Context &) const override {}
};

class DeclarationStatement: public Statement {
public:
    explicit DeclarationStatement(const ast::DeclarationStatement *ds): Statement(ds), ds(ds), decl(transform(ds->decl)) {}
    DeclarationStatement(const DeclarationStatement &) = delete;
    DeclarationStatement &operator=(const DeclarationStatement &) = delete;
    const ast::DeclarationStatement *ds;
    const Variable *decl;

    virtual void generate(Context &context) const override {
        decl->generate_decl(context.cc, false);
    }
};

class AssertStatement: public Statement {
public:
    explicit AssertStatement(const ast::AssertStatement *as): Statement(as), as(as), statements(), expr(transform(as->expr)) {
        for (auto s: as->statements) {
            statements.push_back(transform(s));
        }
    }
    AssertStatement(const AssertStatement &) = delete;
    AssertStatement &operator=(const AssertStatement &) = delete;
    const ast::AssertStatement *as;
    std::vector<const Statement *> statements;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        // TODO: check Class.desiredAssertionStatus
        expr->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/Boolean", "booleanValue", "()Z");
        auto label_skip = context.create_label();
        context.emit_jump(OP_ifne, label_skip);
        for (auto s: statements) {
            s->generate(context);
        }
        context.ca.code << OP_new << context.cf.Class("neon/type/NeonException");
        context.ca.code << OP_dup;
        context.ca.code << OP_ldc_w << context.cf.String(as->source);
        context.ca.code << OP_invokespecial << context.cf.Method("neon/type/NeonException", "<init>", "(Ljava/lang/String;)V");
        context.ca.code << OP_athrow;
        context.jump_target(label_skip);
    }
};

class AssignmentStatement: public Statement {
public:
    explicit AssignmentStatement(const ast::AssignmentStatement *as): Statement(as), as(as), variables(), expr(transform(as->expr)) {
        for (auto v: as->variables) {
            variables.push_back(transform(v));
        }
    }
    AssignmentStatement(const AssignmentStatement &) = delete;
    AssignmentStatement &operator=(const AssignmentStatement &) = delete;
    const ast::AssignmentStatement *as;
    std::vector<const Expression *> variables;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        expr->generate(context);
        for (size_t i = 0; i < variables.size() - 1; i++) {
            context.ca.code << OP_dup;
        }
        for (auto v: variables) {
             v->generate_store(context);
        }
    }
};

class ExpressionStatement: public Statement {
public:
    explicit ExpressionStatement(const ast::ExpressionStatement *es): Statement(es), es(es), expr(transform(es->expr)) {}
    ExpressionStatement(const ExpressionStatement &) = delete;
    ExpressionStatement &operator=(const ExpressionStatement &) = delete;
    const ast::ExpressionStatement *es;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        expr->generate(context);
    }
};

class CompoundStatement: public Statement {
public:
    explicit CompoundStatement(const ast::CompoundStatement *cs): Statement(cs), cs(cs), statements() {
        for (auto s: cs->statements) {
            statements.push_back(transform(s));
        }
    }
    CompoundStatement(const CompoundStatement &) = delete;
    CompoundStatement &operator=(const CompoundStatement &) = delete;
    const ast::CompoundStatement *cs;
    std::vector<const Statement *> statements;

    virtual void generate(Context &context) const override {
        for (auto s: statements) {
            s->generate(context);
        }
    }
};

class BaseLoopStatement: public CompoundStatement {
public:
    explicit BaseLoopStatement(const ast::BaseLoopStatement *bls): CompoundStatement(bls), bls(bls), prologue(), tail() {
        for (auto s: bls->prologue) {
            prologue.push_back(transform(s));
        }
        for (auto s: bls->tail) {
            tail.push_back(transform(s));
        }
    }
    BaseLoopStatement(const BaseLoopStatement &) = delete;
    BaseLoopStatement &operator=(const BaseLoopStatement &) = delete;
    const ast::BaseLoopStatement *bls;
    std::vector<const Statement *> prologue;
    std::vector<const Statement *> tail;

    virtual void generate(Context &context) const override {
        for (auto s: prologue) {
            s->generate(context);
        }
        auto top = context.create_label();
        context.jump_target(top);
        auto skip = context.create_label();
        auto next = context.create_label();
        context.add_loop_labels(bls->loop_id, skip, next);
        CompoundStatement::generate(context);
        context.jump_target(next);
        for (auto s: tail) {
            s->generate(context);
        }
        context.emit_jump(OP_goto, top);
        context.jump_target(skip);
        context.remove_loop_labels(bls->loop_id);
    }
};

class CaseStatement: public Statement {
public:
    class WhenCondition {
    public:
        WhenCondition() {}
        WhenCondition(const WhenCondition &) = delete;
        WhenCondition &operator=(const WhenCondition &) = delete;
        virtual ~WhenCondition() {}
        virtual void generate(Context &context, Context::Label &label_true, Context::Label &label_false) const = 0;
    };
    class ComparisonWhenCondition: public WhenCondition {
    public:
        ComparisonWhenCondition(ast::ComparisonExpression::Comparison comp, const Expression *expr): comp(comp), expr(expr) {}
        ComparisonWhenCondition(const ComparisonWhenCondition &) = delete;
        ComparisonWhenCondition &operator=(const ComparisonWhenCondition &) = delete;
        ast::ComparisonExpression::Comparison comp;
        const Expression *expr;
        virtual void generate(Context &context, Context::Label &label_true, Context::Label &label_false) const override {
            context.ca.code << OP_dup;
            expr->generate(context);
            context.ca.code << OP_invokeinterface << context.cf.InterfaceMethod("java/lang/Comparable", "compareTo", "(Ljava/lang/Object;)I") << static_cast<uint8_t>(2) << static_cast<uint8_t>(0);
            switch (comp) {
                case ast::ComparisonExpression::Comparison::EQ: context.emit_jump(OP_ifeq, label_true); break;
                case ast::ComparisonExpression::Comparison::NE: context.emit_jump(OP_ifne, label_true); break;
                case ast::ComparisonExpression::Comparison::LT: context.emit_jump(OP_iflt, label_true); break;
                case ast::ComparisonExpression::Comparison::GT: context.emit_jump(OP_ifgt, label_true); break;
                case ast::ComparisonExpression::Comparison::LE: context.emit_jump(OP_ifle, label_true); break;
                case ast::ComparisonExpression::Comparison::GE: context.emit_jump(OP_ifge, label_true); break;
            }
            context.emit_jump(OP_goto, label_false);
        }
    };
    class RangeWhenCondition: public WhenCondition {
    public:
        RangeWhenCondition(const Expression *low_expr, const Expression *high_expr): low_expr(low_expr), high_expr(high_expr) {}
        RangeWhenCondition(const RangeWhenCondition &) = delete;
        RangeWhenCondition &operator=(const RangeWhenCondition &) = delete;
        const Expression *low_expr;
        const Expression *high_expr;
        virtual void generate(Context &context, Context::Label &label_true, Context::Label &label_false) const override {
            context.ca.code << OP_dup;
            low_expr->generate(context);
            context.ca.code << OP_invokeinterface << context.cf.InterfaceMethod("java/lang/Comparable", "compareTo", "(Ljava/lang/Object;)I") << static_cast<uint8_t>(2) << static_cast<uint8_t>(0);
            context.emit_jump(OP_iflt, label_false);
            context.ca.code << OP_dup;
            high_expr->generate(context);
            context.ca.code << OP_invokeinterface << context.cf.InterfaceMethod("java/lang/Comparable", "compareTo", "(Ljava/lang/Object;)I") << static_cast<uint8_t>(2) << static_cast<uint8_t>(0);
            context.emit_jump(OP_ifle, label_true);
            context.emit_jump(OP_goto, label_false);
        }
    };
    explicit CaseStatement(const ast::CaseStatement *cs): Statement(cs), cs(cs), expr(transform(cs->expr)), clauses() {
        for (auto &c: cs->clauses) {
            std::vector<const WhenCondition *> whens;
            for (auto w: c.first) {
                auto *cwc = dynamic_cast<const ast::CaseStatement::ComparisonWhenCondition *>(w);
                auto *rwc = dynamic_cast<const ast::CaseStatement::RangeWhenCondition *>(w);
                if (cwc != nullptr) {
                    whens.push_back(new ComparisonWhenCondition(cwc->comp, transform(cwc->expr)));
                } else if (rwc != nullptr) {
                    whens.push_back(new RangeWhenCondition(transform(rwc->low_expr), transform(rwc->high_expr)));
                } else {
                    internal_error("CaseStatement");
                }
            }
            std::vector<const Statement *> statements;
            for (auto s: c.second) {
                statements.push_back(transform(s));
            }
            clauses.push_back(std::make_pair(whens, statements));
        }
    }
    CaseStatement(const CaseStatement &) = delete;
    CaseStatement &operator=(const CaseStatement &) = delete;
    const ast::CaseStatement *cs;
    const Expression *expr;
    std::vector<std::pair<std::vector<const WhenCondition *>, std::vector<const Statement *>>> clauses;

    virtual void generate(Context &context) const override {
        expr->generate(context);
        std::vector<std::pair<Context::Label, Context::Label>> clause_labels;
        for (auto clause: clauses) {
            clause_labels.push_back(std::make_pair(context.create_label(), context.create_label()));
        }
        clause_labels.push_back(std::make_pair(context.create_label(), context.create_label()));
        for (size_t i = 0; i < clauses.size(); i++) {
            auto &clause = clauses[i];
            auto &conditions = clause.first;
            auto &statements = clause.second;
            context.jump_target(clause_labels[i].first);
            for (size_t j = 0; j < conditions.size(); j++) {
                auto label_next = context.create_label();
                conditions[j]->generate(context, clause_labels[i].second, j+1 < conditions.size() ? label_next : clause_labels[i+1].first);
                context.jump_target(label_next);
            }
            context.jump_target(clause_labels[i].second);
            context.ca.code << OP_pop;
            for (auto stmt: statements) {
                stmt->generate(context);
            }
            context.emit_jump(OP_goto, clause_labels.back().second);
        }
        context.jump_target(clause_labels.back().first);
        context.ca.code << OP_pop;
        context.jump_target(clause_labels.back().second);
    }
};

class ExitStatement: public Statement {
public:
    explicit ExitStatement(const ast::ExitStatement *es): Statement(es), es(es) {}
    ExitStatement(const ExitStatement &) = delete;
    ExitStatement &operator=(const ExitStatement &) = delete;
    const ast::ExitStatement *es;

    void generate(Context &context) const override {
        context.emit_jump(OP_goto, context.get_exit_label(es->loop_id));
    }
};

class NextStatement: public Statement {
public:
    explicit NextStatement(const ast::NextStatement *ns): Statement(ns), ns(ns) {}
    NextStatement(const NextStatement &) = delete;
    NextStatement &operator=(const NextStatement &) = delete;
    const ast::NextStatement *ns;

    virtual void generate(Context &context) const override {
        context.emit_jump(OP_goto, context.get_next_label(ns->loop_id));
    }
};

class TryStatementTrap {
public:
    explicit TryStatementTrap(const ast::TryTrap *tt): tt(tt), name(transform(tt->name)), handler() {
        for (auto s: dynamic_cast<const ast::ExceptionHandlerStatement *>(tt->handler)->statements) {
            handler.push_back(transform(s));
        }
    }
    TryStatementTrap(const TryStatementTrap &) = delete;
    TryStatementTrap &operator=(const TryStatementTrap &) = delete;
    const ast::TryTrap *tt;
    const Variable *name;
    std::vector<const Statement *> handler;
};

class TryStatement: public Statement {
public:
    explicit TryStatement(const ast::TryStatement *ts): Statement(ts), ts(ts), statements(), catches() {
        for (auto s: ts->statements) {
            statements.push_back(transform(s));
        }
        for (auto &t: ts->catches) {
            catches.push_back(new TryStatementTrap(&t));
        }
    }
    TryStatement(const TryStatement &) = delete;
    TryStatement &operator=(const TryStatement &) = delete;
    const ast::TryStatement *ts;
    std::vector<const Statement *> statements;
    std::vector<const TryStatementTrap *> catches;

    virtual void generate(Context &context) const override {
        Code_attribute::exception e;
        e.start_pc = static_cast<uint16_t>(context.ca.code.size());
        for (auto s: statements) {
            s->generate(context);
        }
        e.end_pc = static_cast<uint16_t>(context.ca.code.size());
        auto label_end = context.create_label();
        context.emit_jump(OP_goto, label_end);
        for (auto trap: catches) {
            e.handler_pc = static_cast<uint16_t>(context.ca.code.size());
            context.ca.code << OP_pop; // TODO: store in local if indicated
            for (auto s: trap->handler) {
                s->generate(context);
            }
            context.emit_jump(OP_goto, label_end);
            e.catch_type = context.cf.Class("neon/type/NeonException");
            context.ca.exception_table.push_back(e);
        }
        context.jump_target(label_end);
    }
};

class ReturnStatement: public Statement {
public:
    explicit ReturnStatement(const ast::ReturnStatement *rs): Statement(rs), rs(rs), expr(transform(rs->expr)) {}
    ReturnStatement(const ReturnStatement &) = delete;
    ReturnStatement &operator=(const ReturnStatement &) = delete;
    const ast::ReturnStatement *rs;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        if (expr != nullptr) {
            expr->generate(context);
        }
        context.emit_jump(OP_goto, context.label_exit);
    }
};

class IncrementStatement: public Statement {
public:
    explicit IncrementStatement(const ast::IncrementStatement *is): Statement(is), is(is), ref(transform(is->ref)) {}
    IncrementStatement(const IncrementStatement &) = delete;
    IncrementStatement &operator=(const IncrementStatement &) = delete;
    const ast::IncrementStatement *is;
    const Expression *ref;

    virtual void generate(Context &context) const override {
        ref->generate(context);
        (new ConstantNumberExpression(new ast::ConstantNumberExpression(number_from_sint32(is->delta))))->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "add", "(Lneon/type/Number;)Lneon/type/Number;");
        ref->generate_store(context);
    }
};

class IfStatement: public Statement {
public:
    explicit IfStatement(const ast::IfStatement *is): Statement(is), is(is), condition_statements(), else_statements() {
        for (auto cs: is->condition_statements) {
            std::vector<const Statement *> statements;
            for (auto s: cs.second) {
                statements.push_back(transform(s));
            }
            condition_statements.push_back(std::make_pair(transform(cs.first), statements));
        }
        for (auto s: is->else_statements) {
            else_statements.push_back(transform(s));
        }
    }
    IfStatement(const IfStatement &) = delete;
    IfStatement &operator=(const IfStatement &) = delete;
    const ast::IfStatement *is;
    std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    std::vector<const Statement *> else_statements;

    void generate(Context &context) const override
    {
        auto end_label = context.create_label();
        for (auto cs: condition_statements) {
            const Expression *condition = cs.first;
            const std::vector<const Statement *> &statements = cs.second;
            condition->generate(context);
            auto else_label = context.create_label();
            context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/Boolean", "booleanValue", "()Z");
            context.emit_jump(OP_ifeq, else_label);
            for (auto s: statements) {
                s->generate(context);
            }
            context.emit_jump(OP_goto, end_label);
            context.jump_target(else_label);
        }
        for (auto s: else_statements) {
            s->generate(context);
        }
        context.jump_target(end_label);
    }
};

class RaiseStatement: public Statement {
public:
    explicit RaiseStatement(const ast::RaiseStatement *rs): Statement(rs), rs(rs) {}
    RaiseStatement(const RaiseStatement &) = delete;
    RaiseStatement &operator=(const RaiseStatement &) = delete;
    const ast::RaiseStatement *rs;

    virtual void generate(Context &context) const override {
        context.ca.code << OP_new << context.cf.Class("neon/type/NeonException");
        context.ca.code << OP_dup;
        context.ca.code << OP_ldc_w << context.cf.String(rs->exception->name);
        context.ca.code << OP_invokespecial << context.cf.Method("neon/type/NeonException", "<init>", "(Ljava/lang/String;)V");
        context.ca.code << OP_athrow;
    }
};

class ResetStatement: public Statement {
public:
    explicit ResetStatement(const ast::ResetStatement *rs): Statement(rs), rs(rs) {}
    ResetStatement(const ResetStatement &) = delete;
    ResetStatement &operator=(const ResetStatement &) = delete;
    const ast::ResetStatement *rs;

    virtual void generate(Context &) const override {
    }
};

class Function: public Variable {
public:
    explicit Function(const ast::Function *f): Variable(f), f(f), statements(), params(), signature(), out_count(0) {
        // Need to transform the function parameters before transforming
        // the code that might use them (statements).
        signature = "(";
        int i = 0;
        for (auto p: f->params) {
            FunctionParameter *q = new FunctionParameter(p, i);
            params.push_back(q);
            g_variable_cache[p] = q;
            signature.append(q->type->jtype);
            if (q->fp->mode == ast::ParameterType::Mode::INOUT || q->fp->mode == ast::ParameterType::Mode::OUT) {
                out_count++;
            }
            i++;
        }
        signature.append(")");
        if (out_count > 0) {
            signature.append("[Ljava/lang/Object;");
        } else {
            signature.append(dynamic_cast<const TypeFunction *>(type)->returntype->jtype);
        }
        for (auto s: f->statements) {
            statements.push_back(transform(s));
        }
    }
    Function(const Function &) = delete;
    Function &operator=(const Function &) = delete;
    const ast::Function *f;
    std::vector<const Statement *> statements;
    std::vector<FunctionParameter *> params;
    std::string signature;
    int out_count;

    virtual void generate_decl(ClassContext &context, bool) const override {
        method_info m;
        m.access_flags = ACC_PUBLIC | ACC_STATIC;
        m.name_index = context.cf.utf8(make_java_method_name(f->name));
        m.descriptor_index = context.cf.utf8(signature);
        {
            attribute_info a;
            a.attribute_name_index = context.cf.utf8("Code");
            {
                Code_attribute ca;
                ca.max_stack = 16; // TODO
                ca.max_locals = static_cast<uint16_t>(params.size());
                Context function_context(context, ca);
                for (auto s: statements) {
                    s->generate(function_context);
                }
                if (out_count > 0) {
                    function_context.jump_target(function_context.label_exit);
                    function_context.push_integer(1 + out_count);
                    ca.code << OP_anewarray << function_context.cf.Class("java/lang/Object");
                    if (dynamic_cast<const ast::TypeFunction *>(f->type)->returntype != ast::TYPE_NOTHING) {
                        ca.code << OP_dup_x1;
                        ca.code << OP_swap;
                        function_context.push_integer(0);
                        ca.code << OP_swap;
                        ca.code << OP_aastore;
                    } else {
                        int i = 1;
                        for (auto p: params) {
                            if (p->fp->mode == ast::ParameterType::Mode::INOUT || p->fp->mode == ast::ParameterType::Mode::OUT) {
                                ca.code << OP_dup;
                                function_context.push_integer(i);
                                p->generate_load(function_context);
                                ca.code << OP_aastore;
                                i++;
                            }
                        }
                    }
                    ca.code << OP_areturn;
                } else {
                    if (dynamic_cast<const ast::TypeFunction *>(f->type)->returntype != ast::TYPE_NOTHING) {
                        // This OP_aconst_null is only to support cases where the
                        // compiler "allows" compiling a function that doesn't have
                        // any RETURN statement (like an infinite loop).
                        ca.code << OP_aconst_null;
                        function_context.jump_target(function_context.label_exit);
                        ca.code << OP_areturn;
                    } else {
                        function_context.jump_target(function_context.label_exit);
                        ca.code << OP_return;
                    }
                }
                a.info = ca.serialize();
            }
            m.attributes.push_back(a);
        }
        context.cf.methods.push_back(m);
    }
    virtual void generate_load(Context &context) const override {
        // Get a handle to a function for function pointer support.
        context.ca.code << OP_ldc_w << context.cf.String(replace_slashes_with_dots(context.cf.name));
        context.ca.code << OP_invokestatic << context.cf.Method("java/lang/Class", "forName", "(Ljava/lang/String;)Ljava/lang/Class;");
        context.ca.code << OP_ldc_w << context.cf.String(f->name);
        context.push_integer(static_cast<int>(params.size()));
        context.ca.code << OP_anewarray << context.cf.Class("java/lang/Class");
        int i = 0;
        for (auto p: params) {
            context.ca.code << OP_dup;
            context.push_integer(i);
            context.ca.code << OP_ldc_w << context.cf.String(replace_slashes_with_dots(p->type->classname));
            context.ca.code << OP_invokestatic << context.cf.Method("java/lang/Class", "forName", "(Ljava/lang/String;)Ljava/lang/Class;");
            context.ca.code << OP_aastore;
            i++;
        }
        context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/Class", "getMethod", "(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;");
    }
    virtual void generate_store(Context &) const override { internal_error("Function"); }
    virtual void generate_call(Context &context, const std::vector<const Expression *> &args) const override {
        for (auto a: args) {
            a->generate(context);
        }
        context.ca.code << OP_invokestatic << context.cf.Method(context.cf.name, make_java_method_name(f->name), signature);
        if (out_count > 0) {
            int i = 1;
            for (auto p: params) {
                if (p->fp->mode == ast::ParameterType::Mode::INOUT || p->fp->mode == ast::ParameterType::Mode::OUT) {
                    if (dynamic_cast<const DummyExpression *>(args[i-1]) == nullptr) {
                        context.ca.code << OP_dup;
                        context.push_integer(i);
                        context.ca.code << OP_aaload;
                        context.ca.code << OP_checkcast << context.cf.Class(args[i-1]->type->classname);
                        args[i-1]->generate_store(context);
                    }
                    i++;
                }
            }
            if (dynamic_cast<const ast::TypeFunction *>(f->type)->returntype != ast::TYPE_NOTHING) {
                context.push_integer(0);
                context.ca.code << OP_aaload;
                context.ca.code << OP_checkcast << context.cf.Class(dynamic_cast<const TypeFunction *>(type)->returntype->classname);
            } else {
                context.ca.code << OP_pop;
            }
        }
    }
    static std::string replace_slashes_with_dots(std::string s) {
        for (;;) {
            auto i = s.find('/');
            if (i == std::string::npos) {
                break;
            }
            s[i] = '.';
        }
        return s;
    }
    static std::string make_java_method_name(std::string s)
    {
        for (;;) {
            std::string::size_type i = s.find_first_of('.');
            if (i == std::string::npos) {
                break;
            }
            s = s.substr(0, i) + "__" + s.substr(i+1);
        }
        return s;
    }
};

class PredefinedFunction: public Variable {
public:
    explicit PredefinedFunction(const ast::PredefinedFunction *pf): Variable(pf), pf(pf), out_count(0) {
        const ast::TypeFunction *tf = dynamic_cast<const ast::TypeFunction *>(pf->type);
        for (auto p: tf->params) {
            if (p->mode == ast::ParameterType::Mode::INOUT || p->mode == ast::ParameterType::Mode::OUT) {
                out_count++;
            }
        }
    }
    PredefinedFunction(const PredefinedFunction &) = delete;
    PredefinedFunction &operator=(const PredefinedFunction &) = delete;
    const ast::PredefinedFunction *pf;
    int out_count;

    virtual void generate_decl(ClassContext &, bool) const override {}
    virtual void generate_load(Context &) const override { internal_error("PredefinedFunction"); }
    virtual void generate_store(Context &) const override { internal_error("PredefinedFunction"); }
    virtual void generate_call(Context &context, const std::vector<const Expression *> &args) const override {
        for (auto a: args) {
            a->generate(context);
        }
        for (size_t i = 0; i < sizeof(FunctionSignatures)/sizeof(FunctionSignatures[0]); i++) {
            if (pf->name == FunctionSignatures[i].name) {
                context.ca.code << OP_invokestatic << context.cf.Method(FunctionSignatures[i].module, FunctionSignatures[i].function, FunctionSignatures[i].signature);
                if (out_count > 0) {
                    const ast::TypeFunction *tf = dynamic_cast<const ast::TypeFunction *>(pf->type);
                    int a = 1;
                    for (auto p: tf->params) {
                        if (p->mode == ast::ParameterType::Mode::INOUT || p->mode == ast::ParameterType::Mode::OUT) {
                            if (dynamic_cast<const DummyExpression *>(args[a-1]) == nullptr) {
                                context.ca.code << OP_dup;
                                context.push_integer(a);
                                context.ca.code << OP_aaload;
                                context.ca.code << OP_checkcast << context.cf.Class(args[a-1]->type->classname);
                                args[a-1]->generate_store(context);
                            }
                        }
                        a++;
                    }
                    if (tf->returntype != ast::TYPE_NOTHING) {
                        context.push_integer(0);
                        context.ca.code << OP_aaload;
                        context.ca.code << OP_checkcast << context.cf.Class(dynamic_cast<const TypeFunction *>(type)->returntype->classname);
                    } else {
                        context.ca.code << OP_pop;
                    }
                }
                return;
            }
        }
        internal_error("PredefinedFunction: " + pf->name);
    }
};

class ModuleFunction: public Variable {
public:
    explicit ModuleFunction(const ast::ModuleFunction *mf): Variable(mf), mf(mf), signature(), out_count(0) {
        const TypeFunction *functype = dynamic_cast<const TypeFunction *>(transform(mf->type));
        signature = "(";
        int i = 0;
        for (auto p: functype->paramtypes) {
            signature.append(p.second->jtype);
            if (p.first == ast::ParameterType::Mode::INOUT || p.first == ast::ParameterType::Mode::OUT) {
                out_count++;
            }
            i++;
        }
        signature.append(")");
        if (out_count > 0) {
            signature.append("[Ljava/lang/Object;");
        } else {
            signature.append(functype->returntype->jtype);
        }
    }
    ModuleFunction(const ModuleFunction &) = delete;
    ModuleFunction &operator=(const ModuleFunction &) = delete;
    const ast::ModuleFunction *mf;
    std::string signature;
    int out_count;

    virtual void generate_decl(ClassContext &, bool) const override {}
    virtual void generate_load(Context &) const override { internal_error("ModuleFunction"); }
    virtual void generate_store(Context &) const override { internal_error("ModuleFunction"); }
    virtual void generate_call(Context &context, const std::vector<const Expression *> &args) const override {
        for (auto a: args) {
            a->generate(context);
        }
        context.ca.code << OP_invokestatic << context.cf.Method(mf->module->name, Function::make_java_method_name(mf->name), signature);
        // TODO: out parameters
    }
};

class Program {
public:
    Program(CompilerSupport *support, const ast::Program *program): support(support), program(program), statements() {
        for (auto s: program->statements) {
            statements.push_back(transform(s));
        }
    }
    Program(const Program &) = delete;
    Program &operator=(const Program &) = delete;
    virtual ~Program() {}
    CompilerSupport *support;
    const ast::Program *program;
    std::vector<const Statement *> statements;

    virtual void generate() const {
        std::string path;
        std::string::size_type i = program->source_path.find_last_of("/\\:");
        if (i != std::string::npos) {
            path = program->source_path.substr(0, i + 1);
        }
        ClassFile cf(path, program->module_name);
        cf.magic = 0xCAFEBABE;
        cf.minor_version = 0;
        cf.major_version = 49;
        cf.constant_pool_count = 0;
        cf.access_flags = ACC_PUBLIC | ACC_SUPER;
        cf.this_class = cf.Class(cf.name);
        cf.super_class = cf.Class("java/lang/Object");

        ClassContext classcontext(support, cf);
        for (size_t s = 0; s < program->frame->getCount(); s++) {
            auto slot = program->frame->getSlot(s);
            const GlobalVariable *global = dynamic_cast<const GlobalVariable *>(g_variable_cache[dynamic_cast<const ast::GlobalVariable *>(slot.ref)]);
            if (global != nullptr) {
                bool exported = false;
                for (auto e: program->exports) {
                    if (e.second == global->gv) {
                        exported = true;
                        break;
                    }
                }
                global->generate_decl(classcontext, exported);
            }
        }
        {
            method_info m;
            m.access_flags = ACC_PUBLIC | ACC_STATIC;
            m.name_index = cf.utf8("main");
            m.descriptor_index = cf.utf8("([Ljava/lang/String;)V");
            {
                attribute_info code;
                code.attribute_name_index = cf.utf8("Code");
                {
                    Code_attribute ca;
                    ca.max_stack = 16;
                    ca.max_locals = 1;
                    Context context(classcontext, ca);
                    for (auto s: statements) {
                        s->generate(context);
                    }
                    ca.code << OP_return;
                    Code_attribute::exception e;
                    e.start_pc = 0;
                    e.end_pc = static_cast<uint16_t>(ca.code.size());
                    e.handler_pc = static_cast<uint16_t>(ca.code.size());
                    e.catch_type = cf.Class("neon/type/NeonException");
                    ca.exception_table.push_back(e);
                    ca.code << OP_invokevirtual << cf.Method("java/lang/Object", "toString", "()Ljava/lang/String;");
                    ca.code << OP_getstatic << cf.Field("java/lang/System", "err", "Ljava/io/PrintStream;");
                    ca.code << OP_dup;
                    ca.code << OP_ldc_w << cf.String("Unhandled exception ");
                    ca.code << OP_invokevirtual << cf.Method("java/io/PrintStream", "print", "(Ljava/lang/String;)V");
                    ca.code << OP_swap;
                    ca.code << OP_invokevirtual << cf.Method("java/io/PrintStream", "println", "(Ljava/lang/String;)V");
                    ca.code << OP_iconst_1;
                    ca.code << OP_invokestatic << context.cf.Method("java/lang/System", "exit", "(I)V");
                    ca.code << OP_return;
                    code.info = ca.serialize();
                    for (auto t: classcontext.generated_types) {
                        t->generate_class(context);
                    }
                }
                m.attributes.push_back(code);
            }
            cf.methods.push_back(m);
        }

        auto data = cf.serialize();
        support->writeOutput(path + cf.name + ".class", data);
    }
};

class TypeTransformer: public ast::IAstVisitor {
public:
    TypeTransformer(): r(nullptr) {}
    TypeTransformer(const TypeTransformer &) = delete;
    TypeTransformer &operator=(const TypeTransformer &) = delete;
    Type *retval() { if (r == nullptr) internal_error("TypeTransformer"); return r; }
    virtual void visit(const ast::TypeNothing *node) { r = new TypeNothing(node); }
    virtual void visit(const ast::TypeDummy *node) { r = new TypeDummy(node); }
    virtual void visit(const ast::TypeBoolean *node) { r = new TypeBoolean(node); }
    virtual void visit(const ast::TypeNumber *node) { r = new TypeNumber(node); }
    virtual void visit(const ast::TypeString *node) { r = new TypeString(node); }
    virtual void visit(const ast::TypeBytes *node) { r = new TypeBytes(node); }
    virtual void visit(const ast::TypeObject *node) { r = new TypeObject(node); }
    virtual void visit(const ast::TypeFunction *node) { r = new TypeFunction(node); }
    virtual void visit(const ast::TypeArray *node) { r = new TypeArray(node); }
    virtual void visit(const ast::TypeDictionary *node) { r = new TypeDictionary(node); }
    virtual void visit(const ast::TypeRecord *node) { r = new TypeRecord(node); }
    virtual void visit(const ast::TypeClass *node) { r = new TypeRecord(node); }
    virtual void visit(const ast::TypePointer *node) { r = new TypePointer(node); }
    virtual void visit(const ast::TypeInterfacePointer *node) { r = new TypeInterfacePointer(node); }
    virtual void visit(const ast::TypeFunctionPointer *node) { r = new TypeFunctionPointer(node); }
    virtual void visit(const ast::TypeEnum *node) { r = new TypeEnum(node); }
    virtual void visit(const ast::TypeChoice *node) { r = new TypeChoice(node); }
    virtual void visit(const ast::TypeModule *) {}
    virtual void visit(const ast::TypeException *) {}
    virtual void visit(const ast::TypeInterface *) {}
    virtual void visit(const ast::LoopLabel *) {}
    virtual void visit(const ast::PredefinedVariable *) {}
    virtual void visit(const ast::ModuleVariable *) {}
    virtual void visit(const ast::GlobalVariable *) {}
    virtual void visit(const ast::ExternalGlobalVariable *) {}
    virtual void visit(const ast::LocalVariable *) {}
    virtual void visit(const ast::FunctionParameter *) {}
    virtual void visit(const ast::Exception *) {}
    virtual void visit(const ast::Interface *) {}
    virtual void visit(const ast::Constant *) {}
    virtual void visit(const ast::ConstantBooleanExpression *) {}
    virtual void visit(const ast::ConstantNumberExpression *) {}
    virtual void visit(const ast::ConstantStringExpression *) {}
    virtual void visit(const ast::ConstantBytesExpression *) {}
    virtual void visit(const ast::ConstantEnumExpression *) {}
    virtual void visit(const ast::ConstantChoiceExpression *) {}
    virtual void visit(const ast::ConstantNilExpression *) {}
    virtual void visit(const ast::ConstantNowhereExpression *) {}
    virtual void visit(const ast::ConstantNilObject *) {}
    virtual void visit(const ast::TypeConversionExpression *) {}
    virtual void visit(const ast::ArrayLiteralExpression *) {}
    virtual void visit(const ast::DictionaryLiteralExpression *) {}
    virtual void visit(const ast::RecordLiteralExpression *) {}
    virtual void visit(const ast::ClassLiteralExpression *) {}
    virtual void visit(const ast::NewClassExpression *) {}
    virtual void visit(const ast::UnaryMinusExpression *) {}
    virtual void visit(const ast::LogicalNotExpression *) {}
    virtual void visit(const ast::ConditionalExpression *) {}
    virtual void visit(const ast::TryExpression *) {}
    virtual void visit(const ast::DisjunctionExpression *) {}
    virtual void visit(const ast::ConjunctionExpression *) {}
    virtual void visit(const ast::TypeTestExpression *) {}
    virtual void visit(const ast::ChoiceTestExpression *) {}
    virtual void visit(const ast::ArrayInExpression *) {}
    virtual void visit(const ast::DictionaryInExpression *) {}
    virtual void visit(const ast::ChainedComparisonExpression *) {}
    virtual void visit(const ast::BooleanComparisonExpression *) {}
    virtual void visit(const ast::NumericComparisonExpression *) {}
    virtual void visit(const ast::EnumComparisonExpression *) {}
    virtual void visit(const ast::StringComparisonExpression *) {}
    virtual void visit(const ast::BytesComparisonExpression *) {}
    virtual void visit(const ast::ArrayComparisonExpression *) {}
    virtual void visit(const ast::DictionaryComparisonExpression *) {}
    virtual void visit(const ast::RecordComparisonExpression *) {}
    virtual void visit(const ast::PointerComparisonExpression *) {}
    virtual void visit(const ast::ValidPointerExpression *) {}
    virtual void visit(const ast::FunctionPointerComparisonExpression *) {}
    virtual void visit(const ast::AdditionExpression *) {}
    virtual void visit(const ast::SubtractionExpression *) {}
    virtual void visit(const ast::MultiplicationExpression *) {}
    virtual void visit(const ast::DivisionExpression *) {}
    virtual void visit(const ast::ModuloExpression *) {}
    virtual void visit(const ast::ExponentiationExpression *) {}
    virtual void visit(const ast::DummyExpression *) {}
    virtual void visit(const ast::ArrayReferenceIndexExpression *) {}
    virtual void visit(const ast::ArrayValueIndexExpression *) {}
    virtual void visit(const ast::DictionaryReferenceIndexExpression *) {}
    virtual void visit(const ast::DictionaryValueIndexExpression *) {}
    virtual void visit(const ast::StringReferenceIndexExpression *) {}
    virtual void visit(const ast::StringValueIndexExpression *) {}
    virtual void visit(const ast::StringReferenceRangeIndexExpression *) {}
    virtual void visit(const ast::StringValueRangeIndexExpression *) {}
    virtual void visit(const ast::BytesReferenceIndexExpression *) {}
    virtual void visit(const ast::BytesValueIndexExpression *) {}
    virtual void visit(const ast::BytesReferenceRangeIndexExpression *) {}
    virtual void visit(const ast::BytesValueRangeIndexExpression *) {}
    virtual void visit(const ast::ObjectSubscriptExpression *) {}
    virtual void visit(const ast::RecordReferenceFieldExpression *) {}
    virtual void visit(const ast::RecordValueFieldExpression *) {}
    virtual void visit(const ast::ArrayReferenceRangeExpression *) {}
    virtual void visit(const ast::ArrayValueRangeExpression *) {}
    virtual void visit(const ast::ChoiceReferenceExpression *) {}
    virtual void visit(const ast::PointerDereferenceExpression *) {}
    virtual void visit(const ast::ConstantExpression *) {}
    virtual void visit(const ast::VariableExpression *) {}
    virtual void visit(const ast::InterfaceMethodExpression *) {}
    virtual void visit(const ast::InterfacePointerConstructor *) {}
    virtual void visit(const ast::InterfacePointerDeconstructor *) {}
    virtual void visit(const ast::FunctionCall *) {}
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *) {}
    virtual void visit(const ast::TypeDeclarationStatement *) {}
    virtual void visit(const ast::DeclarationStatement *) {}
    virtual void visit(const ast::ExceptionHandlerStatement *) {}
    virtual void visit(const ast::AssertStatement *) {}
    virtual void visit(const ast::AssignmentStatement *) {}
    virtual void visit(const ast::ExpressionStatement *) {}
    virtual void visit(const ast::ReturnStatement *) {}
    virtual void visit(const ast::IncrementStatement *) {}
    virtual void visit(const ast::IfStatement *) {}
    virtual void visit(const ast::BaseLoopStatement *) {}
    virtual void visit(const ast::CaseStatement *) {}
    virtual void visit(const ast::ExitStatement *) {}
    virtual void visit(const ast::NextStatement *) {}
    virtual void visit(const ast::TryStatement *) {}
    virtual void visit(const ast::RaiseStatement *) {}
    virtual void visit(const ast::ResetStatement *) {}
    virtual void visit(const ast::Function *) {}
    virtual void visit(const ast::PredefinedFunction *) {}
    virtual void visit(const ast::ExtensionFunction *) {}
    virtual void visit(const ast::ModuleFunction *) {}
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Type *r;
};

class VariableTransformer: public ast::IAstVisitor {
public:
    VariableTransformer(): r(nullptr) {}
    VariableTransformer(const VariableTransformer &) = delete;
    VariableTransformer &operator=(const VariableTransformer &) = delete;
    Variable *retval() { if (r == nullptr) internal_error("VariableTransformer"); return r; }
    virtual void visit(const ast::TypeNothing *) {}
    virtual void visit(const ast::TypeDummy *) {}
    virtual void visit(const ast::TypeBoolean *) {}
    virtual void visit(const ast::TypeNumber *) {}
    virtual void visit(const ast::TypeString *) {}
    virtual void visit(const ast::TypeBytes *) {}
    virtual void visit(const ast::TypeObject *) {}
    virtual void visit(const ast::TypeFunction *) {}
    virtual void visit(const ast::TypeArray *) {}
    virtual void visit(const ast::TypeDictionary *) {}
    virtual void visit(const ast::TypeRecord *) {}
    virtual void visit(const ast::TypeClass *) {}
    virtual void visit(const ast::TypePointer *) {}
    virtual void visit(const ast::TypeInterfacePointer *) {}
    virtual void visit(const ast::TypeFunctionPointer *) {}
    virtual void visit(const ast::TypeEnum *) {}
    virtual void visit(const ast::TypeChoice *) {}
    virtual void visit(const ast::TypeModule *) {}
    virtual void visit(const ast::TypeException *) {}
    virtual void visit(const ast::TypeInterface *) {}
    virtual void visit(const ast::LoopLabel *) {}
    virtual void visit(const ast::PredefinedVariable *node) { r = new PredefinedVariable(node); }
    virtual void visit(const ast::ModuleVariable *node) { r = new ModuleVariable(node); }
    virtual void visit(const ast::GlobalVariable *node) { r = new GlobalVariable(node); }
    virtual void visit(const ast::ExternalGlobalVariable *) { internal_error("ExternalGlobalVariable"); }
    virtual void visit(const ast::LocalVariable *node) { r = new LocalVariable(node); }
    virtual void visit(const ast::FunctionParameter *) { /*r = new FunctionParameter(node);*/ }
    virtual void visit(const ast::Exception *) {}
    virtual void visit(const ast::Interface *) {}
    virtual void visit(const ast::Constant *) {}
    virtual void visit(const ast::ConstantBooleanExpression *) {}
    virtual void visit(const ast::ConstantNumberExpression *) {}
    virtual void visit(const ast::ConstantStringExpression *) {}
    virtual void visit(const ast::ConstantBytesExpression *) {}
    virtual void visit(const ast::ConstantEnumExpression *) {}
    virtual void visit(const ast::ConstantChoiceExpression *) {}
    virtual void visit(const ast::ConstantNilExpression *) {}
    virtual void visit(const ast::ConstantNowhereExpression *) {}
    virtual void visit(const ast::ConstantNilObject *) {}
    virtual void visit(const ast::TypeConversionExpression *) {}
    virtual void visit(const ast::ArrayLiteralExpression *) {}
    virtual void visit(const ast::DictionaryLiteralExpression *) {}
    virtual void visit(const ast::RecordLiteralExpression *) {}
    virtual void visit(const ast::ClassLiteralExpression *) {}
    virtual void visit(const ast::NewClassExpression *) {}
    virtual void visit(const ast::UnaryMinusExpression *) {}
    virtual void visit(const ast::LogicalNotExpression *) {}
    virtual void visit(const ast::ConditionalExpression *) {}
    virtual void visit(const ast::TryExpression *) {}
    virtual void visit(const ast::DisjunctionExpression *) {}
    virtual void visit(const ast::ConjunctionExpression *) {}
    virtual void visit(const ast::TypeTestExpression *) {}
    virtual void visit(const ast::ChoiceTestExpression *) {}
    virtual void visit(const ast::ArrayInExpression *) {}
    virtual void visit(const ast::DictionaryInExpression *) {}
    virtual void visit(const ast::ChainedComparisonExpression *) {}
    virtual void visit(const ast::BooleanComparisonExpression *) {}
    virtual void visit(const ast::NumericComparisonExpression *) {}
    virtual void visit(const ast::EnumComparisonExpression *) {}
    virtual void visit(const ast::StringComparisonExpression *) {}
    virtual void visit(const ast::BytesComparisonExpression *) {}
    virtual void visit(const ast::ArrayComparisonExpression *) {}
    virtual void visit(const ast::DictionaryComparisonExpression *) {}
    virtual void visit(const ast::RecordComparisonExpression *) {}
    virtual void visit(const ast::PointerComparisonExpression *) {}
    virtual void visit(const ast::ValidPointerExpression *) {}
    virtual void visit(const ast::FunctionPointerComparisonExpression *) {}
    virtual void visit(const ast::AdditionExpression *) {}
    virtual void visit(const ast::SubtractionExpression *) {}
    virtual void visit(const ast::MultiplicationExpression *) {}
    virtual void visit(const ast::DivisionExpression *) {}
    virtual void visit(const ast::ModuloExpression *) {}
    virtual void visit(const ast::ExponentiationExpression *) {}
    virtual void visit(const ast::DummyExpression *) {}
    virtual void visit(const ast::ArrayReferenceIndexExpression *) {}
    virtual void visit(const ast::ArrayValueIndexExpression *) {}
    virtual void visit(const ast::DictionaryReferenceIndexExpression *) {}
    virtual void visit(const ast::DictionaryValueIndexExpression *) {}
    virtual void visit(const ast::StringReferenceIndexExpression *) {}
    virtual void visit(const ast::StringValueIndexExpression *) {}
    virtual void visit(const ast::StringReferenceRangeIndexExpression *) {}
    virtual void visit(const ast::StringValueRangeIndexExpression *) {}
    virtual void visit(const ast::BytesReferenceIndexExpression *) {}
    virtual void visit(const ast::BytesValueIndexExpression *) {}
    virtual void visit(const ast::BytesReferenceRangeIndexExpression *) {}
    virtual void visit(const ast::BytesValueRangeIndexExpression *) {}
    virtual void visit(const ast::ObjectSubscriptExpression *) {}
    virtual void visit(const ast::RecordReferenceFieldExpression *) {}
    virtual void visit(const ast::RecordValueFieldExpression *) {}
    virtual void visit(const ast::ArrayReferenceRangeExpression *) {}
    virtual void visit(const ast::ArrayValueRangeExpression *) {}
    virtual void visit(const ast::ChoiceReferenceExpression *) {}
    virtual void visit(const ast::PointerDereferenceExpression *) {}
    virtual void visit(const ast::ConstantExpression *) {}
    virtual void visit(const ast::VariableExpression *) {}
    virtual void visit(const ast::InterfaceMethodExpression *) {}
    virtual void visit(const ast::InterfacePointerConstructor *) {}
    virtual void visit(const ast::InterfacePointerDeconstructor *) {}
    virtual void visit(const ast::FunctionCall *) {}
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *) {}
    virtual void visit(const ast::TypeDeclarationStatement *) {}
    virtual void visit(const ast::DeclarationStatement *) {}
    virtual void visit(const ast::ExceptionHandlerStatement *) {}
    virtual void visit(const ast::AssertStatement *) {}
    virtual void visit(const ast::AssignmentStatement *) {}
    virtual void visit(const ast::ExpressionStatement *) {}
    virtual void visit(const ast::ReturnStatement *) {}
    virtual void visit(const ast::IncrementStatement *) {}
    virtual void visit(const ast::IfStatement *) {}
    virtual void visit(const ast::BaseLoopStatement *) {}
    virtual void visit(const ast::CaseStatement *) {}
    virtual void visit(const ast::ExitStatement *) {}
    virtual void visit(const ast::NextStatement *) {}
    virtual void visit(const ast::TryStatement *) {}
    virtual void visit(const ast::RaiseStatement *) {}
    virtual void visit(const ast::ResetStatement *) {}
    virtual void visit(const ast::Function *node) { r = new Function(node); }
    virtual void visit(const ast::PredefinedFunction *node) { r = new PredefinedFunction(node); }
    virtual void visit(const ast::ExtensionFunction *) { internal_error("unimplemented"); /*r = new PredefinedFunction(node);*/ }
    virtual void visit(const ast::ModuleFunction *node) { r = new ModuleFunction(node); }
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Variable *r;
};

class ExpressionTransformer: public ast::IAstVisitor {
public:
    ExpressionTransformer(): r(nullptr) {}
    ExpressionTransformer(const ExpressionTransformer &) = delete;
    ExpressionTransformer &operator=(const ExpressionTransformer &) = delete;
    Expression *retval() { if (r == nullptr) internal_error("ExpressionTransformer"); return r; }
    virtual void visit(const ast::TypeNothing *) {}
    virtual void visit(const ast::TypeDummy *) {}
    virtual void visit(const ast::TypeBoolean *) {}
    virtual void visit(const ast::TypeNumber *) {}
    virtual void visit(const ast::TypeString *) {}
    virtual void visit(const ast::TypeBytes *) {}
    virtual void visit(const ast::TypeObject *) {}
    virtual void visit(const ast::TypeFunction *) {}
    virtual void visit(const ast::TypeArray *) {}
    virtual void visit(const ast::TypeDictionary *) {}
    virtual void visit(const ast::TypeRecord *) {}
    virtual void visit(const ast::TypeClass *) {}
    virtual void visit(const ast::TypePointer *) {}
    virtual void visit(const ast::TypeInterfacePointer *) {}
    virtual void visit(const ast::TypeFunctionPointer *) {}
    virtual void visit(const ast::TypeEnum *) {}
    virtual void visit(const ast::TypeChoice *) {}
    virtual void visit(const ast::TypeModule *) {}
    virtual void visit(const ast::TypeException *) {}
    virtual void visit(const ast::TypeInterface *) {}
    virtual void visit(const ast::LoopLabel *) {}
    virtual void visit(const ast::PredefinedVariable *) {}
    virtual void visit(const ast::ModuleVariable *) {}
    virtual void visit(const ast::GlobalVariable *) {}
    virtual void visit(const ast::ExternalGlobalVariable *) {}
    virtual void visit(const ast::LocalVariable *) {}
    virtual void visit(const ast::FunctionParameter *) {}
    virtual void visit(const ast::Exception *) {}
    virtual void visit(const ast::Interface *) {}
    virtual void visit(const ast::Constant *) {}
    virtual void visit(const ast::ConstantBooleanExpression *node) { r = new ConstantBooleanExpression(node); }
    virtual void visit(const ast::ConstantNumberExpression *node) { r = new ConstantNumberExpression(node); }
    virtual void visit(const ast::ConstantStringExpression *node) { r = new ConstantStringExpression(node); }
    virtual void visit(const ast::ConstantBytesExpression *node) { r = new ConstantBytesExpression(node); }
    virtual void visit(const ast::ConstantEnumExpression *node) { r = new ConstantEnumExpression(node); }
    virtual void visit(const ast::ConstantChoiceExpression *) {}
    virtual void visit(const ast::ConstantNilExpression *node) { r = new ConstantNilExpression(node); }
    virtual void visit(const ast::ConstantNowhereExpression *node) { r = new ConstantNowhereExpression(node); }
    virtual void visit(const ast::ConstantNilObject *) { internal_error("ConstantNilObject"); }
    virtual void visit(const ast::TypeConversionExpression *node) { r = new TypeConversionExpression(node); }
    virtual void visit(const ast::ArrayLiteralExpression *node) { r = new ArrayLiteralExpression(node); }
    virtual void visit(const ast::DictionaryLiteralExpression *node) { r = new DictionaryLiteralExpression(node); }
    virtual void visit(const ast::RecordLiteralExpression *node) { r = new RecordLiteralExpression(node); }
    virtual void visit(const ast::ClassLiteralExpression *node) { r = new ClassLiteralExpression(node); }
    virtual void visit(const ast::NewClassExpression *node) { r =  new NewClassExpression(node); }
    virtual void visit(const ast::UnaryMinusExpression *node) { r = new UnaryMinusExpression(node); }
    virtual void visit(const ast::LogicalNotExpression *node) { r = new LogicalNotExpression(node); }
    virtual void visit(const ast::ConditionalExpression *node) { r = new ConditionalExpression(node); }
    virtual void visit(const ast::TryExpression *node) { r = new TryExpression(node); }
    virtual void visit(const ast::DisjunctionExpression *node) { r = new DisjunctionExpression(node); }
    virtual void visit(const ast::ConjunctionExpression *node) { r = new ConjunctionExpression(node); }
    virtual void visit(const ast::TypeTestExpression *) { internal_error("TypeTestExpression"); }
    virtual void visit(const ast::ChoiceTestExpression *) { internal_error("ChoiceTestExpression"); }
    virtual void visit(const ast::ArrayInExpression *node) { r = new ArrayInExpression(node); }
    virtual void visit(const ast::DictionaryInExpression *node) { r =  new DictionaryInExpression(node); }
    virtual void visit(const ast::ChainedComparisonExpression *node) { r = new ChainedComparisonExpression(node); }
    virtual void visit(const ast::BooleanComparisonExpression *node) { r = new BooleanComparisonExpression(node); }
    virtual void visit(const ast::NumericComparisonExpression *node) { r = new NumericComparisonExpression(node); }
    virtual void visit(const ast::EnumComparisonExpression *node) { r = new EnumComparisonExpression(node); }
    virtual void visit(const ast::StringComparisonExpression *node) { r = new StringComparisonExpression(node); }
    virtual void visit(const ast::BytesComparisonExpression *node) { r = new BytesComparisonExpression(node); }
    virtual void visit(const ast::ArrayComparisonExpression *node) { r = new ArrayComparisonExpression(node); }
    virtual void visit(const ast::DictionaryComparisonExpression *node) { r = new DictionaryComparisonExpression(node); }
    virtual void visit(const ast::RecordComparisonExpression *node) { r = new RecordComparisonExpression(node); }
    virtual void visit(const ast::PointerComparisonExpression *node) { r = new PointerComparisonExpression(node); }
    virtual void visit(const ast::ValidPointerExpression *node) { r = new ValidPointerExpression(node); }
    virtual void visit(const ast::FunctionPointerComparisonExpression *node) { r = new FunctionPointerComparisonExpression(node); }
    virtual void visit(const ast::AdditionExpression *node) { r = new AdditionExpression(node); }
    virtual void visit(const ast::SubtractionExpression *node) { r = new SubtractionExpression(node); }
    virtual void visit(const ast::MultiplicationExpression *node) { r = new MultiplicationExpression(node); }
    virtual void visit(const ast::DivisionExpression *node) { r = new DivisionExpression(node); }
    virtual void visit(const ast::ModuloExpression *node) { r = new ModuloExpression(node); }
    virtual void visit(const ast::ExponentiationExpression *node) { r = new ExponentiationExpression(node); }
    virtual void visit(const ast::DummyExpression *node) { r = new DummyExpression(node); }
    virtual void visit(const ast::ArrayReferenceIndexExpression *node) { r = new ArrayReferenceIndexExpression(node); }
    virtual void visit(const ast::ArrayValueIndexExpression *node) { r = new ArrayValueIndexExpression(node); }
    virtual void visit(const ast::DictionaryReferenceIndexExpression *node) { r = new DictionaryReferenceIndexExpression(node); }
    virtual void visit(const ast::DictionaryValueIndexExpression *node) { r = new DictionaryValueIndexExpression(node); }
    virtual void visit(const ast::StringReferenceIndexExpression *node) { r = new StringReferenceIndexExpression(node); }
    virtual void visit(const ast::StringValueIndexExpression *node) { r = new StringValueIndexExpression(node); }
    virtual void visit(const ast::StringReferenceRangeIndexExpression *node) { r = new StringReferenceRangeIndexExpression(node); }
    virtual void visit(const ast::StringValueRangeIndexExpression *node) { r = new StringValueRangeIndexExpression(node); }
    virtual void visit(const ast::BytesReferenceIndexExpression *node) { r = new BytesReferenceIndexExpression(node); }
    virtual void visit(const ast::BytesValueIndexExpression *node) { r = new BytesValueIndexExpression(node); }
    virtual void visit(const ast::BytesReferenceRangeIndexExpression *node) { r = new BytesReferenceRangeIndexExpression(node); }
    virtual void visit(const ast::BytesValueRangeIndexExpression *node) { r = new BytesValueRangeIndexExpression(node); }
    virtual void visit(const ast::ObjectSubscriptExpression *) { internal_error("ObjectSubscriptExpression"); }
    virtual void visit(const ast::RecordReferenceFieldExpression *node) { r = new RecordReferenceFieldExpression(node); }
    virtual void visit(const ast::RecordValueFieldExpression *node) { r = new RecordValueFieldExpression(node); }
    virtual void visit(const ast::ArrayReferenceRangeExpression *node) { r = new ArrayReferenceRangeExpression(node); }
    virtual void visit(const ast::ArrayValueRangeExpression *node) { r = new ArrayValueRangeExpression(node); }
    virtual void visit(const ast::ChoiceReferenceExpression *) {}
    virtual void visit(const ast::PointerDereferenceExpression *node) { r =  new PointerDereferenceExpression(node); }
    virtual void visit(const ast::ConstantExpression *node) { r = transform(node->constant->value); }
    virtual void visit(const ast::VariableExpression *node) { r = new VariableExpression(node); }
    virtual void visit(const ast::InterfaceMethodExpression *node) { r = new InterfaceMethodExpression(node); }
    virtual void visit(const ast::InterfacePointerConstructor *) { internal_error("InterfacePointerConstructor"); }
    virtual void visit(const ast::InterfacePointerDeconstructor *) { internal_error("InterfacePointerDeconstructor"); }
    virtual void visit(const ast::FunctionCall *node) { r = new FunctionCall(node); }
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *) {}
    virtual void visit(const ast::TypeDeclarationStatement *) {}
    virtual void visit(const ast::DeclarationStatement *) {}
    virtual void visit(const ast::ExceptionHandlerStatement *) {}
    virtual void visit(const ast::AssertStatement *) {}
    virtual void visit(const ast::AssignmentStatement *) {}
    virtual void visit(const ast::ExpressionStatement *) {}
    virtual void visit(const ast::ReturnStatement *) {}
    virtual void visit(const ast::IncrementStatement *) {}
    virtual void visit(const ast::IfStatement *) {}
    virtual void visit(const ast::BaseLoopStatement *) {}
    virtual void visit(const ast::CaseStatement *) {}
    virtual void visit(const ast::ExitStatement *) {}
    virtual void visit(const ast::NextStatement *) {}
    virtual void visit(const ast::TryStatement *) {}
    virtual void visit(const ast::RaiseStatement *) {}
    virtual void visit(const ast::ResetStatement *) {}
    virtual void visit(const ast::Function *) {}
    virtual void visit(const ast::PredefinedFunction *) {}
    virtual void visit(const ast::ExtensionFunction *) {}
    virtual void visit(const ast::ModuleFunction *) {}
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Expression *r;
};

class StatementTransformer: public ast::IAstVisitor {
public:
    StatementTransformer(): r(nullptr) {}
    StatementTransformer(const StatementTransformer &) = delete;
    StatementTransformer &operator=(const StatementTransformer &) = delete;
    Statement *retval() { if (r == nullptr) internal_error("StatementTransformer"); return r; }
    virtual void visit(const ast::TypeNothing *) {}
    virtual void visit(const ast::TypeDummy *) {}
    virtual void visit(const ast::TypeBoolean *) {}
    virtual void visit(const ast::TypeNumber *) {}
    virtual void visit(const ast::TypeString *) {}
    virtual void visit(const ast::TypeBytes *) {}
    virtual void visit(const ast::TypeObject *) {}
    virtual void visit(const ast::TypeFunction *) {}
    virtual void visit(const ast::TypeArray *) {}
    virtual void visit(const ast::TypeDictionary *) {}
    virtual void visit(const ast::TypeRecord *) {}
    virtual void visit(const ast::TypeClass *) {}
    virtual void visit(const ast::TypePointer *) {}
    virtual void visit(const ast::TypeInterfacePointer *) {}
    virtual void visit(const ast::TypeFunctionPointer *) {}
    virtual void visit(const ast::TypeEnum *) {}
    virtual void visit(const ast::TypeChoice *) {}
    virtual void visit(const ast::TypeModule *) {}
    virtual void visit(const ast::TypeException *) {}
    virtual void visit(const ast::TypeInterface *) {}
    virtual void visit(const ast::LoopLabel *) {}
    virtual void visit(const ast::PredefinedVariable *) {}
    virtual void visit(const ast::ModuleVariable *) {}
    virtual void visit(const ast::GlobalVariable *) {}
    virtual void visit(const ast::ExternalGlobalVariable *) {}
    virtual void visit(const ast::LocalVariable *) {}
    virtual void visit(const ast::FunctionParameter *) {}
    virtual void visit(const ast::Exception *) {}
    virtual void visit(const ast::Interface *) {}
    virtual void visit(const ast::Constant *) {}
    virtual void visit(const ast::ConstantBooleanExpression *) {}
    virtual void visit(const ast::ConstantNumberExpression *) {}
    virtual void visit(const ast::ConstantStringExpression *) {}
    virtual void visit(const ast::ConstantBytesExpression *) {}
    virtual void visit(const ast::ConstantEnumExpression *) {}
    virtual void visit(const ast::ConstantChoiceExpression *) {}
    virtual void visit(const ast::ConstantNilExpression *) {}
    virtual void visit(const ast::ConstantNowhereExpression *) {}
    virtual void visit(const ast::ConstantNilObject *) {}
    virtual void visit(const ast::TypeConversionExpression *) {}
    virtual void visit(const ast::ArrayLiteralExpression *) {}
    virtual void visit(const ast::DictionaryLiteralExpression *) {}
    virtual void visit(const ast::RecordLiteralExpression *) {}
    virtual void visit(const ast::ClassLiteralExpression *) {}
    virtual void visit(const ast::NewClassExpression *) {}
    virtual void visit(const ast::UnaryMinusExpression *) {}
    virtual void visit(const ast::LogicalNotExpression *) {}
    virtual void visit(const ast::ConditionalExpression *) {}
    virtual void visit(const ast::TryExpression *) {}
    virtual void visit(const ast::DisjunctionExpression *) {}
    virtual void visit(const ast::ConjunctionExpression *) {}
    virtual void visit(const ast::TypeTestExpression *) {}
    virtual void visit(const ast::ChoiceTestExpression *) {}
    virtual void visit(const ast::ArrayInExpression *) {}
    virtual void visit(const ast::DictionaryInExpression *) {}
    virtual void visit(const ast::ChainedComparisonExpression *) {}
    virtual void visit(const ast::BooleanComparisonExpression *) {}
    virtual void visit(const ast::NumericComparisonExpression *) {}
    virtual void visit(const ast::EnumComparisonExpression *) {}
    virtual void visit(const ast::StringComparisonExpression *) {}
    virtual void visit(const ast::BytesComparisonExpression *) {}
    virtual void visit(const ast::ArrayComparisonExpression *) {}
    virtual void visit(const ast::DictionaryComparisonExpression *) {}
    virtual void visit(const ast::RecordComparisonExpression *) {}
    virtual void visit(const ast::PointerComparisonExpression *) {}
    virtual void visit(const ast::ValidPointerExpression *) {}
    virtual void visit(const ast::FunctionPointerComparisonExpression *) {}
    virtual void visit(const ast::AdditionExpression *) {}
    virtual void visit(const ast::SubtractionExpression *) {}
    virtual void visit(const ast::MultiplicationExpression *) {}
    virtual void visit(const ast::DivisionExpression *) {}
    virtual void visit(const ast::ModuloExpression *) {}
    virtual void visit(const ast::ExponentiationExpression *) {}
    virtual void visit(const ast::DummyExpression *) {}
    virtual void visit(const ast::ArrayReferenceIndexExpression *) {}
    virtual void visit(const ast::ArrayValueIndexExpression *) {}
    virtual void visit(const ast::DictionaryReferenceIndexExpression *) {}
    virtual void visit(const ast::DictionaryValueIndexExpression *) {}
    virtual void visit(const ast::StringReferenceIndexExpression *) {}
    virtual void visit(const ast::StringValueIndexExpression *) {}
    virtual void visit(const ast::StringReferenceRangeIndexExpression *) {}
    virtual void visit(const ast::StringValueRangeIndexExpression *) {}
    virtual void visit(const ast::BytesReferenceIndexExpression *) {}
    virtual void visit(const ast::BytesValueIndexExpression *) {}
    virtual void visit(const ast::BytesReferenceRangeIndexExpression *) {}
    virtual void visit(const ast::BytesValueRangeIndexExpression *) {}
    virtual void visit(const ast::ObjectSubscriptExpression *) {}
    virtual void visit(const ast::RecordReferenceFieldExpression *) {}
    virtual void visit(const ast::RecordValueFieldExpression *) {}
    virtual void visit(const ast::ArrayReferenceRangeExpression *) {}
    virtual void visit(const ast::ArrayValueRangeExpression *) {}
    virtual void visit(const ast::ChoiceReferenceExpression *) {}
    virtual void visit(const ast::PointerDereferenceExpression *) {}
    virtual void visit(const ast::ConstantExpression *) {}
    virtual void visit(const ast::VariableExpression *) {}
    virtual void visit(const ast::InterfaceMethodExpression *) {}
    virtual void visit(const ast::InterfacePointerConstructor *) {}
    virtual void visit(const ast::InterfacePointerDeconstructor *) {}
    virtual void visit(const ast::FunctionCall *) {}
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *node) { r = new NullStatement(node); }
    virtual void visit(const ast::TypeDeclarationStatement *node) { r = new TypeDeclarationStatement(node); }
    virtual void visit(const ast::DeclarationStatement *node) { r = new DeclarationStatement(node); }
    virtual void visit(const ast::ExceptionHandlerStatement *) { internal_error("ExceptionHandlerStatement" ); }
    virtual void visit(const ast::AssertStatement *node) { r = new AssertStatement(node); }
    virtual void visit(const ast::AssignmentStatement *node) { r = new AssignmentStatement(node); }
    virtual void visit(const ast::ExpressionStatement *node) { r = new ExpressionStatement(node); }
    virtual void visit(const ast::ReturnStatement *node) { r = new ReturnStatement(node); }
    virtual void visit(const ast::IncrementStatement *node) { r =  new IncrementStatement(node); }
    virtual void visit(const ast::IfStatement *node) { r = new IfStatement(node); }
    virtual void visit(const ast::BaseLoopStatement *node) { r = new BaseLoopStatement(node); }
    virtual void visit(const ast::CaseStatement *node) { r = new CaseStatement(node); }
    virtual void visit(const ast::ExitStatement *node) { r = new ExitStatement(node); }
    virtual void visit(const ast::NextStatement *node) { r = new NextStatement(node); }
    virtual void visit(const ast::TryStatement *node) { r = new TryStatement(node); }
    virtual void visit(const ast::RaiseStatement *node) { r = new RaiseStatement(node); }
    virtual void visit(const ast::ResetStatement *node) { r = new ResetStatement(node); }
    virtual void visit(const ast::Function *) {}
    virtual void visit(const ast::PredefinedFunction *) {}
    virtual void visit(const ast::ExtensionFunction *) {}
    virtual void visit(const ast::ModuleFunction *) {}
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Statement *r;
};

Type *transform(const ast::Type *t)
{
    if (t == nullptr) {
        return nullptr;
    }
    //fprintf(stderr, "transform type %s %s\n", typeid(*t).name(), t->text().c_str());
    auto i = g_type_cache.find(t);
    if (i != g_type_cache.end()) {
        return i->second;
    }
    TypeTransformer tt;
    t->accept(&tt);
    return tt.retval();
}

Variable *transform(const ast::Variable *v)
{
    if (v == nullptr) {
        return nullptr;
    }
    //fprintf(stderr, "transform variable %s %s\n", typeid(*v).name(), v->text().c_str());
    auto i = g_variable_cache.find(v);
    if (i != g_variable_cache.end()) {
        return i->second;
    }
    VariableTransformer vt;
    v->accept(&vt);
    return vt.retval();
}

Expression *transform(const ast::Expression *e)
{
    if (e == nullptr) {
        return nullptr;
    }
    //fprintf(stderr, "transform expression %s %s\n", typeid(*e).name(), e->text().c_str());
    auto i = g_expression_cache.find(e);
    if (i != g_expression_cache.end()) {
        return i->second;
    }
    ExpressionTransformer et;
    e->accept(&et);
    return et.retval();
}

Statement *transform(const ast::Statement *s)
{
    if (s == nullptr) {
        return nullptr;
    }
    //fprintf(stderr, "transform statement %s %s\n", typeid(*s).name(), s->text().c_str());
    auto i = g_statement_cache.find(s);
    if (i != g_statement_cache.end()) {
        return i->second;
    }
    StatementTransformer st;
    s->accept(&st);
    return st.retval();
}

} // namespace jvm

void compile_jvm(CompilerSupport *support, const ast::Program *p, std::string /*output*/, std::map<std::string, std::string> /*options*/)
{
    jvm::g_type_cache.clear();
    jvm::g_variable_cache.clear();
    jvm::g_expression_cache.clear();
    jvm::g_statement_cache.clear();

    jvm::Program *ct = new jvm::Program(support, p);
    ct->generate();
}
