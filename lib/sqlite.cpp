#include <string>

#include <sqlite3.h>

#include "cell.h"
#include "rtl_exec.h"

class DatabaseObject: public Object {
public:
    explicit DatabaseObject(sqlite3 *db): db(db) {}
    DatabaseObject(const DatabaseObject &) = delete;
    DatabaseObject &operator=(const DatabaseObject &) = delete;
    virtual utf8string toString() const { return utf8string("<SQLITE " + std::to_string(reinterpret_cast<intptr_t>(db)) + ">"); }
    sqlite3 *db;
};

static DatabaseObject *check_database(const std::shared_ptr<Object> &pdb)
{
    DatabaseObject *db = dynamic_cast<DatabaseObject *>(pdb.get());
    if (db == nullptr || db->db == NULL) {
        throw RtlException(rtl::ne_sqlite::Exception_SqliteException_InvalidDatabase, utf8string(""));
    }
    return db;
}

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

namespace ne_sqlite {

std::shared_ptr<Object> open(const utf8string &name)
{
    sqlite3 *db;
    int r = sqlite3_open(name.c_str(), &db);
    if (r != SQLITE_OK) {
        throw RtlException(ne_global::Exception_SqlException, utf8string(sqlite3_errmsg(db)));
    }
    return std::shared_ptr<Object> { new DatabaseObject(db) };
}

Cell exec(const std::shared_ptr<Object> &pdb, const utf8string &sql, const std::map<utf8string, utf8string> &parameters)
{
    DatabaseObject *db = check_database(pdb);
    std::vector<Cell> rows;
    sqlite3_stmt *stmt;
    int r = sqlite3_prepare_v2(db->db, sql.c_str(), -1, &stmt, NULL);
    if (r != SQLITE_OK) {
        throw RtlException(ne_global::Exception_SqlException, utf8string(sqlite3_errmsg(db->db)));
    }
    for (auto p: parameters) {
        int c = sqlite3_bind_parameter_index(stmt, p.first.c_str());
        if (c == 0) {
            throw RtlException(Exception_SqliteException_ParameterName, utf8string(p.first.c_str()));
        }
        r = sqlite3_bind_text(stmt, c, p.second.c_str(), -1, SQLITE_TRANSIENT);
        if (r != SQLITE_OK) {
            throw RtlException(ne_global::Exception_SqlException, utf8string(sqlite3_errmsg(db->db)));
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
            throw RtlException(ne_global::Exception_SqlException, utf8string(sqlite3_errmsg(db->db)));
        }
    }
    sqlite3_finalize(stmt);
    return Cell(rows);
}

bool execOne(const std::shared_ptr<Object> &pdb, const utf8string &sql, const std::map<utf8string, utf8string> &parameters, Cell *result)
{
    DatabaseObject *db = check_database(pdb);
    sqlite3_stmt *stmt;
    int r = sqlite3_prepare_v2(db->db, sql.c_str(), -1, &stmt, NULL);
    if (r != SQLITE_OK) {
        throw RtlException(ne_global::Exception_SqlException, utf8string(sqlite3_errmsg(db->db)));
    }
    for (auto p: parameters) {
        int c = sqlite3_bind_parameter_index(stmt, p.first.c_str());
        if (c == 0) {
            throw RtlException(Exception_SqliteException_ParameterName, utf8string(p.first.c_str()));
        }
        r = sqlite3_bind_text(stmt, c, p.second.c_str(), -1, SQLITE_TRANSIENT);
        if (r != SQLITE_OK) {
            throw RtlException(ne_global::Exception_SqlException, utf8string(sqlite3_errmsg(db->db)));
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
            const unsigned char *value = sqlite3_column_text(stmt, i);
            if (value != nullptr) {
                result->array_for_write().push_back(Cell(reinterpret_cast<const char *>(value)));
            } else {
                result->array_for_write().push_back(Cell(""));
            }
        }
    } else {
        throw RtlException(ne_global::Exception_SqlException, utf8string(sqlite3_errmsg(db->db)));
    }
    sqlite3_finalize(stmt);
    return true;
}

Cell execRaw(const std::shared_ptr<Object> &pdb, const utf8string &sql)
{
    DatabaseObject *db = check_database(pdb);
    std::vector<Cell> rows;
    char *errmsg;
    int r = sqlite3_exec(db->db, sql.c_str(), callback, &rows, &errmsg);
    if (r != SQLITE_OK) {
        throw RtlException(ne_global::Exception_SqlException, utf8string(sqlite3_errmsg(db->db)));
    }
    return Cell(rows);
}

void close(const std::shared_ptr<Object> &pdb)
{
    DatabaseObject *db = check_database(pdb);
    sqlite3_close(db->db);
    db->db = NULL;
}

class Cursor {
public:
    Cursor(sqlite3 *db, const utf8string &query): db(db), query(query), stmt(nullptr), columns(0) {}
    virtual ~Cursor() {}
    void open()
    {
        int r = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
        if (r != SQLITE_OK) {
            throw RtlException(ne_global::Exception_SqlException, utf8string(sqlite3_errmsg(db)));
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
            throw RtlException(ne_global::Exception_SqlException, utf8string(sqlite3_errmsg(db)));
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

utf8string cursorDeclare(const std::shared_ptr<Object> &pdb, const utf8string &name, const utf8string &query)
{
    DatabaseObject *db = check_database(pdb);
    Cursors[name] = std::unique_ptr<Cursor> { new Cursor(db->db, query) };
    return name;
}

void cursorOpen(const utf8string &name)
{
    auto c = Cursors.find(name);
    if (c == Cursors.end()) {
        throw RtlException(ne_global::Exception_SqlException, utf8string(name));
    }
    c->second->open();
}

bool cursorFetch(const utf8string &name, Cell *result)
{
    auto c = Cursors.find(name);
    if (c == Cursors.end()) {
        throw RtlException(ne_global::Exception_SqlException, utf8string(name));
    }
    return c->second->fetch(result);
}

void cursorClose(const utf8string &name)
{
    auto c = Cursors.find(name);
    if (c == Cursors.end()) {
        throw RtlException(ne_global::Exception_SqlException, utf8string(name));
    }
    c->second->close();
    Cursors.erase(c);
}

} // namespace ne_sqlite

} // namespace rtl
