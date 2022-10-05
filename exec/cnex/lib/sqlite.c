#include "sqlite.h"

#include <inttypes.h>

#include "assert.h"
#include "cell.h"
#include "exec.h"
#include "nstring.h"
#include "number.h"
#include "object.h"
#include "stack.h"

#include "sqlite3.h"

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif


static Dictionary *Cursors;

void sqlite_initModule(void)
{
    Cursors = dictionary_createDictionary();
}



inline static Object *check_database(Cell *pdb)
{
    if (pdb->type != cObject || pdb->object == NULL || pdb->object->ptr == NULL) {
        return NULL;
    }
    return pdb->object;
}


void object_releaseDatabaseObject(Object *o)
{
    if (o != NULL) {
        assert(o->refcount > 0);
        o->refcount--;
        if (o->refcount <= 0) {
            if (o->ptr != NULL) {
                sqlite3_close_v2(o->ptr);
                o->ptr = NULL;
            }
            free(o);
        }
    }
}

Cell *object_databaseObjectToString(Object *self)
{
    char s[32];
    snprintf(s, sizeof(s), "<SQLITE %p>", self->ptr);
    Cell *r = cell_fromCString(s);
    return r;
}

Object *object_createDatabaseObject(sqlite3 *db)
{
    Object *r = object_createObject();
    r->ptr = db;
    r->release = object_releaseDatabaseObject;
    r->toString = object_databaseObjectToString;

    return r;
}



typedef struct tagTCursor {
    TExecutor *exec;
    sqlite3 *db;
    char *query;
    sqlite3_stmt *stmt;
    int columns;
} Cursor;

static void openCursor(Cursor *self)
{
    int r = sqlite3_prepare_v2(self->db, self->query, -1, &self->stmt, NULL);
    if (r != SQLITE_OK) {
        self->exec->rtl_raise(self->exec , "SqlException", sqlite3_errmsg(self->db));
        return;
    }
    self->columns = sqlite3_column_count(self->stmt);
}

static BOOL fetchCursor(Cursor *self, Cell *result)
{
    int r = sqlite3_step(self->stmt);
    if (r == SQLITE_DONE) {
        return FALSE;
    }
    if (r == SQLITE_ROW) {
        for (int i = 0; i < self->columns; i++) {
            Cell *col = cell_fromCString((const char*)sqlite3_column_text(self->stmt, i));
            cell_arrayAppendElementPointer(result, col);
        }
    } else {
        self->exec->rtl_raise(self->exec, "SqlException", sqlite3_errmsg(self->db));
        return FALSE;
    }
    return TRUE;
}

static void closeCursor(Cursor *self)
{
    sqlite3_finalize(self->stmt);
    self->stmt = NULL;
}

static void freeCursor(Cursor *self)
{
    if (self->stmt) {
        closeCursor(self);
    }
    free(self->query);
    free(self);
}

static Cursor *declareCursor(TExecutor *exec, sqlite3 *db, char *query)
{
    Cursor *r = malloc(sizeof(Cursor));
    if (r == NULL) {
        fatal_error("Could not alloc SQLite Cursor.\n");
    }

    r->db = db;
    r->query = query;
    r->stmt = NULL;
    r->columns = 0;
    r->exec = exec;

    return r;
}

static int callback(void *rowscell, int columns, char **values, char **names)
{
    (void)names; // Unused parameter
    Cell *row = cell_createArrayCell(columns);
    for (int i = 0; i < columns; i++) {
        row->array->data[i].type = cString;
        if (values[i] != NULL) {
            row->array->data[i].string = string_createCString(values[i]);
        } else {
            row->array->data[i].string = string_createCString("");
        }
    }
    cell_arrayAppendElementPointer(rowscell, row);
    return 0;
}



void sqlite_open(TExecutor *exec)
{
    char *name = string_asCString(top(exec->stack)->string); pop(exec->stack);

    sqlite3 *db;
    int r = sqlite3_open(name, &db);
    if (r != SQLITE_OK) {
        exec->rtl_raise(exec, "SqlException", sqlite3_errmsg(db));
        goto cleanup;
    }

    push(exec->stack, cell_fromObject(object_createDatabaseObject(db)));

cleanup:
    free(name);
}

void sqlite_exec(TExecutor *exec)
{
    Cell *parameters = cell_fromCell(top(exec->stack)); pop(exec->stack);
    char *sql = string_asCString(top(exec->stack)->string); pop(exec->stack);
    Object *db = check_database(top(exec->stack)); pop(exec->stack);

    sqlite3_stmt *stmt = NULL;
    if (db == NULL) {
        exec->rtl_raise(exec, "SqliteException.InvalidDatabase", "");
        goto cleanup;
    }
    Cell *rows = cell_createArrayCell(0);
    int r = sqlite3_prepare_v2(db->ptr, sql, -1, &stmt, NULL);
    if (r != SQLITE_OK) {
        exec->rtl_raise(exec, "SqlException", sqlite3_errmsg(db->ptr));
        goto cleanup;
    }
    for (int i = 0; i < parameters->dictionary->len; i++) {
        int c = sqlite3_bind_parameter_index(stmt, TCSTR(parameters->dictionary->data[i].key));
        if (c == 0) {
            exec->rtl_raise(exec, "SqlException.ParameterName", TCSTR(parameters->dictionary->data[i].key));
            goto cleanup;
        }
        r = sqlite3_bind_text(stmt, c, TCSTR(parameters->dictionary->data[i].value->string), -1, SQLITE_TRANSIENT);
        if (r != SQLITE_OK) {
            exec->rtl_raise(exec, "SqlException", sqlite3_errmsg(db->ptr));
            goto cleanup;
        }
    }
    int columns = sqlite3_column_count(stmt);
    for (;;) {
        r = sqlite3_step(stmt);
        if (r == SQLITE_DONE) {
            break;
        }
        if (r == SQLITE_ROW) {
            Cell *row = cell_createArrayCell(0);
            for (int i = 0; i < columns; i++) {
                const unsigned char *value = sqlite3_column_text(stmt, i);
                if (value != NULL) {
                    cell_arrayAppendElementPointer(row, cell_fromCString((const char *)value));
                } else {
                    cell_arrayAppendElementPointer(row, cell_fromCString(""));
                }
            }
            cell_arrayAppendElementPointer(rows, row);
        } else {
            exec->rtl_raise(exec, "SqlException", sqlite3_errmsg(db->ptr));
            goto cleanup;
        }
    }
    push(exec->stack, rows);

cleanup:
    sqlite3_finalize(stmt);
    free(sql);
    cell_freeCell(parameters);
}

void sqlite_execOne(TExecutor *exec)
{
    Cell *parameters = cell_fromCell(top(exec->stack)); pop(exec->stack);
    char *sql = string_asCString(top(exec->stack)->string); pop(exec->stack);
    Object *db = check_database(top(exec->stack)); pop(exec->stack);

    sqlite3_stmt *stmt = NULL;

    if (db == NULL) {
        exec->rtl_raise(exec, "SqliteException.InvalidDatabase", "");
        goto cleanup;
    }

    Cell *result = cell_createArrayCell(0);
    BOOL Retval = FALSE;

    int r = sqlite3_prepare_v2(db->ptr, sql, -1, &stmt, NULL);
    if (r != SQLITE_OK) {
        exec->rtl_raise(exec, "SqlException", sqlite3_errmsg(db->ptr));
        goto cleanup;
    }
    for (int i = 0; i < parameters->dictionary->len; i++) {
        int c = sqlite3_bind_parameter_index(stmt, TCSTR(parameters->dictionary->data[i].key));
        if (c == 0) {
            exec->rtl_raise(exec, "SqlException.ParameterName", TCSTR(parameters->dictionary->data[i].key));
            goto cleanup;
        }
        r = sqlite3_bind_text(stmt, c, TCSTR(parameters->dictionary->data[i].value->string), -1, SQLITE_TRANSIENT);
        if (r != SQLITE_OK) {
            exec->rtl_raise(exec, "SqlException", sqlite3_errmsg(db->ptr));
            goto cleanup;
        }
    }
    int columns = sqlite3_column_count(stmt);
    r = sqlite3_step(stmt);
    if (r == SQLITE_DONE) {
        goto done;
    }
    if (r == SQLITE_ROW) {
        for (int i = 0; i < columns; i++) {
            const unsigned char *value = sqlite3_column_text(stmt, i);
            if (value != NULL) {
                cell_arrayAppendElementPointer(result, cell_fromCString((const char *)value));
            } else {
                cell_arrayAppendElementPointer(result, cell_fromCString(""));
            }
        }
    } else {
        exec->rtl_raise(exec, "SqlException", sqlite3_errmsg(db->ptr));
        goto cleanup;
    }
    Retval = TRUE;

done:
    push(exec->stack, cell_fromBoolean(Retval));
    push(exec->stack, result);

cleanup:
    sqlite3_finalize(stmt);
    free(sql);
    cell_freeCell(parameters);
}

void sqlite_execRaw(TExecutor *exec)
{
    char *sql = string_asCString(top(exec->stack)->string); pop(exec->stack);
    Object *db = check_database(top(exec->stack)); pop(exec->stack);

    if (db == NULL) {
        exec->rtl_raise(exec, "SqliteException.InvalidDatabase", "");
        goto cleanup;
    }

    Cell *rows = cell_createArrayCell(0);
    char *errmsg = NULL;
    int r = sqlite3_exec(db->ptr, sql, callback, rows, &errmsg);
    if (r != SQLITE_OK) {
        exec->rtl_raise(exec, "SqlException", sqlite3_errmsg(db->ptr));
        goto cleanup;
    }

    push(exec->stack, rows);

cleanup:
    free(sql);
}

void sqlite_close(TExecutor *exec)
{
    Object *db = check_database(top(exec->stack)); pop(exec->stack);

    if (db == NULL) {
        exec->rtl_raise(exec, "SqliteException.InvalidDatabase", "");
        return;
    }

    int r = sqlite3_close(db->ptr);
    if (r == SQLITE_OK) {
        db->ptr = NULL;
    } else {
        exec->rtl_raise(exec, "SqlException", sqlite3_errmsg(db->ptr));
    }
}

void sqlite_cursorDeclare(TExecutor *exec)
{
    char *query = string_asCString(top(exec->stack)->string); pop(exec->stack);
    TString *name = string_fromString(top(exec->stack)->string); pop(exec->stack);
    Object *db = check_database(top(exec->stack)); pop(exec->stack);

    if (db == NULL) {
        exec->rtl_raise(exec, "SqliteException.InvalidDatabase", "");
        free(query);
        string_freeString(name);
        return;
    }

    // It is important to note that query will be freed in freeCursor(), so we simply pass it along and don't bother to make a copy of it.
    Cell *c = cell_createOtherCell(declareCursor(exec, db->ptr, query));

    // It is again, important to note that name is also preserved, as it becomes the active KEY for the dictionary entry, so once again, we pass it along.
    dictionary_addDictionaryEntry(Cursors, name, c, -1);

    push(exec->stack, cell_fromString(name));
}

void sqlite_cursorOpen(TExecutor *exec)
{
    Cell *name = cell_fromCell(top(exec->stack)); pop(exec->stack);

    Cell *c = dictionary_findDictionaryEntry(Cursors, name->string);
    if (c == NULL) {
        exec->rtl_raise(exec, "SqlException", TCSTR(name->string));
        goto done;
    }
    openCursor(c->other);

done:
    cell_freeCell(name);
}

void sqlite_cursorFetch(TExecutor *exec)
{
    Cell *name = cell_fromCell(top(exec->stack)); pop(exec->stack);

    Cell *c = dictionary_findDictionaryEntry(Cursors, name->string);
    if (c == NULL) {
        exec->rtl_raise(exec, "SqlException", TCSTR(name->string));
        goto done;
    }
    Cell *result = cell_createArrayCell(0);

    push(exec->stack, cell_fromBoolean(fetchCursor(c->other, result)));
    push(exec->stack, result);

done:
    cell_freeCell(name);
}

void sqlite_cursorClose(TExecutor *exec)
{
    Cell *name = cell_fromCell(top(exec->stack)); pop(exec->stack);

    Cell *c = dictionary_findDictionaryEntry(Cursors, name->string);
    if (c == NULL) {
        exec->rtl_raise(exec, "SqlException", TCSTR(name->string));
        goto cleanup;
    }

    closeCursor(c->other);
    freeCursor(c->other);
    dictionary_removeDictionaryEntry(Cursors, name->string);

cleanup:
    cell_freeCell(name);
}



void sqlite_shutdownModule(void)
{
    // Attempt to close and clean up any remaining cursors
    for (int64_t i = 0; i < Cursors->len; i++) {
        if (Cursors->data[i].value != NULL && Cursors->data[i].value->other != NULL) {
            freeCursor(Cursors->data[i].value->other);
        }
    }
    dictionary_freeDictionary(Cursors);
}
