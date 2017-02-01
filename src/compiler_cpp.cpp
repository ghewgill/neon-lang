#include "ast.h"

#include <assert.h>
#include <fstream>
#include <sstream>

#include "support.h"

namespace cpp {

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

class Context {
public:
    Context(std::ostream &out): out(out) {}
    std::ostream &out;
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
    Type(const ast::Type *t) {
        g_type_cache[t] = this;
    }
    virtual ~Type() {}
    virtual void generate_default(Context &context) const = 0;
private:
    Type(const Type &);
    Type &operator=(const Type &);
};

Type *transform(const ast::Type *t);

class Variable {
public:
    Variable(const ast::Variable *v): type(transform(v->type)) {
        g_variable_cache[v] = this;
    }
    virtual ~Variable() {}
    const Type *type;
    virtual void generate_decl(Context &context) const = 0;
    virtual void generate(Context &context) const = 0;
private:
    Variable(const Variable &);
    Variable &operator=(const Variable &);
};

Variable *transform(const ast::Variable *v);

class Expression {
public:
    Expression(const ast::Expression *node): type(transform(node->type)) {
        g_expression_cache[node] = this;
    }
    virtual ~Expression() {}
    const Type *type;
    virtual void generate(Context &context) const = 0;
private:
    Expression(const Expression &);
    Expression &operator=(const Expression &);
};

Expression *transform(const ast::Expression *e);

class Statement {
public:
    Statement(const ast::Statement *s) {
        g_statement_cache[s] = this;
    }
    virtual ~Statement() {}
    virtual void generate(Context &context) const = 0;
};

Statement *transform(const ast::Statement *s);

class TypeNothing: public Type {
public:
    TypeNothing(const ast::TypeNothing *tn): Type(tn), tn(tn) {}
    const ast::TypeNothing *tn;
    virtual void generate_default(Context &) const override { internal_error("TypeNothing"); }
private:
    TypeNothing(const TypeNothing &);
    TypeNothing &operator=(const TypeNothing &);
};

class TypeDummy: public Type {
public:
    TypeDummy(const ast::TypeDummy *td): Type(td), td(td) {}
    const ast::TypeDummy *td;
    virtual void generate_default(Context &) const override { internal_error("TypeDummy"); }
private:
    TypeDummy(const TypeDummy &);
    TypeDummy &operator=(const TypeDummy &);
};

class TypeBoolean: public Type {
public:
    TypeBoolean(const ast::TypeBoolean *tb): Type(tb), tb(tb) {}
    const ast::TypeBoolean *tb;
    virtual void generate_default(Context &context) const override {
        context.out << "false";
    }
private:
    TypeBoolean(const TypeBoolean &);
    TypeBoolean &operator=(const TypeBoolean &);
};

class TypeNumber: public Type {
public:
    TypeNumber(const ast::TypeNumber *tn): Type(tn), tn(tn) {}
    const ast::TypeNumber *tn;
    virtual void generate_default(Context &context) const override {
        context.out << "0";
    }
private:
    TypeNumber(const TypeNumber &);
    TypeNumber &operator=(const TypeNumber &);
};

class TypeString: public Type {
public:
    TypeString(const ast::TypeString *ts): Type(ts), ts(ts) {}
    const ast::TypeString *ts;
    virtual void generate_default(Context &context) const override {
        context.out << "\"\"";
    }
private:
    TypeString(const TypeString &);
    TypeString &operator=(const TypeString &);
};

class TypeBytes: public Type {
public:
    TypeBytes(const ast::TypeBytes *tb): Type(tb), tb(tb) {}
    const ast::TypeBytes *tb;
    virtual void generate_default(Context &context) const override {
        context.out << "\"\"";
    }
private:
    TypeBytes(const TypeBytes &);
    TypeBytes &operator=(const TypeBytes &);
};

class TypeFunction: public Type {
public:
    TypeFunction(const ast::TypeFunction *tf): Type(tf), tf(tf), returntype(transform(tf->returntype)), paramtypes() {
        for (auto p: tf->params) {
            paramtypes.push_back(std::make_pair(p->mode, transform(p->type)));
        }
    }
    const ast::TypeFunction *tf;
    const Type *returntype;
    std::vector<std::pair<ast::ParameterType::Mode, const Type *>> paramtypes;
    virtual void generate_default(Context &) const override { internal_error("TypeFunction"); }
private:
    TypeFunction(const TypeFunction &);
    TypeFunction &operator=(const TypeFunction &);
};

class TypeArray: public Type {
public:
    TypeArray(const ast::TypeArray *ta): Type(ta), ta(ta), elementtype(transform(ta->elementtype)) {}
    const ast::TypeArray *ta;
    const Type *elementtype;
    virtual void generate_default(Context &context) const override {
        context.out << "neon::Array()";
    }
private:
    TypeArray(const TypeArray &);
    TypeArray &operator=(const TypeArray &);
};

class TypeDictionary: public Type {
public:
    TypeDictionary(const ast::TypeDictionary *td): Type(td), td(td), elementtype(transform(td->elementtype)) {}
    const ast::TypeDictionary *td;
    const Type *elementtype;
    virtual void generate_default(Context &context) const override {
        context.out << "neon::Dictionary()";
    }
private:
    TypeDictionary(const TypeDictionary &);
    TypeDictionary &operator=(const TypeDictionary &);
};

class TypeRecord: public Type {
public:
    TypeRecord(const ast::TypeRecord *tr): Type(tr), tr(tr), field_types() {
        for (auto f: tr->fields) {
            field_types.push_back(transform(f.type));
        }
    }
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
private:
    TypeRecord(const TypeRecord &);
    TypeRecord &operator=(const TypeRecord &);
};

class TypePointer: public Type {
public:
    TypePointer(const ast::TypePointer *tp): Type(tp), tp(tp) {}
    const ast::TypePointer *tp;
    virtual void generate_default(Context &context) const override {
        context.out << "nullptr";
    }
private:
    TypePointer(const TypePointer &);
    TypePointer &operator=(const TypePointer &);
};

class TypeFunctionPointer: public Type {
public:
    TypeFunctionPointer(const ast::TypeFunctionPointer *fp): Type(fp), fp(fp), functype(dynamic_cast<const TypeFunction *>(transform(fp->functype))) {}
    const ast::TypeFunctionPointer *fp;
    const TypeFunction *functype;
    virtual void generate_default(Context &context) const override {
        context.out << "nullptr";
    }
private:
    TypeFunctionPointer(const TypeFunctionPointer &);
    TypeFunctionPointer &operator=(const TypeFunctionPointer &);
};

class TypeEnum: public Type {
public:
    TypeEnum(const ast::TypeEnum *te): Type(te), te(te) {}
    const ast::TypeEnum *te;

    virtual void generate_default(Context &context) const override {
        context.out << "nullptr";
    }
private:
    TypeEnum(const TypeEnum &);
    TypeEnum &operator=(const TypeEnum &);
};

class PredefinedVariable: public Variable {
public:
    PredefinedVariable(const ast::PredefinedVariable *pv): Variable(pv), pv(pv) {}
    const ast::PredefinedVariable *pv;

    virtual void generate_decl(Context &) const override { internal_error("PredefinedVariable"); }
    virtual void generate(Context &context) const override {
        context.out << pv->name;
    }
private:
    PredefinedVariable(const PredefinedVariable &);
    PredefinedVariable &operator=(const PredefinedVariable &);
};

class ModuleVariable: public Variable {
public:
    ModuleVariable(const ast::ModuleVariable *mv): Variable(mv), mv(mv) {}
    const ast::ModuleVariable *mv;

    virtual void generate_decl(Context &) const override { internal_error("ModuleVariable"); }
    virtual void generate(Context &context) const override {
        context.out << mv->name;
    }
private:
    ModuleVariable(const ModuleVariable &);
    ModuleVariable &operator=(const ModuleVariable &);
};

class GlobalVariable: public Variable {
public:
    GlobalVariable(const ast::GlobalVariable *gv): Variable(gv), gv(gv) {}
    const ast::GlobalVariable *gv;

    virtual void generate_decl(Context &) const override {
        internal_error("GlobalVariable");
    }
    virtual void generate(Context &context) const override {
        context.out << gv->name;
    }
private:
    GlobalVariable(const GlobalVariable &);
    GlobalVariable &operator=(const GlobalVariable &);
};

class LocalVariable: public Variable {
public:
    LocalVariable(const ast::LocalVariable *lv): Variable(lv), lv(lv) {}
    const ast::LocalVariable *lv;

    virtual void generate_decl(Context &) const override {
        internal_error("LocalVariable");
    }
    virtual void generate(Context &context) const override {
        context.out << lv->name;
    }
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
    virtual void generate(Context &context) const override {
        context.out << fp->name;
    }
private:
    FunctionParameter(const FunctionParameter &);
    FunctionParameter &operator=(const FunctionParameter &);
};

class ConstantBooleanExpression: public Expression {
public:
    ConstantBooleanExpression(const ast::ConstantBooleanExpression *cbe): Expression(cbe), cbe(cbe) {}
    const ast::ConstantBooleanExpression *cbe;

    virtual void generate(Context &context) const override {
        context.out << (cbe->value ? "true" : "false");
    }
private:
    ConstantBooleanExpression(const ConstantBooleanExpression &);
    ConstantBooleanExpression &operator=(const ConstantBooleanExpression &);
};

class ConstantNumberExpression: public Expression {
public:
    ConstantNumberExpression(const ast::ConstantNumberExpression *cne): Expression(cne), cne(cne) {}
    const ast::ConstantNumberExpression *cne;

    virtual void generate(Context &context) const override {
        context.out << number_to_string(cne->value);
    }
private:
    ConstantNumberExpression(const ConstantNumberExpression &);
    ConstantNumberExpression &operator=(const ConstantNumberExpression &);
};

class ConstantStringExpression: public Expression {
public:
    ConstantStringExpression(const ast::ConstantStringExpression *cse): Expression(cse), cse(cse) {}
    const ast::ConstantStringExpression *cse;

    virtual void generate(Context &context) const override {
        context.out << quoted(cse->value);
    }
private:
    ConstantStringExpression(const ConstantStringExpression &);
    ConstantStringExpression &operator=(const ConstantStringExpression &);
};

class ConstantBytesExpression: public Expression {
public:
    ConstantBytesExpression(const ast::ConstantBytesExpression *cbe): Expression(cbe), cbe(cbe) {}
    const ast::ConstantBytesExpression *cbe;

    virtual void generate(Context &context) const override {
        context.out << "TODO";
    }
private:
    ConstantBytesExpression(const ConstantBytesExpression &);
    ConstantBytesExpression &operator=(const ConstantBytesExpression &);
};

class ConstantEnumExpression: public Expression {
public:
    ConstantEnumExpression(const ast::ConstantEnumExpression *cee): Expression(cee), cee(cee), type(dynamic_cast<const TypeEnum *>(transform(cee->type))) {}
    const ast::ConstantEnumExpression *cee;
    const TypeEnum *type;

    virtual void generate(Context &context) const override {
        context.out << "nullptr"; // TODO
    }
private:
    ConstantEnumExpression(const ConstantEnumExpression &);
    ConstantEnumExpression &operator=(const ConstantEnumExpression &);
};

class ConstantNilExpression: public Expression {
public:
    ConstantNilExpression(const ast::ConstantNilExpression *cne): Expression(cne), cne(cne) {}
    const ast::ConstantNilExpression *cne;

    virtual void generate(Context &context) const override {
        context.out << "nullptr";
    }
private:
    ConstantNilExpression(const ConstantNilExpression &);
    ConstantNilExpression &operator=(const ConstantNilExpression &);
};

class ConstantNowhereExpression: public Expression {
public:
    ConstantNowhereExpression(const ast::ConstantNowhereExpression *cne): Expression(cne), cne(cne) {}
    const ast::ConstantNowhereExpression *cne;

    virtual void generate(Context &context) const override {
        context.out << "nullptr";
    }
private:
    ConstantNowhereExpression(const ConstantNowhereExpression &);
    ConstantNowhereExpression &operator=(const ConstantNowhereExpression &);
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
        context.out << "{";
        for (auto e: elements) {
            e->generate(context);
            context.out << ","; // TODO: omit last comma
        }
        context.out << "}";
    }
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
        context.out << "{";
        bool first = true;
        for (auto d: dict) {
            if (first) {
                first = false;
            } else {
                context.out << ", ";
            }
            context.out << quoted(d.first);
            context.out << ':';
            d.second->generate(context);
        }
        context.out << "}";
    }
private:
    DictionaryLiteralExpression(const DictionaryLiteralExpression &);
    DictionaryLiteralExpression &operator=(const DictionaryLiteralExpression &);
};

class RecordLiteralExpression: public Expression {
public:
    RecordLiteralExpression(const ast::RecordLiteralExpression *rle): Expression(rle), rle(rle), type(dynamic_cast<TypeRecord *>(transform(rle->type))), values() {
        for (auto v: rle->values) {
            values.push_back(transform(v));
        }
    }
    const ast::RecordLiteralExpression *rle;
    const TypeRecord *type;
    std::vector<const Expression *> values;

    virtual void generate(Context &context) const override {
        context.out << "{";
        int i = 0;
        bool first = true;
        for (auto v: values) {
            if (first) {
                first = false;
            } else {
                context.out << ", ";
            }
            context.out << quoted(type->tr->fields[i].name.text);
            context.out << ':';
            v->generate(context);
            i++;
        }
        context.out << "}";
    }
private:
    RecordLiteralExpression(const RecordLiteralExpression &);
    RecordLiteralExpression &operator=(const RecordLiteralExpression &);
};

class NewClassExpression: public Expression {
public:
    NewClassExpression(const ast::NewClassExpression *nce): Expression(nce), nce(nce), value(transform(nce->value)), type(dynamic_cast<const TypeRecord *>(transform(dynamic_cast<const ast::TypeValidPointer *>(nce->type)->reftype))) {}
    const ast::NewClassExpression *nce;
    const Expression *value;
    const TypeRecord *type;

    virtual void generate(Context &context) const override {
        if (value != nullptr) {
            value->generate(context);
        } else {
            type->generate_default(context);
        }
    }
private:
    NewClassExpression(const NewClassExpression &);
    NewClassExpression &operator=(const NewClassExpression &);
};

class UnaryMinusExpression: public Expression {
public:
    UnaryMinusExpression(const ast::UnaryMinusExpression *ume): Expression(ume), ume(ume), value(transform(ume->value)) {}
    const ast::UnaryMinusExpression *ume;
    const Expression *value;

    virtual void generate(Context &context) const override {
        context.out << "(-";
        value->generate(context);
        context.out << ")";
    }
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
        context.out << "(!";
        value->generate(context);
        context.out << ")";
    }
private:
    LogicalNotExpression(const LogicalNotExpression &);
    LogicalNotExpression &operator=(const LogicalNotExpression &);
};

class ConditionalExpression: public Expression {
public:
    ConditionalExpression(const ast::ConditionalExpression *ce): Expression(ce), ce(ce), condition(transform(ce->condition)), left(transform(ce->left)), right(transform(ce->right)) {}
    const ast::ConditionalExpression *ce;
    const Expression *condition;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.out << "(";
        condition->generate(context);
        context.out << " ? ";
        left->generate(context);
        context.out << " : ";
        right->generate(context);
        context.out << ")";
    }
private:
    ConditionalExpression(const ConditionalExpression &);
    ConditionalExpression &operator=(const ConditionalExpression &);
};

class TryExpressionTrap {
public:
    TryExpressionTrap(const ast::TryTrap *tt): tt(tt), name(transform(tt->name)), handler(), gives(transform(dynamic_cast<const ast::Expression *>(tt->handler))) {
        const ast::ExceptionHandlerStatement *h = dynamic_cast<const ast::ExceptionHandlerStatement *>(tt->handler);
        if (h != nullptr) {
            for (auto s: h->statements) {
                handler.push_back(transform(s));
            }
        }
    }
    const ast::TryTrap *tt;
    const Variable *name;
    std::vector<const Statement *> handler;
    const Expression *gives;
private:
    TryExpressionTrap(const TryExpressionTrap &);
    TryExpressionTrap &operator=(const TryExpressionTrap &);
};

class TryExpression: public Expression {
public:
    TryExpression(const ast::TryExpression *te): Expression(te), te(te), expr(transform(te->expr)), catches() {
        for (auto &t: te->catches) {
            catches.push_back(new TryExpressionTrap(&t));
        }
    }
    const ast::TryExpression *te;
    const Expression *expr;
    std::vector<const TryExpressionTrap *> catches;

    virtual void generate(Context &) const override {
        internal_error("TryExpression");
    }
private:
    TryExpression(const TryExpression &);
    TryExpression &operator=(const TryExpression &);
};

class DisjunctionExpression: public Expression {
public:
    DisjunctionExpression(const ast::DisjunctionExpression *de): Expression(de), de(de), left(transform(de->left)), right(transform(de->right)) {}
    const ast::DisjunctionExpression *de;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.out << "(";
        left->generate(context);
        context.out << " || ";
        right->generate(context);
        context.out << ")";
    }
private:
    DisjunctionExpression(const DisjunctionExpression &);
    DisjunctionExpression &operator=(const DisjunctionExpression &);
};

class ConjunctionExpression: public Expression {
public:
    ConjunctionExpression(const ast::ConjunctionExpression *ce): Expression(ce), ce(ce), left(transform(ce->left)), right(transform(ce->right)) {}
    const ast::ConjunctionExpression *ce;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.out << "(";
        left->generate(context);
        context.out << " && ";
        right->generate(context);
        context.out << ")";
    }
private:
    ConjunctionExpression(const ConjunctionExpression &);
    ConjunctionExpression &operator=(const ConjunctionExpression &);
};

class ArrayInExpression: public Expression {
public:
    ArrayInExpression(const ast::ArrayInExpression *aie): Expression(aie), aie(aie), left(transform(aie->left)), right(transform(aie->right)) {}
    const ast::ArrayInExpression *aie;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &) const override {
        internal_error("ArrayInExpression");
    }
private:
    ArrayInExpression(const ArrayInExpression &);
    ArrayInExpression &operator=(const ArrayInExpression &);
};

class DictionaryInExpression: public Expression {
public:
    DictionaryInExpression(const ast::DictionaryInExpression *die): Expression(die), die(die), left(transform(die->left)), right(transform(die->right)) {}
    const ast::DictionaryInExpression *die;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &) const override {
        internal_error("DictionaryInExpression");
    }
private:
    DictionaryInExpression(const DictionaryInExpression &);
    DictionaryInExpression &operator=(const DictionaryInExpression &);
};

class ComparisonExpression: public Expression {
public:
    ComparisonExpression(const ast::ComparisonExpression *ce): Expression(ce), ce(ce), left(transform(ce->left)), right(transform(ce->right)) {}
    const ast::ComparisonExpression *ce;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.out << "(";
        left->generate(context);
        generate_comparison(context);
        right->generate(context);
        context.out << ")";
    }
    virtual void generate_comparison(Context &context) const = 0;
private:
    ComparisonExpression(const ComparisonExpression &);
    ComparisonExpression &operator=(const ComparisonExpression &);
};

class ChainedComparisonExpression: public Expression {
public:
    ChainedComparisonExpression(const ast::ChainedComparisonExpression *cce): Expression(cce), cce(cce), comps() {
        for (auto c: cce->comps) {
            const ComparisonExpression *ce = dynamic_cast<const ComparisonExpression *>(transform(c));
            if (ce == nullptr) {
                internal_error("ChainedComparisonExpression");
            }
            comps.push_back(ce);
        }
    }
    const ast::ChainedComparisonExpression *cce;
    std::vector<const ComparisonExpression *> comps;

    virtual void generate(Context &context) const override {
        bool first = true;
        for (auto c: comps) {
            if (first) {
                first = false;
            } else {
                context.out << " && ";
            }
            c->generate(context);
        }
    }
private:
    ChainedComparisonExpression(const ChainedComparisonExpression &);
    ChainedComparisonExpression &operator=(const ChainedComparisonExpression &);
};

class BooleanComparisonExpression: public ComparisonExpression {
public:
    BooleanComparisonExpression(const ast::BooleanComparisonExpression *bce): ComparisonExpression(bce), bce(bce) {}
    const ast::BooleanComparisonExpression *bce;

    virtual void generate_comparison(Context &context) const override {
        static const char *op[] = {" == ", " != "};
        context.out << op[bce->comp];
    }
private:
    BooleanComparisonExpression(const BooleanComparisonExpression &);
    BooleanComparisonExpression &operator=(const BooleanComparisonExpression &);
};

class NumericComparisonExpression: public ComparisonExpression {
public:
    NumericComparisonExpression(const ast::NumericComparisonExpression *nce): ComparisonExpression(nce), nce(nce) {}
    const ast::NumericComparisonExpression *nce;

    virtual void generate_comparison(Context &context) const override {
        static const char *op[] = {" == ", " != ", " < ", " > ", " <= ", " >= "};
        context.out << op[nce->comp];
    }
private:
    NumericComparisonExpression(const NumericComparisonExpression &);
    NumericComparisonExpression &operator=(const NumericComparisonExpression &);
};

class EnumComparisonExpression: public ComparisonExpression {
public:
    EnumComparisonExpression(const ast::EnumComparisonExpression *ece): ComparisonExpression(ece), ece(ece) {}
    const ast::EnumComparisonExpression *ece;

    virtual void generate_comparison(Context &context) const override {
        static const char *op[] = {" == ", " != "};
        context.out << op[ece->comp];
    }
private:
    EnumComparisonExpression(const EnumComparisonExpression &);
    EnumComparisonExpression &operator=(const EnumComparisonExpression &);
};

class StringComparisonExpression: public ComparisonExpression {
public:
    StringComparisonExpression(const ast::StringComparisonExpression *sce): ComparisonExpression(sce), sce(sce) {}
    const ast::StringComparisonExpression *sce;

    virtual void generate_comparison(Context &context) const override {
        static const char *op[] = {" == ", " != ", " < ", " > ", " <= ", " >= "};
        context.out << op[sce->comp];
    }
private:
    StringComparisonExpression(const StringComparisonExpression &);
    StringComparisonExpression &operator=(const StringComparisonExpression &);
};

class BytesComparisonExpression: public ComparisonExpression {
public:
    BytesComparisonExpression(const ast::BytesComparisonExpression *bce): ComparisonExpression(bce), bce(bce) {}
    const ast::BytesComparisonExpression *bce;

    virtual void generate_comparison(Context &context) const override {
        static const char *op[] = {" == ", " != ", " < ", " > ", " <= ", " >= "};
        context.out << op[bce->comp];
    }
private:
    BytesComparisonExpression(const BytesComparisonExpression &);
    BytesComparisonExpression &operator=(const BytesComparisonExpression &);
};

class ArrayComparisonExpression: public ComparisonExpression {
public:
    ArrayComparisonExpression(const ast::ArrayComparisonExpression *ace): ComparisonExpression(ace), ace(ace) {}
    const ast::ArrayComparisonExpression *ace;

    virtual void generate_comparison(Context &context) const override {
        context.out << "==";
    }
private:
    ArrayComparisonExpression(const ArrayComparisonExpression &);
    ArrayComparisonExpression &operator=(const ArrayComparisonExpression &);
};

class DictionaryComparisonExpression: public ComparisonExpression {
public:
    DictionaryComparisonExpression(const ast::DictionaryComparisonExpression *dce): ComparisonExpression(dce), dce(dce) {}
    const ast::DictionaryComparisonExpression *dce;

    virtual void generate_comparison(Context &) const override {
        internal_error("DictionaryComparisonExpression");
    }
private:
    DictionaryComparisonExpression(const DictionaryComparisonExpression &);
    DictionaryComparisonExpression &operator=(const DictionaryComparisonExpression &);
};

class RecordComparisonExpression: public ComparisonExpression {
public:
    RecordComparisonExpression(const ast::RecordComparisonExpression *rce): ComparisonExpression(rce), rce(rce) {}
    const ast::RecordComparisonExpression *rce;

    virtual void generate_comparison(Context &) const override {
        internal_error("RecordComparisonExpression");
    }
private:
    RecordComparisonExpression(const RecordComparisonExpression &);
    RecordComparisonExpression &operator=(const RecordComparisonExpression &);
};

class PointerComparisonExpression: public ComparisonExpression {
public:
    PointerComparisonExpression(const ast::PointerComparisonExpression *pce): ComparisonExpression(pce), pce(pce) {}
    const ast::PointerComparisonExpression *pce;

    virtual void generate_comparison(Context &context) const override {
        context.out << " == ";
    }
private:
    PointerComparisonExpression(const PointerComparisonExpression &);
    PointerComparisonExpression &operator=(const PointerComparisonExpression &);
};

class ValidPointerExpression: public PointerComparisonExpression {
public:
    ValidPointerExpression(const ast::ValidPointerExpression *vpe): PointerComparisonExpression(vpe), vpe(vpe), var(transform(vpe->var)) {}
    const ast::ValidPointerExpression *vpe;
    const Variable *var;

    virtual void generate(Context &context) const override {
        context.out << "(";
        var->generate(context);
        context.out << "=";
        left->generate(context);
        context.out << ")";
        context.out << "!=";
        right->generate(context);
    }
private:
    ValidPointerExpression(const ValidPointerExpression &);
    ValidPointerExpression &operator=(const ValidPointerExpression &);
};

class FunctionPointerComparisonExpression: public ComparisonExpression {
public:
    FunctionPointerComparisonExpression(const ast::FunctionPointerComparisonExpression *fpce): ComparisonExpression(fpce), fpce(fpce) {}
    const ast::FunctionPointerComparisonExpression *fpce;

    virtual void generate_comparison(Context &context) const override {
        static const char *op[] = {" == ", " != "};
        context.out << op[fpce->comp];
    }
private:
    FunctionPointerComparisonExpression(const FunctionPointerComparisonExpression &);
    FunctionPointerComparisonExpression &operator=(const FunctionPointerComparisonExpression &);
};

class AdditionExpression: public Expression {
public:
    AdditionExpression(const ast::AdditionExpression *ae): Expression(ae), ae(ae), left(transform(ae->left)), right(transform(ae->right)) {}
    const ast::AdditionExpression *ae;
    const Expression *left;
    const Expression *right;

    virtual void generate(Context &context) const override {
        context.out << "(";
        left->generate(context);
        context.out << "+";
        right->generate(context);
        context.out << ")";
    }
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
        context.out << "(";
        left->generate(context);
        context.out << "-";
        right->generate(context);
        context.out << ")";
    }
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
        context.out << "(";
        left->generate(context);
        context.out << "*";
        right->generate(context);
        context.out << ")";
    }
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
        context.out << "neon.global.divide(";
        left->generate(context);
        context.out << ", ";
        right->generate(context);
        context.out << ")";
    }
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
        context.out << "(";
        left->generate(context);
        context.out << "%";
        right->generate(context);
        context.out << ")";
    }
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
        context.out << "neon::math.pow(";
        left->generate(context);
        context.out << ",";
        right->generate(context);
        context.out << ")";
    }
private:
    ExponentiationExpression(const ExponentiationExpression &);
    ExponentiationExpression &operator=(const ExponentiationExpression &);
};

class DummyExpression: public Expression {
public:
    DummyExpression(const ast::DummyExpression *de): Expression(de), de(de) {}
    const ast::DummyExpression *de;

    virtual void generate(Context &) const override { internal_error("DummyExpression"); }
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
        context.out << "[";
        index->generate(context);
        context.out << "]";
    }
private:
    ArrayReferenceIndexExpression(const ArrayReferenceIndexExpression &);
    ArrayReferenceIndexExpression &operator=(const ArrayReferenceIndexExpression &);
};

class ArrayValueIndexExpression: public Expression {
public:
    ArrayValueIndexExpression(const ast::ArrayValueIndexExpression *avie): Expression(avie), avie(avie), array(transform(avie->array)), index(transform(avie->index)) {}
    const ast::ArrayValueIndexExpression *avie;
    const Expression *array;
    const Expression *index;

    virtual void generate(Context &context) const override {
        array->generate(context);
        context.out << "[";
        index->generate(context);
        context.out << "]";
    }
private:
    ArrayValueIndexExpression(const ArrayValueIndexExpression &);
    ArrayValueIndexExpression &operator=(const ArrayValueIndexExpression &);
};

class DictionaryReferenceIndexExpression: public Expression {
public:
    DictionaryReferenceIndexExpression(const ast::DictionaryReferenceIndexExpression *drie): Expression(drie), drie(drie), dictionary(transform(drie->dictionary)), index(transform(drie->index)) {}
    const ast::DictionaryReferenceIndexExpression *drie;
    const Expression *dictionary;
    const Expression *index;

    virtual void generate(Context &context) const override {
        dictionary->generate(context);
        context.out << "[";
        index->generate(context);
        context.out << "]";
    }
private:
    DictionaryReferenceIndexExpression(const DictionaryReferenceIndexExpression &);
    DictionaryReferenceIndexExpression &operator=(const DictionaryReferenceIndexExpression &);
};

class DictionaryValueIndexExpression: public Expression {
public:
    DictionaryValueIndexExpression(const ast::DictionaryValueIndexExpression *dvie): Expression(dvie), dvie(dvie), dictionary(transform(dvie->dictionary)), index(transform(dvie->index)) {}
    const ast::DictionaryValueIndexExpression *dvie;
    const Expression *dictionary;
    const Expression *index;

    virtual void generate(Context &) const override {
        internal_error("DictionaryValueIndexExpression");
    }
private:
    DictionaryValueIndexExpression(const DictionaryValueIndexExpression &);
    DictionaryValueIndexExpression &operator=(const DictionaryValueIndexExpression &);
};

class StringReferenceIndexExpression: public Expression {
public:
    StringReferenceIndexExpression(const ast::StringReferenceIndexExpression *srie): Expression(srie), srie(srie), ref(transform(srie->ref)), first(transform(srie->first)), last(transform(srie->last)) {}
    const ast::StringReferenceIndexExpression *srie;
    const Expression *ref;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &) const override {
        internal_error("StringReferenceIndexExpression");
    }
private:
    StringReferenceIndexExpression(const StringReferenceIndexExpression &);
    StringReferenceIndexExpression &operator=(const StringReferenceIndexExpression &);
};

class StringValueIndexExpression: public Expression {
public:
    StringValueIndexExpression(const ast::StringValueIndexExpression *svie): Expression(svie), svie(svie), str(transform(svie->str)), first(transform(svie->first)), last(transform(svie->last)) {}
    const ast::StringValueIndexExpression *svie;
    const Expression *str;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &) const override {
        internal_error("StringValueIndexExpression");
    }
private:
    StringValueIndexExpression(const StringValueIndexExpression &);
    StringValueIndexExpression &operator=(const StringValueIndexExpression &);
};

class BytesReferenceIndexExpression: public Expression {
public:
    BytesReferenceIndexExpression(const ast::BytesReferenceIndexExpression *brie): Expression(brie), brie(brie), ref(transform(brie->ref)), first(transform(brie->first)), last(transform(brie->last)) {}
    const ast::BytesReferenceIndexExpression *brie;
    const Expression *ref;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &) const override {
        internal_error("BytesReferenceIndexExpression");
    }
private:
    BytesReferenceIndexExpression(const BytesReferenceIndexExpression &);
    BytesReferenceIndexExpression &operator=(const BytesReferenceIndexExpression &);
};

class BytesValueIndexExpression: public Expression {
public:
    BytesValueIndexExpression(const ast::BytesValueIndexExpression *bvie): Expression(bvie), bvie(bvie), data(transform(bvie->str)), first(transform(bvie->first)), last(transform(bvie->last)) {}
    const ast::BytesValueIndexExpression *bvie;
    const Expression *data;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &) const override {
        internal_error("BytesValueIndexExpression");
    }
private:
    BytesValueIndexExpression(const BytesValueIndexExpression &);
    BytesValueIndexExpression &operator=(const BytesValueIndexExpression &);
};

class RecordReferenceFieldExpression: public Expression {
public:
    RecordReferenceFieldExpression(const ast::RecordReferenceFieldExpression *rrfe): Expression(rrfe), rrfe(rrfe), ref(transform(rrfe->ref)), rectype(dynamic_cast<const TypeRecord *>(transform(rrfe->ref->type))), fieldtype(transform(rrfe->type)) {}
    const ast::RecordReferenceFieldExpression *rrfe;
    const Expression *ref;
    const TypeRecord *rectype;
    const Type *fieldtype;

    virtual void generate(Context &context) const override {
        ref->generate(context);
        context.out << "." << rrfe->field;
    }
private:
    RecordReferenceFieldExpression(const RecordReferenceFieldExpression &);
    RecordReferenceFieldExpression &operator=(const RecordReferenceFieldExpression &);
};

class RecordValueFieldExpression: public Expression {
public:
    RecordValueFieldExpression(const ast::RecordValueFieldExpression *rvfe): Expression(rvfe), rvfe(rvfe), rec(transform(rvfe->rec)), rectype(dynamic_cast<const TypeRecord *>(transform(rvfe->rec->type))), fieldtype(transform(rvfe->type)) {}
    const ast::RecordValueFieldExpression *rvfe;
    const Expression *rec;
    const TypeRecord *rectype;
    const Type *fieldtype;

    virtual void generate(Context &context) const override {
        rec->generate(context);
        context.out << "." << rvfe->field;
    }
private:
    RecordValueFieldExpression(const RecordValueFieldExpression &);
    RecordValueFieldExpression &operator=(const RecordValueFieldExpression &);
};

class ArrayReferenceRangeExpression: public Expression {
public:
    ArrayReferenceRangeExpression(const ast::ArrayReferenceRangeExpression *arre): Expression(arre), arre(arre), ref(transform(arre->ref)), first(transform(arre->first)), last(transform(arre->last)) {}
    const ast::ArrayReferenceRangeExpression *arre;
    const Expression *ref;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &) const override {
        internal_error("ArrayReferenceRangeExpression");
    }
private:
    ArrayReferenceRangeExpression(const ArrayReferenceRangeExpression &);
    ArrayReferenceRangeExpression &operator=(const ArrayReferenceRangeExpression &);
};

class ArrayValueRangeExpression: public Expression {
public:
    ArrayValueRangeExpression(const ast::ArrayValueRangeExpression *avre): Expression(avre), avre(avre), array(transform(avre->array)), first(transform(avre->first)), last(transform(avre->last)) {}
    const ast::ArrayValueRangeExpression *avre;
    const Expression *array;
    const Expression *first;
    const Expression *last;

    virtual void generate(Context &) const override {
        internal_error("ArrayValueRangeExpression");
    }
private:
    ArrayValueRangeExpression(const ArrayValueRangeExpression &);
    ArrayValueRangeExpression &operator=(const ArrayValueRangeExpression &);
};

class PointerDereferenceExpression: public Expression {
public:
    PointerDereferenceExpression(const ast::PointerDereferenceExpression *pde): Expression(pde), pde(pde), ptr(transform(pde->ptr)) {}
    const ast::PointerDereferenceExpression *pde;
    const Expression *ptr;

    virtual void generate(Context &context) const override {
        ptr->generate(context);
    }
private:
    PointerDereferenceExpression(const PointerDereferenceExpression &);
    PointerDereferenceExpression &operator=(const PointerDereferenceExpression &);
};

class VariableExpression: public Expression {
public:
    VariableExpression(const ast::VariableExpression *ve): Expression(ve), ve(ve), var(transform(ve->var)) {}
    const ast::VariableExpression *ve;
    const Variable *var;

    virtual void generate(Context &context) const override {
        var->generate(context);
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
        func->generate(context);
        context.out << "(";
        bool first = true;
        for (auto a: args) {
            if (first) {
                first = false;
            } else {
                context.out << ",";
            }
            a->generate(context);
        }
        context.out << ")";
    }
private:
    FunctionCall(const FunctionCall &);
    FunctionCall &operator=(const FunctionCall &);
};

class NullStatement: public Statement {
public:
    NullStatement(const ast::NullStatement *ns): Statement(ns), ns(ns) {}
    const ast::NullStatement *ns;

    virtual void generate(Context &) const override {}
private:
    NullStatement(const NullStatement &);
    NullStatement &operator=(const NullStatement &);
};

class DeclarationStatement: public Statement {
public:
    DeclarationStatement(const ast::DeclarationStatement *ds): Statement(ds), ds(ds), decl(transform(ds->decl)) {}
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
    AssertStatement(const ast::AssertStatement *as): Statement(as), as(as), statements(), expr(transform(as->expr)) {
        for (auto s: as->statements) {
            statements.push_back(transform(s));
        }
    }
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
private:
    AssertStatement(const AssertStatement &);
    AssertStatement &operator=(const AssertStatement &);
};

class AssignmentStatement: public Statement {
public:
    AssignmentStatement(const ast::AssignmentStatement *as): Statement(as), as(as), variables(), expr(transform(as->expr)) {
        for (auto v: as->variables) {
            variables.push_back(transform(v));
        }
    }
    const ast::AssignmentStatement *as;
    std::vector<const Expression *> variables;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        for (auto v: variables) {
            if (dynamic_cast<const DummyExpression *>(v) == nullptr) {
                v->generate(context);
                context.out << "=";
            }
        }
        expr->generate(context);
        context.out << ";";
    }
private:
    AssignmentStatement(const AssignmentStatement &);
    AssignmentStatement &operator=(const AssignmentStatement &);
};

class ExpressionStatement: public Statement {
public:
    ExpressionStatement(const ast::ExpressionStatement *es): Statement(es), es(es), expr(transform(es->expr)) {}
    const ast::ExpressionStatement *es;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        expr->generate(context);
        context.out << ";";
    }
private:
    ExpressionStatement(const ExpressionStatement &);
    ExpressionStatement &operator=(const ExpressionStatement &);
};

class CompoundStatement: public Statement {
public:
    CompoundStatement(const ast::CompoundStatement *cs): Statement(cs), cs(cs), statements() {
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

    virtual void generate(Context &) const override {
        internal_error("BaseLoopStatement");
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
        virtual void generate(Context &context, const Expression *value) const = 0;
    private:
        WhenCondition(const WhenCondition &);
        WhenCondition &operator=(const WhenCondition &);
    };
    class ComparisonWhenCondition: public WhenCondition {
    public:
        ComparisonWhenCondition(ast::ComparisonExpression::Comparison comp, const Expression *expr): comp(comp), expr(expr) {}
        ast::ComparisonExpression::Comparison comp;
        const Expression *expr;
        virtual void generate(Context &context, const Expression *value) const override {
            static const char *op[] = {" == ", " != ", " < ", " > ", " <= ", " >= "};
            value->generate(context);
            context.out << op[comp];
            expr->generate(context);
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
        virtual void generate(Context &context, const Expression *value) const override {
            value->generate(context);
            context.out << " >= ";
            low_expr->generate(context);
            context.out << " && ";
            value->generate(context);
            context.out << " <= ";
            high_expr->generate(context);
        }
    private:
        RangeWhenCondition(const RangeWhenCondition &);
        RangeWhenCondition &operator=(const RangeWhenCondition &);
    };
    CaseStatement(const ast::CaseStatement *cs): Statement(cs), cs(cs), expr(transform(cs->expr)), clauses() {
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
private:
    CaseStatement(const CaseStatement &);
    CaseStatement &operator=(const CaseStatement &);
};

class ExitStatement: public Statement {
public:
    ExitStatement(const ast::ExitStatement *es): Statement(es), es(es) {}
    const ast::ExitStatement *es;

    void generate(Context &context) const override {
        context.out << "break;"; // TODO: use label to get correct loop
    }
private:
    ExitStatement(const ExitStatement &);
    ExitStatement &operator=(const ExitStatement &);
};

class NextStatement: public Statement {
public:
    NextStatement(const ast::NextStatement *ns): Statement(ns), ns(ns) {}
    const ast::NextStatement *ns;

    virtual void generate(Context &context) const override {
        context.out << "continue;"; // TODO: use label to get correct loop
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
private:
    TryStatementTrap(const TryStatementTrap &);
    TryStatementTrap &operator=(const TryStatementTrap &);
};

class TryStatement: public Statement {
public:
    TryStatement(const ast::TryStatement *ts): Statement(ts), ts(ts), statements(), catches() {
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
private:
    TryStatement(const TryStatement &);
    TryStatement &operator=(const TryStatement &);
};

class ReturnStatement: public Statement {
public:
    ReturnStatement(const ast::ReturnStatement *rs): Statement(rs), rs(rs), expr(transform(rs->expr)) {}
    const ast::ReturnStatement *rs;
    const Expression *expr;

    virtual void generate(Context &context) const override {
        context.out << "return";
        if (expr != nullptr) {
            context.out << " ";
            expr->generate(context);
        }
        context.out << ";";
    }
private:
    ReturnStatement(const ReturnStatement &);
    ReturnStatement &operator=(const ReturnStatement &);
};

class IncrementStatement: public Statement {
public:
    IncrementStatement(const ast::IncrementStatement *is): Statement(is), is(is), ref(transform(is->ref)) {}
    const ast::IncrementStatement *is;
    const Expression *ref;

    virtual void generate(Context &context) const override {
        ref->generate(context);
        context.out << "+=" << is->delta << ";";
    }
private:
    IncrementStatement(const IncrementStatement &);
    IncrementStatement &operator=(const IncrementStatement &);
};

class IfStatement: public Statement {
public:
    IfStatement(const ast::IfStatement *is): Statement(is), is(is), condition_statements(), else_statements() {
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
private:
    IfStatement(const IfStatement &);
    IfStatement &operator=(const IfStatement &);
};

class RaiseStatement: public Statement {
public:
    RaiseStatement(const ast::RaiseStatement *rs): Statement(rs), rs(rs), info(transform(rs->info)) {}
    const ast::RaiseStatement *rs;
    const Expression *info;

    virtual void generate(Context &context) const override {
        context.out << "throw neon::NeonException(" << quoted(rs->exception->name) << ", ";
        info->generate(context);
        context.out << ");";
    }
private:
    RaiseStatement(const RaiseStatement &);
    RaiseStatement &operator=(const RaiseStatement &);
};

class ResetStatement: public Statement {
public:
    ResetStatement(const ast::ResetStatement *rs): Statement(rs), rs(rs) {}
    const ast::ResetStatement *rs;

    virtual void generate(Context &) const override {}
private:
    ResetStatement(const ResetStatement &);
    ResetStatement &operator=(const ResetStatement &);
};

class Function: public Variable {
public:
    Function(const ast::Function *f): Variable(f), f(f), statements(), params(), out_count(0) {
        // Need to transform the function parameters before transforming
        // the code that might use them (statements).
        int i = 0;
        for (auto p: f->params) {
            FunctionParameter *q = new FunctionParameter(p, i);
            params.push_back(q);
            g_variable_cache[p] = q;
            if (q->fp->mode == ast::ParameterType::INOUT || q->fp->mode == ast::ParameterType::OUT) {
                out_count++;
            }
            i++;
        }
        for (auto s: f->statements) {
            statements.push_back(transform(s));
        }
    }
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
    virtual void generate(Context &context) const override {
        context.out << f->name;
    }
private:
    Function(const Function &);
    Function &operator=(const Function &);
};

class PredefinedFunction: public Variable {
public:
    PredefinedFunction(const ast::PredefinedFunction *pf): Variable(pf), pf(pf), out_count(0) {
        const ast::TypeFunction *tf = dynamic_cast<const ast::TypeFunction *>(pf->type);
        for (auto p: tf->params) {
            if (p->mode == ast::ParameterType::INOUT || p->mode == ast::ParameterType::OUT) {
                out_count++;
            }
        }
    }
    const ast::PredefinedFunction *pf;
    int out_count;

    virtual void generate_decl(Context &) const override { internal_error("PredefinedFunction"); }
    virtual void generate(Context &context) const override {
        auto dollar = pf->name.find('$');
        if (dollar != std::string::npos) {
            context.out << "neon::" << pf->name.substr(0, dollar) << "." << pf->name.substr(dollar+1);
        } else {
            context.out << "neon::global::" << pf->name;
        }
    }
private:
    PredefinedFunction(const PredefinedFunction &);
    PredefinedFunction &operator=(const PredefinedFunction &);
};

class ModuleFunction: public Variable {
public:
    ModuleFunction(const ast::ModuleFunction *mf): Variable(mf), mf(mf), out_count(0) {
        const TypeFunction *functype = dynamic_cast<const TypeFunction *>(transform(mf->type));
        int i = 0;
        for (auto p: functype->paramtypes) {
            if (p.first == ast::ParameterType::INOUT || p.first == ast::ParameterType::OUT) {
                out_count++;
            }
            i++;
        }
    }
    const ast::ModuleFunction *mf;
    int out_count;

    virtual void generate_decl(Context &) const override { internal_error("ModuleFunction"); }
    virtual void generate(Context &context) const override {
        context.out << mf->name;
    }
private:
    ModuleFunction(const ModuleFunction &);
    ModuleFunction &operator=(const ModuleFunction &);
};

class Program {
public:
    Program(CompilerSupport *support, const ast::Program *program): support(support), program(program), statements() {
        for (auto s: program->statements) {
            statements.push_back(transform(s));
        }
    }
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
        context.out << "int main(int, const char *) { try {";
        for (auto s: statements) {
            s->generate(context);
        }
        context.out << "} catch (neon::NeonException &x) { std::cerr << \"Unhandled exception \" << x.name << \"\\n\"; } }";
        std::vector<unsigned char> src;
        std::string t = out.str();
        std::copy(t.begin(), t.end(), std::back_inserter(src));
        support->writeOutput(path + program->module_name + ".cpp", src);
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
    virtual void visit(const ast::TypeRecord *node) { r = new TypeRecord(node); }
    virtual void visit(const ast::TypeClass *node) { r = new TypeRecord(node); }
    virtual void visit(const ast::TypePointer *node) { r = new TypePointer(node); }
    virtual void visit(const ast::TypeFunctionPointer *node) { r = new TypeFunctionPointer(node); }
    virtual void visit(const ast::TypeEnum *node) { r = new TypeEnum(node); }
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
    virtual void visit(const ast::BytesReferenceIndexExpression *) {}
    virtual void visit(const ast::BytesValueIndexExpression *) {}
    virtual void visit(const ast::RecordReferenceFieldExpression *) {}
    virtual void visit(const ast::RecordValueFieldExpression *) {}
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
    virtual void visit(const ast::PredefinedVariable *node) { r = new PredefinedVariable(node); }
    virtual void visit(const ast::ModuleVariable *node) { r = new ModuleVariable(node); }
    virtual void visit(const ast::GlobalVariable *node) { r = new GlobalVariable(node); }
    virtual void visit(const ast::ExternalGlobalVariable *) { internal_error("ExternalGlobalVariable"); }
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
    virtual void visit(const ast::BytesReferenceIndexExpression *) {}
    virtual void visit(const ast::BytesValueIndexExpression *) {}
    virtual void visit(const ast::RecordReferenceFieldExpression *) {}
    virtual void visit(const ast::RecordValueFieldExpression *) {}
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
    virtual void visit(const ast::ModuleFunction *node) { r = new ModuleFunction(node); }
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
    virtual void visit(const ast::ConstantEnumExpression *node) { r = new ConstantEnumExpression(node); }
    virtual void visit(const ast::ConstantNilExpression *node) { r = new ConstantNilExpression(node); }
    virtual void visit(const ast::ConstantNowhereExpression *node) { r = new ConstantNowhereExpression(node); }
    virtual void visit(const ast::ArrayLiteralExpression *node) { r = new ArrayLiteralExpression(node); }
    virtual void visit(const ast::DictionaryLiteralExpression *node) { r = new DictionaryLiteralExpression(node); }
    virtual void visit(const ast::RecordLiteralExpression *node) { r = new RecordLiteralExpression(node); }
    virtual void visit(const ast::NewClassExpression *node) { r =  new NewClassExpression(node); }
    virtual void visit(const ast::UnaryMinusExpression *node) { r = new UnaryMinusExpression(node); }
    virtual void visit(const ast::LogicalNotExpression *node) { r = new LogicalNotExpression(node); }
    virtual void visit(const ast::ConditionalExpression *node) { r = new ConditionalExpression(node); }
    virtual void visit(const ast::TryExpression *node) { r = new TryExpression(node); }
    virtual void visit(const ast::DisjunctionExpression *node) { r = new DisjunctionExpression(node); }
    virtual void visit(const ast::ConjunctionExpression *node) { r = new ConjunctionExpression(node); }
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
    virtual void visit(const ast::BytesReferenceIndexExpression *node) { r = new BytesReferenceIndexExpression(node); }
    virtual void visit(const ast::BytesValueIndexExpression *node) { r = new BytesValueIndexExpression(node); }
    virtual void visit(const ast::RecordReferenceFieldExpression *node) { r = new RecordReferenceFieldExpression(node); }
    virtual void visit(const ast::RecordValueFieldExpression *node) { r = new RecordValueFieldExpression(node); }
    virtual void visit(const ast::ArrayReferenceRangeExpression *node) { r = new ArrayReferenceRangeExpression(node); }
    virtual void visit(const ast::ArrayValueRangeExpression *node) { r = new ArrayValueRangeExpression(node); }
    virtual void visit(const ast::PointerDereferenceExpression *node) { r =  new PointerDereferenceExpression(node); }
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
    virtual void visit(const ast::BytesReferenceIndexExpression *) {}
    virtual void visit(const ast::BytesValueIndexExpression *) {}
    virtual void visit(const ast::RecordReferenceFieldExpression *) {}
    virtual void visit(const ast::RecordValueFieldExpression *) {}
    virtual void visit(const ast::ArrayReferenceRangeExpression *) {}
    virtual void visit(const ast::ArrayValueRangeExpression *) {}
    virtual void visit(const ast::PointerDereferenceExpression *) {}
    virtual void visit(const ast::ConstantExpression *) {}
    virtual void visit(const ast::VariableExpression *) {}
    virtual void visit(const ast::FunctionCall *) {}
    virtual void visit(const ast::StatementExpression *) {}
    virtual void visit(const ast::NullStatement *node) { r = new NullStatement(node); }
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

} // namespace cpp

void compile_cpp(CompilerSupport *support, const ast::Program *p)
{
    cpp::g_type_cache.clear();
    cpp::g_variable_cache.clear();
    cpp::g_expression_cache.clear();
    cpp::g_statement_cache.clear();

    cpp::Program *ct = new cpp::Program(support, p);
    ct->generate();
}
