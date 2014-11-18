#ifndef CELL_H
#define CELL_H

#include <map>
#include <vector>

#include "number.h"

class Cell {
public:
    Cell();
    Cell(const Cell &rhs);
    explicit Cell(Cell *value);
    explicit Cell(bool value);
    explicit Cell(Number value);
    explicit Cell(const std::string &value);
    explicit Cell(const char *value);
    explicit Cell(const std::vector<Cell> &value);
    explicit Cell(const std::map<std::string, Cell> &value);
    Cell &operator=(const Cell &rhs);
    bool operator==(const Cell &rhs) const;

    Cell *address();
    bool boolean();
    Number number();
    std::string &string();
    std::vector<Cell> &array();
    std::map<std::string, Cell> &dictionary();

private:
    enum {
        cNone,
        cAddress,
        cBoolean,
        cNumber,
        cString,
        cArray,
        cDictionary
    } type;

    Cell *address_value;
    bool boolean_value;
    Number number_value;
    std::string string_value;
    std::vector<Cell> array_value;
    std::map<std::string, Cell> dictionary_value;
};

#endif
