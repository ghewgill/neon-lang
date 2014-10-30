#ifndef CELL_H
#define CELL_H

#include <map>
#include <vector>

#include "number.h"

class Variant {
public:
    Variant();
    explicit Variant(const Variant &rhs);
    explicit Variant(Variant *value);
    explicit Variant(bool value);
    explicit Variant(Number value);
    explicit Variant(const std::string &value);
    explicit Variant(const char *value);
    explicit Variant(const std::vector<Variant> &value);
    explicit Variant(const std::map<std::string, Variant> &value);
    Variant &operator=(const Variant &rhs);
    bool operator==(const Variant &rhs) const;

    Variant *address_value;
    bool boolean_value;
    Number number_value;
    std::string string_value;
    std::vector<Variant> array_value;
    std::map<std::string, Variant> dictionary_value;
};

#endif
