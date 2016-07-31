#include <string>

#include <sqlite3.h>

#include "cell.h"

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

Cell exec(void *db, const std::string &sql)
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
