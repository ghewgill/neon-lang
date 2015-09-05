#ifndef CELL_H
#define CELL_H

#include <map>
#include <memory>
#include <vector>

#include "number.h"
#include "utf8string.h"

class Cell {
public:
    Cell();
    Cell(const Cell &rhs);
    explicit Cell(Cell *value);
    explicit Cell(bool value);
    explicit Cell(Number value);
    explicit Cell(const utf8string &value);
    explicit Cell(const char *value);
    explicit Cell(const std::vector<Cell> &value, bool alloced = false);
    explicit Cell(const std::map<utf8string, Cell> &value);
    Cell &operator=(const Cell &rhs);
    bool operator==(const Cell &rhs) const;

    enum Type {
        cNone,
        cAddress,
        cBoolean,
        cNumber,
        cString,
        cArray,
        cDictionary
    };
    Type get_type() const { return type; }

    Cell *&address();
    bool &boolean();
    Number &number();
    const utf8string &string();
    utf8string &string_for_write();
    const std::vector<Cell> &array();
    std::vector<Cell> &array_for_write();
    Cell &array_index_for_read(size_t i);
    Cell &array_index_for_write(size_t i);
    const std::map<utf8string, Cell> &dictionary();
    std::map<utf8string, Cell> &dictionary_for_write();
    Cell &dictionary_index_for_read(const utf8string &index);
    Cell &dictionary_index_for_write(const utf8string &index);

    struct GC {
        GC(bool alloced = false): alloced(alloced), marked(false) {}
        const bool alloced;
        bool marked;
    } gc;

private:
    Type type;
    Cell *address_value;
    bool boolean_value;
    Number number_value;
    std::shared_ptr<utf8string> string_ptr;
    std::shared_ptr<std::vector<Cell>> array_ptr;
    std::shared_ptr<std::map<utf8string, Cell>> dictionary_ptr;
};

#endif
