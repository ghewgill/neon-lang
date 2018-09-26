#include <string>

#include <sqlite3.h>

#include "cell.h"
#include "rtl_exec.h"

static int callback(void *rowscell, int columns, char **values, char ** /*names*/)
{
    std::vector<Cell> *rows = static_cast<std::vector<Cell> *>(rowscell);
    std::vector<Cell> row;
    for (int i = 0; i < columns; i++) {
        row.push_back(Cell(values[i]));
    }
    rows->push_back(Cell(row));
    return 0;
}

namespace rtl {

namespace sqlite {

void *open(const utf8string &name)
{
    sqlite3 *db;
    int r = sqlite3_open(name.c_str(), &db);
    if (r != SQLITE_OK) {
        throw RtlException(global::Exception_SqlException, utf8string(sqlite3_errmsg(db)), r);
    }
    return db;
}

Cell exec(void *db, const utf8string &sql, const std::map<utf8string, utf8string> &parameters)
{
    std::vector<Cell> rows;
    sqlite3_stmt *stmt;
    int r = sqlite3_prepare_v2(static_cast<sqlite3 *>(db), sql.c_str(), -1, &stmt, NULL);
    if (r != SQLITE_OK) {
        throw RtlException(global::Exception_SqlException, utf8string(sqlite3_errmsg(static_cast<sqlite3 *>(db))), r);
    }
    for (auto p: parameters) {
        int c = sqlite3_bind_parameter_index(stmt, p.first.c_str());
        if (c == 0) {
            throw RtlException(Exception_SqliteException_ParameterName, utf8string(p.first.c_str()));
        }
        r = sqlite3_bind_text(stmt, c, p.second.c_str(), -1, SQLITE_TRANSIENT);
        if (r != SQLITE_OK) {
            throw RtlException(global::Exception_SqlException, utf8string(sqlite3_errmsg(static_cast<sqlite3 *>(db))), r);
        }
    }
    int columns = sqlite3_column_count(stmt);
    for (;;) {
        r = sqlite3_step(stmt);
        if (r == SQLITE_DONE) {
            break;
        }
        if (r == SQLITE_ROW) {
            std::vector<Cell> row;
            for (int i = 0; i < columns; i++) {
                row.push_back(Cell(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i))));
            }
            rows.push_back(Cell(row));
        } else {
            throw RtlException(global::Exception_SqlException, utf8string(sqlite3_errmsg(static_cast<sqlite3 *>(db))), r);
        }
    }
    sqlite3_finalize(stmt);
    return Cell(rows);
}

bool execOne(void *db, const utf8string &sql, const std::map<utf8string, utf8string> &parameters, Cell *result)
{
    sqlite3_stmt *stmt;
    int r = sqlite3_prepare_v2(static_cast<sqlite3 *>(db), sql.c_str(), -1, &stmt, NULL);
    if (r != SQLITE_OK) {
        throw RtlException(global::Exception_SqlException, utf8string(sqlite3_errmsg(static_cast<sqlite3 *>(db))), r);
    }
    for (auto p: parameters) {
        int c = sqlite3_bind_parameter_index(stmt, p.first.c_str());
        if (c == 0) {
            throw RtlException(Exception_SqliteException_ParameterName, utf8string(p.first.c_str()));
        }
        r = sqlite3_bind_text(stmt, c, p.second.c_str(), -1, SQLITE_TRANSIENT);
        if (r != SQLITE_OK) {
            throw RtlException(global::Exception_SqlException, utf8string(sqlite3_errmsg(static_cast<sqlite3 *>(db))), r);
        }
    }
    int columns = sqlite3_column_count(stmt);
    r = sqlite3_step(stmt);
    if (r == SQLITE_DONE) {
        //fprintf(stderr, "sqlite no data: %s\n", sql.c_str());
        return false;
    }
    if (r == SQLITE_ROW) {
        for (int i = 0; i < columns; i++) {
            result->array_for_write().push_back(Cell(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i))));
        }
    } else {
        throw RtlException(global::Exception_SqlException, utf8string(sqlite3_errmsg(static_cast<sqlite3 *>(db))), r);
    }
    sqlite3_finalize(stmt);
    return true;
}

Cell execRaw(void *db, const utf8string &sql)
{
    std::vector<Cell> rows;
    char *errmsg;
    int r = sqlite3_exec(static_cast<sqlite3 *>(db), sql.c_str(), callback, &rows, &errmsg);
    if (r != SQLITE_OK) {
        throw RtlException(global::Exception_SqlException, utf8string(sqlite3_errmsg(static_cast<sqlite3 *>(db))), r);
    }
    return Cell(rows);
}

void close(void *db)
{
    sqlite3_close(static_cast<sqlite3 *>(db));
}

class Cursor {
public:
    Cursor(sqlite3 *db, const utf8string &query): db(db), query(query), stmt(nullptr), columns(0) {}
    virtual ~Cursor() {}
    void open()
    {
        int r = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
        if (r != SQLITE_OK) {
            throw RtlException(global::Exception_SqlException, utf8string(sqlite3_errmsg(static_cast<sqlite3 *>(db))), r);
        }
        columns = sqlite3_column_count(stmt);
    }
    bool fetch(Cell *result)
    {
        int r = sqlite3_step(stmt);
        if (r == SQLITE_DONE) {
            //fprintf(stderr, "sqlite no data: %s\n", sql.c_str());
            return false;
        }
        if (r == SQLITE_ROW) {
            for (int i = 0; i < columns; i++) {
                result->array_for_write().push_back(Cell(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i))));
            }
        } else {
            throw RtlException(global::Exception_SqlException, utf8string(sqlite3_errmsg(static_cast<sqlite3 *>(db))), r);
        }
        return true;
    }
    void close()
    {
        sqlite3_finalize(stmt);
    }

    sqlite3 *db;
    const utf8string query;
    sqlite3_stmt *stmt;
    int columns;
private:
    Cursor(const Cursor &);
    Cursor &operator=(const Cursor &);
};

std::map<utf8string, std::unique_ptr<Cursor>> Cursors;

utf8string cursorDeclare(void *db, const utf8string &name, const utf8string &query)
{
    Cursors[name] = std::unique_ptr<Cursor> { new Cursor(static_cast<sqlite3 *>(db), query) };
    return name;
}

void cursorOpen(const utf8string &name)
{
    auto c = Cursors.find(name);
    if (c == Cursors.end()) {
        throw RtlException(global::Exception_SqlException, utf8string(name));
    }
    c->second->open();
}

bool cursorFetch(const utf8string &name, Cell *result)
{
    auto c = Cursors.find(name);
    if (c == Cursors.end()) {
        throw RtlException(global::Exception_SqlException, utf8string(name));
    }
    return c->second->fetch(result);
}

void cursorClose(const utf8string &name)
{
    auto c = Cursors.find(name);
    if (c == Cursors.end()) {
        throw RtlException(global::Exception_SqlException, utf8string(name));
    }
    c->second->close();
    Cursors.erase(c);
}

} // namespace sqlite

} // namespace rtl
