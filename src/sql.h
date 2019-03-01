#ifndef SQL_H
#define SQL_H

#include <memory>
#include <string>
#include <vector>

#include "token.h"

enum SqlWheneverCondition {
    NotFound,
    SqlError,
    SqlWarning,
    SqlWheneverConditionCount
};

enum class SqlWheneverAction {
    Continue,
    SqlPrint,
    Stop,
    DoExitLoop,
    DoExitFor,
    DoExitForeach,
    DoExitRepeat,
    DoExitWhile,
    DoNextLoop,
    DoNextFor,
    DoNextForeach,
    DoNextRepeat,
    DoNextWhile,
    DoRaiseException
};

class SqlValue {
public:
    virtual ~SqlValue() {}
};

class SqlValueLiteral: public SqlValue {
public:
    explicit SqlValueLiteral(const std::string &value): value(value) {}
    const std::string value;
private:
    SqlValueLiteral(const SqlValueLiteral &);
    SqlValueLiteral &operator=(const SqlValueLiteral &);
};

class SqlValueVariable: public SqlValue {
public:
    explicit SqlValueVariable(const Token &variable): variable(variable) {}
    const Token variable;
private:
    SqlValueVariable(const SqlValueVariable &);
    SqlValueVariable &operator=(const SqlValueVariable &);
};

class SqlIdentifier {
public:
    virtual ~SqlIdentifier() {}
    virtual std::string text() const = 0;
};

class SqlIdentifierSymbol: public SqlIdentifier {
public:
    explicit SqlIdentifierSymbol(const std::string &ident): ident(ident) {}
    virtual std::string text() const override { return ident; }
    const std::string ident;
private:
    SqlIdentifierSymbol(const SqlIdentifierSymbol &);
    SqlIdentifierSymbol &operator=(const SqlIdentifierSymbol &);
};

class SqlIdentifierVariable: public SqlIdentifier {
public:
    explicit SqlIdentifierVariable(const Token &variable): variable(variable) {}
    virtual std::string text() const override { return ":" + variable.text; }
    const Token variable;
private:
    SqlIdentifierVariable(const SqlIdentifierVariable &);
    SqlIdentifierVariable &operator=(const SqlIdentifierVariable &);
};

class SqlStatement {
public:
    virtual ~SqlStatement() {}
};

class SqlCloseStatement: public SqlStatement {
public:
    SqlCloseStatement(std::unique_ptr<SqlIdentifier> &&name): name(std::move(name)) {}
    std::unique_ptr<SqlIdentifier> name;
};

class SqlConnectStatement: public SqlStatement {
public:
    SqlConnectStatement(std::unique_ptr<SqlValue> &&target, std::unique_ptr<SqlIdentifier> &&name): target(std::move(target)), name(std::move(name)) {}
    std::unique_ptr<SqlValue> target;
    std::unique_ptr<SqlIdentifier> name;
};

class SqlDeclareStatement: public SqlStatement {
public:
    SqlDeclareStatement(std::unique_ptr<SqlIdentifier> &&cursor, bool binary, bool insensitive, bool scroll, bool hold): cursor(std::move(cursor)), binary(binary), insensitive(insensitive), scroll(scroll), hold(hold) {}
    std::unique_ptr<SqlIdentifier> cursor;
    const bool binary;
    const bool insensitive;
    const bool scroll;
    const bool hold;
};

class SqlDeclarePreparedStatement: public SqlDeclareStatement {
public:
    SqlDeclarePreparedStatement(std::unique_ptr<SqlIdentifier> &&cursor, bool binary, bool insensitive, bool scroll, bool hold, std::unique_ptr<SqlIdentifier> &&name): SqlDeclareStatement(std::move(cursor), binary, insensitive, scroll, hold), name(std::move(name)) {}
    std::unique_ptr<SqlIdentifier> name;
};

class SqlDeclareQueryStatement: public SqlDeclareStatement {
public:
    SqlDeclareQueryStatement(std::unique_ptr<SqlIdentifier> &&cursor, bool binary, bool insensitive, bool scroll, bool hold, const std::string &query): SqlDeclareStatement(std::move(cursor), binary, insensitive, scroll, hold), query(query) {}
    const std::string query;
};

class SqlDisconnectStatement: public SqlStatement {
public:
    SqlDisconnectStatement(std::unique_ptr<SqlIdentifier> &&name, bool default_, bool all): name(std::move(name)), default_(default_), all(all) {}
    std::unique_ptr<SqlIdentifier> name;
    const bool default_;
    const bool all;
};

class SqlExecuteStatement: public SqlStatement {
public:
    SqlExecuteStatement(std::unique_ptr<SqlValue> &&statement): statement(std::move(statement)) {}
    std::unique_ptr<SqlValue> statement;
};

class SqlFetchStatement: public SqlStatement {
public:
    SqlFetchStatement(std::unique_ptr<SqlIdentifier> &&name): name(std::move(name)) {}
    std::unique_ptr<SqlIdentifier> name;
};

class SqlOpenStatement: public SqlStatement {
public:
    SqlOpenStatement(std::unique_ptr<SqlIdentifier> &&name, std::vector<std::unique_ptr<SqlValue>> &&values): name(std::move(name)), values(std::move(values)) {}
    std::unique_ptr<SqlIdentifier> name;
    std::vector<std::unique_ptr<SqlValue>> values;
};

class SqlPrepareStatement: public SqlStatement {
public:
    SqlPrepareStatement(std::unique_ptr<SqlIdentifier> &&name, std::unique_ptr<SqlValue> &&statement): name(std::move(name)), statement(std::move(statement)) {}
    std::unique_ptr<SqlIdentifier> name;
    std::unique_ptr<SqlValue> statement;
};

class SqlSetConnectionStatement: public SqlStatement {
public:
    SqlSetConnectionStatement(std::unique_ptr<SqlIdentifier> &&name): name(std::move(name)) {}
    std::unique_ptr<SqlIdentifier> name;
};

class SqlWheneverStatement: public SqlStatement {
public:
    SqlWheneverStatement(SqlWheneverCondition condition, SqlWheneverAction action): condition(condition), action(action) {}
    const SqlWheneverCondition condition;
    const SqlWheneverAction action;
private:
    SqlWheneverStatement(const SqlWheneverStatement &);
    SqlWheneverStatement &operator=(const SqlWheneverStatement &);
};

class SqlQueryStatement: public SqlStatement {
public:
    explicit SqlQueryStatement(const std::string &query): query(query) {}
    const std::string query;
private:
    SqlQueryStatement(const SqlQueryStatement &);
    SqlQueryStatement &operator=(const SqlQueryStatement &);
};

class SqlStatementInfo {
public:
    SqlStatementInfo(std::unique_ptr<SqlStatement> &&sql, const std::vector<Token> &assignments, const std::vector<Token> &parameters): sql(std::move(sql)), assignments(assignments), parameters(parameters) {}
    std::unique_ptr<SqlStatement> sql;
    std::vector<Token> assignments;
    std::vector<Token> parameters;
};

std::unique_ptr<SqlStatementInfo> parseSqlStatement(const Token &token, const std::string &statement);

#endif
