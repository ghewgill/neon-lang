#include <string>

#include <sqlite3.h>

#include "cell.h"

static int callback(void *rowscell, int columns, char **values, char ** /*names*/)
{
    Cell *rows = static_cast<Cell *>(rowscell);
    std::vector<Cell> row;
    for (int i = 0; i < columns; i++) {
        row.push_back(Cell(values[i]));
    }
    rows->array().push_back(Cell(row));
    return 0;
}

namespace rtl {

void *sqlite$open(const std::string &name)
{
    sqlite3 *db;
    int r = sqlite3_open(name.c_str(), &db);
    if (r != SQLITE_OK) {
    }
    return db;
}

Cell sqlite$exec(void *db, const std::string &sql)
{
    Cell rows;
    char *errmsg;
    int r = sqlite3_exec(static_cast<sqlite3 *>(db), sql.c_str(), callback, &rows, &errmsg);
    if (r != SQLITE_OK) {
        fprintf(stderr, "sqlite3_exec error: %s\n", errmsg);
    }
    return rows;
}

void sqlite$close(void *db)
{
    sqlite3_close(static_cast<sqlite3 *>(db));
}

} // namespace rtl
