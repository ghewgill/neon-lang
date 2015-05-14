#include "cell.h"

#include <assert.h>

Cell::Cell()
  : type(cNone),
    address_value(nullptr),
    boolean_value(false),
    number_value(),
    string_value(),
    array_ptr(),
    dictionary_value()
{
}

Cell::Cell(const Cell &rhs)
  : type(rhs.type),
    address_value(rhs.address_value),
    boolean_value(rhs.boolean_value),
    number_value(rhs.number_value),
    string_value(rhs.string_value),
    array_ptr(rhs.array_ptr),
    dictionary_value(rhs.dictionary_value)
{
}

Cell::Cell(Cell *value)
  : type(cAddress),
    address_value(value),
    boolean_value(false),
    number_value(),
    string_value(),
    array_ptr(),
    dictionary_value()
{
}

Cell::Cell(bool value)
  : type(cBoolean),
    address_value(nullptr),
    boolean_value(value),
    number_value(),
    string_value(),
    array_ptr(),
    dictionary_value()
{
}

Cell::Cell(Number value)
  : type(cNumber),
    address_value(nullptr),
    boolean_value(false),
    number_value(value),
    string_value(),
    array_ptr(),
    dictionary_value()
{
}

Cell::Cell(const std::string &value)
  : type(cString),
    address_value(nullptr),
    boolean_value(false),
    number_value(),
    string_value(value),
    array_ptr(),
    dictionary_value()
{
}

Cell::Cell(const char *value)
  : type(cString),
    address_value(nullptr),
    boolean_value(false),
    number_value(),
    string_value(value),
    array_ptr(),
    dictionary_value()
{
}

Cell::Cell(const std::vector<Cell> &value)
  : type(cArray),
    address_value(nullptr),
    boolean_value(false),
    number_value(),
    string_value(),
    array_ptr(std::make_shared<std::vector<Cell>>(value)),
    dictionary_value()
{
}

Cell::Cell(const std::map<std::string, Cell> &value)
  : type(cDictionary),
    address_value(nullptr),
    boolean_value(false),
    number_value(),
    string_value(),
    array_ptr(),
    dictionary_value(value)
{
}

Cell &Cell::operator=(const Cell &rhs)
{
    if (&rhs == this) {
        return *this;
    }
    type = rhs.type;
    address_value = rhs.address_value;
    boolean_value = rhs.boolean_value;
    number_value = rhs.number_value;
    string_value = rhs.string_value;
    array_ptr = rhs.array_ptr;
    dictionary_value = rhs.dictionary_value;
    return *this;
}

bool Cell::operator==(const Cell &rhs) const
{
    assert(type == rhs.type);
    switch (type) {
        case cNone:         return false;
        case cAddress:      return address_value == rhs.address_value;
        case cBoolean:      return boolean_value == rhs.boolean_value;
        case cNumber:       return number_is_equal(number_value, rhs.number_value);
        case cString:       return string_value == rhs.string_value;
        case cArray:        return *array_ptr == *rhs.array_ptr;
        case cDictionary:   return dictionary_value == rhs.dictionary_value;
    }
    return false;
}

Cell *&Cell::address()
{
    if (type == cNone) {
        type = cAddress;
    }
    assert(type == cAddress);
    return address_value;
}

bool &Cell::boolean()
{
    if (type == cNone) {
        type = cBoolean;
    }
    assert(type == cBoolean);
    return boolean_value;
}

Number &Cell::number()
{
    if (type == cNone) {
        type = cNumber;
    }
    assert(type == cNumber);
    return number_value;
}

std::string &Cell::string()
{
    if (type == cNone) {
        type = cString;
    }
    assert(type == cString);
    return string_value;
}

const std::vector<Cell> &Cell::array()
{
    if (type == cNone) {
        type = cArray;
    }
    assert(type == cArray);
    if (not array_ptr) {
        array_ptr = std::make_shared<std::vector<Cell>>();
    }
    return *array_ptr;
}

Cell &Cell::array_index_for_read(size_t i)
{
    if (not array_ptr) {
        array_ptr = std::make_shared<std::vector<Cell>>();
    }
    return array_ptr->at(i);
}

Cell &Cell::array_index_for_write(size_t i)
{
    if (not array_ptr) {
        array_ptr = std::make_shared<std::vector<Cell>>();
    }
    if (not array_ptr.unique()) {
        array_ptr = std::make_shared<std::vector<Cell>>(*array_ptr);
    }
    if (i >= array_ptr->size()) {
        array_ptr->resize(i+1);
    }
    return array_ptr->at(i);
}

std::map<std::string, Cell> &Cell::dictionary()
{
    if (type == cNone) {
        type = cDictionary;
    }
    assert(type == cDictionary);
    return dictionary_value;
}
