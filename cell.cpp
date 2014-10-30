#include "cell.h"

Variant::Variant()
  : address_value(NULL),
    boolean_value(false),
    number_value(),
    string_value(),
    array_value(),
    dictionary_value()
{
}

Variant::Variant(const Variant &rhs)
  : address_value(rhs.address_value),
    boolean_value(rhs.boolean_value),
    number_value(rhs.number_value),
    string_value(rhs.string_value),
    array_value(rhs.array_value),
    dictionary_value(rhs.dictionary_value)
{
}

Variant::Variant(Variant *value)
  : address_value(value),
    boolean_value(false),
    number_value(),
    string_value(),
    array_value(),
    dictionary_value()
{
}

Variant::Variant(bool value)
  : address_value(NULL),
    boolean_value(value),
    number_value(),
    string_value(),
    array_value(),
    dictionary_value()
{
}

Variant::Variant(Number value)
  : address_value(NULL),
    boolean_value(false),
    number_value(value),
    string_value(),
    array_value(),
    dictionary_value()
{
}

Variant::Variant(const std::string &value)
  : address_value(NULL),
    boolean_value(false),
    number_value(),
    string_value(value),
    array_value(),
    dictionary_value()
{
}

Variant::Variant(const char *value)
  : address_value(NULL),
    boolean_value(false),
    number_value(),
    string_value(value),
    array_value(),
    dictionary_value()
{
}

Variant::Variant(const std::vector<Variant> &value)
  : address_value(NULL),
    boolean_value(false),
    number_value(),
    string_value(),
    array_value(value),
    dictionary_value()
{
}

Variant::Variant(const std::map<std::string, Variant> &value)
  : address_value(NULL),
    boolean_value(false),
    number_value(),
    string_value(),
    array_value(),
    dictionary_value(value)
{
}

Variant &Variant::operator=(const Variant &rhs)
{
    if (&rhs == this) {
        return *this;
    }
    address_value = rhs.address_value;
    boolean_value = rhs.boolean_value;
    number_value = rhs.number_value;
    string_value = rhs.string_value;
    array_value = rhs.array_value;
    dictionary_value = rhs.dictionary_value;
    return *this;
}

bool Variant::operator==(const Variant &rhs) const
{
    // This currently compares everything (assuming that unused
    // values are initialised to default values). If in the future
    // the cell type remembers what type it holds, this can be
    // improved.
    return address_value == rhs.address_value
        && boolean_value == rhs.boolean_value
        && number_is_equal(number_value, rhs.number_value)
        && string_value == rhs.string_value
        && array_value == rhs.array_value
        && dictionary_value == rhs.dictionary_value;
}
