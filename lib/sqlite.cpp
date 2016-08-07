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

void *open(const std::string &name)
{
    sqlite3 *db;
    int r = sqlite3_open(name.c_str(), &db);
    if (r != SQLITE_OK) {
    }
    return db;
}

Cell exec(void *db, const std::string &sql, const std::map<utf8string, utf8string> &parameters)
{
    std::vector<Cell> rows;
    sqlite3_stmt *stmt;
    int r = sqlite3_prepare_v2(static_cast<sqlite3 *>(db), sql.c_str(), -1, &stmt, NULL);
    if (r != SQLITE_OK) {
        fprintf(stderr, "sqlite3_prepare error %d ext=%d\n", r, sqlite3_extended_errcode(static_cast<sqlite3 *>(db)));
    }
    for (auto p: parameters) {
        int c = sqlite3_bind_parameter_index(stmt, p.first.c_str());
        if (c == 0) {
            throw RtlException(Exception_ParameterNameException, p.first.c_str());
        }
        r = sqlite3_bind_text(stmt, c, p.second.c_str(), -1, SQLITE_TRANSIENT);
        if (r != SQLITE_OK) {
            fprintf(stderr, "sqlite3_bind_text error\n");
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
            fprintf(stderr, "sqlite3_step error\n");
            break;
        }
    }
    return Cell(rows);
}

Cell execRaw(void *db, const std::string &sql)
{
    std::vector<Cell> rows;
    char *errmsg;
    int r = sqlite3_exec(static_cast<sqlite3 *>(db), sql.c_str(), callback, &rows, &errmsg);
    if (r != SQLITE_OK) {
        fprintf(stderr, "sqlite3_exec error: %s\n", errmsg);
    }
    return Cell(rows);
}

void close(void *db)
{
    sqlite3_close(static_cast<sqlite3 *>(db));
}

} // namespace sqlite

} // namespace rtl
