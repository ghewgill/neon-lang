#ifndef CELL_H
#define CELL_H

#include <map>
#include <memory>
#include <vector>

#include "number.h"
#include "object.h"
#include "utf8string.h"

// TODO: See if we can use std::variant (C++17) for this.

class Cell {
public:
    Cell();
    Cell(const Cell &rhs);
    explicit Cell(Cell *value);
    explicit Cell(bool value);
    explicit Cell(Number value);
    explicit Cell(const utf8string &value);
    explicit Cell(const char *value);
    explicit Cell(const std::vector<unsigned char> &value);
    explicit Cell(const std::shared_ptr<Object> &value);
    explicit Cell(const std::vector<Cell> &value, bool alloced = false);
    explicit Cell(const std::map<utf8string, Cell> &value);
    static Cell makeOther(void *p) { Cell r; r.type = Type::Other; r.other_ptr = p; return r; }
    Cell &operator=(const Cell &rhs);
    bool operator==(const Cell &rhs) const;

    enum class Type {
        None,
        Address,
        Boolean,
        Number,
        String,
        Bytes,
        Object,
        Array,
        Dictionary,
        Other
    };
    Type get_type() const { return type; }

    Cell *&address();
    bool &boolean();
    Number &number();
    const utf8string &string();
    utf8string &string_for_write();
    const std::vector<unsigned char> &bytes();
    void set_bytes(const std::vector<unsigned char> &bytes);
    std::shared_ptr<Object> object();
    const std::vector<Cell> &array();
    std::vector<Cell> &array_for_write();
    Cell &array_index_for_read(size_t i);
    Cell &array_index_for_write(size_t i);
    const std::map<utf8string, Cell> &dictionary();
    std::map<utf8string, Cell> &dictionary_for_write();
    Cell &dictionary_index_for_read(const utf8string &index);
    Cell &dictionary_index_for_write(const utf8string &index);
    void *&other();

    struct GC {
        explicit GC(bool alloced = false): alloced(alloced), marked(false) {}
        GC(const GC &) = delete;
        GC &operator=(const GC &) = delete;
        const bool alloced;
        bool marked;
    } gc;

private:
    Type type;
    Cell *address_value;
    bool boolean_value;
    Number number_value;
    std::shared_ptr<utf8string> string_ptr;
    std::shared_ptr<std::vector<unsigned char>> bytes_ptr;
    std::shared_ptr<Object> object_ptr;
    std::shared_ptr<std::vector<Cell>> array_ptr;
    std::shared_ptr<std::map<utf8string, Cell>> dictionary_ptr;
    void *other_ptr;
};

#endif
