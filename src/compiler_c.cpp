#include "ast.h"

#include <assert.h>
#include <fstream>
#include <sstream>

#include "support.h"

namespace c {

std::string quoted(const std::string &s)
{
    std::string r = "\"";
    for (auto c: s) {
        switch (c) {
            case '"':  r.append("\\\""); break;
            case '\\': r.append("\\\\"); break;
            default:
                if (c < 0x20) {
                    static const char hex[] = "0123456789abcdef";
                    char buf[10];
                    buf[0] = '\\';
                    buf[1] = 'x';
                    buf[2] = hex[(c >> 4) & 0xf];
                    buf[3] = hex[c & 0xf];
                    buf[4] = 0;
                    r.append(buf);
                } else {
                    r.push_back(c);
                }
                break;
        }
    }
    r.append("\"");
    return r;
}

class Type;
class Variable;
class Expression;
class Statement;

static std::map<const ast::Type *, Type *> g_type_cache;
static std::map<const ast::Variable *, Variable *> g_variable_cache;
static std::map<const ast::Expression *, Expression *> g_expression_cache;
static std::map<const ast::Statement *, Statement *> g_statement_cache;

class Context {
public:
    std::ostream &out;
private:
    int tmp_counter;
public:
    explicit Context(std::ostream &out): out(out), tmp_counter(0) {}
    Context(const Context &) = delete;
    Context &operator=(const Context &) = delete;
    std::string get_temp_name(const Type *type);
};

class Type {
public:
    explicit Type(const ast::Type *t, const std::string &name): name(name) {
        g_type_cache[t] = this;
    }
    Type(const Type &) = delete;
    Type &operator=(const Type &) = delete;
    virtual ~Type() {}
    std::string name;
    virtual void generate_default(Context &context) const = 0;
};

Type *transform(const ast::Type *t);

class Variable {
public:
    explicit Variable(const ast::Variable *v): v(v), type(transform(v->type)) {
        g_variable_cache[v] = this;
    }
    Variable(const Variable &) = delete;
    Variable &operator=(const Variable &) = delete;
    virtual ~Variable() {}
    const ast::Variable *v;
    const Type *type;
    virtual void generate_decl(Context &context) const = 0;
    virtual std::string generate(Context &context) const = 0;
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
    virtual std::string generate(Context &context) const = 0;
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
    explicit TypeNothing(const ast::TypeNothing *tn): Type(tn, "[Nothing]"), tn(tn) {}
    TypeNothing(const TypeNothing &) = delete;
    TypeNothing &operator=(const TypeNothing &) = delete;
    const ast::TypeNothing *tn;
    virtual void generate_default(Context &) const override { internal_error("TypeNothing"); }
};

class TypeDummy: public Type {
public:
    explicit TypeDummy(const ast::TypeDummy *td): Type(td, "[Dummy]"), td(td) {}
    TypeDummy(const TypeDummy &) = delete;
    TypeDummy &operator=(const TypeDummy &) = delete;
    const ast::TypeDummy *td;
    virtual void generate_default(Context &) const override { internal_error("TypeDummy"); }
};

class TypeBoolean: public Type {
public:
    explicit TypeBoolean(const ast::TypeBoolean *tb): Type(tb, "int"), tb(tb) {}
    TypeBoolean(const TypeBoolean &) = delete;
    TypeBoolean &operator=(const TypeBoolean &) = delete;
    const ast::TypeBoolean *tb;
    virtual void generate_default(Context &context) const override {
        context.out << "false";
    }
};

class TypeNumber: public Type {
public:
    explicit TypeNumber(const ast::TypeNumber *tn): Type(tn, "Ne_Number"), tn(tn) {}
    TypeNumber(const TypeNumber &) = delete;
    TypeNumber &operator=(const TypeNumber &) = delete;
    const ast::TypeNumber *tn;
    virtual void generate_default(Context &context) const override {
        context.out << "0";
    }
};

class TypeString: public Type {
public:
    explicit TypeString(const ast::TypeString *ts): Type(ts, "Ne_String"), ts(ts) {}
    TypeString(const TypeString &) = delete;
    TypeString &operator=(const TypeString &) = delete;
    const ast::TypeString *ts;
    virtual void generate_default(Context &context) const override {
        context.out << "\"\"";
    }
};

class TypeBytes: public Type {
public:
    explicit TypeBytes(const ast::TypeBytes *tb): Type(tb, "Ne_Bytes"), tb(tb) {}
    TypeBytes(const TypeBytes &) = delete;
    TypeBytes &operator=(const TypeBytes &) = delete;
    const ast::TypeBytes *tb;
    virtual void generate_default(Context &context) const override {
        context.out << "\"\"";
    }
};

class TypeObject: public Type {
public:
    explicit TypeObject(const ast::TypeObject *to): Type(to, "Ne_Object"), to(to) {}
    TypeObject(const TypeObject &) = delete;
    TypeObject &operator=(const TypeObject &) = delete;
    const ast::TypeObject *to;
    virtual void generate_default(Context &context) const override {
        context.out << "nullptr";
    }
};

class TypeFunction: public Type {
public:
    explicit TypeFunction(const ast::TypeFunction *tf): Type(tf, "[Function]"), tf(tf), returntype(transform(tf->returntype)), paramtypes() {
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
    explicit TypeArray(const ast::TypeArray *ta): Type(ta, "Ne_Array"), ta(ta), elementtype(transform(ta->elementtype)) {}
    TypeArray(const TypeArray &) = delete;
    TypeArray &operator=(const TypeArray &) = delete;
    const ast::TypeArray *ta;
    const Type *elementtype;
    virtual void generate_default(Context &context) const override {
        context.out << "neon::Array()";
    }
};

class TypeDictionary: public Type {
public:
    explicit TypeDictionary(const ast::TypeDictionary *td): Type(td, "Ne_Dictionary"), td(td), elementtype(transform(td->elementtype)) {}
    TypeDictionary(const TypeDictionary &) = delete;
    TypeDictionary &operator=(const TypeDictionary &) = delete;
    const ast::TypeDictionary *td;
    const Type *elementtype;
    virtual void generate_default(Context &context) const override {
        context.out << "neon::Dictionary()";
    }
};

class TypeRecord: public Type {
public:
    explicit TypeRecord(const ast::TypeRecord *tr): Type(tr, "TODO"), tr(tr), field_types() {
        for (auto f: tr->fields) {
            field_types.push_back(transform(f.type));
        }
    }
    TypeRecord(const TypeRecord &) = delete;
    TypeRecord &operator=(const TypeRecord &) = delete;
    const ast::TypeRecord *tr;
    std::vector<const Type *> field_types;

    virtual void generate_default(Context &context) const override {
        context.out << "{";
        bool first = true;
        for (size_t i = 0; i < tr->fields.size(); i++) {
            if (first) {
                first = false;
            } else {
                context.out << ", ";
            }
            context.out << quoted(tr->fields[i].name.text) << ":";
            field_types[i]->generate_default(context);
        }
        context.out << "}";
    }
};

class TypePointer: public Type {
public:
    explicit TypePointer(const ast::TypePointer *tp): Type(tp, "Ne_Pointer"), tp(tp) {}
    TypePointer(const TypePointer &) = delete;
    TypePointer &operator=(const TypePointer &) = delete;
    const ast::TypePointer *tp;
    virtual void generate_default(Context &context) const override {
        context.out << "nullptr";
    }
};

class TypeInterfacePointer: public Type {
public:
    explicit TypeInterfacePointer(const ast::TypeInterfacePointer *tip): Type(tip, "Ne_InterfacePointer"), tip(tip) {}
    TypeInterfacePointer(const TypeInterfacePointer &) = delete;
    TypeInterfacePointer &operator=(const TypeInterfacePointer &) = delete;
    const ast::TypeInterfacePointer *tip;
    virtual void generate_default(Context &context) const override {
        context.out << "nullptr";
    }
};

class TypeFunctionPointer: public Type {
public:
    explicit TypeFunctionPointer(const ast::TypeFunctionPointer *fp): Type(fp, "Ne_FunctionPointer"), fp(fp), functype(dynamic_cast<const TypeFunction *>(transform(fp->functype))) {}
    TypeFunctionPointer(const TypeFunctionPointer &) = delete;
    TypeFunctionPointer &operator=(const TypeFunctionPointer &) = delete;
    const ast::TypeFunctionPointer *fp;
    const TypeFunction *functype;
    virtual void generate_default(Context &context) const override {
        context.out << "nullptr";
    }
};

class TypeEnum: public Type {
public:
    explicit TypeEnum(const ast::TypeEnum *te): Type(te, "Ne_Enum"), te(te) {}
    TypeEnum(const TypeEnum &) = delete;
    TypeEnum &operator=(const TypeEnum &) = delete;
    const ast::TypeEnum *te;

    virtual void generate_default(Context &context) const override {
        context.out << "nullptr";
    }
};

class PredefinedVariable: public Variable {
public:
    explicit PredefinedVariable(const ast::PredefinedVariable *pv): Variable(pv), pv(pv) {}
    PredefinedVariable(const PredefinedVariable &) = delete;
    PredefinedVariable &operator=(const PredefinedVariable &) = delete;
    const ast::PredefinedVariable *pv;

    virtual void generate_decl(Context &) const override { internal_error("PredefinedVariable"); }
    virtual std::string generate(Context &) const override {
        return pv->name;
    }
};

class ModuleVariable: public Variable {
public:
    explicit ModuleVariable(const ast::ModuleVariable *mv): Variable(mv), mv(mv) {}
    ModuleVariable(const ModuleVariable &) = delete;
    ModuleVariable &operator=(const ModuleVariable &) = delete;
    const ast::ModuleVariable *mv;

    virtual void generate_decl(Context &) const override { internal_error("ModuleVariable"); }
    virtual std::string generate(Context &) const override {
        return mv->name;
    }
};

class GlobalVariable: public Variable {
public:
    explicit GlobalVariable(const ast::GlobalVariable *gv): Variable(gv), gv(gv) {}
    GlobalVariable(const GlobalVariable &) = delete;
    GlobalVariable &operator=(const GlobalVariable &) = delete;
    const ast::GlobalVariable *gv;

    virtual void generate_decl(Context &context) const override {
        context.out << type->name << " " << v->name << ";\n";
    }
    virtual std::string generate(Context &) const override {
        return gv->name;
    }
};

class LocalVariable: public Variable {
public:
    explicit LocalVariable(const ast::LocalVariable *lv): Variable(lv), lv(lv) {}
    LocalVariable(const LocalVariable &) = delete;
    LocalVariable &operator=(const LocalVariable &) = delete;
    const ast::LocalVariable *lv;

    virtual void generate_decl(Context &) const override {
        internal_error("LocalVariable");
    }
    virtual std::string generate(Context &) const override {
        return lv->name;
    }
};

class FunctionParameter: public Variable {
public:
    explicit FunctionParameter(const ast::FunctionParameter *fp, int index): Variable(fp), fp(fp), index(index) {}
    FunctionParameter(const FunctionParameter &) = delete;
    FunctionParameter &operator=(const FunctionParameter &) = delete;
    const ast::FunctionParameter *fp;
    const int index;

    virtual void generate_decl(Context &) const override { internal_error("FunctionParameter"); }
    virtual std::string generate(Context &) const override {
        return fp->name;
    }
};

class ConstantBooleanExpression: public Expression {
public:
    explicit ConstantBooleanExpression(const ast::ConstantBooleanExpression *cbe): Expression(cbe), cbe(cbe) {}
    ConstantBooleanExpression(const ConstantBooleanExpression &) = delete;
    ConstantBooleanExpression &operator=(const ConstantBooleanExpression &) = delete;
    const ast::ConstantBooleanExpression *cbe;

    virtual std::string generate(Context &context) const override {
        std::string result = context.get_temp_name(type);
        context.out << result << "=" << (cbe->value ? "1" : "0") << ";\n";
        return result;
    }
};

class ConstantNumberExpression: public Expression {
public:
    explicit ConstantNumberExpression(const ast::ConstantNumberExpression *cne): Expression(cne), cne(cne) {}
    ConstantNumberExpression(const ConstantNumberExpression &) = delete;
    ConstantNumberExpression &operator=(const ConstantNumberExpression &) = delete;
    const ast::ConstantNumberExpression *cne;

    virtual std::string generate(Context &context) const override {
        std::string result = context.get_temp_name(type);
        context.out << "Ne_Number_init_literal(&" << result << "," << number_to_string(cne->value) << ");\n";
        return result;
    }
};

class ConstantStringExpression: public Expression {
public:
    explicit ConstantStringExpression(const ast::ConstantStringExpression *cse): Expression(cse), cse(cse) {}
    ConstantStringExpression(const ConstantStringExpression &) = delete;
    ConstantStringExpression &operator=(const ConstantStringExpression &) = delete;
    const ast::ConstantStringExpression *cse;

    virtual std::string generate(Context &context) const override {
        std::string result = context.get_temp_name(type);
        context.out << "Ne_String_init_literal(&" << result << "," << quoted(cse->value.str()) << ");\n";
        return result;
    }
};

class ConstantBytesExpression: public Expression {
public:
    explicit ConstantBytesExpression(const ast::ConstantBytesExpression *cbe): Expression(cbe), cbe(cbe) {}
    ConstantBytesExpression(const ConstantBytesExpression &) = delete;
    ConstantBytesExpression &operator=(const ConstantBytesExpression &) = delete;
    const ast::ConstantBytesExpression *cbe;

    virtual std::string generate(Context &) const override {
        internal_error("ConstantBytesExpression");
    }
};

class ConstantEnumExpression: public Expression {
public:
    explicit ConstantEnumExpression(const ast::ConstantEnumExpression *cee): Expression(cee), cee(cee), type(dynamic_cast<const TypeEnum *>(transform(cee->type))) {}
    ConstantEnumExpression(const ConstantEnumExpression &) = delete;
    ConstantEnumExpression &operator=(const ConstantEnumExpression &) = delete;
    const ast::ConstantEnumExpression *cee;
    const TypeEnum *type;

    virtual std::string generate(Context &) const override {
        internal_error("ConstantEnumExpression");
    }
};

class ConstantNilExpression: public Expression {
public:
    explicit ConstantNilExpression(const ast::ConstantNilExpression *cne): Expression(cne), cne(cne) {}
    ConstantNilExpression(const ConstantNilExpression &) = delete;
    ConstantNilExpression &operator=(const ConstantNilExpression &) = delete;
    const ast::ConstantNilExpression *cne;

    virtual std::string generate(Context &) const override {
        return "NULL";
    }
};

class ConstantNowhereExpression: public Expression {
public:
    explicit ConstantNowhereExpression(const ast::ConstantNowhereExpression *cne): Expression(cne), cne(cne) {}
    ConstantNowhereExpression(const ConstantNowhereExpression &) = delete;
    ConstantNowhereExpression &operator=(const ConstantNowhereExpression &) = delete;
    const ast::ConstantNowhereExpression *cne;

    virtual std::string generate(Context &) const override {
        return "NULL";
    }
};

class TypeConversionExpression: public Expression {
public:
    explicit TypeConversionExpression(const ast::TypeConversionExpression *tce): Expression(tce), tce(tce), expr(transform(tce->expr)) {}
    TypeConversionExpression(const TypeConversionExpression &) = delete;
    TypeConversionExpression &operator=(const TypeConversionExpression &) = delete;
    const ast::TypeConversionExpression *tce;
    const Expression *expr;

    virtual std::string generate(Context &context) const override {
        return expr->generate(context);
    }
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

    virtual std::string generate(Context &) const override {
        internal_error("ArrayLiteralExpression");
    }
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

    virtual std::string generate(Context &) const override {
        internal_error("DictionaryLiteralExpression");
    }
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

    virtual std::string generate(Context &) const override {
        internal_error("RecordLiteralExpression");
    }
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

    virtual std::string generate(Context &) const override {
        internal_error("NewClassExpression");
    }
};

class UnaryMinusExpression: public Expression {
public:
    explicit UnaryMinusExpression(const ast::UnaryMinusExpression *ume): Expression(ume), ume(ume), value(transform(ume->value)) {}
    UnaryMinusExpression(const UnaryMinusExpression &) = delete;
    UnaryMinusExpression &operator=(const UnaryMinusExpression &) = delete;
    const ast::UnaryMinusExpression *ume;
    const Expression *value;

    virtual std::string generate(Context &context) const override {
        std::string valuename = value->generate(context);
        std::string result = context.get_temp_name(type);
        context.out << result << "=Ne_Number_negate(" << valuename << ");\n";
        return result;
    }
};

class LogicalNotExpression: public Expression {
public:
    explicit LogicalNotExpression(const ast::LogicalNotExpression *lne): Expression(lne), lne(lne), value(transform(lne->value)) {}
    LogicalNotExpression(const LogicalNotExpression &) = delete;
    LogicalNotExpression &operator=(const LogicalNotExpression &) = delete;
    const ast::LogicalNotExpression *lne;
    const Expression *value;

    virtual std::string generate(Context &context) const override {
        std::string valuename = value->generate(context);
        std::string result = context.get_temp_name(type);
        context.out << result << "=!" << valuename << ";\n";
        return result;
    }
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

    virtual std::string generate(Context &context) const override {
        std::string condname = condition->generate(context);
        std::string result = context.get_temp_name(type);
        context.out << "if (" << condname << ") {\n";
        std::string leftname = left->generate(context);
        context.out << result << "=" << leftname << ";\n";
        context.out << "} else {\n";
        std::string rightname = right->generate(context);
        context.out << result << "=" << rightname << ";\n";
        context.out << "}\n";
        return result;
    }
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

    virtual std::string generate(Context &) const override {
        internal_error("TryExpression");
    }
};

class DisjunctionExpression: public Expression {
public:
    explicit DisjunctionExpression(const ast::DisjunctionExpression *de): Expression(de), de(de), left(transform(de->left)), right(transform(de->right)) {}
    DisjunctionExpression(const DisjunctionExpression &) = delete;
    DisjunctionExpression &operator=(const DisjunctionExpression &) = delete;
    const ast::DisjunctionExpression *de;
    const Expression *left;
    const Expression *right;

    virtual std::string generate(Context &context) const override {
        std::string leftname = left->generate(context);
        std::string result = context.get_temp_name(type);
        context.out << "int " << result << "=" << leftname << ";\n";
        context.out << "if (!" << result << ") {\n";
        std::string rightname = right->generate(context);
        context.out << result << "=" << rightname << ";\n";
        context.out << "}\n";
        return result;
    }
};

class ConjunctionExpression: public Expression {
public:
    explicit ConjunctionExpression(const ast::ConjunctionExpression *ce): Expression(ce), ce(ce), left(transform(ce->left)), right(transform(ce->right)) {}
    ConjunctionExpression(const ConjunctionExpression &) = delete;
    ConjunctionExpression &operator=(const ConjunctionExpression &) = delete;
    const ast::ConjunctionExpression *ce;
    const Expression *left;
    const Expression *right;

    virtual std::string generate(Context &context) const override {
        std::string leftname = left->generate(context);
        std::string result = context.get_temp_name(type);
        context.out << "int " << result << "=" << leftname << ";\n";
        context.out << "if (" << result << ") {\n";
        std::string rightname = right->generate(context);
        context.out << result << "=" << rightname << ";\n";
        context.out << "}\n";
        return result;
    }
};

class ArrayInExpression: public Expression {
public:
    explicit ArrayInExpression(const ast::ArrayInExpression *aie): Expression(aie), aie(aie), left(transform(aie->left)), right(transform(aie->right)) {}
    ArrayInExpression(const ArrayInExpression &) = delete;
    ArrayInExpression &operator=(const ArrayInExpression &) = delete;
    const ast::ArrayInExpression *aie;
    const Expression *left;
    const Expression *right;

    virtual std::string generate(Context &) const override {
        internal_error("ArrayInExpression");
    }
};

class DictionaryInExpression: public Expression {
public:
    explicit DictionaryInExpression(const ast::DictionaryInExpression *die): Expression(die), die(die), left(transform(die->left)), right(transform(die->right)) {}
    DictionaryInExpression(const DictionaryInExpression &) = delete;
    DictionaryInExpression &operator=(const DictionaryInExpression &) = delete;
    const ast::DictionaryInExpression *die;
    const Expression *left;
    const Expression *right;

    virtual std::string generate(Context &) const override {
        internal_error("DictionaryInExpression");
    }
};

class ComparisonExpression: public Expression {
public:
    explicit ComparisonExpression(const ast::ComparisonExpression *ce): Expression(ce), ce(ce), left(transform(ce->left)), right(transform(ce->right)) {}
    ComparisonExpression(const ComparisonExpression &) = delete;
    ComparisonExpression &operator=(const ComparisonExpression &) = delete;
    const ast::ComparisonExpression *ce;
    const Expression *left;
    const Expression *right;

    virtual std::string generate(Context &) const override {
        internal_error("ComparisonExpression");
    }
    virtual void generate_comparison(Context &context) const = 0;
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

    virtual std::string generate(Context &) const override {
        internal_error("ChainedComparisonExpression");
    }
};

class BooleanComparisonExpression: public ComparisonExpression {
public:
    explicit BooleanComparisonExpression(const ast::BooleanComparisonExpression *bce): ComparisonExpression(bce), bce(bce) {}
    BooleanComparisonExpression(const BooleanComparisonExpression &) = delete;
    BooleanComparisonExpression &operator=(const BooleanComparisonExpression &) = delete;
    const ast::BooleanComparisonExpression *bce;

    virtual void generate_comparison(Context &context) const override {
        switch (bce->comp) {
            case ast::ComparisonExpression::Comparison::EQ: context.out << " == "; break;
            case ast::ComparisonExpression::Comparison::NE: context.out << " != "; break;
            default:
                internal_error("unexpected comparison type");
        }
    }
};

class NumericComparisonExpression: public ComparisonExpression {
public:
    explicit NumericComparisonExpression(const ast::NumericComparisonExpression *nce): ComparisonExpression(nce), nce(nce) {}
    NumericComparisonExpression(const NumericComparisonExpression &) = delete;
    NumericComparisonExpression &operator=(const NumericComparisonExpression &) = delete;
    const ast::NumericComparisonExpression *nce;

    virtual void generate_comparison(Context &context) const override {
        switch (nce->comp) {
            case ast::ComparisonExpression::Comparison::EQ: context.out << " == "; break;
            case ast::ComparisonExpression::Comparison::NE: context.out << " != "; break;
            case ast::ComparisonExpression::Comparison::LT: context.out << " < "; break;
            case ast::ComparisonExpression::Comparison::GT: context.out << " > "; break;
            case ast::ComparisonExpression::Comparison::LE: context.out << " <= "; break;
            case ast::ComparisonExpression::Comparison::GE: context.out << " >= "; break;
        }
    }
};

class EnumComparisonExpression: public ComparisonExpression {
public:
    explicit EnumComparisonExpression(const ast::EnumComparisonExpression *ece): ComparisonExpression(ece), ece(ece) {}
    EnumComparisonExpression(const EnumComparisonExpression &) = delete;
    EnumComparisonExpression &operator=(const EnumComparisonExpression &) = delete;
    const ast::EnumComparisonExpression *ece;

    virtual void generate_comparison(Context &context) const override {
        switch (ece->comp) {
            case ast::ComparisonExpression::Comparison::EQ: context.out << " == "; break;
            case ast::ComparisonExpression::Comparison::NE: context.out << " != "; break;
            default:
                internal_error("unexpected comparison type");
        }
    }
};

class StringComparisonExpression: public ComparisonExpression {
public:
    explicit StringComparisonExpression(const ast::StringComparisonExpression *sce): ComparisonExpression(sce), sce(sce) {}
    StringComparisonExpression(const StringComparisonExpression &) = delete;
    StringComparisonExpression &operator=(const StringComparisonExpression &) = delete;
    const ast::StringComparisonExpression *sce;

    virtual void generate_comparison(Context &context) const override {
        switch (sce->comp) {
            case ast::ComparisonExpression::Comparison::EQ: context.out << " == "; break;
            case ast::ComparisonExpression::Comparison::NE: context.out << " != "; break;
            case ast::ComparisonExpression::Comparison::LT: context.out << " < "; break;
            case ast::ComparisonExpression::Comparison::GT: context.out << " > "; break;
            case ast::ComparisonExpression::Comparison::LE: context.out << " <= "; break;
            case ast::ComparisonExpression::Comparison::GE: context.out << " >= "; break;
        }
    }
};

class BytesComparisonExpression: public ComparisonExpression {
public:
    explicit BytesComparisonExpression(const ast::BytesComparisonExpression *bce): ComparisonExpression(bce), bce(bce) {}
    BytesComparisonExpression(const BytesComparisonExpression &) = delete;
    BytesComparisonExpression &operator=(const BytesComparisonExpression &) = delete;
    const ast::BytesComparisonExpression *bce;

    virtual void generate_comparison(Context &context) const override {
        switch (bce->comp) {
            case ast::ComparisonExpression::Comparison::EQ: context.out << " == "; break;
            case ast::ComparisonExpression::Comparison::NE: context.out << " != "; break;
            case ast::ComparisonExpression::Comparison::LT: context.out << " < "; break;
            case ast::ComparisonExpression::Comparison::GT: context.out << " > "; break;
            case ast::ComparisonExpression::Comparison::LE: context.out << " <= "; break;
            case ast::ComparisonExpression::Comparison::GE: context.out << " >= "; break;
        }
    }
};

class ArrayComparisonExpression: public ComparisonExpression {
public:
    explicit ArrayComparisonExpression(const ast::ArrayComparisonExpression *ace): ComparisonExpression(ace), ace(ace) {}
    ArrayComparisonExpression(const ArrayComparisonExpression &) = delete;
    ArrayComparisonExpression &operator=(const ArrayComparisonExpression &) = delete;
    const ast::ArrayComparisonExpression *ace;

    virtual void generate_comparison(Context &context) const override {
        context.out << "==";
    }
};

class DictionaryComparisonExpression: public ComparisonExpression {
public:
    explicit DictionaryComparisonExpression(const ast::DictionaryComparisonExpression *dce): ComparisonExpression(dce), dce(dce) {}
    DictionaryComparisonExpression(const DictionaryComparisonExpression &) = delete;
    DictionaryComparisonExpression &operator=(const DictionaryComparisonExpression &) = delete;
    const ast::DictionaryComparisonExpression *dce;

    virtual void generate_comparison(Context &) const override {
        internal_error("DictionaryComparisonExpression");
    }
};

class RecordComparisonExpression: public ComparisonExpression {
public:
    explicit RecordComparisonExpression(const ast::RecordComparisonExpression *rce): ComparisonExpression(rce), rce(rce) {}
    RecordComparisonExpression(const RecordComparisonExpression &) = delete;
    RecordComparisonExpression &operator=(const RecordComparisonExpression &) = delete;
    const ast::RecordComparisonExpression *rce;

    virtual void generate_comparison(Context &) const override {
        internal_error("RecordComparisonExpression");
    }
};

class PointerComparisonExpression: public ComparisonExpression {
public:
    explicit PointerComparisonExpression(const ast::PointerComparisonExpression *pce): ComparisonExpression(pce), pce(pce) {}
    PointerComparisonExpression(const PointerComparisonExpression &) = delete;
    PointerComparisonExpression &operator=(const PointerComparisonExpression &) = delete;
    const ast::PointerComparisonExpression *pce;

    virtual void generate_comparison(Context &context) const override {
        context.out << " == ";
    }
};

class ValidPointerExpression: public PointerComparisonExpression {
public:
    explicit ValidPointerExpression(const ast::ValidPointerExpression *vpe): PointerComparisonExpression(vpe), vpe(vpe), var(transform(vpe->var)) {}
    ValidPointerExpression(const ValidPointerExpression &) = delete;
    ValidPointerExpression &operator=(const ValidPointerExpression &) = delete;
    const ast::ValidPointerExpression *vpe;
    const Variable *var;

    virtual std::string generate(Context &) const override {
        internal_error("ValidPointerExpression");
    }
};

class FunctionPointerComparisonExpression: public ComparisonExpression {
public:
    explicit FunctionPointerComparisonExpression(const ast::FunctionPointerComparisonExpression *fpce): ComparisonExpression(fpce), fpce(fpce) {}
    FunctionPointerComparisonExpression(const FunctionPointerComparisonExpression &) = delete;
    FunctionPointerComparisonExpression &operator=(const FunctionPointerComparisonExpression &) = delete;
    const ast::FunctionPointerComparisonExpression *fpce;

    virtual void generate_comparison(Context &context) const override {
        switch (fpce->comp) {
            case ast::ComparisonExpression::Comparison::EQ: context.out << " == "; break;
            case ast::ComparisonExpression::Comparison::NE: context.out << " != "; break;
            default:
                internal_error("unexpected comparison type");
        }
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

    virtual std::string generate(Context &context) const override {
        std::string leftname = left->generate(context);
        std::string rightname = right->generate(context);
        std::string result = context.get_temp_name(type);
        context.out << "Ne_Number_add(&" << result << ",&" << leftname << ",&" << rightname << ")";
        return result;
    }
};

class SubtractionExpression: public Expression {
public:
    explicit SubtractionExpression(const ast::SubtractionExpression *se): Expression(se), se(se), left(transform(se->left)), right(transform(se->right)) {}
    SubtractionExpression(const SubtractionExpression &) = delete;
    SubtractionExpression &operator=(const SubtractionExpression &) = delete;
    const ast::SubtractionExpression *se;
    const Expression *left;
    const Expression *right;

    virtual std::string generate(Context &context) const override {
        std::string leftname = left->generate(context);
        std::string rightname = right->generate(context);
        std::string result = context.get_temp_name(type);
        context.out << "Ne_Number_subtract(&" << result << ",&" << leftname << ",&" << rightname << ")";
        return result;
    }
};

class MultiplicationExpression: public Expression {
public:
    explicit MultiplicationExpression(const ast::MultiplicationExpression *me): Expression(me), me(me), left(transform(me->left)), right(transform(me->right)) {}
    MultiplicationExpression(const MultiplicationExpression &) = delete;
    MultiplicationExpression &operator=(const MultiplicationExpression &) = delete;
    const ast::MultiplicationExpression *me;
    const Expression *left;
    const Expression *right;

    virtual std::string generate(Context &context) const override {
        std::string leftname = left->generate(context);
        std::string rightname = right->generate(context);
        std::string result = context.get_temp_name(type);
        context.out << "Ne_Number_multiply(&" << result << ",&" << leftname << ",&" << rightname << ")";
        return result;
    }
};

class DivisionExpression: public Expression {
public:
    explicit DivisionExpression(const ast::DivisionExpression *de): Expression(de), de(de), left(transform(de->left)), right(transform(de->right)) {}
    DivisionExpression(const DivisionExpression &) = delete;
    DivisionExpression &operator=(const DivisionExpression &) = delete;
    const ast::DivisionExpression *de;
    const Expression *left;
    const Expression *right;

    virtual std::string generate(Context &context) const override {
        std::string leftname = left->generate(context);
        std::string rightname = right->generate(context);
        std::string result = context.get_temp_name(type);
        context.out << "Ne_Number_divide(&" << result << ",&" << leftname << ",&" << rightname << ")";
        return result;
    }
};

class ModuloExpression: public Expression {
public:
    explicit ModuloExpression(const ast::ModuloExpression *me): Expression(me), me(me), left(transform(me->left)), right(transform(me->right)) {}
    ModuloExpression(const ModuloExpression &) = delete;
    ModuloExpression &operator=(const ModuloExpression &) = delete;
    const ast::ModuloExpression *me;
    const Expression *left;
    const Expression *right;

    virtual std::string generate(Context &context) const override {
        std::string leftname = left->generate(context);
        std::string rightname = right->generate(context);
        std::string result = context.get_temp_name(type);
        context.out << "Ne_Number_mod(&" << result << ",&" << leftname << ",&" << rightname << ")";
        return result;
    }
};

class ExponentiationExpression: public Expression {
public:
    explicit ExponentiationExpression(const ast::ExponentiationExpression *ee): Expression(ee), ee(ee), left(transform(ee->left)), right(transform(ee->right)) {}
    ExponentiationExpression(const ExponentiationExpression &) = delete;
    ExponentiationExpression &operator=(const ExponentiationExpression &) = delete;
    const ast::ExponentiationExpression *ee;
    const Expression *left;
    const Expression *right;

    virtual std::string generate(Context &context) const override {
        std::string leftname = left->generate(context);
        std::string rightname = right->generate(context);
        std::string result = context.get_temp_name(type);
        context.out << "Ne_Number_pow(&" << result << ",&" << leftname << ",&" << rightname << ")";
        return result;
    }
};

class DummyExpression: public Expression {
public:
    explicit DummyExpression(const ast::DummyExpression *de): Expression(de), de(de) {}
    DummyExpression(const DummyExpression &) = delete;
    DummyExpression &operator=(const DummyExpression &) = delete;
    const ast::DummyExpression *de;

    virtual std::string generate(Context &) const override { internal_error("DummyExpression"); }
};

class ArrayReferenceIndexExpression: public Expression {
public:
    explicit ArrayReferenceIndexExpression(const ast::ArrayReferenceIndexExpression *arie): Expression(arie), arie(arie), array(transform(arie->array)), index(transform(arie->index)) {}
    ArrayReferenceIndexExpression(const ArrayReferenceIndexExpression &) = delete;
    ArrayReferenceIndexExpression &operator=(const ArrayReferenceIndexExpression &) = delete;
    const ast::ArrayReferenceIndexExpression *arie;
    const Expression *array;
    const Expression *index;

    virtual std::string generate(Context &context) const override {
        std::string indexname = index->generate(context);
        return array->generate(context) + "[" + indexname + "]";
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

    virtual std::string generate(Context &context) const override {
        std::string indexname = index->generate(context);
        return array->generate(context) + "[" + indexname + "]";
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

    virtual std::string generate(Context &context) const override {
        std::string indexname = index->generate(context);
        return dictionary->generate(context) + "[" + indexname + "]";
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

    virtual std::string generate(Context &) const override {
        internal_error("DictionaryValueIndexExpression");
    }
};

class StringReferenceIndexExpression: public Expression {
public:
    explicit StringReferenceIndexExpression(const ast::StringReferenceIndexExpression *srie): Expression(srie), srie(srie), ref(transform(srie->ref)), index(transform(srie->index)) {}
    StringReferenceIndexExpression(const StringReferenceIndexExpression &) = delete;
    StringReferenceIndexExpression &operator=(const StringReferenceIndexExpression &) = delete;
    const ast::StringReferenceIndexExpression *srie;
    const Expression *ref;
    const Expression *index;

    virtual std::string generate(Context &) const override {
        internal_error("StringReferenceIndexExpression");
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

    virtual std::string generate(Context &) const override {
        internal_error("StringValueIndexExpression");
    }
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

    virtual std::string generate(Context &) const override {
        internal_error("StringReferenceRangeIndexExpression");
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

    virtual std::string generate(Context &) const override {
        internal_error("StringValueRangeIndexExpression");
    }
};

class BytesReferenceIndexExpression: public Expression {
public:
    explicit BytesReferenceIndexExpression(const ast::BytesReferenceIndexExpression *brie): Expression(brie), brie(brie), ref(transform(brie->ref)), index(transform(brie->index)) {}
    BytesReferenceIndexExpression(const BytesReferenceIndexExpression &) = delete;
    BytesReferenceIndexExpression &operator=(const BytesReferenceIndexExpression &) = delete;
    const ast::BytesReferenceIndexExpression *brie;
    const Expression *ref;
    const Expression *index;

    virtual std::string generate(Context &) const override {
        internal_error("BytesReferenceIndexExpression");
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

    virtual std::string generate(Context &) const override {
        internal_error("BytesValueIndexExpression");
    }
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

    virtual std::string generate(Context &) const override {
        internal_error("BytesReferenceRangeIndexExpression");
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

    virtual std::string generate(Context &) const override {
        internal_error("BytesValueRangeIndexExpression");
    }
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

    virtual std::string generate(Context &context) const override {
        return ref->generate(context) + "." + rrfe->field;
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

    virtual std::string generate(Context &context) const override {
        return rec->generate(context) + "." + rvfe->field;
    }
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

    virtual std::string generate(Context &) const override {
        internal_error("ArrayReferenceRangeExpression");
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

    virtual std::string generate(Context &) const override {
        internal_error("ArrayValueRangeExpression");
    }
};

class PointerDereferenceExpression: public Expression {
public:
    explicit PointerDereferenceExpression(const ast::PointerDereferenceExpression *pde): Expression(pde), pde(pde), ptr(transform(pde->ptr)) {}
    PointerDereferenceExpression(const PointerDereferenceExpression &) = delete;
    PointerDereferenceExpression &operator=(const PointerDereferenceExpression &) = delete;
    const ast::PointerDereferenceExpression *pde;
    const Expression *ptr;

    virtual std::string generate(Context &context) const override {
        return ptr->generate(context);
    }
};

class VariableExpression: public Expression {
public:
    explicit VariableExpression(const ast::VariableExpression *ve): Expression(ve), ve(ve), var(transform(ve->var)) {}
    VariableExpression(const VariableExpression &) = delete;
    VariableExpression &operator=(const VariableExpression &) = delete;
    const ast::VariableExpression *ve;
    const Variable *var;

    virtual std::string generate(Context &context) const override {
        return var->generate(context);
    }
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

    virtual std::string generate(Context &context) const override {
        std::string result;
        std::string call = func->generate(context) + "(";
        if (dynamic_cast<const ast::TypeFunction *>(fc->func->type)->returntype != ast::TYPE_NOTHING) {
            std::string arglist;
            for (auto a: args) {
                arglist.append(",&");
                arglist.append(a->generate(context));
            }
            result = context.get_temp_name(type);
            call.append("&" + result + arglist + ")");
        } else {
            bool first = true;
            for (auto a: args) {
                if (first) {
                    first = false;
                } else {
                    call.append(",");
                }
                call.append("&");
                call.append(a->generate(context));
            }
            call.append(")");
        }
        context.out << call << ";\n";
        return result;
    }
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
        decl->generate_decl(context);
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
        context.out << "if (!(";
        expr->generate(context);
        context.out << ")) {";
        for (auto s: statements) {
            s->generate(context);
        }
        context.out << "}";
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
        std::string exprname = expr->generate(context);
        for (auto v: variables) {
            if (dynamic_cast<const DummyExpression *>(v) == nullptr) {
                std::string vname = v->generate(context);
                context.out << v->type->name << "_assign(&" << vname << ",&" << exprname << ");\n";
            }
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
        context.out << ";\n";
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
        printf("here\n");
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

    virtual void generate(Context &) const override {
        internal_error("BaseLoopStatement");
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
        virtual void generate(Context &context, const Expression *value) const = 0;
    };
    class ComparisonWhenCondition: public WhenCondition {
    public:
        ComparisonWhenCondition(ast::ComparisonExpression::Comparison comp, const Expression *expr): comp(comp), expr(expr) {}
        ComparisonWhenCondition(const ComparisonWhenCondition &) = delete;
        ComparisonWhenCondition &operator=(const ComparisonWhenCondition &) = delete;
        ast::ComparisonExpression::Comparison comp;
        const Expression *expr;
        virtual void generate(Context &context, const Expression *value) const override {
            value->generate(context);
            switch (comp) {
                case ast::ComparisonExpression::Comparison::EQ: context.out << " == "; break;
                case ast::ComparisonExpression::Comparison::NE: context.out << " != "; break;
                case ast::ComparisonExpression::Comparison::LT: context.out << " < "; break;
                case ast::ComparisonExpression::Comparison::GT: context.out << " > "; break;
                case ast::ComparisonExpression::Comparison::LE: context.out << " <= "; break;
                case ast::ComparisonExpression::Comparison::GE: context.out << " >= "; break;
            }
            expr->generate(context);
        }
    };
    class RangeWhenCondition: public WhenCondition {
    public:
        RangeWhenCondition(const Expression *low_expr, const Expression *high_expr): low_expr(low_expr), high_expr(high_expr) {}
        RangeWhenCondition(const RangeWhenCondition &) = delete;
        RangeWhenCondition &operator=(const RangeWhenCondition &) = delete;
        const Expression *low_expr;
        const Expression *high_expr;
        virtual void generate(Context &context, const Expression *value) const override {
            value->generate(context);
            context.out << " >= ";
            low_expr->generate(context);
            context.out << " && ";
            value->generate(context);
            context.out << " <= ";
            high_expr->generate(context);
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
        bool first_clause = true;
        for (auto c: clauses) {
            if (first_clause) {
                first_clause = false;
            } else {
                context.out << " else ";
            }
            if (not c.first.empty()) {
                context.out << "if (";
                bool first_condition = true;
                for (auto w: c.first) {
                    if (first_condition) {
                        first_condition = false;
                    } else {
                        context.out << " || ";
                    }
                    w->generate(context, expr);
                }
                context.out << ")";
            }
            context.out << "{";
            for (auto s: c.second) {
                s->generate(context);
            }
            context.out << "}";
        }
    }
};

class ExitStatement: public Statement {
public:
    explicit ExitStatement(const ast::ExitStatement *es): Statement(es), es(es) {}
    ExitStatement(const ExitStatement &) = delete;
    ExitStatement &operator=(const ExitStatement &) = delete;
    const ast::ExitStatement *es;

    void generate(Context &context) const override {
        context.out << "break;\n"; // TODO: use label to get correct loop
    }
};

class NextStatement: public Statement {
public:
    explicit NextStatement(const ast::NextStatement *ns): Statement(ns), ns(ns) {}
    NextStatement(const NextStatement &) = delete;
    NextStatement &operator=(const NextStatement &) = delete;
    const ast::NextStatement *ns;

    virtual void generate(Context &context) const override {
        context.out << "continue;\n"; // TODO: use label to get correct loop
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
        context.out << "try {";
        for (auto s: statements) {
            s->generate(context);
        }
        context.out << "}";
        for (auto c: catches) {
            if (c->name != nullptr) {
                context.out << " catch (";
                c->name->generate(context);
                context.out << ") {";
            } else {
                context.out << " catch (__x) {";
            }
            for (auto s: c->handler) {
                s->generate(context);
            }
            context.out << "}";
        }
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
        context.out << "return";
        if (expr != nullptr) {
            context.out << " ";
            expr->generate(context);
        }
        context.out << ";\n";
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
        context.out << "+=" << is->delta << ";\n";
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

    void generate(Context &context) const override {
        bool first = true;
        for (auto c: condition_statements) {
            if (first) {
                first = false;
            } else {
                context.out << "else ";
            }
            context.out << "if (";
            c.first->generate(context);
            context.out << ") {";
            for (auto s: c.second) {
                s->generate(context);
            }
            context.out << "}";
        }
        if (else_statements.size() > 0) {
            context.out << " else {";
            for (auto s: else_statements) {
                s->generate(context);
            }
            context.out << "}";
        }
    }
};

class RaiseStatement: public Statement {
public:
    explicit RaiseStatement(const ast::RaiseStatement *rs): Statement(rs), rs(rs), info(transform(rs->info)) {}
    RaiseStatement(const RaiseStatement &) = delete;
    RaiseStatement &operator=(const RaiseStatement &) = delete;
    const ast::RaiseStatement *rs;
    const Expression *info;

    virtual void generate(Context &context) const override {
        context.out << "throw neon::NeonException(" << quoted(rs->exception->name) << ", ";
        info->generate(context);
        context.out << ");\n";
    }
};

class ResetStatement: public Statement {
public:
    explicit ResetStatement(const ast::ResetStatement *rs): Statement(rs), rs(rs) {}
    ResetStatement(const ResetStatement &) = delete;
    ResetStatement &operator=(const ResetStatement &) = delete;
    const ast::ResetStatement *rs;

    virtual void generate(Context &) const override {}
};

class Function: public Variable {
public:
    explicit Function(const ast::Function *f): Variable(f), f(f), statements(), params(), out_count(0) {
        // Need to transform the function parameters before transforming
        // the code that might use them (statements).
        int i = 0;
        for (auto p: f->params) {
            FunctionParameter *q = new FunctionParameter(p, i);
            params.push_back(q);
            g_variable_cache[p] = q;
            if (q->fp->mode == ast::ParameterType::Mode::INOUT || q->fp->mode == ast::ParameterType::Mode::OUT) {
                out_count++;
            }
            i++;
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
    int out_count;

    virtual void generate_decl(Context &context) const override {
        context.out << "void " << f->name << "(";
        bool first = true;
        for (auto p: params) {
            if (first) {
                first = false;
            } else {
                context.out << ", ";
            }
            context.out << p->fp->name;
        }
        context.out << ") {";
        for (auto s: statements) {
            s->generate(context);
        }
        context.out << "}";
    }
    virtual std::string generate(Context &) const override {
        return f->name;
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

    virtual void generate_decl(Context &) const override { internal_error("PredefinedFunction"); }
    virtual std::string generate(Context &) const override {
        auto dollar = pf->name.find('$');
        if (dollar != std::string::npos) {
            return "neon::" + pf->name.substr(0, dollar) + "." + pf->name.substr(dollar+1);
        } else {
            return "Ne_" + pf->name;
        }
    }
};

class ModuleFunction: public Variable {
public:
    explicit ModuleFunction(const ast::ModuleFunction *mf): Variable(mf), mf(mf), out_count(0) {
        const TypeFunction *functype = dynamic_cast<const TypeFunction *>(transform(mf->type));
        int i = 0;
        for (auto p: functype->paramtypes) {
            if (p.first == ast::ParameterType::Mode::INOUT || p.first == ast::ParameterType::Mode::OUT) {
                out_count++;
            }
            i++;
        }
    }
    ModuleFunction(const ModuleFunction &) = delete;
    ModuleFunction &operator=(const ModuleFunction &) = delete;
    const ast::ModuleFunction *mf;
    int out_count;

    virtual void generate_decl(Context &) const override { internal_error("ModuleFunction"); }
    virtual std::string generate(Context &) const override {
        return mf->name;
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
        std::stringstream out;
        out << "#include \"neon.h\"\n";
        Context context(out);
        for (size_t s = 0; s < program->frame->getCount(); s++) {
            auto slot = program->frame->getSlot(s);
            const GlobalVariable *global = dynamic_cast<const GlobalVariable *>(g_variable_cache[dynamic_cast<const ast::GlobalVariable *>(slot.ref)]);
            if (global != nullptr) {
                global->generate_decl(context);
            }
        }
        context.out << "int main(int argc, const char *argv[]) {\n";
        for (auto s: statements) {
            s->generate(context);
        }
        context.out << "return 0;\n}\n";
        std::vector<unsigned char> src;
        std::string t = out.str();
        std::copy(t.begin(), t.end(), std::back_inserter(src));
        support->writeOutput(path + program->module_name + ".c", src);
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
    virtual void visit(const ast::ConstantNilExpression *node) { r = new ConstantNilExpression(node); }
    virtual void visit(const ast::ConstantNowhereExpression *node) { r = new ConstantNowhereExpression(node); }
    virtual void visit(const ast::ConstantNilObject *) { internal_error("ConstantNilObject"); }
    virtual void visit(const ast::TypeConversionExpression * node) { r = new TypeConversionExpression(node); }
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
    virtual void visit(const ast::TypeTestExpression *) {}
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
    virtual void visit(const ast::ObjectSubscriptExpression *) { /* TODO */ }
    virtual void visit(const ast::RecordReferenceFieldExpression *node) { r = new RecordReferenceFieldExpression(node); }
    virtual void visit(const ast::RecordValueFieldExpression *node) { r = new RecordValueFieldExpression(node); }
    virtual void visit(const ast::ArrayReferenceRangeExpression *node) { r = new ArrayReferenceRangeExpression(node); }
    virtual void visit(const ast::ArrayValueRangeExpression *node) { r = new ArrayValueRangeExpression(node); }
    virtual void visit(const ast::PointerDereferenceExpression *node) { r =  new PointerDereferenceExpression(node); }
    virtual void visit(const ast::ConstantExpression *node) { r = transform(node->constant->value); }
    virtual void visit(const ast::VariableExpression *node) { r = new VariableExpression(node); }
    virtual void visit(const ast::InterfaceMethodExpression *) { internal_error("InterfaceMethodExpression"); }
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

std::string Context::get_temp_name(const Type *type) {
    ++tmp_counter;
    std::string name = "tmp" + std::to_string(tmp_counter);
    out << type->name << " " << name << ";\n";
    return name;
}

} // namespace c

void compile_c(CompilerSupport *support, const ast::Program *p)
{
    c::g_type_cache.clear();
    c::g_variable_cache.clear();
    c::g_expression_cache.clear();
    c::g_statement_cache.clear();

    c::Program *ct = new c::Program(support, p);
    ct->generate();
}
