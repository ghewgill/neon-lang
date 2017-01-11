#include "ast.h"

#include <assert.h>
#include <fstream>

namespace jvm {

const uint16_t ACC_PUBLIC = 0x0001;
const uint16_t ACC_STATIC = 0x0008;
const uint16_t ACC_SUPER  = 0x0020;

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
const uint8_t OP_aload_0        =  42;
const uint8_t OP_astore         =  58;
const uint8_t OP_astore_0       =  75;
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
const uint8_t OP_goto           = 167;
const uint8_t OP_areturn        = 176;
const uint8_t OP_return         = 177;
const uint8_t OP_getstatic      = 178;
const uint8_t OP_putstatic      = 179;
const uint8_t OP_invokevirtual  = 182;
const uint8_t OP_invokespecial  = 183;
const uint8_t OP_invokestatic   = 184;
const uint8_t OP_invokeinterface= 185;
const uint8_t OP_new            = 187;
const uint8_t OP_newarray       = 188;
const uint8_t OP_arraylength    = 190;
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
    ClassFile(const std::string &name): name(name), magic(0), minor_version(0), major_version(0), constant_pool_count(0), constant_pool(), access_flags(0), this_class(0), super_class(0), interfaces(), fields(), methods(), attributes(), utf8_constants(), Class_constants(), String_constants(), NameAndType_constants(), Field_constants(), Method_constants(), InterfaceMethod_constants() {}
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
    uint16_t NameAndType(const std::string &name, const std::string &descriptor) {
        auto key = std::make_pair(name, descriptor);
        auto i = NameAndType_constants.find(key);
        if (i != NameAndType_constants.end()) {
            return i->second;
        }
        cp_info cp;
        cp.tag = CONSTANT_NameAndType;
        cp.info << utf8(name);
        cp.info << utf8(descriptor);
        constant_pool.push_back(cp);
        constant_pool_count++;
        NameAndType_constants[key] = constant_pool_count;
        return constant_pool_count;
    }

    std::map<std::tuple<std::string, std::string, std::string>, uint16_t> Field_constants;
    uint16_t Field(const std::string &cls, const std::string &name, const std::string &descriptor) {
        auto key = std::make_tuple(cls, name, descriptor);
        auto i = Field_constants.find(key);
        if (i != Field_constants.end()) {
            return i->second;
        }
        cp_info cp;
        cp.tag = CONSTANT_Fieldref;
        cp.info << Class(cls);
        cp.info << NameAndType(name, descriptor);
        constant_pool.push_back(cp);
        constant_pool_count++;
        Field_constants[key] = constant_pool_count;
        return constant_pool_count;
    }

    std::map<std::tuple<std::string, std::string, std::string>, uint16_t> Method_constants;
    uint16_t Method(const std::string &cls, const std::string &name, const std::string &descriptor) {
        auto key = std::make_tuple(cls, name, descriptor);
        auto i = Method_constants.find(key);
        if (i != Method_constants.end()) {
            return i->second;
        }
        cp_info cp;
        cp.tag = CONSTANT_Methodref;
        cp.info << Class(cls);
        cp.info << NameAndType(name, descriptor);
        constant_pool.push_back(cp);
        constant_pool_count++;
        Method_constants[key] = constant_pool_count;
        return constant_pool_count;
    }

    std::map<std::tuple<std::string, std::string, std::string>, uint16_t> InterfaceMethod_constants;
    uint16_t InterfaceMethod(const std::string &cls, const std::string &name, const std::string &descriptor) {
        auto key = std::make_tuple(cls, name, descriptor);
        auto i = InterfaceMethod_constants.find(key);
        if (i != InterfaceMethod_constants.end()) {
            return i->second;
        }
        cp_info cp;
        cp.tag = CONSTANT_InterfaceMethodref;
        cp.info << Class(cls);
        cp.info << NameAndType(name, descriptor);
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
private:
    ClassFile(const ClassFile &);
    ClassFile &operator=(const ClassFile &);
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
    Context(ClassFile &cf, Code_attribute &ca): cf(cf), ca(ca), label_exit(), loop_labels() {}
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
private:
    Context(const Context &);
    Context &operator=(const Context &);
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
    Type(const std::string &classname, const std::string &jtype = ""): classname(classname), jtype(jtype.empty() ? "L" + classname + ";" : jtype) {}
    virtual ~Type() {}
    const std::string classname;
    const std::string jtype;
private:
    Type(const Type &);
    Type &operator=(const Type &);
};

Type *transform(const ast::Type *t);

class TypeNothing: public Type {
public:
    TypeNothing(const ast::TypeNothing *tn): Type("V", "V"), tn(tn) {}
    const ast::TypeNothing *tn;
private:
    TypeNothing(const TypeNothing &);
    TypeNothing &operator=(const TypeNothing &);
};

class TypeDummy: public Type {
public:
    TypeDummy(const ast::TypeDummy *td): Type(""), td(td) {}
    const ast::TypeDummy *td;
private:
    TypeDummy(const TypeDummy &);
    TypeDummy &operator=(const TypeDummy &);
};

class TypeBoolean: public Type {
public:
    TypeBoolean(const ast::TypeBoolean *tb): Type("java/lang/Boolean"), tb(tb) {}
    const ast::TypeBoolean *tb;
private:
    TypeBoolean(const TypeBoolean &);
    TypeBoolean &operator=(const TypeBoolean &);
};

class TypeNumber: public Type {
public:
    TypeNumber(const ast::TypeNumber *tn): Type("neon/type/Number"), tn(tn) {}
    const ast::TypeNumber *tn;
private:
    TypeNumber(const TypeNumber &);
    TypeNumber &operator=(const TypeNumber &);
};

class TypeString: public Type {
public:
    TypeString(const ast::TypeString *ts): Type("java/lang/String"), ts(ts) {}
    const ast::TypeString *ts;
private:
    TypeString(const TypeString &);
    TypeString &operator=(const TypeString &);
};

class TypeBytes: public Type {
public:
    TypeBytes(const ast::TypeBytes *tb): Type("[B", "[B"), tb(tb) {}
    const ast::TypeBytes *tb;
private:
    TypeBytes(const TypeBytes &);
    TypeBytes &operator=(const TypeBytes &);
};

class TypeFunction: public Type {
public:
    TypeFunction(const ast::TypeFunction *tf): Type(""), tf(tf), returntype(transform(tf->returntype)) {}
    const ast::TypeFunction *tf;
    const Type *returntype;
private:
    TypeFunction(const TypeFunction &);
    TypeFunction &operator=(const TypeFunction &);
};

class TypeArray: public Type {
public:
    TypeArray(const ast::TypeArray *ta): Type("neon/type/Array"), ta(ta), elementtype(transform(ta->elementtype)) {}
    const ast::TypeArray *ta;
    const Type *elementtype;
private:
    TypeArray(const TypeArray &);
    TypeArray &operator=(const TypeArray &);
};

class TypeDictionary: public Type {
public:
    TypeDictionary(const ast::TypeDictionary *td): Type("java/util/HashMap"), td(td), elementtype(transform(td->elementtype)) {}
    const ast::TypeDictionary *td;
    const Type *elementtype;
private:
    TypeDictionary(const TypeDictionary &);
    TypeDictionary &operator=(const TypeDictionary &);
};

class Variable {
public:
    Variable(const ast::Variable *v): type(transform(v->type)) {}
    virtual ~Variable() {}
    const Type *type;
    virtual void generate_decl(Context &context) const = 0;
    virtual void generate_load(Context &context) const = 0;
    virtual void generate_store(Context &context) const = 0;
    virtual void generate_call(Context &context) const = 0;
private:
    Variable(const Variable &);
    Variable &operator=(const Variable &);
};

Variable *transform(const ast::Variable *v);

class GlobalVariable: public Variable {
public:
    GlobalVariable(const ast::GlobalVariable *gv): Variable(gv), gv(gv) {}
    const ast::GlobalVariable *gv;

    virtual void generate_decl(Context &context) const override {
        field_info f;
        f.access_flags = ACC_STATIC;
        f.name_index = context.cf.utf8(gv->name);
        f.descriptor_index = context.cf.utf8(type->jtype);
        context.cf.fields.push_back(f);
    }
    virtual void generate_load(Context &context) const override {
        context.ca.code << OP_getstatic << context.cf.Field(context.cf.name, gv->name, type->jtype);
    }
    virtual void generate_store(Context &context) const override {
        context.ca.code << OP_putstatic << context.cf.Field(context.cf.name, gv->name, type->jtype);
    }
    virtual void generate_call(Context &) const override { internal_error("GlobalVariable"); }
private:
    GlobalVariable(const GlobalVariable &);
    GlobalVariable &operator=(const GlobalVariable &);
};

class LocalVariable: public Variable {
public:
    LocalVariable(const ast::LocalVariable *lv): Variable(lv), lv(lv), index(-1) {}
    const ast::LocalVariable *lv;
    mutable int index; // TODO

    virtual void generate_decl(Context &) const override {}
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
    virtual void generate_call(Context &) const override { internal_error("LocalVariable"); }
private:
    LocalVariable(const LocalVariable &);
    LocalVariable &operator=(const LocalVariable &);
};

class FunctionParameter: public Variable {
public:
    FunctionParameter(const ast::FunctionParameter *fp, int index): Variable(fp), fp(fp), index(index) {}
    const ast::FunctionParameter *fp;
    const int index;

    virtual void generate_decl(Context &) const override { internal_error("FunctionParameter"); }
    virtual void generate_load(Context &context) const override {
        if (index < 4) {
            context.ca.code << static_cast<uint8_t>(OP_aload_0 + index);
        } else if (index < 256) {
            context.ca.code << OP_aload << static_cast<uint8_t>(index);
        } else {
            context.ca.code << OP_wide << OP_aload << static_cast<uint16_t>(index);
        }
    }
    virtual void generate_store(Context &) const override { internal_error("FunctionParameter"); }
    virtual void generate_call(Context &) const override { internal_error("FunctionParameter"); }
private:
    FunctionParameter(const FunctionParameter &);
    FunctionParameter &operator=(const FunctionParameter &);
};

class Expression {
public:
    Expression(const ast::Expression *node): type(transform(node->type)) {}
    virtual ~Expression() {}
    const Type *type;
    virtual void generate(Context &context) const = 0;
    virtual void generate_call(Context &context) const = 0;
    virtual void generate_store(Context &context) const = 0;
private:
    Expression(const Expression &);
    Expression &operator=(const Expression &);
};

Expression *transform(const ast::Expression *e);

class ConstantBooleanExpression: public Expression {
public:
    ConstantBooleanExpression(const ast::ConstantBooleanExpression *cbe): Expression(cbe), cbe(cbe) {}
    const ast::ConstantBooleanExpression *cbe;

    virtual void generate(Context &context) const override {
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", cbe->value ? "TRUE" : "FALSE", "Ljava/lang/Boolean;");
    }
    virtual void generate_call(Context &) const override { internal_error("ConstantBooleanExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantBooleanExpression"); }
private:
    ConstantBooleanExpression(const ConstantBooleanExpression &);
    ConstantBooleanExpression &operator=(const ConstantBooleanExpression &);
};

class ConstantNumberExpression: public Expression {
public:
    ConstantNumberExpression(const ast::ConstantNumberExpression *cne): Expression(cne), cne(cne) {}
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
    virtual void generate_call(Context &) const override { internal_error("ConstantNumberExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantNumberExpression"); }
private:
    ConstantNumberExpression(const ConstantNumberExpression &);
    ConstantNumberExpression &operator=(const ConstantNumberExpression &);
};

class ConstantStringExpression: public Expression {
public:
    ConstantStringExpression(const ast::ConstantStringExpression *cse): Expression(cse), cse(cse) {}
    const ast::ConstantStringExpression *cse;

    virtual void generate(Context &context) const override {
        context.ca.code << OP_ldc_w << context.cf.String(cse->value);
    }

    virtual void generate_call(Context &) const override { internal_error("ConstantStringExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantStringExpression"); }
private:
    ConstantStringExpression(const ConstantStringExpression &);
    ConstantStringExpression &operator=(const ConstantStringExpression &);
};

class ConstantBytesExpression: public Expression {
public:
    ConstantBytesExpression(const ast::ConstantBytesExpression *cbe): Expression(cbe), cbe(cbe) {}
    const ast::ConstantBytesExpression *cbe;

    virtual void generate(Context &context) const override {
        context.push_integer(cbe->contents.size());
        context.ca.code << OP_newarray << T_BYTE;
        int i = 0;
        for (auto c: cbe->contents) {
            context.ca.code << OP_dup;
            context.push_integer(i);
            context.push_integer(static_cast<int8_t>(c & 0xff));
            context.ca.code << OP_bastore;
        }
    }

    virtual void generate_call(Context &) const override { internal_error("ConstantBytesExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantBytesExpression"); }
private:
    ConstantBytesExpression(const ConstantBytesExpression &);
    ConstantBytesExpression &operator=(const ConstantBytesExpression &);
};

class ConstantNilExpression: public Expression {
public:
    ConstantNilExpression(const ast::ConstantNilExpression *cne): Expression(cne), cne(cne) {}
    const ast::ConstantNilExpression *cne;

    virtual void generate(Context &context) const override {
        context.ca.code << OP_aconst_null;
    }

    virtual void generate_call(Context &) const override { internal_error("ConstantNilExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ConstantNilExpression"); }
private:
    ConstantNilExpression(const ConstantNilExpression &);
    ConstantNilExpression &operator=(const ConstantNilExpression &);
};

class ArrayLiteralExpression: public Expression {
public:
    ArrayLiteralExpression(const ast::ArrayLiteralExpression *ale): Expression(ale), ale(ale), elements() {
        for (auto e: ale->elements) {
            elements.push_back(transform(e));
        }
    }
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

    virtual void generate_call(Context &) const override { internal_error("ArrayLiteralExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ArrayLiteralExpression"); }
private:
    ArrayLiteralExpression(const ArrayLiteralExpression &);
    ArrayLiteralExpression &operator=(const ArrayLiteralExpression &);
};

class DictionaryLiteralExpression: public Expression {
public:
    DictionaryLiteralExpression(const ast::DictionaryLiteralExpression *dle): Expression(dle), dle(dle), dict() {
        for (auto d: dle->dict) {
            dict[d.first] = transform(d.second);
        }
    }
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
    virtual void generate_call(Context &) const override { internal_error("DictionaryLiteralExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DictionaryLiteralExpression"); }
private:
    DictionaryLiteralExpression(const DictionaryLiteralExpression &);
    DictionaryLiteralExpression &operator=(const DictionaryLiteralExpression &);
};

class UnaryMinusExpression: public Expression {
public:
    UnaryMinusExpression(const ast::UnaryMinusExpression *ume): Expression(ume), ume(ume), value(transform(ume->value)) {}
    const ast::UnaryMinusExpression *ume;
    const Expression *value;

    virtual void generate(Context &context) const override {
        value->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "negate", "()Lneon/type/Number;");
    }
    virtual void generate_call(Context &) const override { internal_error("UnaryMinusExpression"); }
    virtual void generate_store(Context &) const override { internal_error("UnaryMinusExpression"); }
private:
    UnaryMinusExpression(const UnaryMinusExpression &);
    UnaryMinusExpression &operator=(const UnaryMinusExpression &);
};

class LogicalNotExpression: public Expression {
public:
    LogicalNotExpression(const ast::LogicalNotExpression *lne): Expression(lne), lne(lne), value(transform(lne->value)) {}
    const ast::LogicalNotExpression *lne;
    const Expression *value;

    virtual void generate(Context &context) const override {
        value->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/Boolean", "booleanValue", "()Z");
        context.ca.code << OP_iconst_1;
        context.ca.code << OP_ixor;
        context.ca.code << OP_invokestatic << context.cf.Method("java/lang/Boolean", "valueOf", "(Z)Ljava/lang/Boolean;");
    }
    virtual void generate_call(Context &) const override { internal_error("LogicalNotExpression"); }
    virtual void generate_store(Context &) const override { internal_error("LogicalNotExpression"); }
private:
    LogicalNotExpression(const LogicalNotExpression &);
    LogicalNotExpression &operator=(const LogicalNotExpression &);
};

class ComparisonExpression: public Expression {
public:
    ComparisonExpression(const ast::ComparisonExpression *ce): Expression(ce), ce(ce), left(transform(ce->left)), right(transform(ce->right)) {}
    const ast::ComparisonExpression *ce;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        right->generate(context);
        generate_comparison(context);
    }
    virtual void generate_comparison(Context &context) const = 0;

    virtual void generate_call(Context &) const override { internal_error("ComparisonExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ComparisonExpression"); }
private:
    ComparisonExpression(const ComparisonExpression &);
    ComparisonExpression &operator=(const ComparisonExpression &);
};

class NumericComparisonExpression: public ComparisonExpression {
public:
    NumericComparisonExpression(const ast::NumericComparisonExpression *nce): ComparisonExpression(nce), nce(nce) {}
    const ast::NumericComparisonExpression *nce;

    virtual void generate_comparison(Context &context) const override {
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "compareTo", "(Lneon/type/Number;)I");
        static const uint8_t op[] = {OP_ifeq, OP_ifne, OP_iflt, OP_ifgt, OP_ifle, OP_ifge};
        auto label_true = context.create_label();
        context.emit_jump(op[nce->comp], label_true);
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "FALSE", "Ljava/lang/Boolean;");
        auto label_false = context.create_label();
        context.emit_jump(OP_goto, label_false);
        context.jump_target(label_true);
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "TRUE", "Ljava/lang/Boolean;");
        context.jump_target(label_false);
    }
private:
    NumericComparisonExpression(const NumericComparisonExpression &);
    NumericComparisonExpression &operator=(const NumericComparisonExpression &);
};

class StringComparisonExpression: public ComparisonExpression {
public:
    StringComparisonExpression(const ast::StringComparisonExpression *sce): ComparisonExpression(sce), sce(sce) {}
    const ast::StringComparisonExpression *sce;

    virtual void generate_comparison(Context &context) const override {
        context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/String", "compareTo", "(Ljava/lang/String;)I");
        static const uint8_t op[] = {OP_ifeq, OP_ifne, OP_iflt, OP_ifgt, OP_ifle, OP_ifge};
        auto label_true = context.create_label();
        context.emit_jump(op[sce->comp], label_true);
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "FALSE", "Ljava/lang/Boolean;");
        auto label_false = context.create_label();
        context.emit_jump(OP_goto, label_false);
        context.jump_target(label_true);
        context.ca.code << OP_getstatic << context.cf.Field("java/lang/Boolean", "TRUE", "Ljava/lang/Boolean;");
        context.jump_target(label_false);
    }
private:
    StringComparisonExpression(const StringComparisonExpression &);
    StringComparisonExpression &operator=(const StringComparisonExpression &);
};

class AdditionExpression: public Expression {
public:
    AdditionExpression(const ast::AdditionExpression *ae): Expression(ae), ae(ae), left(transform(ae->left)), right(transform(ae->right)) {}
    const ast::AdditionExpression *ae;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        right->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "add", "(Lneon/type/Number;)Lneon/type/Number;");
    }
    virtual void generate_call(Context &) const override { internal_error("AdditionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("AdditionExpression"); }
private:
    AdditionExpression(const AdditionExpression &);
    AdditionExpression &operator=(const AdditionExpression &);
};

class SubtractionExpression: public Expression {
public:
    SubtractionExpression(const ast::SubtractionExpression *se): Expression(se), se(se), left(transform(se->left)), right(transform(se->right)) {}
    const ast::SubtractionExpression *se;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        right->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "subtract", "(Lneon/type/Number;)Lneon/type/Number;");
    }
    virtual void generate_call(Context &) const override { internal_error("SubtractionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("SubtractionExpression"); }
private:
    SubtractionExpression(const SubtractionExpression &);
    SubtractionExpression &operator=(const SubtractionExpression &);
};

class MultiplicationExpression: public Expression {
public:
    MultiplicationExpression(const ast::MultiplicationExpression *me): Expression(me), me(me), left(transform(me->left)), right(transform(me->right)) {}
    const ast::MultiplicationExpression *me;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        right->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "multiply", "(Lneon/type/Number;)Lneon/type/Number;");
    }
    virtual void generate_call(Context &) const override { internal_error("MultiplicationExpression"); }
    virtual void generate_store(Context &) const override { internal_error("MultiplicationExpression"); }
private:
    MultiplicationExpression(const MultiplicationExpression &);
    MultiplicationExpression &operator=(const MultiplicationExpression &);
};

class DivisionExpression: public Expression {
public:
    DivisionExpression(const ast::DivisionExpression *de): Expression(de), de(de), left(transform(de->left)), right(transform(de->right)) {}
    const ast::DivisionExpression *de;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        right->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "divide", "(Lneon/type/Number;)Lneon/type/Number;");
    }
    virtual void generate_call(Context &) const override { internal_error("DivisionExpression"); }
    virtual void generate_store(Context &) const override { internal_error("DivisionExpression"); }
private:
    DivisionExpression(const DivisionExpression &);
    DivisionExpression &operator=(const DivisionExpression &);
};

class ModuloExpression: public Expression {
public:
    ModuloExpression(const ast::ModuloExpression *me): Expression(me), me(me), left(transform(me->left)), right(transform(me->right)) {}
    const ast::ModuloExpression *me;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        right->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "remainder", "(Lneon/type/Number;)Lneon/type/Number;");
    }
    virtual void generate_call(Context &) const override { internal_error("ModuloExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ModuloExpression"); }
private:
    ModuloExpression(const ModuloExpression &);
    ModuloExpression &operator=(const ModuloExpression &);
};

class ExponentiationExpression: public Expression {
public:
    ExponentiationExpression(const ast::ExponentiationExpression *ee): Expression(ee), ee(ee), left(transform(ee->left)), right(transform(ee->right)) {}
    const ast::ExponentiationExpression *ee;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        left->generate(context);
        right->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "pow", "(Lneon/type/Number;)Lneon/type/Number;");
    }
    virtual void generate_call(Context &) const override { internal_error("ExponentiationExpression"); }
    virtual void generate_store(Context &) const override { internal_error("ExponentiationExpression"); }
private:
    ExponentiationExpression(const ExponentiationExpression &);
    ExponentiationExpression &operator=(const ExponentiationExpression &);
};

class DummyExpression: public Expression {
public:
    DummyExpression(const ast::DummyExpression *de): Expression(de), de(de) {}
    const ast::DummyExpression *de;

    virtual void generate(Context &) const override { internal_error("DummyExpression"); }
    virtual void generate_call(Context &) const override { internal_error("DummyExpression"); }
    virtual void generate_store(Context &context) const override {
        context.ca.code << OP_pop;
    }
private:
    DummyExpression(const DummyExpression &);
    DummyExpression &operator=(const DummyExpression &);
};

class ArrayReferenceIndexExpression: public Expression {
public:
    ArrayReferenceIndexExpression(const ast::ArrayReferenceIndexExpression *arie): Expression(arie), arie(arie), array(transform(arie->array)), index(transform(arie->index)) {}
    const ast::ArrayReferenceIndexExpression *arie;
    const Expression *array;
    const Expression *index;

    virtual void generate(Context &context) const override {
        array->generate(context);
        index->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Array", "get", "(Lneon/type/Number;)Ljava/lang/Object;");
        context.ca.code << OP_checkcast << context.cf.Class(dynamic_cast<const TypeArray *>(array->type)->elementtype->classname);
    }
    virtual void generate_call(Context &) const override { internal_error("ArrayReferenceIndexExpression"); }
    virtual void generate_store(Context &context) const override {
        array->generate(context);
        context.ca.code << OP_swap;
        index->generate(context);
        context.ca.code << OP_swap;
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Array", "set", "(Lneon/type/Number;Ljava/lang/Object;)Ljava/lang/Object;");
        context.ca.code << OP_pop;
    }
private:
    ArrayReferenceIndexExpression(const ArrayReferenceIndexExpression &);
    ArrayReferenceIndexExpression &operator=(const ArrayReferenceIndexExpression &);
};

class VariableExpression: public Expression {
public:
    VariableExpression(const ast::VariableExpression *ve): Expression(ve), ve(ve), var(transform(ve->var)) {}
    const ast::VariableExpression *ve;
    const Variable *var;

    virtual void generate(Context &context) const override {
        var->generate_load(context);
    }

    virtual void generate_call(Context &context) const override {
        var->generate_call(context);
    }
    virtual void generate_store(Context &context) const override {
        var->generate_store(context);
    }
private:
    VariableExpression(const VariableExpression &);
    VariableExpression &operator=(const VariableExpression &);
};

class FunctionCall: public Expression {
public:
    FunctionCall(const ast::FunctionCall *fc): Expression(fc), fc(fc), func(transform(fc->func)), args() {
        for (auto a: fc->args) {
            args.push_back(transform(a));
        }
    }
    const ast::FunctionCall *fc;
    const Expression *func;
    std::vector<const Expression *> args;

    virtual void generate(Context &context) const override {
        for (auto a: args) {
            a->generate(context);
        }
        func->generate_call(context);
    }

    virtual void generate_call(Context &) const override { internal_error("FunctionCall"); }
    virtual void generate_store(Context &) const override { internal_error("FunctionCall"); }
private:
    FunctionCall(const FunctionCall &);
    FunctionCall &operator=(const FunctionCall &);
};

class Statement {
public:
    virtual ~Statement() {}
    virtual void generate(Context &context) const = 0;
};

Statement *transform(const ast::Statement *s);

class NullStatement: public Statement {
public:
    NullStatement(const ast::NullStatement *ns): ns(ns) {}
    const ast::NullStatement *ns;

    virtual void generate(Context &) const override {}
private:
    NullStatement(const NullStatement &);
    NullStatement &operator=(const NullStatement &);
};

class DeclarationStatement: public Statement {
public:
    DeclarationStatement(const ast::DeclarationStatement *ds): ds(ds), decl(transform(ds->decl)) {}
    const ast::DeclarationStatement *ds;
    const Variable *decl;

    virtual void generate(Context &context) const override {
        decl->generate_decl(context);
    }
private:
    DeclarationStatement(const DeclarationStatement &);
    DeclarationStatement &operator=(const DeclarationStatement &);
};

class AssertStatement: public Statement {
public:
    AssertStatement(const ast::AssertStatement *as): as(as), statements(), expr(transform(as->expr)) {
        for (auto s: as->statements) {
            statements.push_back(transform(s));
        }
    }
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
        // TODO: create code to throw assertion exception
        context.jump_target(label_skip);
    }
private:
    AssertStatement(const AssertStatement &);
    AssertStatement &operator=(const AssertStatement &);
};

class AssignmentStatement: public Statement {
public:
    AssignmentStatement(const ast::AssignmentStatement *as): as(as), variables(), expr(transform(as->expr)) {
        for (auto v: as->variables) {
            variables.push_back(transform(v));
        }
    }
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
private:
    AssignmentStatement(const AssignmentStatement &);
    AssignmentStatement &operator=(const AssignmentStatement &);
};

class ExpressionStatement: public Statement {
public:
    ExpressionStatement(const ast::ExpressionStatement *es): es(es), expr(transform(es->expr)) {}
    const ast::ExpressionStatement *es;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        expr->generate(context);
    }
private:
    ExpressionStatement(const ExpressionStatement &);
    ExpressionStatement &operator=(const ExpressionStatement &);
};

class CompoundStatement: public Statement {
public:
    CompoundStatement(const ast::CompoundStatement *cs): cs(cs), statements() {
        for (auto s: cs->statements) {
            statements.push_back(transform(s));
        }
    }
    const ast::CompoundStatement *cs;
    std::vector<const Statement *> statements;

    virtual void generate(Context &context) const override {
        for (auto s: statements) {
            s->generate(context);
        }
    }
private:
    CompoundStatement(const CompoundStatement &);
    CompoundStatement &operator=(const CompoundStatement &);
};

class BaseLoopStatement: public CompoundStatement {
public:
    BaseLoopStatement(const ast::BaseLoopStatement *bls): CompoundStatement(bls), bls(bls), prologue(), tail() {
        for (auto s: bls->prologue) {
            prologue.push_back(transform(s));
        }
        for (auto s: bls->tail) {
            tail.push_back(transform(s));
        }
    }
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
private:
    BaseLoopStatement(const BaseLoopStatement &);
    BaseLoopStatement &operator=(const BaseLoopStatement &);
};

class CaseStatement: public Statement {
public:
    class WhenCondition {
    public:
        WhenCondition() {}
        virtual ~WhenCondition() {}
        virtual void generate(Context &context, Context::Label &label_true, Context::Label &label_false) const = 0;
    private:
        WhenCondition(const WhenCondition &);
        WhenCondition &operator=(const WhenCondition &);
    };
    class ComparisonWhenCondition: public WhenCondition {
    public:
        ComparisonWhenCondition(ast::ComparisonExpression::Comparison comp, const Expression *expr): comp(comp), expr(expr) {}
        ast::ComparisonExpression::Comparison comp;
        const Expression *expr;
        virtual void generate(Context &context, Context::Label &label_true, Context::Label &label_false) const override {
            context.ca.code << OP_dup;
            expr->generate(context);
            context.ca.code << OP_invokeinterface << context.cf.InterfaceMethod("java/lang/Comparable", "compareTo", "(Ljava/lang/Object;)I") << static_cast<uint8_t>(2) << static_cast<uint8_t>(0);
            static const uint8_t op[] = {OP_ifeq, OP_ifne, OP_iflt, OP_ifgt, OP_ifle, OP_ifge};
            context.emit_jump(op[comp], label_true);
            context.emit_jump(OP_goto, label_false);
        }
    private:
        ComparisonWhenCondition(const ComparisonWhenCondition &);
        ComparisonWhenCondition &operator=(const ComparisonWhenCondition &);
    };
    class RangeWhenCondition: public WhenCondition {
    public:
        RangeWhenCondition(const Expression *low_expr, const Expression *high_expr): low_expr(low_expr), high_expr(high_expr) {}
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
    private:
        RangeWhenCondition(const RangeWhenCondition &);
        RangeWhenCondition &operator=(const RangeWhenCondition &);
    };
    CaseStatement(const ast::CaseStatement *cs): cs(cs), expr(transform(cs->expr)), clauses() {
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
        context.jump_target(clause_labels.back().second);
    }
private:
    CaseStatement(const CaseStatement &);
    CaseStatement &operator=(const CaseStatement &);
};

class ExitStatement: public Statement {
public:
    ExitStatement(const ast::ExitStatement *es): es(es) {}
    const ast::ExitStatement *es;

    void generate(Context &context) const override {
        context.emit_jump(OP_goto, context.get_exit_label(es->loop_id));
    }
private:
    ExitStatement(const ExitStatement &);
    ExitStatement &operator=(const ExitStatement &);
};

class NextStatement: public Statement {
public:
    NextStatement(const ast::NextStatement *ns): ns(ns) {}
    const ast::NextStatement *ns;

    virtual void generate(Context &context) const override {
        context.emit_jump(OP_goto, context.get_next_label(ns->loop_id));
    }
private:
    NextStatement(const NextStatement &);
    NextStatement &operator=(const NextStatement &);
};

class TryStatementTrap {
public:
    TryStatementTrap(const ast::TryTrap *tt): tt(tt), name(transform(tt->name)), handler() {
        for (auto s: dynamic_cast<const ast::ExceptionHandlerStatement *>(tt->handler)->statements) {
            handler.push_back(transform(s));
        }
    }
    const ast::TryTrap *tt;
    const Variable *name;
    std::vector<const Statement *> handler;
};

class TryStatement: public Statement {
public:
    TryStatement(const ast::TryStatement *ts): ts(ts), statements() {
        for (auto s: ts->statements) {
            statements.push_back(transform(s));
        }
        for (auto &t: ts->catches) {
            catches.push_back(new TryStatementTrap(&t));
        }
    }
    const ast::TryStatement *ts;
    std::vector<const Statement *> statements;
    std::vector<const TryStatementTrap *> catches;

    virtual void generate(Context &context) const override {
        Code_attribute::exception e;
        e.start_pc = context.ca.code.size();
        for (auto s: statements) {
            s->generate(context);
        }
        e.end_pc = context.ca.code.size();
        auto label_end = context.create_label();
        context.emit_jump(OP_goto, label_end);
        for (auto trap: catches) {
            e.handler_pc = context.ca.code.size();
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
private:
    TryStatement(const TryStatement &);
    TryStatement &operator=(const TryStatement &);
};

class ReturnStatement: public Statement {
public:
    ReturnStatement(const ast::ReturnStatement *rs): rs(rs), expr(transform(rs->expr)) {}
    const ast::ReturnStatement *rs;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        if (expr != nullptr) {
            expr->generate(context);
        }
        context.emit_jump(OP_goto, context.label_exit);
    }
private:
    ReturnStatement(const ReturnStatement &);
    ReturnStatement &operator=(const ReturnStatement &);
};

class IncrementStatement: public Statement {
public:
    IncrementStatement(const ast::IncrementStatement *is): is(is), ref(transform(is->ref)) {}
    const ast::IncrementStatement *is;
    const Expression *ref;

    virtual void generate(Context &context) const override {
        ref->generate(context);
        (new ConstantNumberExpression(new ast::ConstantNumberExpression(number_from_sint32(is->delta))))->generate(context);
        context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "add", "(Lneon/type/Number;)Lneon/type/Number;");
        ref->generate_store(context);
    }
private:
    IncrementStatement(const IncrementStatement &);
    IncrementStatement &operator=(const IncrementStatement &);
};

class IfStatement: public Statement {
public:
    IfStatement(const ast::IfStatement *is): is(is), condition_statements(), else_statements() {
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
private:
    IfStatement(const IfStatement &);
    IfStatement &operator=(const IfStatement &);
};

class RaiseStatement: public Statement {
public:
    RaiseStatement(const ast::RaiseStatement *rs): rs(rs) {}
    const ast::RaiseStatement *rs;

    virtual void generate(Context &context) const override {
        context.ca.code << OP_aconst_null; // TODO
        context.ca.code << OP_athrow;
    }
private:
    RaiseStatement(const RaiseStatement &);
    RaiseStatement &operator=(const RaiseStatement &);
};

class ResetStatement: public Statement {
public:
    ResetStatement(const ast::ResetStatement *rs): rs(rs) {}
    const ast::ResetStatement *rs;

    virtual void generate(Context &) const override {
    }
private:
    ResetStatement(const ResetStatement &);
    ResetStatement &operator=(const ResetStatement &);
};

class Function: public Variable {
public:
    Function(const ast::Function *f): Variable(f), f(f), statements(), params(), signature() {
        // Need to transform the function parameters before transforming
        // the code that might use them (statements).
        signature = "(";
        int i = 0;
        for (auto p: f->params) {
            FunctionParameter *q = new FunctionParameter(p, i);
            params.push_back(q);
            g_variable_cache[p] = q;
            signature.append(q->type->jtype);
            i++;
        }
        signature.append(")");
        signature.append(dynamic_cast<const TypeFunction *>(type)->returntype->jtype);
        for (auto s: f->statements) {
            statements.push_back(transform(s));
        }
    }
    const ast::Function *f;
    std::vector<const Statement *> statements;
    std::vector<FunctionParameter *> params;
    std::string signature;

    virtual void generate_decl(Context &context) const override {
        method_info m;
        m.access_flags = ACC_PUBLIC | ACC_STATIC;
        m.name_index = context.cf.utf8(f->name);
        m.descriptor_index = context.cf.utf8(signature);
        {
            attribute_info a;
            a.attribute_name_index = context.cf.utf8("Code");
            {
                Code_attribute ca;
                ca.max_stack = 8;
                ca.max_locals = static_cast<uint16_t>(params.size());
                Context function_context(context.cf, ca);
                for (auto s: statements) {
                    s->generate(function_context);
                }
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
                a.info = ca.serialize();
            }
            m.attributes.push_back(a);
        }
        context.cf.methods.push_back(m);
    }
    virtual void generate_load(Context &) const override { internal_error("Function"); }
    virtual void generate_store(Context &) const override { internal_error("Function"); }
    virtual void generate_call(Context &context) const override {
        context.ca.code << OP_invokestatic << context.cf.Method(context.cf.name, f->name, signature);
    }
private:
    Function(const Function &);
    Function &operator=(const Function &);
};

class PredefinedFunction: public Variable {
public:
    PredefinedFunction(const ast::PredefinedFunction *pf): Variable(pf), pf(pf) {}
    const ast::PredefinedFunction *pf;

    virtual void generate_decl(Context &) const override {}
    virtual void generate_load(Context &) const override { internal_error("PredefinedFunction"); }
    virtual void generate_store(Context &) const override { internal_error("PredefinedFunction"); }
    virtual void generate_call(Context &context) const override {
        if (pf->name == "print") {
            context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "print", "(Ljava/lang/String;)V");
        } else if (pf->name == "number__toString" || pf->name == "str") {
            context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Number", "toString", "()Ljava/lang/String;");
        } else if (pf->name == "concat") {
            context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/String", "concat", "(Ljava/lang/String;)Ljava/lang/String;");
        } else if (pf->name == "array__append") {
            context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Array", "add", "(Ljava/lang/Object;)Z");
        } else if (pf->name == "array__size") {
            context.ca.code << OP_invokevirtual << context.cf.Method("neon/type/Array", "size_n", "()Lneon/type/Number;");
        } else if (pf->name == "array__concat") {
            context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "array__concat", "(Lneon/type/Array;Lneon/type/Array;)Lneon/type/Array;");
        } else if (pf->name == "array__toString__number") {
            context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/Object", "toString", "()Ljava/lang/String;");
        } else if (pf->name == "array__extend") {
            context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "array__extend", "(Lneon/type/Array;Lneon/type/Array;)V");
        } else if (pf->name == "array__toBytes__number") {
            context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "array__toBytes__number", "(Lneon/type/Array;)[B");
        } else if (pf->name == "bytes__size") {
            context.ca.code << OP_arraylength;
            context.ca.code << OP_new << context.cf.Class("neon/type/Number");
            context.ca.code << OP_dup_x1;
            context.ca.code << OP_swap;
            context.ca.code << OP_invokespecial << context.cf.Method("neon/type/Number", "<init>", "(I)V");
        } else if (pf->name == "bytes__toArray") {
            context.ca.code << OP_invokestatic << context.cf.Method("neon/Global", "bytes__toArray", "([B)Lneon/type/Array;");
        } else if (pf->name == "strb" || pf->name == "boolean__toString") {
            context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/Boolean", "toString", "()Ljava/lang/String;");
            context.ca.code << OP_invokevirtual << context.cf.Method("java/lang/String", "toUpperCase", "()Ljava/lang/String;");
        } else {
            internal_error("PredefinedFunction: " + pf->name);
        }
    }
private:
    PredefinedFunction(const PredefinedFunction &);
    PredefinedFunction &operator=(const PredefinedFunction &);
};

class Program {
public:
    Program(const ast::Program *program): program(program), statements() {
        for (auto s: program->statements) {
            statements.push_back(transform(s));
        }
    }
    virtual ~Program() {}
    const ast::Program *program;
    std::vector<const Statement *> statements;

    virtual void generate() const {
        std::string name = program->source_path;
        std::string path;
        std::string::size_type i = name.find_last_of("/\\:");
        if (i != std::string::npos) {
            path = name.substr(0, i + 1);
            name = name.substr(i + 1);
        }
        if (name.length() > 5 && name.substr(name.length() - 5) == ".neon") {
            name = name.substr(0, name.length() - 5);
        }
        ClassFile cf(name);
        cf.magic = 0xCAFEBABE;
        cf.minor_version = 0;
        cf.major_version = 49;
        cf.constant_pool_count = 0;
        cf.access_flags = ACC_PUBLIC | ACC_SUPER;
        cf.this_class = cf.Class(cf.name);
        cf.super_class = cf.Class("Ljava/lang/Object;");

        for (size_t i = 0; i < program->frame->getCount(); i++) {
            auto slot = program->frame->getSlot(i);
            const ast::GlobalVariable *global = dynamic_cast<const ast::GlobalVariable *>(slot.ref);
            if (global != nullptr) {
                field_info f;
                f.access_flags = ACC_STATIC;
                f.name_index = cf.utf8(slot.name);
                if (global->type == ast::TYPE_BOOLEAN) {
                    f.descriptor_index = cf.utf8("Ljava/lang/Boolean;");
                } else if (global->type == ast::TYPE_NUMBER) {
                    f.descriptor_index = cf.utf8("Lneon/type/Number;");
                } else if (global->type == ast::TYPE_STRING) {
                    f.descriptor_index = cf.utf8("Ljava/lang/String;");
                } else if (global->type == ast::TYPE_BYTES) {
                    f.descriptor_index = cf.utf8("[B");
                } else if (dynamic_cast<const ast::TypeArray *>(global->type) != nullptr) {
                    f.descriptor_index = cf.utf8("Lneon/type/Array;");
                } else if (dynamic_cast<const ast::TypeDictionary *>(global->type) != nullptr) {
                    f.descriptor_index = cf.utf8("Ljava/util/HashMap;");
                } else {
                    internal_error("type of global unhandled: " + global->type->text());
                }
                cf.fields.push_back(f);
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
                    ca.max_stack = 8;
                    ca.max_locals = 1;
                    Context context(cf, ca);
                    for (auto s: statements) {
                        s->generate(context);
                    }
                    ca.code << OP_return;
                    Code_attribute::exception e;
                    e.start_pc = 0;
                    e.end_pc = ca.code.size();
                    e.handler_pc = ca.code.size();
                    e.catch_type = cf.Class("neon/type/NeonException");
                    ca.exception_table.push_back(e);
                    ca.code << OP_invokevirtual << cf.Method("java/lang/Object", "toString", "()Ljava/lang/String;");
                    ca.code << OP_getstatic << cf.Field("java/lang/System", "err", "Ljava/io/PrintStream;");
                    ca.code << OP_swap;
                    ca.code << OP_invokevirtual << cf.Method("java/io/PrintStream", "println", "(Ljava/lang/String;)V");
                    ca.code << OP_return;
                    code.info = ca.serialize();
                }
                m.attributes.push_back(code);
            }
            cf.methods.push_back(m);
        }

        std::ofstream f(path + cf.name + ".class", std::ios::binary);
        auto data = cf.serialize();
        f.write(reinterpret_cast<const char *>(data.data()), data.size());
    }
private:
    Program(const Program &);
    Program &operator=(const Program &);
};

class TypeTransformer: public ast::IAstVisitor {
public:
    TypeTransformer(): r(nullptr) {}
    Type *retval() { if (r == nullptr) internal_error("TypeTransformer"); return r; }
    virtual void visit(const ast::TypeNothing *node) { r = new TypeNothing(node); }
    virtual void visit(const ast::TypeDummy *node) { r = new TypeDummy(node); }
    virtual void visit(const ast::TypeBoolean *node) { r = new TypeBoolean(node); }
    virtual void visit(const ast::TypeNumber *node) { r = new TypeNumber(node); }
    virtual void visit(const ast::TypeString *node) { r = new TypeString(node); }
    virtual void visit(const ast::TypeBytes *node) { r = new TypeBytes(node); }
    virtual void visit(const ast::TypeFunction *node) { r = new TypeFunction(node); }
    virtual void visit(const ast::TypeArray *node) { r = new TypeArray(node); }
    virtual void visit(const ast::TypeDictionary *node) { r = new TypeDictionary(node); }
    virtual void visit(const ast::TypeRecord *) {}
    virtual void visit(const ast::TypeClass *) {}
    virtual void visit(const ast::TypePointer *) {}
    virtual void visit(const ast::TypeFunctionPointer *) {}
    virtual void visit(const ast::TypeEnum *) {}
    virtual void visit(const ast::TypeModule *) {}
    virtual void visit(const ast::TypeException *) {}
    virtual void visit(const ast::LoopLabel *) {}
    virtual void visit(const ast::PredefinedVariable *) {}
    virtual void visit(const ast::ModuleVariable *) {}
    virtual void visit(const ast::GlobalVariable *) {}
    virtual void visit(const ast::ExternalGlobalVariable *) {}
    virtual void visit(const ast::LocalVariable *) {}
    virtual void visit(const ast::FunctionParameter *) {}
    virtual void visit(const ast::Exception *) {}
    virtual void visit(const ast::Constant *) {}
    virtual void visit(const ast::ConstantBooleanExpression *) {}
    virtual void visit(const ast::ConstantNumberExpression *) {}
    virtual void visit(const ast::ConstantStringExpression *) {}
    virtual void visit(const ast::ConstantBytesExpression *) {}
    virtual void visit(const ast::ConstantEnumExpression *) {}
    virtual void visit(const ast::ConstantNilExpression *) {}
    virtual void visit(const ast::ConstantNowhereExpression *) {}
    virtual void visit(const ast::ArrayLiteralExpression *) {}
    virtual void visit(const ast::DictionaryLiteralExpression *) {}
    virtual void visit(const ast::RecordLiteralExpression *) {}
    virtual void visit(const ast::NewClassExpression *) {}
    virtual void visit(const ast::UnaryMinusExpression *) {}
    virtual void visit(const ast::LogicalNotExpression *) {}
    virtual void visit(const ast::ConditionalExpression *) {}
    virtual void visit(const ast::TryExpression *) {}
    virtual void visit(const ast::DisjunctionExpression *) {}
    virtual void visit(const ast::ConjunctionExpression *) {}
    virtual void visit(const ast::ArrayInExpression *) {}
    virtual void visit(const ast::DictionaryInExpression *) {}
    virtual void visit(const ast::ChainedComparisonExpression *) {}
    virtual void visit(const ast::BooleanComparisonExpression *) {}
    virtual void visit(const ast::NumericComparisonExpression *) {}
    virtual void visit(const ast::StringComparisonExpression *) {}
    virtual void visit(const ast::ArrayComparisonExpression *) {}
    virtual void visit(const ast::DictionaryComparisonExpression *) {}
    virtual void visit(const ast::PointerComparisonExpression *) {}
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
    virtual void visit(const ast::BytesReferenceIndexExpression *) {}
    virtual void visit(const ast::BytesValueIndexExpression *) {}
    virtual void visit(const ast::ArrayReferenceRangeExpression *) {}
    virtual void visit(const ast::ArrayValueRangeExpression *) {}
    virtual void visit(const ast::PointerDereferenceExpression *) {}
    virtual void visit(const ast::ConstantExpression *) {}
    virtual void visit(const ast::VariableExpression *) {}
    virtual void visit(const ast::FunctionCall *) {}
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *) {}
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
    virtual void visit(const ast::ModuleFunction *) {}
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Type *r;
private:
    TypeTransformer(const TypeTransformer &);
    TypeTransformer &operator=(const TypeTransformer &);
};

class VariableTransformer: public ast::IAstVisitor {
public:
    VariableTransformer(): r(nullptr) {}
    Variable *retval() { if (r == nullptr) internal_error("VariableTransformer"); return r; }
    virtual void visit(const ast::TypeNothing *) {}
    virtual void visit(const ast::TypeDummy *) {}
    virtual void visit(const ast::TypeBoolean *) {}
    virtual void visit(const ast::TypeNumber *) {}
    virtual void visit(const ast::TypeString *) {}
    virtual void visit(const ast::TypeBytes *) {}
    virtual void visit(const ast::TypeFunction *) {}
    virtual void visit(const ast::TypeArray *) {}
    virtual void visit(const ast::TypeDictionary *) {}
    virtual void visit(const ast::TypeRecord *) {}
    virtual void visit(const ast::TypeClass *) {}
    virtual void visit(const ast::TypePointer *) {}
    virtual void visit(const ast::TypeFunctionPointer *) {}
    virtual void visit(const ast::TypeEnum *) {}
    virtual void visit(const ast::TypeModule *) {}
    virtual void visit(const ast::TypeException *) {}
    virtual void visit(const ast::LoopLabel *) {}
    virtual void visit(const ast::PredefinedVariable *) {}
    virtual void visit(const ast::ModuleVariable *) {}
    virtual void visit(const ast::GlobalVariable *node) { r = new GlobalVariable(node); }
    virtual void visit(const ast::ExternalGlobalVariable *) {}
    virtual void visit(const ast::LocalVariable *node) { r = new LocalVariable(node); }
    virtual void visit(const ast::FunctionParameter *) { /*r = new FunctionParameter(node);*/ }
    virtual void visit(const ast::Exception *) {}
    virtual void visit(const ast::Constant *) {}
    virtual void visit(const ast::ConstantBooleanExpression *) {}
    virtual void visit(const ast::ConstantNumberExpression *) {}
    virtual void visit(const ast::ConstantStringExpression *) {}
    virtual void visit(const ast::ConstantBytesExpression *) {}
    virtual void visit(const ast::ConstantEnumExpression *) {}
    virtual void visit(const ast::ConstantNilExpression *) {}
    virtual void visit(const ast::ConstantNowhereExpression *) {}
    virtual void visit(const ast::ArrayLiteralExpression *) {}
    virtual void visit(const ast::DictionaryLiteralExpression *) {}
    virtual void visit(const ast::RecordLiteralExpression *) {}
    virtual void visit(const ast::NewClassExpression *) {}
    virtual void visit(const ast::UnaryMinusExpression *) {}
    virtual void visit(const ast::LogicalNotExpression *) {}
    virtual void visit(const ast::ConditionalExpression *) {}
    virtual void visit(const ast::TryExpression *) {}
    virtual void visit(const ast::DisjunctionExpression *) {}
    virtual void visit(const ast::ConjunctionExpression *) {}
    virtual void visit(const ast::ArrayInExpression *) {}
    virtual void visit(const ast::DictionaryInExpression *) {}
    virtual void visit(const ast::ChainedComparisonExpression *) {}
    virtual void visit(const ast::BooleanComparisonExpression *) {}
    virtual void visit(const ast::NumericComparisonExpression *) {}
    virtual void visit(const ast::StringComparisonExpression *) {}
    virtual void visit(const ast::ArrayComparisonExpression *) {}
    virtual void visit(const ast::DictionaryComparisonExpression *) {}
    virtual void visit(const ast::PointerComparisonExpression *) {}
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
    virtual void visit(const ast::BytesReferenceIndexExpression *) {}
    virtual void visit(const ast::BytesValueIndexExpression *) {}
    virtual void visit(const ast::ArrayReferenceRangeExpression *) {}
    virtual void visit(const ast::ArrayValueRangeExpression *) {}
    virtual void visit(const ast::PointerDereferenceExpression *) {}
    virtual void visit(const ast::ConstantExpression *) {}
    virtual void visit(const ast::VariableExpression *) {}
    virtual void visit(const ast::FunctionCall *) {}
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *) {}
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
    virtual void visit(const ast::ModuleFunction *) {}
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Variable *r;
private:
    VariableTransformer(const VariableTransformer &);
    VariableTransformer &operator=(const VariableTransformer &);
};

class ExpressionTransformer: public ast::IAstVisitor {
public:
    ExpressionTransformer(): r(nullptr) {}
    Expression *retval() { if (r == nullptr) internal_error("ExpressionTransformer"); return r; }
    virtual void visit(const ast::TypeNothing *) {}
    virtual void visit(const ast::TypeDummy *) {}
    virtual void visit(const ast::TypeBoolean *) {}
    virtual void visit(const ast::TypeNumber *) {}
    virtual void visit(const ast::TypeString *) {}
    virtual void visit(const ast::TypeBytes *) {}
    virtual void visit(const ast::TypeFunction *) {}
    virtual void visit(const ast::TypeArray *) {}
    virtual void visit(const ast::TypeDictionary *) {}
    virtual void visit(const ast::TypeRecord *) {}
    virtual void visit(const ast::TypeClass *) {}
    virtual void visit(const ast::TypePointer *) {}
    virtual void visit(const ast::TypeFunctionPointer *) {}
    virtual void visit(const ast::TypeEnum *) {}
    virtual void visit(const ast::TypeModule *) {}
    virtual void visit(const ast::TypeException *) {}
    virtual void visit(const ast::LoopLabel *) {}
    virtual void visit(const ast::PredefinedVariable *) {}
    virtual void visit(const ast::ModuleVariable *) {}
    virtual void visit(const ast::GlobalVariable *) {}
    virtual void visit(const ast::ExternalGlobalVariable *) {}
    virtual void visit(const ast::LocalVariable *) {}
    virtual void visit(const ast::FunctionParameter *) {}
    virtual void visit(const ast::Exception *) {}
    virtual void visit(const ast::Constant *) {}
    virtual void visit(const ast::ConstantBooleanExpression *node) { r = new ConstantBooleanExpression(node); }
    virtual void visit(const ast::ConstantNumberExpression *node) { r = new ConstantNumberExpression(node); }
    virtual void visit(const ast::ConstantStringExpression *node) { r = new ConstantStringExpression(node); }
    virtual void visit(const ast::ConstantBytesExpression *node) { r = new ConstantBytesExpression(node); }
    virtual void visit(const ast::ConstantEnumExpression *) {}
    virtual void visit(const ast::ConstantNilExpression *node) { r = new ConstantNilExpression(node); }
    virtual void visit(const ast::ConstantNowhereExpression *) {}
    virtual void visit(const ast::ArrayLiteralExpression *node) { r = new ArrayLiteralExpression(node); }
    virtual void visit(const ast::DictionaryLiteralExpression *node) { r = new DictionaryLiteralExpression(node); }
    virtual void visit(const ast::RecordLiteralExpression *) {}
    virtual void visit(const ast::NewClassExpression *) {}
    virtual void visit(const ast::UnaryMinusExpression *node) { r = new UnaryMinusExpression(node); }
    virtual void visit(const ast::LogicalNotExpression *node) { r = new LogicalNotExpression(node); }
    virtual void visit(const ast::ConditionalExpression *) {}
    virtual void visit(const ast::TryExpression *) {}
    virtual void visit(const ast::DisjunctionExpression *) {}
    virtual void visit(const ast::ConjunctionExpression *) {}
    virtual void visit(const ast::ArrayInExpression *) {}
    virtual void visit(const ast::DictionaryInExpression *) {}
    virtual void visit(const ast::ChainedComparisonExpression *) {}
    virtual void visit(const ast::BooleanComparisonExpression *) {}
    virtual void visit(const ast::NumericComparisonExpression *node) { r = new NumericComparisonExpression(node); }
    virtual void visit(const ast::StringComparisonExpression *node) { r = new StringComparisonExpression(node); }
    virtual void visit(const ast::ArrayComparisonExpression *) {}
    virtual void visit(const ast::DictionaryComparisonExpression *) {}
    virtual void visit(const ast::PointerComparisonExpression *) {}
    virtual void visit(const ast::FunctionPointerComparisonExpression *) {}
    virtual void visit(const ast::AdditionExpression *node) { r = new AdditionExpression(node); }
    virtual void visit(const ast::SubtractionExpression *node) { r = new SubtractionExpression(node); }
    virtual void visit(const ast::MultiplicationExpression *node) { r = new MultiplicationExpression(node); }
    virtual void visit(const ast::DivisionExpression *node) { r = new DivisionExpression(node); }
    virtual void visit(const ast::ModuloExpression *node) { r = new ModuloExpression(node); }
    virtual void visit(const ast::ExponentiationExpression *node) { r = new ExponentiationExpression(node); }
    virtual void visit(const ast::DummyExpression *node) { r = new DummyExpression(node); }
    virtual void visit(const ast::ArrayReferenceIndexExpression *node) { r = new ArrayReferenceIndexExpression(node); }
    virtual void visit(const ast::ArrayValueIndexExpression *) {}
    virtual void visit(const ast::DictionaryReferenceIndexExpression *) {}
    virtual void visit(const ast::DictionaryValueIndexExpression *) {}
    virtual void visit(const ast::StringReferenceIndexExpression *) {}
    virtual void visit(const ast::StringValueIndexExpression *) {}
    virtual void visit(const ast::BytesReferenceIndexExpression *) {}
    virtual void visit(const ast::BytesValueIndexExpression *) {}
    virtual void visit(const ast::ArrayReferenceRangeExpression *) {}
    virtual void visit(const ast::ArrayValueRangeExpression *) {}
    virtual void visit(const ast::PointerDereferenceExpression *) {}
    virtual void visit(const ast::ConstantExpression *node) { r = transform(node->constant->value); }
    virtual void visit(const ast::VariableExpression *node) { r = new VariableExpression(node); }
    virtual void visit(const ast::FunctionCall *node) { r = new FunctionCall(node); }
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *) {}
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
    virtual void visit(const ast::ModuleFunction *) {}
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Expression *r;
private:
    ExpressionTransformer(const ExpressionTransformer &);
    ExpressionTransformer &operator=(const ExpressionTransformer &);
};

class StatementTransformer: public ast::IAstVisitor {
public:
    StatementTransformer(): r(nullptr) {}
    Statement *retval() { if (r == nullptr) internal_error("StatementTransformer"); return r; }
    virtual void visit(const ast::TypeNothing *) {}
    virtual void visit(const ast::TypeDummy *) {}
    virtual void visit(const ast::TypeBoolean *) {}
    virtual void visit(const ast::TypeNumber *) {}
    virtual void visit(const ast::TypeString *) {}
    virtual void visit(const ast::TypeBytes *) {}
    virtual void visit(const ast::TypeFunction *) {}
    virtual void visit(const ast::TypeArray *) {}
    virtual void visit(const ast::TypeDictionary *) {}
    virtual void visit(const ast::TypeRecord *) {}
    virtual void visit(const ast::TypeClass *) {}
    virtual void visit(const ast::TypePointer *) {}
    virtual void visit(const ast::TypeFunctionPointer *) {}
    virtual void visit(const ast::TypeEnum *) {}
    virtual void visit(const ast::TypeModule *) {}
    virtual void visit(const ast::TypeException *) {}
    virtual void visit(const ast::LoopLabel *) {}
    virtual void visit(const ast::PredefinedVariable *) {}
    virtual void visit(const ast::ModuleVariable *) {}
    virtual void visit(const ast::GlobalVariable *) {}
    virtual void visit(const ast::ExternalGlobalVariable *) {}
    virtual void visit(const ast::LocalVariable *) {}
    virtual void visit(const ast::FunctionParameter *) {}
    virtual void visit(const ast::Exception *) {}
    virtual void visit(const ast::Constant *) {}
    virtual void visit(const ast::ConstantBooleanExpression *) {}
    virtual void visit(const ast::ConstantNumberExpression *) {}
    virtual void visit(const ast::ConstantStringExpression *) {}
    virtual void visit(const ast::ConstantBytesExpression *) {}
    virtual void visit(const ast::ConstantEnumExpression *) {}
    virtual void visit(const ast::ConstantNilExpression *) {}
    virtual void visit(const ast::ConstantNowhereExpression *) {}
    virtual void visit(const ast::ArrayLiteralExpression *) {}
    virtual void visit(const ast::DictionaryLiteralExpression *) {}
    virtual void visit(const ast::RecordLiteralExpression *) {}
    virtual void visit(const ast::NewClassExpression *) {}
    virtual void visit(const ast::UnaryMinusExpression *) {}
    virtual void visit(const ast::LogicalNotExpression *) {}
    virtual void visit(const ast::ConditionalExpression *) {}
    virtual void visit(const ast::TryExpression *) {}
    virtual void visit(const ast::DisjunctionExpression *) {}
    virtual void visit(const ast::ConjunctionExpression *) {}
    virtual void visit(const ast::ArrayInExpression *) {}
    virtual void visit(const ast::DictionaryInExpression *) {}
    virtual void visit(const ast::ChainedComparisonExpression *) {}
    virtual void visit(const ast::BooleanComparisonExpression *) {}
    virtual void visit(const ast::NumericComparisonExpression *) {}
    virtual void visit(const ast::StringComparisonExpression *) {}
    virtual void visit(const ast::ArrayComparisonExpression *) {}
    virtual void visit(const ast::DictionaryComparisonExpression *) {}
    virtual void visit(const ast::PointerComparisonExpression *) {}
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
    virtual void visit(const ast::BytesReferenceIndexExpression *) {}
    virtual void visit(const ast::BytesValueIndexExpression *) {}
    virtual void visit(const ast::ArrayReferenceRangeExpression *) {}
    virtual void visit(const ast::ArrayValueRangeExpression *) {}
    virtual void visit(const ast::PointerDereferenceExpression *) {}
    virtual void visit(const ast::ConstantExpression *) {}
    virtual void visit(const ast::VariableExpression *) {}
    virtual void visit(const ast::FunctionCall *) {}
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *node) { r = new NullStatement(node); }
    virtual void visit(const ast::DeclarationStatement *node) { r = new DeclarationStatement(node); }
    virtual void visit(const ast::ExceptionHandlerStatement *) {}
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
    virtual void visit(const ast::ModuleFunction *) {}
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Statement *r;
private:
    StatementTransformer(const StatementTransformer &);
    StatementTransformer &operator=(const StatementTransformer &);
};

Type *transform(const ast::Type *t)
{
    if (t == nullptr) {
        return nullptr;
    }
    fprintf(stderr, "transform type %s\n", typeid(*t).name());
    auto i = g_type_cache.find(t);
    if (i != g_type_cache.end()) {
        return i->second;
    }
    TypeTransformer tt;
    t->accept(&tt);
    auto r = tt.retval();
    g_type_cache[t] = r;
    return r;
}

Variable *transform(const ast::Variable *v)
{
    if (v == nullptr) {
        return nullptr;
    }
    fprintf(stderr, "transform variable %s\n", typeid(*v).name());
    auto i = g_variable_cache.find(v);
    if (i != g_variable_cache.end()) {
        return i->second;
    }
    VariableTransformer vt;
    v->accept(&vt);
    auto r = vt.retval();
    g_variable_cache[v] = r;
    return r;
}

Expression *transform(const ast::Expression *e)
{
    if (e == nullptr) {
        return nullptr;
    }
    fprintf(stderr, "transform expression %s\n", typeid(*e).name());
    auto i = g_expression_cache.find(e);
    if (i != g_expression_cache.end()) {
        return i->second;
    }
    ExpressionTransformer et;
    e->accept(&et);
    auto r = et.retval();
    g_expression_cache[e] = r;
    return r;
}

Statement *transform(const ast::Statement *s)
{
    if (s == nullptr) {
        return nullptr;
    }
    fprintf(stderr, "transform statement %s\n", typeid(*s).name());
    auto i = g_statement_cache.find(s);
    if (i != g_statement_cache.end()) {
        return i->second;
    }
    StatementTransformer st;
    s->accept(&st);
    auto r = st.retval();
    g_statement_cache[s] = r;
    return r;
}

} // namespace jvm

void compile_jvm(const ast::Program *p)
{
    jvm::Program *ct = new jvm::Program(p);
    ct->generate();
}
