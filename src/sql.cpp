#include "sql.h"

#include <algorithm>
#include <iso646.h>

#include "util.h"

namespace sql {

enum Token {
    NONE,
    IDENTIFIER,
    VALUE,
    SYMBOL,
    COLON,
    COMMA,
    EQUALS,

    ALL,
    AS,
    BINARY,
    CLOSE,
    CONNECT,
    CONNECTION,
    CONTINUE,
    CREATE,
    CURRENT,
    CURSOR,
    DECLARE,
    DEFAULT,
    DELETE,
    DESCRIBE,
    DISCONNECT,
    DO,
    DROP,
    EXECUTE,
    EXIT,
    FETCH,
    FOR,
    FOREACH,
    FOUND,
    FROM,
    HOLD,
    IMMEDIATE,
    IN,
    INSENSITIVE,
    INSERT,
    INTO,
    LOOP,
    NEXT,
    NO,
    NOT,
    OPEN,
    PREPARE,
    RAISE,
    REPEAT,
    SCROLL,
    SELECT,
    SET,
    SQL,
    SQLERROR,
    SQLPRINT,
    SQLWARNING,
    STOP,
    TO,
    USING,
    VALUES,
    WHENEVER,
    WHILE,
    WITH,
    WITHOUT
};

class Lexer {
public:
    Lexer(const ::Token &token, const std::string &statement)
      : token(token),
        statement(statement),
        index(0),
        current(NONE),
        current_index(),
        current_value(),
        last_token(),
        this_token()
    {
        update_current();
    }
    Lexer(const Lexer &) = delete;
    Lexer &operator=(const Lexer &) = delete;

    std::string::size_type position()
    {
        return current_index;
    }

    Token next()
    {
        last_token = this_token;
        Token r = current;
        update_current();
        return r;
    }

    Token peek()
    {
        return current;
    }

    std::string peek_rest()
    {
        return statement.substr(current_index);
    }

    std::string rest()
    {
        std::string r = peek_rest();
        current = NONE;
        current_index = statement.length();
        return r;
    }

    std::string value()
    {
        return current_value;
    }

    ::Token get_this_token()
    {
        return this_token;
    }

    ::Token get_last_token()
    {
        return last_token;
    }

//private:
    const ::Token &token;
    const std::string statement;
    std::string::size_type index;
    Token current;
    std::string::size_type current_index;
    std::string current_value;
    ::Token last_token;
    ::Token this_token;

    void update_current()
    {
        while (index < statement.length() && isspace(statement[index])) {
            index++;
        }
        if (index >= statement.length()) {
            current = NONE;
            return;
        }
        current_index = index;
        if (statement[index] == '"') {
            current = VALUE;
            index++;
            auto start = index;
            while (index < statement.length() && statement[index] != '"') {
                index++;
            }
            current_value = statement.substr(start, index-start);
            if (index < statement.length()) {
                index++;
            }
        } else if (statement[index] == ':') {
            current = COLON;
            index++;
        } else if (statement[index] == ',') {
            current = COMMA;
            index++;
        } else if (statement[index] == '=') {
            current = EQUALS;
            index++;
        } else if (isalpha(statement[index])) {
            current = IDENTIFIER;
            auto start = index;
            while (index < statement.length() && isalnum(statement[index])) {
                index++;
            }
            current_value = statement.substr(start, index-start);
            std::string keyword;
            //std::transform(current_value.begin(), current_value.end(), std::back_inserter(keyword), ::toupper);
            for (auto c: current_value) {
                keyword.push_back(static_cast<char>(::toupper(c)));
            }
            if (keyword == "ALL")           current = ALL;
            if (keyword == "AS")            current = AS;
            if (keyword == "BINARY")        current = BINARY;
            if (keyword == "CLOSE")         current = CLOSE;
            if (keyword == "CONNECT")       current = CONNECT;
            if (keyword == "CONNECTION")    current = CONNECTION;
            if (keyword == "CONTINUE")      current = CONTINUE;
            if (keyword == "CREATE")        current = CREATE;
            if (keyword == "CURRENT")       current = CURRENT;
            if (keyword == "CURSOR")        current = CURSOR;
            if (keyword == "DECLARE")       current = DECLARE;
            if (keyword == "DEFAULT")       current = DEFAULT;
            if (keyword == "DELETE")        current = DELETE;
            if (keyword == "DESCRIBE")      current = DESCRIBE;
            if (keyword == "DISCONNECT")    current = DISCONNECT;
            if (keyword == "DO")            current = DO;
            if (keyword == "DROP")          current = DROP;
            if (keyword == "EXECUTE")       current = EXECUTE;
            if (keyword == "EXIT")          current = EXIT;
            if (keyword == "FETCH")         current = FETCH;
            if (keyword == "FOR")           current = FOR;
            if (keyword == "FOREACH")       current = FOREACH;
            if (keyword == "FOUND")         current = FOUND;
            if (keyword == "FROM")          current = FROM;
            if (keyword == "HOLD")          current = HOLD;
            if (keyword == "IMMEDIATE")     current = IMMEDIATE;
            if (keyword == "IN")            current = IN;
            if (keyword == "INSENSITIVE")   current = INSENSITIVE;
            if (keyword == "INSERT")        current = INSERT;
            if (keyword == "INTO")          current = INTO;
            if (keyword == "LOOP")          current = LOOP;
            if (keyword == "NEXT")          current = NEXT;
            if (keyword == "NO")            current = NO;
            if (keyword == "NOT")           current = NOT;
            if (keyword == "OPEN")          current = OPEN;
            if (keyword == "PREPARE")       current = PREPARE;
            if (keyword == "RAISE")         current = RAISE;
            if (keyword == "REPEAT")        current = REPEAT;
            if (keyword == "SCROLL")        current = SCROLL;
            if (keyword == "SELECT")        current = SELECT;
            if (keyword == "SET")           current = SET;
            if (keyword == "SQL")           current = SQL;
            if (keyword == "SQLERROR")      current = SQLERROR;
            if (keyword == "SQLPRINT")      current = SQLPRINT;
            if (keyword == "SQLWARNING")    current = SQLWARNING;
            if (keyword == "STOP")          current = STOP;
            if (keyword == "TO")            current = TO;
            if (keyword == "USING")         current = USING;
            if (keyword == "VALUES")        current = VALUES;
            if (keyword == "WHENEVER")      current = WHENEVER;
            if (keyword == "WHILE")         current = WHILE;
            if (keyword == "WITH")          current = WITH;
            if (keyword == "WITHOUT")       current = WITHOUT;
        } else {
            current = SYMBOL;
            current_value = statement[index];
            index++;
        }
        this_token = ::Token(token.source);
        this_token.line = token.line;
        this_token.column = token.column + current_index;
        this_token.text = statement.substr(current_index, index);
    }
};

class Parser {
public:
    explicit Parser(Lexer &lexer): lexer(lexer), assignments(), parameters() {}
    Parser(const Parser &) = delete;
    Parser &operator=(const Parser &) = delete;
    Lexer &lexer;
    std::vector<::Token> assignments;
    std::vector<::Token> parameters;

    bool check_next(Token expected)
    {
        return lexer.next() == expected;
    }

    std::unique_ptr<SqlIdentifier> identifier()
    {
        if (lexer.peek() == IDENTIFIER) {
            std::string ident = lexer.value();
            lexer.next();
            return std::unique_ptr<SqlIdentifier> { new SqlIdentifierSymbol(ident) };
        } else if (lexer.peek() == COLON) {
            lexer.next();
            if (lexer.peek() != IDENTIFIER) {
                error(4213, lexer.get_last_token(), "identifier expected");
            }
            std::string variable = lexer.value();
            lexer.next();
            return std::unique_ptr<SqlIdentifier> { new SqlIdentifierVariable(lexer.get_last_token()) };
        } else {
            error(4214, lexer.get_this_token(), "identifier or :variable expected");
        }
    }

    std::unique_ptr<SqlValue> value()
    {
        if (lexer.peek() == VALUE) {
            std::string value = lexer.value();
            lexer.next();
            return std::unique_ptr<SqlValue> { new SqlValueLiteral(value) };
        } else if (lexer.peek() == COLON) {
            lexer.next();
            if (lexer.peek() != IDENTIFIER) {
                error(4215, lexer.get_last_token(), "identifier expected");
            }
            std::string variable = lexer.value();
            lexer.next();
            return std::unique_ptr<SqlValue> { new SqlValueVariable(lexer.get_last_token()) };
        } else {
            error(4216, lexer.get_this_token(), "value or :variable expected");
        }
    }

    std::unique_ptr<SqlCloseStatement> parseSqlCloseStatement()
    {
        if (not check_next(CLOSE)) {
            internal_error("CLOSE expected");
        }
        std::unique_ptr<SqlIdentifier> name = identifier();
        return std::unique_ptr<SqlCloseStatement> { new SqlCloseStatement(std::move(name)) };
    }

    std::unique_ptr<SqlConnectStatement> parseSqlConnectStatement()
    {
        if (not check_next(CONNECT)) {
            internal_error("CONNECT expected");
        }
        if (not check_next(TO)) {
            error(4201, lexer.get_last_token(), "TO expected");
        }
        std::unique_ptr<SqlValue> target = value();
        std::unique_ptr<SqlIdentifier> name = nullptr;
        if (lexer.peek() == AS) {
            lexer.next();
            name = identifier();
        }
        return std::unique_ptr<SqlConnectStatement> { new SqlConnectStatement(std::move(target), std::move(name)) };
    }

    std::unique_ptr<SqlDeclareStatement> parseSqlDeclareStatement()
    {
        if (not check_next(DECLARE)) {
            internal_error("DECLARE expected");
        }
        std::unique_ptr<SqlIdentifier> name = identifier();
        bool binary = false;
        bool insensitive = false;
        bool scroll = false;
        bool hold = false;
        for (;;) {
            if (lexer.peek() == BINARY) {
                lexer.next();
                binary = true;
            } else if (lexer.peek() == INSENSITIVE) {
                lexer.next();
                insensitive = true;
            } else if (lexer.peek() == NO) {
                lexer.next();
                if (not check_next(SCROLL)) {
                    error(4202, lexer.get_last_token(), "SCROLL expected");
                }
                scroll = false;
            } else if (lexer.peek() == SCROLL) {
                lexer.next();
                scroll = true;
            } else {
                break;
            }
        }
        if (not check_next(CURSOR)) {
            error(4203, lexer.get_last_token(), "CURSOR expected");
        }
        if (lexer.peek() == WITH) {
            lexer.next();
            if (not check_next(HOLD)) {
                error(4204, lexer.get_last_token(), "HOLD expected");
            }
            hold = true;
        } else if (lexer.peek() == WITHOUT) {
            lexer.next();
            if (not check_next(HOLD)) {
                error(4205, lexer.get_last_token(), "HOLD expected");
            }
            hold = false;
        }
        if (not check_next(FOR)) {
            error(4206, lexer.get_last_token(), "FOR expected");
        }
        if (lexer.peek() == COLON || lexer.peek() == IDENTIFIER) {
            return std::unique_ptr<SqlDeclareStatement> { new SqlDeclarePreparedStatement(std::move(name), binary, insensitive, scroll, hold, identifier()) };
        } else if (lexer.peek() == SELECT || lexer.peek() == VALUES) {
            return std::unique_ptr<SqlDeclareStatement> { new SqlDeclareQueryStatement(std::move(name), binary, insensitive, scroll, hold, lexer.rest()) };
        } else {
            error(4217, lexer.get_this_token(), "SELECT, VALUES, or :variable expected");
        }
    }

    std::unique_ptr<SqlDisconnectStatement> parseSqlDisconnectStatement()
    {
        if (not check_next(DISCONNECT)) {
            internal_error("DISCONNECTED expected");
        }
        std::unique_ptr<SqlIdentifier> name;
        bool default_ = false;
        bool all = false;
        if (lexer.peek() == IDENTIFIER || lexer.peek() == COLON) {
            name = identifier();
        } else if (lexer.peek() == NONE) {
            name = nullptr;
        } else if (lexer.peek() == CURRENT) {
            lexer.next();
            name = nullptr;
        } else if (lexer.peek() == DEFAULT) {
            lexer.next();
            default_ = true;
        } else if (lexer.peek() == ALL) {
            lexer.next();
            all = true;
        } else {
            error(4218, lexer.get_this_token(), "connection type expected");
        }
        return std::unique_ptr<SqlDisconnectStatement> { new SqlDisconnectStatement(std::move(name), default_, all) };
    }

    std::unique_ptr<SqlExecuteStatement> parseSqlExecuteStatement()
    {
        if (not check_next(EXECUTE)) {
            internal_error("EXECUTE expected");
        }
        if (not check_next(IMMEDIATE)) {
            error(4207, lexer.get_last_token(), "IMMEDIATE expected");
        }
        return std::unique_ptr<SqlExecuteStatement> { new SqlExecuteStatement(value()) };
    }

    std::unique_ptr<SqlFetchStatement> parseSqlFetchStatement()
    {
        if (not check_next(FETCH)) {
            internal_error("FETCH expected");
        }
        if (lexer.peek() == NEXT) {
            lexer.next();
        } else {
            error(4219, lexer.get_this_token(), "NEXT expected");
        }
        if (lexer.peek() == FROM || lexer.peek() == IN) {
            lexer.next();
        } else {
            error(4220, lexer.get_this_token(), "FROM or IN expected");
        }
        std::unique_ptr<SqlIdentifier> name = identifier();
        std::string query;
        extractIntoClause(query);
        lexer.rest(); // TODO: USING
        return std::unique_ptr<SqlFetchStatement> { new SqlFetchStatement(std::move(name)) };
    }

    std::unique_ptr<SqlOpenStatement> parseSqlOpenStatement()
    {
        if (not check_next(OPEN)) {
            internal_error("OPEN expected");
        }
        std::unique_ptr<SqlIdentifier> name = identifier();
        std::vector<std::unique_ptr<SqlValue>> values;
        if (lexer.peek() == USING) {
            for (;;) {
                values.push_back(value());
                if (lexer.peek() != COMMA) {
                    break;
                }
                lexer.next();
            }
        }
        return std::unique_ptr<SqlOpenStatement> { new SqlOpenStatement(std::move(name), std::move(values)) };
    }

    std::unique_ptr<SqlPrepareStatement> parseSqlPrepareStatement()
    {
        if (not check_next(PREPARE)) {
            internal_error("PREPARE expected");
        }
        std::unique_ptr<SqlIdentifier> name = identifier();
        if (not check_next(FROM)) {
            error(4208, lexer.get_last_token(), "FROM expected");
        }
        std::unique_ptr<SqlValue> statement = value();
        return std::unique_ptr<SqlPrepareStatement> { new SqlPrepareStatement(std::move(name), std::move(statement)) };
    }

    std::unique_ptr<SqlSetConnectionStatement> parseSqlSetStatement()
    {
        if (not check_next(SET)) {
            internal_error("SET expected");
        }
        if (not check_next(CONNECTION)) {
            error(4209, lexer.get_last_token(), "CONNECTION expected");
        }
        if (lexer.peek() != TO && lexer.peek() != EQUALS) {
            error(4210, lexer.get_this_token(), "TO or '=' expected");
        }
        std::unique_ptr<SqlIdentifier> name = identifier();
        return std::unique_ptr<SqlSetConnectionStatement> { new SqlSetConnectionStatement(std::move(name)) };
    }

    std::unique_ptr<SqlWheneverStatement> parseSqlWheneverStatement()
    {
        if (not check_next(WHENEVER)) {
            internal_error("WHENEVER expected");
        }
        SqlWheneverCondition condition;
        switch (lexer.next()) {
            case NOT:
                if (not check_next(FOUND)) {
                    error(4211, lexer.get_last_token(), "FOUND expected");
                }
                condition = NotFound;
                break;
            case SQLERROR:
                condition = SqlError;
                break;
            case SQLWARNING:
                condition = SqlWarning;
                break;
            default:
                error(4221, lexer.get_last_token(), "NOT FOUND, SQLERROR, or SQLWARNING expected");
        }
        SqlWheneverAction action;
        switch (lexer.next()) {
            case CONTINUE: action = SqlWheneverAction::Continue; break;
            case SQLPRINT: action = SqlWheneverAction::SqlPrint; break;
            case STOP:     action = SqlWheneverAction::Stop;     break;
            case DO:
                switch (lexer.next()) {
                    case EXIT:
                        switch (lexer.next()) {
                            case LOOP:      action = SqlWheneverAction::DoExitLoop;    break;
                            case FOR:       action = SqlWheneverAction::DoExitFor;     break;
                            case FOREACH:   action = SqlWheneverAction::DoExitForeach; break;
                            case REPEAT:    action = SqlWheneverAction::DoExitRepeat;  break;
                            case WHILE:     action = SqlWheneverAction::DoExitWhile;   break;
                            default:
                                error(4222, lexer.get_last_token(), "loop type expected");
                        }
                        break;
                    case NEXT:
                        switch (lexer.next()) {
                            case LOOP:      action = SqlWheneverAction::DoNextLoop;    break;
                            case FOR:       action = SqlWheneverAction::DoNextFor;     break;
                            case FOREACH:   action = SqlWheneverAction::DoNextForeach; break;
                            case REPEAT:    action = SqlWheneverAction::DoNextRepeat;  break;
                            case WHILE:     action = SqlWheneverAction::DoNextWhile;   break;
                            default:
                                error(4223, lexer.get_last_token(), "loop type expected");
                        }
                        break;
                    case RAISE:
                        if (lexer.peek() == IDENTIFIER && lexer.value() == "SqlException") {
                            lexer.next();
                            action = SqlWheneverAction::DoRaiseException;
                        } else {
                            error(4224, lexer.get_this_token(), "SqlException expected");
                        }
                        break;
                    default:
                        error(4225, lexer.get_last_token(), "EXIT, NEXT, or RAISE expected");
                }
                break;
            default:
                error(4226, lexer.get_last_token(), "CONTINUE, SQLPRINT, STOP, or DO expected");
        }
        return std::unique_ptr<SqlWheneverStatement> { new SqlWheneverStatement(condition, action) };
    }

    void extractIntoClause(std::string &query)
    {
        auto start = lexer.position();
        while (lexer.peek() != NONE && lexer.peek() != INTO) {
            lexer.next();
        }
        if (lexer.peek() != INTO) {
            return;
        }
        auto into = lexer.position();
        lexer.next();
        for (;;) {
            if (lexer.peek() != COLON) {
                break;
            }
            lexer.next();
            if (lexer.peek() != IDENTIFIER) {
                error(4227, lexer.get_this_token(), "identifier expected");
            }
            // Make a temporary token and adjust the string to just the part we want.
            ::Token t = lexer.get_this_token();
            t.text = lexer.value();
            assignments.push_back(t);
            lexer.next();
            if (lexer.peek() != COMMA) {
                break;
            }
            lexer.next();
        }
        auto end = lexer.position();
        query = lexer.statement.substr(start, into-start) + lexer.statement.substr(end);
        lexer.rest();
    }

    void extractParameters(size_t query_offset, const std::string &query)
    {
        std::string name;
        std::string sql = query + " "; // add a non-identifier terminator for scanning this
        std::string::size_type i = 0;
        while (i < sql.length()) {
            if (sql[i] == ':') {
                name = ":";
            } else if (not name.empty()) {
                if (isalnum(sql[i])) {
                    name.push_back(sql[i]);
                } else {
                    ::Token tok = ::Token(lexer.token);
                    tok.column = tok.column + query_offset + i - name.length();
                    tok.text = name;
                    parameters.push_back(tok);
                    name = std::string();
                }
            }
            i++;
        }
    }

    std::unique_ptr<SqlStatement> parseSqlStatement()
    {
        if (not check_next(SQL)) {
            error(4212, lexer.get_last_token(), "SQL expected");
        }
        auto query_offset = lexer.position();
        auto query = lexer.peek_rest();
        switch (lexer.peek()) {
            case CLOSE:
                return parseSqlCloseStatement();
            case CONNECT:
                return parseSqlConnectStatement();
            case DECLARE:
                return parseSqlDeclareStatement();
            case DISCONNECT:
                return parseSqlDisconnectStatement();
            case EXECUTE:
                return parseSqlExecuteStatement();
            case FETCH:
                return parseSqlFetchStatement();
            case OPEN:
                return parseSqlOpenStatement();
            case PREPARE:
                return parseSqlPrepareStatement();
            case SET:
                return parseSqlSetStatement();
            case WHENEVER:
                return parseSqlWheneverStatement();
            case CREATE:
            case DELETE:
            case DROP:
            case INSERT:
                lexer.rest();
                extractParameters(query_offset, query);
                return std::unique_ptr<SqlStatement> { new SqlQueryStatement(query) };
            case SELECT:
                extractIntoClause(query);
                extractParameters(query_offset, query);
                return std::unique_ptr<SqlStatement> { new SqlQueryStatement(query) };
            default:
                lexer.rest();
                return std::unique_ptr<SqlStatement> { new SqlQueryStatement(query) };
        }
    }
};

} // namespace sql

std::unique_ptr<SqlStatementInfo> parseSqlStatement(const ::Token &token, const std::string &statement)
{
    sql::Lexer lexer(token, statement);
    sql::Parser parser(lexer);
    std::unique_ptr<SqlStatement> r = parser.parseSqlStatement();
    if (lexer.peek() != sql::NONE) {
        error(4229, lexer.get_this_token(), "';' expected");
    }
    return std::unique_ptr<SqlStatementInfo> { new SqlStatementInfo(std::move(r), parser.assignments, parser.parameters) };
}
