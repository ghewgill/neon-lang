#include "cell.h"

#include <assert.h>
#include <iso646.h>

Cell::Cell()
  : gc(),
    type(Type::None),
    address_value(nullptr),
    boolean_value(false),
    number_value(),
    string_ptr(),
    array_ptr(),
    dictionary_ptr()
{
}

Cell::Cell(const Cell &rhs)
  : gc(),
    type(rhs.type),
    address_value(rhs.address_value),
    boolean_value(rhs.boolean_value),
    number_value(rhs.number_value),
    string_ptr(rhs.string_ptr),
    array_ptr(rhs.array_ptr),
    dictionary_ptr(rhs.dictionary_ptr)
{
}

Cell::Cell(Cell *value)
  : gc(),
    type(Type::Address),
    address_value(value),
    boolean_value(false),
    number_value(),
    string_ptr(),
    array_ptr(),
    dictionary_ptr()
{
}

Cell::Cell(bool value)
  : gc(),
    type(Type::Boolean),
    address_value(nullptr),
    boolean_value(value),
    number_value(),
    string_ptr(),
    array_ptr(),
    dictionary_ptr()
{
}

Cell::Cell(Number value)
  : gc(),
    type(Type::Number),
    address_value(nullptr),
    boolean_value(false),
    number_value(value),
    string_ptr(),
    array_ptr(),
    dictionary_ptr()
{
}

Cell::Cell(const utf8string &value)
  : gc(),
    type(Type::String),
    address_value(nullptr),
    boolean_value(false),
    number_value(),
    string_ptr(std::make_shared<utf8string>(value)),
    array_ptr(),
    dictionary_ptr()
{
}

Cell::Cell(const char *value)
  : gc(),
    type(Type::String),
    address_value(nullptr),
    boolean_value(false),
    number_value(),
    string_ptr(std::make_shared<utf8string>(value)),
    array_ptr(),
    dictionary_ptr()
{
}

Cell::Cell(const std::vector<Cell> &value, bool alloced)
  : gc(alloced),
    type(Type::Array),
    address_value(nullptr),
    boolean_value(false),
    number_value(),
    string_ptr(),
    array_ptr(std::make_shared<std::vector<Cell>>(value)),
    dictionary_ptr()
{
}

Cell::Cell(const std::map<utf8string, Cell> &value)
  : gc(),
    type(Type::Dictionary),
    address_value(nullptr),
    boolean_value(false),
    number_value(),
    string_ptr(),
    array_ptr(),
    dictionary_ptr(std::make_shared<std::map<utf8string, Cell>>(value))
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
    string_ptr = rhs.string_ptr;
    array_ptr = rhs.array_ptr;
    dictionary_ptr = rhs.dictionary_ptr;
    return *this;
}

bool Cell::operator==(const Cell &rhs) const
{
    assert(type == rhs.type);
    switch (type) {
        case Type::None:         return false;
        case Type::Address:      return address_value == rhs.address_value;
        case Type::Boolean:      return boolean_value == rhs.boolean_value;
        case Type::Number:       return number_is_equal(number_value, rhs.number_value);
        case Type::String:       return *string_ptr == *rhs.string_ptr;
        case Type::Array:        return *array_ptr == *rhs.array_ptr;
        case Type::Dictionary:   return *dictionary_ptr == *rhs.dictionary_ptr;
    }
    return false;
}

Cell *&Cell::address()
{
    if (type == Type::None) {
        type = Type::Address;
    }
    assert(type == Type::Address);
    return address_value;
}

bool &Cell::boolean()
{
    if (type == Type::None) {
        type = Type::Boolean;
    }
    assert(type == Type::Boolean);
    return boolean_value;
}

Number &Cell::number()
{
    if (type == Type::None) {
        type = Type::Number;
    }
    assert(type == Type::Number);
    return number_value;
}

const utf8string &Cell::string()
{
    if (type == Type::None) {
        type = Type::String;
    }
    assert(type == Type::String);
    if (not string_ptr) {
        string_ptr = std::make_shared<utf8string>();
    }
    return *string_ptr;
}

utf8string &Cell::string_for_write()
{
    if (type == Type::None) {
        type = Type::String;
    }
    assert(type == Type::String);
    if (not string_ptr) {
        string_ptr = std::make_shared<utf8string>();
    }
    if (not string_ptr.unique()) {
        string_ptr = std::make_shared<utf8string>(*string_ptr);
    }
    return *string_ptr;
}

const std::vector<Cell> &Cell::array()
{
    if (type == Type::None) {
        type = Type::Array;
    }
    assert(type == Type::Array);
    if (not array_ptr) {
        array_ptr = std::make_shared<std::vector<Cell>>();
    }
    return *array_ptr;
}

std::vector<Cell> &Cell::array_for_write()
{
    if (type == Type::None) {
        type = Type::Array;
    }
    assert(type == Type::Array);
    if (not array_ptr) {
        array_ptr = std::make_shared<std::vector<Cell>>();
    }
    if (not array_ptr.unique()) {
        array_ptr = std::make_shared<std::vector<Cell>>(*array_ptr);
    }
    return *array_ptr;
}

Cell &Cell::array_index_for_read(size_t i)
{
    if (type == Type::None) {
        type = Type::Array;
    }
    assert(type == Type::Array);
    if (not array_ptr) {
        array_ptr = std::make_shared<std::vector<Cell>>();
    }
    return array_ptr->at(i);
}

Cell &Cell::array_index_for_write(size_t i)
{
    if (type == Type::None) {
        type = Type::Array;
    }
    assert(type == Type::Array);
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

const std::map<utf8string, Cell> &Cell::dictionary()
{
    if (type == Type::None) {
        type = Type::Dictionary;
    }
    assert(type == Type::Dictionary);
    if (not dictionary_ptr) {
        dictionary_ptr = std::make_shared<std::map<utf8string, Cell>>();
    }
    return *dictionary_ptr;
}

std::map<utf8string, Cell> &Cell::dictionary_for_write()
{
    if (type == Type::None) {
        type = Type::Dictionary;
    }
    assert(type == Type::Dictionary);
    if (not dictionary_ptr) {
        dictionary_ptr = std::make_shared<std::map<utf8string, Cell>>();
    }
    if (not dictionary_ptr.unique()) {
        dictionary_ptr = std::make_shared<std::map<utf8string, Cell>>(*dictionary_ptr);
    }
    return *dictionary_ptr;
}

Cell &Cell::dictionary_index_for_read(const utf8string &index)
{
    if (type == Type::None) {
        type = Type::Dictionary;
    }
    assert(type == Type::Dictionary);
    if (not dictionary_ptr) {
        dictionary_ptr = std::make_shared<std::map<utf8string, Cell>>();
    }
    return dictionary_ptr->at(index);
}

Cell &Cell::dictionary_index_for_write(const utf8string &index)
{
    if (type == Type::None) {
        type = Type::Dictionary;
    }
    assert(type == Type::Dictionary);
    if (not dictionary_ptr) {
        dictionary_ptr = std::make_shared<std::map<utf8string, Cell>>();
    }
    if (not dictionary_ptr.unique()) {
        dictionary_ptr = std::make_shared<std::map<utf8string, Cell>>(*dictionary_ptr);
    }
    return dictionary_ptr->operator[](index);
}
