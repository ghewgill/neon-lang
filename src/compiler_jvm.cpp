#include "ast.h"

#include <fstream>

namespace jvm {

const uint16_t ACC_PUBLIC = 0x0001;
const uint16_t ACC_STATIC = 0x0008;
const uint16_t ACC_SUPER  = 0x0020;

const uint8_t CONSTANT_Class              =  7;
const uint8_t CONSTANT_Fieldref           =  9;
const uint8_t CONSTANT_Methodref          = 10;
//const uint8_t CONSTANT_InterfaceMethodref = 11;
const uint8_t CONSTANT_String             =  8;
//const uint8_t CONSTANT_Integer            =  3;
//const uint8_t CONSTANT_Float              =  4;
//const uint8_t CONSTANT_Long               =  5;
//const uint8_t CONSTANT_Double             =  6;
const uint8_t CONSTANT_NameAndType        = 12;
const uint8_t CONSTANT_Utf8               =  1;
//const uint8_t CONSTANT_MethodHandle       = 15;
//const uint8_t CONSTANT_MethodType         = 16;
//const uint8_t CONSTANT_InvokeDynamic      = 18;

const uint8_t OP_ldc           =  18;
const uint8_t OP_return        = 177;
//const uint8_t OP_getstatic     = 178;
//const uint8_t OP_invokevirtual = 182;
const uint8_t OP_invokestatic  = 184;

std::vector<uint8_t> &operator<<(std::vector<uint8_t> &a, uint8_t u8)
{
    a.push_back(u8);
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

struct ClassFile {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    //uint16_t constant_pool_count;
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

    uint16_t utf8(const std::string &s) {
        cp_info cp;
        cp.tag = CONSTANT_Utf8;
        cp.info << static_cast<uint16_t>(s.length());
        for (auto c: s) {
            cp.info << static_cast<uint8_t>(c);
        }
        constant_pool.push_back(cp);
        return constant_pool.size();
    }

    uint16_t Class(const std::string &s) {
        cp_info cp;
        cp.tag = CONSTANT_Class;
        cp.info << utf8(s);
        constant_pool.push_back(cp);
        return constant_pool.size();
    }

    uint16_t String(const std::string &s) {
        cp_info cp;
        cp.tag = CONSTANT_String;
        cp.info << utf8(s);
        constant_pool.push_back(cp);
        return constant_pool.size();
    }

    uint16_t NameAndType(const std::string &name, const std::string &descriptor) {
        cp_info cp;
        cp.tag = CONSTANT_NameAndType;
        cp.info << utf8(name);
        cp.info << utf8(descriptor);
        constant_pool.push_back(cp);
        return constant_pool.size();
    }

    uint16_t Field(const std::string &cls, const std::string &name, const std::string &descriptor) {
        cp_info cp;
        cp.tag = CONSTANT_Fieldref;
        cp.info << Class(cls);
        cp.info << NameAndType(name, descriptor);
        constant_pool.push_back(cp);
        return constant_pool.size();
    }

    uint16_t Method(const std::string &cls, const std::string &name, const std::string &descriptor) {
        cp_info cp;
        cp.tag = CONSTANT_Methodref;
        cp.info << Class(cls);
        cp.info << NameAndType(name, descriptor);
        constant_pool.push_back(cp);
        return constant_pool.size();
    }

    std::vector<uint8_t> serialize() {
        std::vector<uint8_t> r;
        r << magic;
        r << minor_version;
        r << major_version;
        r << static_cast<uint16_t>(constant_pool.size() + 1);
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

class Context {
public:
    Context(ClassFile &cf, Code_attribute &ca): cf(cf), ca(ca) {}
    ClassFile &cf;
    Code_attribute &ca;
};

class Variable {
public:
    virtual void generate_call(Context &context) const = 0;
};

Variable *transform(const ast::Variable *v);

class PredefinedFunction: public Variable {
public:
    PredefinedFunction(const ast::PredefinedFunction *pf): pf(pf) {}
    const ast::PredefinedFunction *pf;

    virtual void generate_call(Context &context) const override {
        if (pf->name == "print") {
            context.ca.code << OP_invokestatic;
            context.ca.code << context.cf.Method("neon/Global", "print", "(Ljava/lang/String;)V");
        } else {
            internal_error("PredefinedFunction");
        }
    }
};

class Expression {
public:
    virtual void generate(Context &context) const = 0;
    virtual void generate_call(Context &context) const = 0;
};

Expression *transform(const ast::Expression *e);

class ConstantStringExpression: public Expression {
public:
    ConstantStringExpression(const ast::ConstantStringExpression *cse): cse(cse) {}
    const ast::ConstantStringExpression *cse;

    virtual void generate(Context &context) const override {
        context.ca.code << OP_ldc;
        context.ca.code << static_cast<uint8_t>(context.cf.String(cse->value));
    }

    virtual void generate_call(Context &) const override { internal_error("ConstantStringExpression"); }
};

class VariableExpression: public Expression {
public:
    VariableExpression(const ast::VariableExpression *ve): ve(ve) {
        var = transform(ve->var);
    }
    const ast::VariableExpression *ve;
    const Variable *var;

    virtual void generate(Context &) const override {
    }

    virtual void generate_call(Context &context) const override {
        var->generate_call(context);
    }
};

class FunctionCall: public Expression {
public:
    FunctionCall(const ast::FunctionCall *fc): fc(fc) {
        func = transform(fc->func);
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
};

class Statement {
public:
    virtual void generate(Context &context) const = 0;
};

Statement *transform(const ast::Statement *s);

class ExpressionStatement: public Statement {
public:
    ExpressionStatement(const ast::ExpressionStatement *es): es(es) {
        expr = transform(es->expr);
    }
    const ast::ExpressionStatement *es;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        expr->generate(context);
    }
};

class Program {
public:
    Program(const ast::Program *program): program(program) {
        for (auto s: program->statements) {
            statements.push_back(transform(s));
        }
    }
    const ast::Program *program;
    std::vector<const Statement *> statements;

    virtual void generate() const {
        ClassFile cf;
        cf.magic = 0xCAFEBABE;
        cf.minor_version = 0;
        cf.major_version = 49;
        cf.access_flags = ACC_PUBLIC | ACC_SUPER;
        cf.this_class = cf.Class("hello");
        cf.super_class = cf.Class("Ljava/lang/Object;");
        {
            method_info m;
            m.access_flags = ACC_PUBLIC | ACC_STATIC;
            m.name_index = cf.utf8("main");
            m.descriptor_index = cf.utf8("([Ljava/lang/String;)V");
            {
                attribute_info a;
                a.attribute_name_index = cf.utf8("Code");
                {
                    Code_attribute ca;
                    ca.max_stack = 5;
                    ca.max_locals = 1;
                    Context context(cf, ca);
                    for (auto s: statements) {
                        s->generate(context);
                    }
                    ca.code << OP_return;
                    a.info = ca.serialize();
                }
                m.attributes.push_back(a);
            }
            cf.methods.push_back(m);
        }
        std::ofstream f("hello.class");
        auto data = cf.serialize();
        f.write(reinterpret_cast<const char *>(data.data()), data.size());
    }
};

#if 0
class TypeTransformer: public ast::IAstVisitor {
public:
    TypeTransformer(): r(nullptr) {}
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
    virtual void visit(const ast::NullStatement *) {}
    virtual void visit(const ast::ExceptionHandlerStatement *) {}
    virtual void visit(const ast::AssertStatement *) {}
    virtual void visit(const ast::AssignmentStatement *) {}
    virtual void visit(const ast::ExpressionStatement *) {}
    virtual void visit(const ast::ReturnStatement *) {}
    virtual void visit(const ast::IncrementStatement *) {}
    virtual void visit(const ast::IfStatement *) {}
    virtual void visit(const ast::LoopStatement *) {}
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
};
#endif

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
    virtual void visit(const ast::ExceptionHandlerStatement *) {}
    virtual void visit(const ast::AssertStatement *) {}
    virtual void visit(const ast::AssignmentStatement *) {}
    virtual void visit(const ast::ExpressionStatement *) {}
    virtual void visit(const ast::ReturnStatement *) {}
    virtual void visit(const ast::IncrementStatement *) {}
    virtual void visit(const ast::IfStatement *) {}
    virtual void visit(const ast::LoopStatement *) {}
    virtual void visit(const ast::CaseStatement *) {}
    virtual void visit(const ast::ExitStatement *) {}
    virtual void visit(const ast::NextStatement *) {}
    virtual void visit(const ast::TryStatement *) {}
    virtual void visit(const ast::RaiseStatement *) {}
    virtual void visit(const ast::ResetStatement *) {}
    virtual void visit(const ast::Function *) {}
    virtual void visit(const ast::PredefinedFunction *node) { r = new PredefinedFunction(node); }
    virtual void visit(const ast::ModuleFunction *) {}
    virtual void visit(const ast::Module *) {}
    virtual void visit(const ast::Program *) {}
private:
    Variable *r;
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
    virtual void visit(const ast::ConstantBooleanExpression *) {}
    virtual void visit(const ast::ConstantNumberExpression *) {}
    virtual void visit(const ast::ConstantStringExpression *node) { r = new ConstantStringExpression(node); }
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
    virtual void visit(const ast::VariableExpression *node) { r = new VariableExpression(node); }
    virtual void visit(const ast::FunctionCall *node) { r = new FunctionCall(node); }
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *) {}
    virtual void visit(const ast::ExceptionHandlerStatement *) {}
    virtual void visit(const ast::AssertStatement *) {}
    virtual void visit(const ast::AssignmentStatement *) {}
    virtual void visit(const ast::ExpressionStatement *) {}
    virtual void visit(const ast::ReturnStatement *) {}
    virtual void visit(const ast::IncrementStatement *) {}
    virtual void visit(const ast::IfStatement *) {}
    virtual void visit(const ast::LoopStatement *) {}
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
    virtual void visit(const ast::NullStatement *) {}
    virtual void visit(const ast::ExceptionHandlerStatement *) {}
    virtual void visit(const ast::AssertStatement *) {}
    virtual void visit(const ast::AssignmentStatement *) {}
    virtual void visit(const ast::ExpressionStatement *node) { r = new ExpressionStatement(node); }
    virtual void visit(const ast::ReturnStatement *) {}
    virtual void visit(const ast::IncrementStatement *) {}
    virtual void visit(const ast::IfStatement *) {}
    virtual void visit(const ast::LoopStatement *) {}
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
    Statement *r;
};

Variable *transform(const ast::Variable *v)
{
    printf("transform %s\n", typeid(*v).name());
    VariableTransformer vt;
    v->accept(&vt);
    return vt.retval();
}

Expression *transform(const ast::Expression *e)
{
    printf("transform %s\n", typeid(*e).name());
    ExpressionTransformer et;
    e->accept(&et);
    return et.retval();
}

Statement *transform(const ast::Statement *s)
{
    printf("transform %s\n", typeid(*s).name());
    StatementTransformer st;
    s->accept(&st);
    return st.retval();
}

} // namespace jvm

void compile_jvm(const ast::Program *p)
{
    jvm::Program *ct = new jvm::Program(p);
    ct->generate();
}
